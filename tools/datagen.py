#!/usr/bin/env python3
"""Generate C initializer includes for data tables from the player's own main.dol.

Raw data tables in the decomp source do not carry their bytes. A table is written as

    // DUMP_DATA lbl_802F8F08 u32
    static const u32 lbl_802F8F08[256] = {
    #include "dumpdata/lbl_802F8F08.inc"
    };

The marker names the symbols.txt label and the element type. This script reads the
label's address and size from config/<ver>/symbols.txt, reads those bytes (big-endian)
out of orig/<ver>/sys/main.dol, and writes build/<ver>/include/dumpdata/<label>.inc
(already on every compiler's include path).

Element types:
  u8 s8 u16 s16 u32 s32 f32 f64 char wchar (wchar = u16)
  cstrptr     address of a named lbl_<address> C string, or zero
  cstr        pointer to a C string, emitted as an exact-byte string literal
  tableptr    pointer to an explicitly marked DUMP_DATA table
  {t1,t2,...}   one struct row per element, fields aligned naturally;
                a field may repeat as t[n] (emitted as a nested {...}).
Options after the type:
  addr=0x8xxxxxxx  select an entry when a label name is ambiguous
  count=N          use the first N elements or rows of the label
  group=N          group N struct rows in each inner array of a 2D initializer

`--check-relocs` audits every marker against the dtk-split target objects
(build/<ver>/obj). Each non-null pointer requires a matching R_PPC_ADDR32
relocation. Other fields must have no relocations.

Floats are emitted as the shortest decimal that parses back to the identical bits;
NaN, infinities and denormals are refused (leave such tables as source).

Usage:
  python tools/datagen.py --version REXE01 --out build/REXE01/include/dumpdata [--stamp f]
  python tools/datagen.py --list           # print markers found (configure.py uses this)
  python tools/datagen.py --check-relocs   # audit markers against the split objects
  python tools/datagen.py --verify-objects # verify table data and pointer targets in compiled objects
"""

import argparse
import os
import re
import struct
import sys
from pathlib import Path
from typing import Dict, List, NamedTuple, Optional, Tuple

ROOT = Path(__file__).resolve().parent.parent
SCAN_DIRS = ["src", "libs"]
SCAN_EXT = {".c", ".cpp", ".h", ".hpp", ".inc"}
MARKER_RE = re.compile(
    r"^\s*//\s*DUMP_DATA\s+(\S+)\s+(\{[^}]*\}|\S+)((?:\s+(?:addr|count|group)=\w+)*)"
)
SYM_RE = re.compile(
    r"^(\S+)\s*=\s*(\.[\w.]+):(0x[0-9A-Fa-f]+);\s*//(.*)$"
)

SCALARS = {
    # name: (size, struct fmt, kind)
    "u8": (1, ">B", "hex"),
    "s8": (1, ">b", "dec"),
    "char": (1, ">b", "dec"),
    "u16": (2, ">H", "hex"),
    "wchar": (2, ">H", "hex"),
    "s16": (2, ">h", "dec"),
    "u32": (4, ">I", "hex"),
    "s32": (4, ">i", "dec"),
    "f32": (4, ">f", "f32"),
    "f64": (8, ">d", "f64"),
    "cstrptr": (4, ">I", "cstrptr"),
    "cstr": (4, ">I", "cstr"),
    "tableptr": (4, ">I", "tableptr"),
}


class Marker:
    def __init__(self, label: str, type_: str, addr: Optional[int], src: Path, line: int,
                 count: Optional[int] = None, group: int = 0):
        self.label = label
        self.type = type_
        self.addr = addr
        self.count = count
        self.group = group
        self.src = src
        self.line = line


def find_markers() -> List[Marker]:
    out: List[Marker] = []
    for d in SCAN_DIRS:
        for dirpath, _, files in os.walk(ROOT / d):
            for fn in files:
                p = Path(dirpath) / fn
                if p.suffix not in SCAN_EXT:
                    continue
                try:
                    text = p.read_text(encoding="utf-8", errors="replace")
                except OSError:
                    continue
                if "DUMP_DATA" not in text:
                    continue
                for i, line in enumerate(text.splitlines(), 1):
                    m = MARKER_RE.match(line)
                    if m:
                        opts = dict(o.split("=", 1) for o in m.group(3).split())
                        addr = int(opts["addr"], 16) if "addr" in opts else None
                        count = int(opts["count"], 0) if "count" in opts else None
                        group = int(opts.get("group", "0"), 0)
                        out.append(Marker(m.group(1), m.group(2), addr, p, i, count, group))
    out.sort(key=lambda m: (str(m.src), m.line))
    return out


def load_symbols(path: Path) -> Dict[str, List[Tuple[int, int]]]:
    syms: Dict[str, List[Tuple[int, int]]] = {}
    for line in path.read_text(encoding="utf-8").splitlines():
        m = SYM_RE.match(line)
        if not m:
            continue
        size_m = re.search(r"size:(0x[0-9A-Fa-f]+)", m.group(4))
        if not size_m:
            continue
        syms.setdefault(m.group(1), []).append((int(m.group(3), 16), int(size_m.group(1), 16)))
    return syms


class Dol:
    def __init__(self, path: Path):
        self.data = path.read_bytes()
        hdr = struct.unpack(">18I18I18I", self.data[:0xD8])
        self.sections = [
            (hdr[18 + i], hdr[36 + i], hdr[i]) for i in range(18) if hdr[36 + i]
        ]  # (addr, size, file offset)

    def read(self, addr: int, size: int) -> bytes:
        for a, s, off in self.sections:
            if a <= addr and addr + size <= a + s:
                return self.data[off + addr - a : off + addr - a + size]
        raise ValueError(f"0x{addr:08X}+0x{size:X} is not file-backed in the DOL (bss?)")

    def c_string(self, addr: int) -> bytes:
        for base, size, off in self.sections:
            if base <= addr < base + size:
                start = off + addr - base
                end = self.data.find(b"\0", start, off + size)
                if end != -1:
                    return self.data[start:end]
        raise ValueError(f"no terminated C string at 0x{addr:08X}")


def parse_type(t: str) -> List[Tuple[str, int]]:
    """Return a list of (scalar, count) fields; count 0 = plain scalar."""
    if t.startswith("{"):
        fields = [f.strip() for f in t[1:-1].split(",") if f.strip()]
    else:
        fields = [t]
    out = []
    for f in fields:
        m = re.fullmatch(r"(\w+)(?:\[(\d+)\])?", f)
        if not m or m.group(1) not in SCALARS:
            raise ValueError(f"unknown element type {f!r}")
        out.append((m.group(1), int(m.group(2)) if m.group(2) else 0))
    return out


def fmt_f32(v: float, bits: int) -> str:
    exp = (bits >> 23) & 0xFF
    if exp == 0xFF or (exp == 0 and bits & 0x7FFFFF):
        raise ValueError(f"f32 0x{bits:08X} is NaN/inf/denormal")
    if bits == 0x80000000:
        return "-0.0f"
    for prec in range(6, 10):
        s = "%.*g" % (prec, v)
        if struct.unpack(">I", struct.pack(">f", float(s)))[0] == bits:
            break
    else:
        raise ValueError(f"f32 0x{bits:08X} does not round-trip")
    if not any(c in s for c in ".en"):
        s += ".0"
    return s + "f"


def fmt_f64(v: float, bits: int) -> str:
    exp = (bits >> 52) & 0x7FF
    if exp == 0x7FF or (exp == 0 and bits & 0xFFFFFFFFFFFFF):
        raise ValueError(f"f64 0x{bits:016X} is NaN/inf/denormal")
    if bits == 0x8000000000000000:
        return "-0.0"
    s = repr(v)
    if struct.unpack(">Q", struct.pack(">d", float(s)))[0] != bits:
        raise ValueError(f"f64 0x{bits:016X} does not round-trip")
    if not any(c in s for c in ".en"):
        s += ".0"
    return s


def fmt_scalar(name: str, raw: bytes, references: Dict[str, Dict[int, str]], dol: Dol) -> str:
    size, fmt, kind = SCALARS[name]
    v = struct.unpack(fmt, raw)[0]
    if kind == "cstr":
        if not v:
            return "0"
        content = "".join(chr(ch) if 0x20 <= ch < 0x7F and ch not in b'"\\?' else f"\\{ch:03o}"
                          for ch in dol.c_string(v))
        return f'"{content}"'
    if kind in references:
        if not v:
            return "0"
        if v not in references[kind]:
            raise ValueError(f"unresolved {kind} at 0x{v:08X}")
        return references[kind][v]
    if kind == "hex":
        return "0x%0*X" % (size * 2, v)
    if kind == "dec":
        # INT_MIN cannot be written as a plain negative literal
        if size == 4 and v == -0x80000000:
            return "(-0x7FFFFFFF - 1)"
        return str(v)
    if kind == "f32":
        return fmt_f32(v, struct.unpack(">I", raw)[0])
    return fmt_f64(v, struct.unpack(">Q", raw)[0])


def layout(fields: List[Tuple[str, int]]) -> Tuple[List[Tuple[str, int, int]], int]:
    """Field offsets with natural alignment; returns ([(scalar, count, offset)], row size)."""
    off = 0
    align = 1
    out = []
    for name, count in fields:
        size = SCALARS[name][0]
        off = (off + size - 1) // size * size
        align = max(align, size)
        out.append((name, count, off))
        off += size * max(count, 1)
    row = (off + align - 1) // align * align
    return out, row


def render(m: Marker, data: bytes, references: Dict[str, Dict[int, str]], dol: Dol) -> str:
    fields = parse_type(m.type)
    lay, row = layout(fields)
    if len(data) % row:
        raise ValueError(f"size 0x{len(data):X} is not a multiple of row size 0x{row:X}")
    n = len(data) // row
    is_struct = m.type.startswith("{")
    lines: List[str] = []
    if not is_struct:
        name = fields[0][0]
        size = SCALARS[name][0]
        vals = [fmt_scalar(name, data[i : i + size], references, dol) for i in range(0, len(data), size)]
        per = {1: 16, 2: 8, 4: 8, 8: 4}[size] if SCALARS[name][2] not in ("f32", "f64") else 4
        for i in range(0, len(vals), per):
            lines.append("    " + ", ".join(vals[i : i + per]) + ",")
        return "\n".join(lines) + "\n"
    for r in range(n):
        base = r * row
        covered = bytearray(row)
        parts = []
        for name, count, off in lay:
            size = SCALARS[name][0]
            for k in range(max(count, 1)):
                covered[off + k * size : off + (k + 1) * size] = b"\x01" * size
            if count:
                sub = [
                    fmt_scalar(name, data[base + off + k * size : base + off + (k + 1) * size], references, dol)
                    for k in range(count)
                ]
                parts.append("{" + ", ".join(sub) + "}")
            else:
                parts.append(fmt_scalar(name, data[base + off : base + off + size], references, dol))
        for i in range(row):
            if not covered[i] and data[base + i]:
                raise ValueError(f"row {r}: padding byte +0x{i:X} is nonzero")
        lines.append("    {" + ", ".join(parts) + "},")
    if m.group:
        if n % m.group:
            raise ValueError(f"{n} rows cannot be grouped into arrays of {m.group}")
        group_lines = []
        for i in range(0, n, m.group):
            group_lines.append("    {")
            group_lines.extend("    " + row_text for row_text in lines[i : i + m.group])
            group_lines.append("    },")
        lines = group_lines
    return "\n".join(lines) + "\n"


def resolve(m: Marker, syms: Dict[str, List[Tuple[int, int]]]) -> Tuple[int, int]:
    cands = syms.get(m.label, [])
    if m.addr is not None:
        cands = [c for c in cands if c[0] == m.addr]
    if len(cands) != 1:
        raise ValueError(
            f"{m.label}: {len(cands)} matching symbols.txt entries (add addr=0x... if ambiguous)"
        )
    return cands[0]


def table_size(marker: Marker, symbol_size: int) -> int:
    if marker.count is None:
        return symbol_size
    row = layout(parse_type(marker.type))[1]
    size = marker.count * row
    if size < 0 or size > symbol_size:
        raise ValueError(f"{marker.label}: count={marker.count} runs outside the label")
    return size


def string_symbols(symbol_path: Path) -> Dict[int, str]:
    result = {}
    for line in symbol_path.read_text(encoding="utf-8").splitlines():
        match = SYM_RE.match(line)
        if match is None:
            continue
        name, section, address, metadata = match.groups()
        addr = int(address, 16)
        if (name == f"lbl_{addr:08X}" and section in (".rodata", ".data", ".sdata")
                and re.search(r"\bdata:string1?\b", metadata)):
            result[addr] = name
    return result


def table_symbols(markers: List[Marker], syms) -> Dict[int, str]:
    result = {}
    for marker in markers:
        address, _ = resolve(marker, syms)
        if address in result and result[address] != marker.label:
            raise ValueError(f"ambiguous table pointer target at 0x{address:08X}")
        result[address] = marker.label
    return result


class ElfSymbol(NamedTuple):
    name: str
    value: int
    size: int
    section: int


class Relocation(NamedTuple):
    offset: int
    type: int
    symbol: ElfSymbol
    addend: int


class ElfTables:
    """Read symbol data and RELA entries in a big-endian PowerPC ELF32 object."""

    def __init__(self, path: Path):
        self.data = data = path.read_bytes()
        self.sections = []
        self.symbols = []
        self.defined = {}
        if data[:4] != b"\x7fELF":
            return
        start, = struct.unpack_from(">I", data, 0x20)
        entry, count = struct.unpack_from(">HH", data, 0x2E)
        self.sections = [struct.unpack_from(">10I", data, start + i * entry) for i in range(count)]
        symtab = next((s for s in self.sections if s[1] == 2), None)
        if symtab is None:
            return
        strings = self.sections[symtab[6]][4]
        for off in range(symtab[4], symtab[4] + symtab[5], 16):
            nm, val, size, _, _, index = struct.unpack_from(">IIIBBH", data, off)
            nm += strings
            name = data[nm : data.index(b"\0", nm)].decode("latin-1")
            symbol = ElfSymbol(name, val, size, index)
            self.symbols.append(symbol)
            if 0 < index < len(self.sections):
                self.defined[name] = symbol

    def relocations(self, symbol: ElfSymbol, size: int) -> List[Relocation]:
        rows = []
        for section in self.sections:
            if section[1] != 4 or section[7] != symbol.section:  # SHT_RELA
                continue
            for off in range(section[4], section[4] + section[5], 12):
                address, info, addend = struct.unpack_from(">IIi", self.data, off)
                if symbol.value <= address < symbol.value + size:
                    rows.append(Relocation(address - symbol.value, info & 0xFF, self.symbols[info >> 8], addend))
        return sorted(rows)

    def contents(self, symbol: ElfSymbol) -> bytes:
        section = self.sections[symbol.section]
        start = section[4] + symbol.value
        return b"" if section[1] == 8 else self.data[start:start + symbol.size]  # SHT_NOBITS

    def pointed_string(self, relocation: Relocation) -> Optional[bytes]:
        symbol = relocation.symbol
        if not 0 < symbol.section < len(self.sections):
            return None
        section = self.sections[symbol.section]
        offset = relocation.addend + symbol.value
        if section[1] == 8 or not 0 <= offset < section[5]:
            return None
        start = section[4] + offset
        end = self.data.find(b"\0", start, section[4] + section[5])
        return self.data[start:end] if end != -1 else None


def pointer_values(marker: Marker, data: bytes) -> List[Tuple[int, str, int]]:
    fields, row = layout(parse_type(marker.type))
    sites = [(offset + 4 * index, name) for name, count, offset in fields if name in ("cstrptr", "cstr", "tableptr")
             for index in range(max(count, 1))]
    if not sites:
        return []
    if len(data) % row:
        raise ValueError(f"{marker.label}: size does not match row layout")
    return [(base + offset, name, value) for base in range(0, len(data), row) for offset, name in sites
            if (value := struct.unpack_from(">I", data, base + offset)[0]) != 0]


def check_relocs(ver: str, markers: List[Marker], syms, dol: Dol) -> int:
    import json

    ranges = {}
    for marker in markers:
        address, size = resolve(marker, syms)
        ranges[marker.label] = address, table_size(marker, size)
    seen: Dict[str, List[Tuple[int, int]]] = {}
    # only the current split's units (build/<ver>/obj may hold stale objects from older splits)
    cfg = json.loads((ROOT / "build" / ver / "config.json").read_text(encoding="utf-8"))
    for unit in cfg["units"]:
        obj = ROOT / unit["object"]
        if not obj.exists():
            continue
        elf = ElfTables(obj)
        for label in elf.defined.keys() & ranges.keys():
            relocs = elf.relocations(elf.defined[label], ranges[label][1])
            seen.setdefault(label, []).extend((r.offset, r.type) for r in relocs)
    bad = 0
    for m in markers:
        pointers = pointer_values(m, dol.read(*ranges[m.label]))
        if m.label not in seen:
            print(f"?? {m.label}: not found in any split object")
            bad += 1
        elif sorted(seen[m.label]) != [(offset, 1) for offset, _, _ in pointers]:  # R_PPC_ADDR32
            print(f"!! {m.label}: relocation offsets or types disagree with the marker")
            bad += 1
        else:
            print(f"ok {m.label}")
    return 1 if bad else 0


def verify_table(elf: ElfTables, marker: Marker, expected: bytes, dol: Dol, references) -> None:
    symbol = elf.defined.get(marker.label)
    if symbol is None:
        raise ValueError("symbol missing from compiled object")
    got = bytearray(elf.contents(symbol)[:len(expected)])
    pointers = pointer_values(marker, expected)
    relocs = elf.relocations(symbol, len(expected))
    if [(r.offset, r.type) for r in relocs] != [(offset, 1) for offset, _, _ in pointers]:  # R_PPC_ADDR32
        raise ValueError("compiled relocation offsets or types disagree with the marker")
    for relocation, (offset, kind, address) in zip(relocs, pointers):
        if kind == "cstr":
            matches = elf.pointed_string(relocation) == dol.c_string(address)
        else:
            name = references[kind].get(address)
            target = elf.defined.get(name)
            matches = name is not None and relocation.symbol.name == name and not relocation.addend
            if not matches and target is not None:
                matches = (target.section == relocation.symbol.section
                           and target.value == relocation.symbol.value + relocation.addend)
        if not matches:
            raise ValueError(f"wrong pointer target at +0x{offset:X} (expected DOL address 0x{address:08X})")
        got[offset:offset + 4] = expected[offset:offset + 4]
    if got != expected:
        raise ValueError("compiled data differs from the DOL")


def verify_objects(ver: str, markers: List[Marker], syms, dol: Dol, references) -> int:
    # Splits may give SDK sources object paths unrelated to their source directories.
    ninja = (ROOT / "build.ninja").read_text(encoding="utf-8")
    paths = {
        src.replace("\\", "/"): obj.replace("\\", "/")
        for obj, src in re.findall(r"^build (\S+\.o): mwcc\w* \$\n\s+(\S+)", ninja, re.M)
    }
    objects = {}
    bad = 0
    for marker in markers:
        relative = marker.src.relative_to(ROOT).as_posix()
        if relative in paths:
            obj = ROOT / paths[relative]
        else:
            obj = ROOT / "build" / ver / "src" / Path(relative.removeprefix("src/")).with_suffix(".o")
        try:
            if obj not in objects:
                objects[obj] = ElfTables(obj)
            address, size = resolve(marker, syms)
            data = dol.read(address, table_size(marker, size))
            verify_table(objects[obj], marker, data, dol, references)
        except (OSError, ValueError) as error:
            print(f"!! {marker.label}: {error}")
            bad += 1
        else:
            print(f"ok {marker.label}")
    return 1 if bad else 0


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--version", default="REXE01")
    ap.add_argument("--dol", type=Path)
    ap.add_argument("--symbols", type=Path)
    ap.add_argument("--out", type=Path)
    ap.add_argument("--stamp", type=Path, help="touch this file when done")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("--check-relocs", action="store_true")
    ap.add_argument("--verify-objects", action="store_true")
    a = ap.parse_args()

    markers = find_markers()
    if a.list:
        for m in markers:
            print(f"{m.label}\t{m.type}\t{m.src.relative_to(ROOT).as_posix()}:{m.line}")
        return 0
    symbol_path = a.symbols or ROOT / "config" / a.version / "symbols.txt"
    syms = load_symbols(symbol_path)
    dol = Dol(a.dol or ROOT / "orig" / a.version / "sys" / "main.dol")
    if a.check_relocs:
        return check_relocs(a.version, markers, syms, dol)

    references = {"cstrptr": string_symbols(symbol_path), "tableptr": table_symbols(markers, syms)}
    if a.verify_objects:
        return verify_objects(a.version, markers, syms, dol, references)
    out = a.out or ROOT / "build" / a.version / "include" / "dumpdata"
    out.mkdir(parents=True, exist_ok=True)
    labels = set()
    err = 0
    for m in markers:
        if m.label in labels:
            print(f"{m.src}:{m.line}: duplicate DUMP_DATA marker {m.label}", file=sys.stderr)
            err = 1
            continue
        labels.add(m.label)
        try:
            addr, size = resolve(m, syms)
            text = (
                f"/* generated by tools/datagen.py from main.dol 0x{addr:08X}+0x{size:X}; do not edit */\n"
                + render(m, dol.read(addr, table_size(m, size)), references, dol)
            )
        except ValueError as e:
            print(f"{m.src}:{m.line}: {m.label}: {e}", file=sys.stderr)
            err = 1
            continue
        dst = out / f"{m.label}.inc"
        # only rewrite on change, so editing one source does not recompile every user
        if not dst.exists() or dst.read_text(encoding="utf-8") != text:
            dst.write_text(text, encoding="utf-8", newline="\n")
    if a.stamp and not err:
        a.stamp.parent.mkdir(parents=True, exist_ok=True)
        a.stamp.write_text(f"{len(markers)} tables\n")
    return err


if __name__ == "__main__":
    sys.exit(main())
