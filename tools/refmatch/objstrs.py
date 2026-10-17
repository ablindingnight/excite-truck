# Print the strings in a compiled object's data section with offsets, to compare its string pool
# against the original's (see strdump.py for the original side).
# Usage: python objstrs.py <obj.o> [section, default .data]
import os, re, subprocess, sys

obj = sys.argv[1]
sect = sys.argv[2] if len(sys.argv) > 2 else ".data"
out = os.path.join("build", "try", "objstrs.s")
subprocess.run([os.path.abspath("build/tools/dtk.exe"), "elf", "disasm", obj, out], capture_output=True)
text = open(out, encoding="utf-8", errors="replace").read()
m = re.search(r'^(?:\.section )?%s\b.*?$(.*?)(?=^\.section |^\.(?:text|data|bss|rodata|sdata|sbss|sdata2|sbss2)\b|\Z)'
              % re.escape(sect), text, re.M | re.S)
if not m:
    sys.exit("no %s section" % sect)
mem = bytearray()
for line in m.group(1).splitlines():
    s = line.split("*/")[-1].split("#")[0].strip()
    if s.startswith(".4byte "):
        v = s[7:].split(",")[0].strip()
        mem += int(v, 16).to_bytes(4, "big") if v.startswith("0x") else b"\xff" * 4
    elif s.startswith(".2byte "):
        mem += int(s[7:], 16).to_bytes(2, "big")
    elif s.startswith(".byte "):
        mem += bytes(int(x, 16) for x in s[6:].split(","))
    elif s.startswith(".string "):
        mem += bytes(s[8:].strip()[1:-1], "latin1").decode("unicode_escape").encode("latin1") + b"\0"
    elif s.startswith(".skip "):
        mem += b"\0" * int(s[6:], 16)
a = 0
while a < len(mem):
    if mem[a] == 0:
        a += 1
        continue
    start = a
    while a < len(mem) and mem[a]:
        a += 1
    print("%#06x %r" % (start, mem[start:a].decode("latin1")))
print("size %#x" % len(mem))
