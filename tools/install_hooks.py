# Install the repository's git hooks (hash check before commit, backup after commit).
# Usage: python tools/install_hooks.py
import os, shutil, subprocess
hooks = subprocess.run(["git", "rev-parse", "--git-path", "hooks"], capture_output=True, text=True, check=True).stdout.strip()
os.makedirs(hooks, exist_ok=True)
for name in ("pre-commit", "post-commit"):
    dst = os.path.join(hooks, name)
    shutil.copyfile(os.path.join("tools", "hooks", name), dst)
    os.chmod(dst, 0o755)
    print("installed", dst)
