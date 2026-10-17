#!/bin/sh
# Word-level diff of one target function against one compiled function: shows the instructions
# (with branch offsets) where they differ. fndiff.sh normalizes branch targets away, so use this when
# fndiff is clean but multimatch still reports a mismatch. Call words (bl) are skipped.
# Usage: rawdiff.sh <target_fn_name> <ref_obj> <ref_fn_name>
t="$1"; obj="$2"; r="$3"; tmp="${FNDIFF_TMP:-${TMPDIR:-/tmp}}/rawdiff.$$"
mkdir -p "$tmp"
f=$(grep -rl --include="*.s" "^\.fn $t," build/REXE01/asm 2>/dev/null | head -1)
build/tools/dtk.exe elf disasm "$obj" "$tmp/ref.s" >/dev/null 2>&1
words() { grep -E "^/\* [0-9A-F]+ " | sed -E 's/^\/\* ([0-9A-F]+ )?[0-9A-F]+  ([0-9A-F ]{11}) \*\/\s*/\2 /'; }
awk "/^\.fn $t,/,/^\.endfn/" "$f" | words > "$tmp/t.txt"
awk "/^\.fn $r,/,/^\.endfn/" "$tmp/ref.s" | words > "$tmp/r.txt"
# Words differing only by relocation (same disassembly text, e.g. lis/addi of a symbol) are skipped.
paste -d'|' "$tmp/t.txt" "$tmp/r.txt" | awk -F'|' '{split($1,a," "); split($2,b," "); ta=$1; tb=$2; sub(/^([0-9A-F][0-9A-F] ){4}/,"",ta); sub(/^([0-9A-F][0-9A-F] ){4}/,"",tb); if (a[1]a[2]a[3]a[4] != b[1]b[2]b[3]b[4] && a[1] !~ /^4B|^48/ && (ta != tb || ta ~ /\.L_/)) print NR": "$1"  <>  "$2}' | head -20
rm -rf "$tmp"
