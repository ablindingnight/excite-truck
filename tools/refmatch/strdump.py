# Print the strings in a range of the original's data sections with their offsets, decoded from
# dtk's split asm (handles .string/.byte/.4byte/.skip). Offsets are relative to <base> (default
# <lo>), so they line up with `addi rX, rBase, off` string-pool references in the code.
# Usage: python strdump.py <lo hex> <hi hex> [base hex] [--all]   (--all also prints short/binary runs)
import os, re, sys

args = [a for a in sys.argv[1:] if not a.startswith("--")]
lo, hi = int(args[0], 16), int(args[1], 16)
base = int(args[2], 16) if len(args) > 2 else lo
show_all = "--all" in sys.argv

mem = {}
for f in sorted(os.listdir("build/REXE01/asm")):
    if not f.endswith(".s") or "_text" in f:
        continue
    addr = None
    for line in open(os.path.join("build/REXE01/asm", f), encoding="utf-8", errors="replace"):
        m = re.match(r"\.obj (\S+),", line)
        if m:
            n = re.search(r"([0-9A-F]{8})", m.group(1))
            if n and addr is None:
                addr = int(n.group(1), 16)
            continue
        m = re.match(r"/\* ([0-9A-F]{8}) ", line)
        if m:
            addr = int(m.group(1), 16)
        if addr is None:
            continue
        s = line.split("*/")[-1].split("#")[0].strip()
        data = None
        if s.startswith(".string "):
            data = bytes(s[8:].strip()[1:-1], "latin1").decode("unicode_escape").encode("latin1") + b"\0"
        elif s.startswith(".byte "):
            data = bytes(int(x, 16) for x in s[6:].split(","))
        elif s.startswith(".4byte "):
            v = s[7:].split(",")[0].strip()
            data = int(v, 16).to_bytes(4, "big") if v.startswith("0x") else b"\xff\xff\xff\xff"
        elif s.startswith(".2byte "):
            data = int(s[7:].split(",")[0], 16).to_bytes(2, "big")
        elif s.startswith(".skip "):
            data = b"\0" * int(s[6:], 16)
        if data is not None:
            for i, b in enumerate(data):
                mem[addr + i] = b
            addr += len(data)

a = lo
while a < hi:
    if mem.get(a, 0) == 0:
        a += 1
        continue
    start = a
    buf = bytearray()
    while a < hi and mem.get(a, 0) != 0:
        buf.append(mem[a])
        a += 1
    text = buf.decode("latin1")
    printable = all(32 <= c < 127 or c in (9, 10, 13) for c in buf)
    if show_all or (printable and len(buf) >= 3):
        print("%#06x %08x %r" % (start - base, start, text))
