#!/usr/bin/env python3

###
# Generates build files for the project.
# This file also includes the project configuration,
# such as compiler flags and the object matching status.
#
# Usage:
#   python3 configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import sys
from pathlib import Path
from typing import Any, Dict, List

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "REXE01",  # 0
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str.upper,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--binutils",
    metavar="BINARY",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
parser.add_argument(
    "--debug",
    action="store_true",
    help="build with debug info (non-matching)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (optional)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (optional)",
)
parser.add_argument(
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds equivalent (but non-matching) or modded objects",
)
parser.add_argument(
    "--warn",
    dest="warn",
    type=str,
    choices=["all", "off", "error"],
    help="how to handle warnings",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)

# Apply arguments
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.generate_map = args.map
config.non_matching = args.non_matching
config.sjiswrap_path = args.sjiswrap
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions
config.binutils_tag = "2.42-1"
config.compilers_tag = "20250520"
config.dtk_tag = "v1.8.0"
config.objdiff_tag = "v3.0.0-beta.8"
config.sjiswrap_tag = "v1.2.2"
config.wibo_tag = "0.6.16"

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
]
config.ldflags = [
    "-fp hardware",
    "-nodefaults",
]
if args.debug:
    config.ldflags.append("-g")  # Or -gdwarf-2 for Wii linkers
if args.map:
    config.ldflags.append("-mapunused")
    # config.ldflags.append("-listclosure") # For Wii linkers

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Optional numeric ID for decomp.me preset
# Can be overridden in libraries or objects
config.scratch_preset_id = None

# Base flags, common to most GC/Wii games.
# Generally leave untouched, with overrides added below.
cflags_base = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    # "-W all",
    "-O4,p",
    "-inline auto",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-i include",
    "-i include/MSL",
    "-i include/MSL/internal",
    f"-i build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
]

# Debug flags
if args.debug:
    # Or -sym dwarf-2 for Wii compilers
    cflags_base.extend(["-sym on", "-DDEBUG=1"])
else:
    cflags_base.append("-DNDEBUG=1")

# Warning flags
if args.warn == "all":
    cflags_base.append("-W all")
elif args.warn == "off":
    cflags_base.append("-W off")
elif args.warn == "error":
    cflags_base.append("-W error")

# Metrowerks library flags
cflags_runtime = [
    *cflags_base,
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-gccinc",
    "-common off",
    "-inline auto",
]

# Revolution SDK flags (matches ogws)
cflags_rvl = [
    *cflags_base,
    "-enc SJIS",
    "-fp_contract off",
    "-ipa file",
    "-ir include/revolution/BTE",  # Broadcom stack headers
    "-DREVOLUTION",
    "-DVERSION_RSPE01_00",  # ogws sources: Excite Truck matches the Wii Sports rev 0 SDK variants
]

# Game code flags (see docs/compiler.md): generic PowerPC scheduling, no fused multiply-add.
cflags_game = [
    *cflags_base,
    "-proc generic",
    "-fp_contract off",
    "-enc SJIS",
]

# Revolution SDK flags for sources from the Twilight Princess decomp (zeldaret/tp, RZDE01_00),
# which ships the same Sep 2006 SDK build. Kept separate from ogws headers.
cflags_rvl_tp = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    "-O4,p",
    "-inline auto",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-i libs/tp_include",
    "-i libs/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common/Include",
    "-i libs/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common_Embedded/Math/Include",
    "-i libs/PowerPC_EABI_Support/MSL/MSL_C/PPC_EABI/Include",
    "-i libs/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/Include",
    "-i libs/PowerPC_EABI_Support/Runtime/Inc",
    "-i libs/revolution/include",
    "-i libs/revolution/include/revolution",
    "-ir libs/revolution/src",
    "-DVERSION=3",
    "-D__GEKKO__",
    "-D__REVOLUTION_SDK__",
    "-DSDK_SEP2006",
    "-DNDEBUG=1",
    "-enc SJIS",
    "-fp_contract off",
    "-sym on",
    "-ipa file",
]


def TpRevolution(path: str, matching: bool) -> Object:
    # Unit "revolution/<path>", source at libs/revolution/src/<path>
    return Object(matching, f"revolution/{path}", source=path)


# REL flags
cflags_rel = [
    *cflags_base,
    "-sdata 0",
    "-sdata2 0",
]

config.linker_version = "GC/3.0a5"


# Helper function for Dolphin libraries
def DolphinLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_base,
        "progress_category": "sdk",
        "objects": objects,
    }


# Helper function for REL script objects
def Rel(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.3.2",
        "cflags": cflags_rel,
        "progress_category": "game",
        "objects": objects,
    }


Matching = True                   # Object matches and should be linked
NonMatching = False               # Object does not match and should not be linked
Equivalent = config.non_matching  # Object should be linked when configured with --non-matching


# Object is only matching for specific versions
def MatchingFor(*versions):
    return config.version in versions


config.warn_missing_config = True
config.warn_missing_source = False
config.libs = [
    {
        "lib": "Runtime.PPCEABI.H",
        "mw_version": "GC/3.0a5",
        "cflags": cflags_runtime,
        "progress_category": "sdk",  # str | List[str]
        "objects": [
            Object(Matching, "Runtime.PPCEABI.H/__va_arg.c"),
            Object(Matching, "Runtime.PPCEABI.H/global_destructor_chain.c"),
            Object(Matching, "Runtime.PPCEABI.H/NMWException.cp", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "Runtime.PPCEABI.H/ptmf.c"),
            Object(Matching, "Runtime.PPCEABI.H/runtime.c"),
            Object(Matching, "Runtime.PPCEABI.H/__init_cpp_exceptions.cpp"),
            Object(Matching, "Runtime.PPCEABI.H/Gecko_ExceptionPPC.cp"),
            Object(Matching, "Runtime.PPCEABI.H/GCN_mem_alloc.c"),
        ],
    },
    {
        "lib": "RVL_SDK",
        "mw_version": "GC/3.0a5",
        "cflags": cflags_rvl,
        "progress_category": "sdk",
        "objects": [
            Object(Matching, "revolution/OS/OSAlloc.c"),
            Object(Matching, "revolution/OS/OSAudioSystem.c"),
            Object(Matching, "revolution/OS/OSContext.c"),
            Object(Matching, "revolution/OS/OSFatal.c"),
            Object(Matching, "revolution/OS/OSFont.c"),
            Object(Matching, "revolution/OS/OSMemory.c"),
            Object(Matching, "revolution/OS/OSMutex.c"),
            Object(Matching, "revolution/OS/OSUtf.c"),
            Object(Matching, "revolution/OS/OSCache.c"),
            Object(Matching, "revolution/MTX/mtx44.c"),
            Object(Matching, "revolution/DSP/dsp_task.c"),
            Object(Matching, "revolution/DVD/dvdFatal.c"),
            Object(Matching, "revolution/GX/GXGeometry.c"),
            Object(Matching, "revolution/GX/GXPixel.c"),
            Object(Matching, "revolution/OS/__ppc_eabi_init.c"),
            Object(Matching, "revolution/BTE/gki/common/gki_time.c"),
            Object(Matching, "revolution/BTE/bta/sys/bta_sys_conn.c"),
            Object(Matching, "revolution/BTE/bta/sys/bta_sys_main.c"),
            Object(Matching, "revolution/SC/scapi.c"),
            Object(Matching, "revolution/AX/AXProf.c"),
            Object(Matching, "revolution/BASE/PPCArch.c"),
            Object(Matching, "revolution/DB/db.c"),
            Object(Matching, "revolution/DVD/dvdqueue.c"),
            Object(Matching, "revolution/DVD/dvderror.c"),
            Object(Matching, "revolution/DVD/dvdidutils.c"),
            Object(Matching, "revolution/EXI/EXIUart.c"),
            Object(Matching, "revolution/GX/GXBump.c"),
            Object(Matching, "revolution/GX/GXDisplayList.c"),
            Object(Matching, "revolution/MEM/mem_allocator.c"),
            Object(Matching, "revolution/MEM/mem_list.c"),
            Object(Matching, "revolution/MTX/mtxvec.c"),
            Object(Matching, "revolution/OS/OSAlarm.c"),
            Object(Matching, "revolution/OS/OSArena.c"),
            Object(Matching, "revolution/OS/OSInterrupt.c"),
            Object(Matching, "revolution/SI/SISamplingRate.c"),
            Object(Matching, "revolution/BTE/main/bte_logmsg.c"),
            Object(Matching, "revolution/BTE/bta/sys/ptim.c"),
            Object(Matching, "revolution/TPL/TPL.c"),
            Object(Matching, "revolution/EUART/euart.c"),
            Object(Matching, "revolution/IPC/ipcMain.c"),
            Object(Matching, "revolution/IPC/ipcclt.c"),
            Object(Matching, "revolution/VF/pf_w_clib.c"),
            Object(Matching, "revolution/VF/pf_cp932.c"),
            Object(Matching, "revolution/VF/pf_init_prfile2.c"),
        ],
    },
]


config.libs.append(
    {
        "lib": "RVL_SDK_tp",
        "mw_version": "GC/3.0a3",
        "src_dir": "libs/revolution/src",
        "cflags": cflags_rvl_tp,
        "progress_category": "sdk",
        "objects": [
            TpRevolution("mem/mem_heapCommon.c", Matching),
            TpRevolution("os/OSError.c", Matching),
            TpRevolution("wpad/WPADEncrypt.c", Matching),
            TpRevolution("wud/WUDHidHost.c", Matching),
            TpRevolution("axfx/AXFXReverbHi.c", Matching),
            TpRevolution("ipc/ipcProfile.c", Matching),
            TpRevolution("mtx/mtx.c", Matching),
            TpRevolution("dsp/dsp_debug.c", Matching),
            TpRevolution("ipc/memory.c", Matching),
            TpRevolution("dsp/dsp.c", Matching),
            TpRevolution("vi/vi3in1.c", Matching),
            TpRevolution("gx/GXAttr.c", Matching),
            TpRevolution("os/OSRtc.c", Matching),
            TpRevolution("gx/GXTev.c", Matching),
            TpRevolution("gx/GXInit.c", Matching),
            TpRevolution("os/OSThread.c", Matching),
            TpRevolution("os/OSSync.c", Matching),
            TpRevolution("exi/EXICommon.c", Matching),
            TpRevolution("gx/GXFrameBuf.c", Matching),
            TpRevolution("gx/GXPerf.c", Matching),
            TpRevolution("mtx/vec.c", Matching),
            TpRevolution("os/OSMessage.c", Matching),
            TpRevolution("os/OSTime.c", Matching),
            TpRevolution("pad/Pad.c", Matching),
            TpRevolution("si/SIBios.c", Matching),
            TpRevolution("nand/NANDCheck.c", Matching),
        ],
    }
)


# Optional callback to adjust link order. This can be used to add, remove, or reorder objects.
# This is called once per module, with the module ID and the current link order.
#
# For example, this adds "dummy.c" to the end of the DOL link order if configured with --non-matching.
# "dummy.c" *must* be configured as a Matching (or Equivalent) object in order to be linked.
def link_order_callback(module_id: int, objects: List[str]) -> List[str]:
    # Don't modify the link order for matching builds
    if not config.non_matching:
        return objects
    if module_id == 0:  # DOL
        return objects + ["dummy.c"]
    return objects

# Uncomment to enable the link order callback.
# config.link_order_callback = link_order_callback


# Optional extra categories for progress tracking
# Adjust as desired for your project
config.progress_categories = [
    ProgressCategory("game", "Game Code"),
    ProgressCategory("sdk", "SDK Code"),
]
config.progress_each_module = args.verbose
# Optional extra arguments to `objdiff-cli report generate`
config.progress_report_args = [
    # Marks relocations as mismatching if the target value is different
    # Default is "functionRelocDiffs=none", which is most lenient
    # "--config functionRelocDiffs=data_value",
]

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
elif args.mode == "progress":
    # Print progress information
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
