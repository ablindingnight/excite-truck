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
  {t1,t2,...}   one struct row per element, fields aligned naturally;
                a field may repeat as t[n] (emitted as a nested {...}).
If a label name is not unique in symbols.txt, add addr=0x8xxxxxxx after the type.

Only tables WITHOUT relocations may be converted (no pointers). `--check-relocs` audits
every marker against the dtk-split target objects (build/<ver>/obj) and fails if any
relocation falls inside a marked table.

Floats are emitted as the shortest decimal that parses back to the identical bits;
NaN, infinities and denormals are refused (leave such tables as source).

Usage:
  python tools/datagen.py --version REXE01 --out build/REXE01/include/dumpdata [--stamp f]
  python tools/datagen.py --list           # print markers found (configure.py uses this)
  python tools/datagen.py --check-relocs   # audit markers against the split objects
"""

import argparse
import os
import re
import struct
import sys
from pathlib import Path
from typing import Dict, List, Optional, Tuple

ROOT = Path(__file__).resolve().parent.parent
SCAN_DIRS = ["src", "libs"]
SCAN_EXT = {".c", ".cpp", ".h", ".hpp", ".inc"}
MARKER_RE = re.compile(
    r"^\s*//\s*DUMP_DATA\s+(\S+)\s+(\{[^}]*\}|\S+)(?:\s+addr=(0x[0-9A-Fa-f]+))?"
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
}


class Marker:
    def __init__(self, label: str, type_: str, addr: Optional[int], src: Path, line: int):
        self.label = label
        self.type = type_
        self.addr = addr
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
                        addr = int(m.group(3), 16) if m.group(3) else None
                        out.append(Marker(m.group(1), m.group(2), addr, p, i))
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


def fmt_scalar(name: str, raw: bytes) -> str:
    size, fmt, kind = SCALARS[name]
    v = struct.unpack(fmt, raw)[0]
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


def render(m: Marker, data: bytes) -> str:
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
        vals = [fmt_scalar(name, data[i : i + size]) for i in range(0, len(data), size)]
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
                    fmt_scalar(name, data[base + off + k * size : base + off + (k + 1) * size])
                    for k in range(count)
                ]
                parts.append("{" + ", ".join(sub) + "}")
            else:
                parts.append(fmt_scalar(name, data[base + off : base + off + size]))
        for i in range(row):
            if not covered[i] and data[base + i]:
                raise ValueError(f"row {r}: padding byte +0x{i:X} is nonzero")
        lines.append("    {" + ", ".join(parts) + "},")
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


# ---------------------------------------------------------------- relocation audit
def elf_relocs_in(path: Path, ranges: Dict[str, Tuple[int, int]]) -> Dict[str, int]:
    """Count relocations inside the named symbols' ranges in a big-endian ELF32 object."""
    d = path.read_bytes()
    if d[:4] != b"\x7fELF":
        return {}
    shoff, = struct.unpack_from(">I", d, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from(">HHH", d, 0x2E)
    secs = [struct.unpack_from(">IIIIIIIIII", d, shoff + i * shentsize) for i in range(shnum)]
    symtab = next((s for s in secs if s[1] == 2), None)
    if symtab is None:
        return {}
    strtab = secs[symtab[6]]

    def cstr(off: int) -> str:
        end = d.index(b"\0", off)
        return d[off:end].decode("latin-1")

    found: Dict[str, Tuple[int, int, int]] = {}  # name -> (shndx, value, size)
    for i in range(symtab[5] // 16):
        nm, val, sz, info, oth, shndx = struct.unpack_from(">IIIBBH", d, symtab[4] + i * 16)
        name = cstr(strtab[4] + nm)
        if name in ranges and 0 < shndx < 0xFF00:  # defined here
            found[name] = (shndx, val, sz)
    out: Dict[str, int] = {}
    for s in secs:
        if s[1] != 4:  # SHT_RELA
            continue
        target = s[7]
        for name, (shndx, val, sz) in found.items():
            if shndx != target:
                continue
            size = ranges[name][1]
            cnt = 0
            for j in range(s[5] // 12):
                r_off, = struct.unpack_from(">I", d, s[4] + j * 12)
                if val <= r_off < val + size:
                    cnt += 1
            out[name] = out.get(name, 0) + cnt
    for name in found:
        out.setdefault(name, 0)
    return out


def check_relocs(ver: str, markers: List[Marker], syms) -> int:
    ranges = {m.label: resolve(m, syms) for m in markers}
    import json

    seen: Dict[str, int] = {}
    # only the current split's units (build/<ver>/obj may hold stale objects from older splits)
    cfg = json.loads((ROOT / "build" / ver / "config.json").read_text(encoding="utf-8"))
    for unit in cfg["units"]:
        obj = ROOT / unit["object"]
        if obj.exists():
            for k, v in elf_relocs_in(obj, ranges).items():
                seen[k] = seen.get(k, 0) + v
    bad = 0
    for m in markers:
        if m.label not in seen:
            print(f"?? {m.label}: not found in any split object")
            bad += 1
        elif seen[m.label]:
            print(f"!! {m.label}: {seen[m.label]} relocations - must stay as source")
            bad += 1
        else:
            print(f"ok {m.label}")
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
    a = ap.parse_args()

    markers = find_markers()
    if a.list:
        for m in markers:
            print(f"{m.label}\t{m.type}\t{m.src.relative_to(ROOT).as_posix()}:{m.line}")
        return 0
    syms = load_symbols(a.symbols or ROOT / "config" / a.version / "symbols.txt")
    if a.check_relocs:
        return check_relocs(a.version, markers, syms)

    dol = Dol(a.dol or ROOT / "orig" / a.version / "sys" / "main.dol")
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
                + render(m, dol.read(addr, size))
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
