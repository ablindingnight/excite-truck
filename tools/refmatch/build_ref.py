# Compile a reference decomp's sources with a given MWCC into an output dir.
# Usage: python build_ref.py <preset> <ref_repo> <compiler_dir> <out_dir> <src_subdir> [extra flags]
#   preset: "ogws" (Wii Sports, src/revolution) or "tp" (Twilight Princess Wii rev 0, libs/revolution/src)
import os, subprocess, sys
from concurrent.futures import ThreadPoolExecutor

COMMON = ["-nodefaults", "-proc", "gekko", "-align", "powerpc", "-enum", "int", "-fp", "hardware",
          "-Cpp_exceptions", "off", "-O4,p", "-inline", "auto", "-pragma", "cats off",
          "-pragma", "warn_notinlined off", "-maxerrors", "1", "-nosyspath", "-RTTI", "off",
          "-fp_contract", "on", "-str", "reuse", "-DNDEBUG=1"]
PRESETS = {
    "ogws": COMMON + ["-i", "include", "-i", "include/MSL", "-i", "include/MSL/internal",
                      "-DBUILD_VERSION=0", "-DVERSION_RSPE01_00", "-ir", "include/revolution/BTE",
                      "-DREVOLUTION", "-enc", "SJIS", "-fp_contract", "off", "-ipa", "file"],
    "tp": COMMON + ["-i", "include", "-i", "src",
                    "-i", "libs/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common/Include",
                    "-i", "libs/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common_Embedded/Math/Include",
                    "-i", "libs/PowerPC_EABI_Support/MSL/MSL_C/PPC_EABI/Include",
                    "-i", "libs/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/Include",
                    "-i", "libs/PowerPC_EABI_Support/Runtime/Inc",
                    "-DVERSION=3", "-D__GEKKO__", "-D__REVOLUTION_SDK__",
                    "-i", "libs/revolution/include", "-i", "libs/revolution/include/revolution",
                    "-enc", "SJIS", "-ir", "libs/revolution/src", "-fp_contract", "off", "-sym", "on",
                    "-inline", "auto", "-ipa", "file", "-DSDK_SEP2006", "-DNW4HBM_DEBUG"],
}
# Mario Strikers Charged (complete decomp); VERSION_R4QJ01 selects its Jan 2007 SDK variants
PRESETS["msc"] = ["-nodefaults", "-proc", "gekko", "-align", "powerpc", "-enum", "int", "-fp", "hardware",
                  "-Cpp_exceptions", "off", "-O4,p", "-pragma", "cats off", "-pragma", "warn_notinlined off",
                  "-maxerrors", "1", "-nosyspath", "-RTTI", "off", "-fp_contract", "on", "-str", "reuse",
                  "-enc", "SJIS", "-i", "include", "-i", "libs/Runtime/include", "-i", "libs/MSL_C/include",
                  "-i", "libs/RVL_SDK/include", "-i", "libs/MetroTRK/include", "-DBUILD_VERSION=3",
                  "-DVERSION_R4QJ01", "-DNDEBUG=1", "-inline", "auto", "-ipa", "file", "-fp_contract", "off"]
PRESETS["msc_msl"] = [f for f in PRESETS["msc"][:-6]] + ["-use_lmw_stmw", "on", "-str", "reuse,pool,readonly",
                      "-gccinc", "-common", "off", "-inline", "auto"]

preset, ref, cc, out, sub = sys.argv[1:6]
flags = PRESETS[preset] + sys.argv[6:]
srcs = []
for d, _, fs in os.walk(os.path.join(ref, sub)):
    for f in fs:
        if f.endswith((".c", ".cpp", ".cp")):
            srcs.append(os.path.relpath(os.path.join(d, f), ref))

def build(s):
    o = os.path.join(out, os.path.splitext(s)[0] + ".o")
    os.makedirs(os.path.dirname(o), exist_ok=True)
    lang = ["-lang", "c++"] if s.endswith((".cpp", ".cp")) else ["-lang", "c"]
    r = subprocess.run([os.path.join(cc, "mwcceppc.exe"), *flags, *lang, "-c", s, "-o", o],
                       cwd=ref, capture_output=True, text=True)
    return s, r.returncode, (r.stdout + r.stderr)[-400:]

ok = bad = 0
with ThreadPoolExecutor(8) as ex:
    for s, rc, log in ex.map(build, srcs):
        if rc:
            bad += 1
            print("FAIL", s, log.replace("\n", " | ")[-200:])
        else:
            ok += 1
print(f"built {ok}, failed {bad}")
