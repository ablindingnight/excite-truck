#!/bin/sh
# Diff one target function (from build/REXE01/asm) against one function in a reference object.
# Usage: fndiff.sh <target_fn_name> <ref_obj> <ref_fn_name>
t="$1"; obj="$2"; r="$3"; tmp="${FNDIFF_TMP:-${TMPDIR:-/tmp}}/fndiff.$$"
mkdir -p "$tmp"
f=$(grep -l "^\.fn $t," build/REXE01/asm/*.s build/REXE01/asm/*/*.s build/REXE01/asm/*/*/*.s 2>/dev/null | head -1)
[ -n "$f" ] || { echo "no target asm file defines $t"; rm -rf "$tmp"; exit 1; }
build/tools/dtk.exe elf disasm "$obj" "$tmp/ref.s" >/dev/null 2>&1
norm() { grep -oE "\*/\s+.*" | sed -E 's/\*\/\s*//; s/\.L_[0-9A-F]+/L/g; s/(bl|b) [^ ]+$/ SYM/; s/"?[^ ,(]+"?(\+0x[0-9a-f]+)?@(ha|l|h|sda21)/SYM@/g'; }
awk "/^\.fn $t,/,/^\.endfn/" "$f" | norm > "$tmp/t.txt"
awk "/^\.fn $r,/,/^\.endfn/" "$tmp/ref.s" | norm > "$tmp/r.txt"
echo "target $t: $(wc -l < "$tmp/t.txt") insns   ref $r: $(wc -l < "$tmp/r.txt") insns"
diff "$tmp/t.txt" "$tmp/r.txt"
rm -rf "$tmp"
