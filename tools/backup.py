# Back up the whole project history to ../backups as a single git bundle file, plus a copy of any
# uncommitted changes. Restore with: git clone <bundle> excite-truck-restored
# Usage: python tools/backup.py [label]
import datetime, os, subprocess, sys

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
dest = os.path.join(os.path.dirname(root), "backups")
os.makedirs(dest, exist_ok=True)
stamp = datetime.datetime.now().strftime("%Y-%m-%d_%H%M")
label = ("_" + sys.argv[1]) if len(sys.argv) > 1 else ""
head = subprocess.run(["git", "rev-parse", "--short", "HEAD"], cwd=root, capture_output=True, text=True).stdout.strip()

bundle = os.path.join(dest, f"excite-truck_{stamp}_{head}{label}.bundle")
subprocess.run(["git", "bundle", "create", bundle, "--all"], cwd=root, check=True, capture_output=True)
subprocess.run(["git", "bundle", "verify", bundle], cwd=root, check=True, capture_output=True)
print(f"history: {bundle} ({os.path.getsize(bundle) / 1e6:.1f} MB)")

# uncommitted work (tracked edits + new files), if any
diff = subprocess.run(["git", "diff", "HEAD", "--binary"], cwd=root, capture_output=True).stdout
new = subprocess.run(["git", "ls-files", "--others", "--exclude-standard"], cwd=root, capture_output=True, text=True).stdout.split()
if diff:
    patch = bundle.replace(".bundle", "_uncommitted.patch")
    open(patch, "wb").write(diff)
    print(f"uncommitted edits: {patch}")
if new:
    print(f"warning: {len(new)} untracked file(s) not backed up; commit them first: {' '.join(new[:5])}")
if not diff and not new:
    print("no uncommitted changes")
