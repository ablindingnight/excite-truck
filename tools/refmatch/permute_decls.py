# Try orderings of a function's local variable declarations until its code matches the original
# (MWCC's register and stack-slot assignment follows declaration order). The declarations are the
# consecutive lines right after the function's opening "{" (up to the first blank line).
# Usage: python permute_decls.py <source> <function signature prefix> <target fn> <ref fn> [cflags] [mw] [--moves]
# --moves only tries moving one declaration to another position (n^2 orders instead of n!), then
# repeats from the best order found, for functions with too many locals to permute fully.
import itertools, os, re, subprocess, sys

moves = "--moves" in sys.argv
argv = [a for a in sys.argv if a != "--moves"]
src, sig, target, ref = argv[1:5]
cflags = argv[5] if len(argv) > 5 else "cflags_msc_sdk"
mw = argv[6] if len(argv) > 6 else "GC/3.0a5"
orig = open(src).read()
i = orig.index(sig)
start = orig.index("{\n", i) + 2
end = orig.index("\n\n", start) + 1
decls = orig[start:end].splitlines(keepends=True)
obj = os.path.join("build", "try", "permute.o")
env = dict(os.environ, FNDIFF_TMP=os.path.join("build", "try"))


def score(lines):
    open(src, "w", newline="\n").write(orig[:start] + "".join(lines) + orig[end:])
    r = subprocess.run([sys.executable, "-I", "tools/refmatch/tpcc.py", src, obj, cflags, mw], capture_output=True, text=True)
    if r.returncode:
        return 10**6
    d = subprocess.run(["sh", "tools/refmatch/fndiff.sh", target, obj, ref], capture_output=True, text=True, env=env).stdout
    if not d.startswith("target "):
        return 10**6  # function missing from the object
    return len(d.splitlines()) - 1


def move_orders(lines):
    while True:
        base = list(lines)
        for i in range(len(base)):
            for j in range(len(base)):
                if i != j:
                    t = base[:i] + base[i + 1:]
                    t.insert(j, base[i])
                    yield t
        if best[1] == base or best[0] == 0:
            return
        lines = best[1]


best = None
try:
    orders = itertools.permutations(decls)
    if moves:
        best = (score(decls), list(decls))
        print("start: %d diff lines" % best[0], flush=True)
        orders = move_orders(decls)
    for n, perm in enumerate(orders):
        perm = list(perm)
        sc = score(perm)
        if best is None or sc < best[0]:
            best = (sc, perm)
            print("%5d: %d diff lines" % (n, sc), flush=True)
        if sc == 0:
            break
finally:
    if best and best[0] == 0:
        open(src, "w", newline="\n").write(orig[:start] + "".join(best[1]) + orig[end:])
        print("MATCH with order:\n" + "".join(best[1]))
    else:
        open(src, "w", newline="\n").write(orig)
        print("no match; best %d diff lines:\n%s" % (best[0], "".join(best[1])) if best else "nothing compiled")
