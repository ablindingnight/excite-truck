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
    "-DGAME_REXE01",  # Excite Truck-specific differences from Wii Sports
]

# Game code flags (see docs/compiler.md): generic PowerPC scheduling, no fused multiply-add.
cflags_game = [
    *cflags_base,
    "-inline noauto",  # tiny helpers stay out of line (app.cpp ResolveResName etc.)
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
    "-DGAME_REXE01",  # Excite Truck-specific differences from TP's Sep 2006 build
    "-DNDEBUG=1",
    "-enc SJIS",
    "-fp_contract off",
    "-sym on",
    "-ipa file",
]


def TpRevolution(path: str, matching: bool, **kwargs) -> Object:
    # Unit "revolution/<path>", source at libs/revolution/src/<path>
    return Object(matching, f"revolution/{path}", source=path, **kwargs)


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
            Object(Matching, "revolution/MEM/mem_frameHeap.c"),
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
            Object(Matching, "revolution/SC/scsystem.c"),
            Object(Matching, "revolution/SC/scapi.c"),
            Object(Matching, "revolution/SC/scapi_prdinfo.c"),
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
            Object(Matching, "revolution/OS/OSStateTM.c"),
            Object(Matching, "revolution/OS/OSPlayRecord.c"),
            Object(Matching, "revolution/OS/OSStateFlags.c"),
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


# TP's Home Button library (same Sep 2006 era as ET's; built with NW4HBM asserts like ET's).
# Same as cflags_rvl_tp but WITHOUT -sym on: with debug info MWCC puts out-of-line inline
# functions in separate .text sections, while ET's HBM has them interleaved at first use.
cflags_rvl_tp_hbm = [
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
    "-DGAME_REXE01",  # Excite Truck-specific differences from TP's Sep 2006 build
    "-DNDEBUG=1",
    "-enc SJIS",
    "-fp_contract off",
    "-ipa file",
    "-DNW4HBM_DEBUG",
    "-i libs/revolution/src/homebuttonLib",
]

config.libs.append(
    {
        "lib": "HBM_tp",
        "mw_version": "GC/3.0a5",
        "src_dir": "libs/revolution/src",
        "cflags": cflags_rvl_tp_hbm,
        "progress_category": "sdk",
        "objects": [
            TpRevolution("homebuttonLib/nw4hbm/lyt/lyt_layout.cpp", Matching),
            TpRevolution("homebuttonLib/nw4hbm/ut/ut_CharWriter.cpp", Matching),
            TpRevolution("homebuttonLib/nw4hbm/lyt/lyt_window.cpp", Matching),
            TpRevolution("homebuttonLib/nw4hbm/lyt/lyt_textBox.cpp", Matching),
            TpRevolution("homebuttonLib/nw4hbm/lyt/lyt_arcResourceAccessor.cpp", Matching),
            TpRevolution("homebuttonLib/nw4hbm/lyt/lyt_drawInfo.cpp", Matching),
            TpRevolution("homebuttonLib/nw4hbm/db/db_directPrint.cpp", Matching),
            TpRevolution("homebuttonLib/HBMBase.cpp", Matching),
            TpRevolution("homebuttonLib/HBMAnmController.cpp", Matching),
            TpRevolution("homebuttonLib/HBMGUIManager.cpp", Matching),
            TpRevolution("homebuttonLib/HBMController.cpp", Matching),
            TpRevolution("homebuttonLib/HBMRemoteSpk.cpp", Matching),
            TpRevolution("homebuttonLib/nw4hbm/lyt/lyt_pane.cpp", Matching),
            TpRevolution("homebuttonLib/nw4hbm/lyt/lyt_material.cpp", NonMatching),
            TpRevolution("homebuttonLib/nw4hbm/lyt/lyt_common.cpp", Matching),
            TpRevolution("homebuttonLib/nw4hbm/lyt/lyt_group.cpp", Matching),
        ],
    }
)

# Nintendo-supplied SDK MP3 (Layer III) decoder linked between SD and WENC (same tables as the Wii Photo
# Channel); library/object name unknown. Every function saves registers with stmw.
config.libs.append(
    {
        "lib": "SDK_mp3",
        "mw_version": "GC/3.0a3",
        "src_dir": "src",
        "cflags": [*cflags_rvl_tp, "-use_lmw_stmw on", "-inline noauto", "-i src/sdk_mp3"],
        "progress_category": "sdk",
        "objects": [
            Object(NonMatching, "sdk_mp3/mp3frame.c"),
            Object(NonMatching, "sdk_mp3/mp3decode.c"),
            Object(NonMatching, "sdk_mp3/mp3dequant.c"),
            Object(NonMatching, "sdk_mp3/mp3layer3.c"),
        ],
    }
)

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
            TpRevolution("wud/debug_msg.c", NonMatching),
            TpRevolution("wud/WUD.c", Matching),
            TpRevolution("wud/WUDHidHost.c", Matching),
            TpRevolution("axfx/AXFXReverbHi.c", Matching),
            TpRevolution("ipc/ipcProfile.c", Matching),
            TpRevolution("wpad/WPAD.c", Matching),
            TpRevolution("wpad/WPADHIDParser.c", Matching),
            TpRevolution("mem/mem_expHeap.c", Matching),
            TpRevolution("mtx/mtx.c", Matching),
            TpRevolution("dsp/dsp_debug.c", Matching),
            TpRevolution("ipc/memory.c", Matching),
            TpRevolution("dsp/dsp.c", Matching),
            TpRevolution("vi/vi3in1.c", Matching),
            TpRevolution("gx/GXAttr.c", Matching),
            TpRevolution("os/OSRtc.c", Matching),
            TpRevolution("os/OSSemaphore.c", Matching),
            TpRevolution("sd/isd.c", NonMatching),  # ISD_ReadCardRegister: 2 insns scheduled differently
            TpRevolution("gx/GXTev.c", Matching),
            TpRevolution("gx/GXInit.c", Matching),
            TpRevolution("gx/GXFifo.c", Matching),
            TpRevolution("gx/GXTexture.c", Matching),
            TpRevolution("kpad/KPAD.c", NonMatching),
            TpRevolution("ai/ai.c", Matching),
            TpRevolution("os/OS.c", Matching, mw_version="GC/3.0a5"),
            TpRevolution("os/OSReset.c", Matching, mw_version="GC/3.0a5"),
            TpRevolution("nand/nand.c", Matching),
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



# MSL C library from the Mario Strikers Charged decomp (yannicksuter/mscharged-decomp, CC0),
# built with its SDK flags and per-file extras.
cflags_msc = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    "-O4,p",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-enc SJIS",
    "-i libs/msc/include",
    "-i libs/msc/libs/Runtime/include",
    "-i libs/msc/libs/MSL_C/include",
    "-i libs/msc/libs/RVL_SDK/include",
    "-i libs/msc/libs/MetroTRK/include",
    "-DNDEBUG=1",
    "-DGAME_REXE01",  # Excite Truck-specific differences from Mario Strikers Charged
    "-inline auto",
    "-ipa file",
    "-fp_contract off",
]

# Home Button Menu (nw4hbm) from the Strikers decomp: its SDK flags plus HBM assertions.
cflags_msc_hbm = [
    *cflags_msc,
    "-i libs/msc/src/RVL_SDK/hbm",
    "-DHBM_ASSERT",
    "-DVERSION_R4QJ01",
]

config.libs.append(
    {
        "lib": "MSL_C",
        "mw_version": "GC/3.0a5",
        "src_dir": "libs/msc/src",
        "cflags": cflags_msc,
        "progress_category": "sdk",
        "objects": [
            Object(Matching, "MSL/direct_io.c", extra_cflags=['-Cpp_exceptions on', '-use_lmw_stmw on']),
            Object(Matching, "MSL/file_io.c", extra_cflags=['-Cpp_exceptions on', '-use_lmw_stmw on']),
            Object(Matching, "MSL/FILE_POS.c", extra_cflags=['-Cpp_exceptions on', '-use_lmw_stmw on']),
            Object(Matching, "MSL/mbstring.c", extra_cflags=['-use_lmw_stmw on']),
            Object(Matching, "MSL/mem.c"),
            Object(Matching, "MSL/mem_funcs.c"),
            Object(Matching, "MSL/qsort.c", extra_cflags=["-Cpp_exceptions on", "-use_lmw_stmw on"]),
            Object(Matching, "MSL/scanf.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "MSL/strtold.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "MSL/strtoul.c", extra_cflags=["-Cpp_exceptions on", "-use_lmw_stmw on"]),
            Object(Matching, "MSL/wstring.c"),
            Object(Matching, "MSL/wchar_io.c"),
            Object(Matching, "MSL/s_copysign.c"),
            Object(Matching, "MSL/s_cos.c"),
            Object(Matching, "MSL/s_floor.c"),
            Object(Matching, "MSL/s_frexp.c"),
            Object(Matching, "MSL/s_ldexp.c"),
            Object(Matching, "MSL/s_sin.c"),
            Object(Matching, "MSL/math_api.c"),
            Object(Matching, "MSL/misc_io.c"),
            Object(Matching, "MSL/string.c"),
            Object(Matching, "MSL/uart_console_io_gcn.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "MSL/math_sun.c", extra_cflags=['-Cpp_exceptions on']),
            Object(Matching, "MSL/extras.c"),
            Object(Matching, "MSL/e_atan2.c"),
            Object(Matching, "MSL/s_ceil.c"),
            Object(Matching, "MSL/s_tan.c"),
            Object(Matching, "MSL/k_cos.c"),
            Object(Matching, "MSL/k_sin.c"),
            Object(Matching, "MSL/k_tan.c"),
            Object(Matching, "MSL/s_atan.c"),
            Object(Matching, "MSL/e_rem_pio2.c"),
            Object(Matching, "MSL/k_rem_pio2.c", extra_cflags=['-use_lmw_stmw on']),
            Object(Matching, "MSL/e_pow.c"),
            Object(Matching, "MSL/e_acos.c"),
            Object(Matching, "MSL/e_sqrt.c"),
            Object(NonMatching, "MSL/abort_exit_ppc_eabi.c", extra_cflags=['-Cpp_exceptions on']),
            Object(Matching, "MSL/alloc.c", extra_cflags=['-Cpp_exceptions on', '-use_lmw_stmw on']),
            Object(Matching, "MSL/ansi_files.c", extra_cflags=['-Cpp_exceptions on']),
            Object(Matching, "MSL/ansi_fp.c", extra_cflags=['-Cpp_exceptions on', '-rostr', '-use_lmw_stmw on']),
            Object(Matching, "MSL/arith.c"),
            Object(Matching, "MSL/buffer_io.c", extra_cflags=['-Cpp_exceptions on', '-use_lmw_stmw on']),
            Object(NonMatching, "MSL/char_io.c", extra_cflags=['-Cpp_exceptions on', '-use_lmw_stmw on']),
            Object(NonMatching, "MSL/ctype.c"),
            Object(NonMatching, "MSL/errno.c"),
            Object(NonMatching, "MSL/float.c"),
            Object(NonMatching, "MSL/locale.c", extra_cflags=['-rostr']),
            Object(NonMatching, "MSL/math_ppc.c"),
            Object(NonMatching, "MSL/rand.c"),
            Object(Matching, "MSL/w_acos.c", extra_cflags=["-D_IEEE_LIBM"]),
            Object(Matching, "MSL/w_atan2.c", extra_cflags=["-D_IEEE_LIBM"]),
            Object(Matching, "MSL/w_pow.c", extra_cflags=["-D_IEEE_LIBM"]),
            Object(Matching, "MSL/w_sqrt.c", extra_cflags=["-D_IEEE_LIBM"]),
            Object(NonMatching, "MSL/wctype.c"),
            Object(NonMatching, "MSL/wmem.c"),
            Object(NonMatching, "MSL/wprintf.c"),
            Object(Matching, "MSL/e_asin.c"),
            Object(Matching, "MSL/e_log.c"),
            Object(Matching, "MSL/e_fmod.c"),
            Object(Matching, "MSL/w_asin.c", extra_cflags=["-D_IEEE_LIBM"]),
            Object(Matching, "MSL/w_log.c", extra_cflags=["-D_IEEE_LIBM"]),
            Object(Matching, "MSL/w_fmod.c", extra_cflags=["-D_IEEE_LIBM"]),
            Object(Matching, "MSL/printf.c", mw_version="GC/3.0a5.2", extra_cflags=["-Cpp_exceptions on", "-use_lmw_stmw on", "-str reuse,pool,readonly"]),
        ],
    }
)

config.libs.append(
    {
        "lib": "HBM",
        "mw_version": "GC/3.0a5.2",
        "src_dir": "libs/msc/src",
        "cflags": cflags_msc_hbm,
        "progress_category": "sdk",
        "objects": [
            Object(Matching, "RVL_SDK/hbm/HBMFrameController.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/db/db_assert.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/db/db_console.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/db/db_mapFile.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_animation.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_bounding.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_picture.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/math/math_triangular.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_AxManager.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_AxVoice.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_Bank.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_BankFile.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_BasicSound.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_Channel.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_DisposeCallbackManager.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_DvdSoundArchive.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_EnvGenerator.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_ExternalSoundPlayer.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_FrameHeap.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_InstancePool.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_Lfo.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_MemorySoundArchive.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_MidiSeqPlayer.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_MmlParser.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_MmlSeqTrackAllocator.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_MmlSeqTrack.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundArchiveFile.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundArchivePlayer.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_StrmSound.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_WaveSound.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_WsdTrack.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_NandSoundArchive.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_RemoteSpeaker.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_RemoteSpeakerManager.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SeqFile.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SeqPlayer.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SeqSound.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SeqSoundHandle.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SeqTrack.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundArchive.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundArchiveLoader.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundHandle.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundHeap.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundPlayer.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundStartable.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundSystem.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundThread.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_StrmChannel.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_StrmFile.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_StrmPlayer.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_StrmSoundHandle.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_TaskManager.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_TaskThread.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_Util.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_WaveFile.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_WavePlayer.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_WaveSoundHandle.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_WsdFile.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_WsdPlayer.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_binaryFileFormat.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_CharStrmReader.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_FileStream.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_Font.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_IOStream.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_LinkList.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_list.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_ResFont.cpp"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_ResFontBase.cpp"),
        ],
    }
)


# Revolution SDK sources from the Strikers decomp (its Jan 2007 SDK variants).
cflags_msc_sdk = [
    *cflags_msc,
    "-i libs/msc/include",
    "-DBUILD_VERSION=3",
    "-DVERSION_R4QJ01",
]

# A few of Excite Truck's SDK files were built at -O3 (flagsearch.py finds these). mwcc keeps the
# higher of two -O levels, so the base -O4,p has to be replaced rather than overridden.
cflags_msc_sdk_O3 = ["-O3,p" if f == "-O4,p" else f for f in cflags_msc_sdk]

# Broadcom Bluetooth stack (BTE) from the Strikers decomp.
cflags_msc_bte = [
    *cflags_msc_sdk,
    "-i libs/msc/libs/RVL_SDK/include/private/bte",
]

config.libs.append(
    {
        "lib": "BTE_msc",
        "mw_version": "GC/3.0a5",
        "src_dir": "libs/msc/src",
        "cflags": cflags_msc_bte,
        "progress_category": "sdk",
        "objects": [
            Object(Matching, "RVL_SDK/bte/bd.c"),
            Object(Matching, "RVL_SDK/bte/bta_dm_act.c"),
            Object(Matching, "RVL_SDK/bte/bta_dm_api.c"),
            Object(Matching, "RVL_SDK/bte/bta_dm_main.c"),
            Object(Matching, "RVL_SDK/bte/bta_dm_pm.c"),
            Object(Matching, "RVL_SDK/bte/bta_hh_act.c"),
            Object(Matching, "RVL_SDK/bte/bta_hh_api.c"),
            Object(Matching, "RVL_SDK/bte/bta_hh_main.c"),
            Object(Matching, "RVL_SDK/bte/bta_hh_utils.c"),
            Object(Matching, "RVL_SDK/bte/bte_hcisu.c"),
            Object(Matching, "RVL_SDK/bte/bte_main.c"),
            Object(Matching, "RVL_SDK/bte/btm_acl.c"),
            Object(Matching, "RVL_SDK/bte/btm_dev.c"),
            Object(Matching, "RVL_SDK/bte/btm_devctl.c"),
            Object(Matching, "RVL_SDK/bte/btm_discovery.c"),
            Object(Matching, "RVL_SDK/bte/btm_inq.c"),
            Object(Matching, "RVL_SDK/bte/btm_main.c"),
            Object(Matching, "RVL_SDK/bte/btm_pm.c"),
            Object(Matching, "RVL_SDK/bte/btm_sco.c"),
            Object(Matching, "RVL_SDK/bte/btm_sec.c"),
            Object(Matching, "RVL_SDK/bte/btu_hcif.c"),
            Object(Matching, "RVL_SDK/bte/btu_init.c"),
            Object(Matching, "RVL_SDK/bte/btu_task1.c"),
            Object(Matching, "RVL_SDK/bte/gap_api.c"),
            Object(Matching, "RVL_SDK/bte/gap_conn.c"),
            Object(Matching, "RVL_SDK/bte/gap_utils.c"),
            Object(Matching, "RVL_SDK/bte/gki_buffer.c"),
            Object(Matching, "RVL_SDK/bte/gki_ppc.c"),
            Object(Matching, "RVL_SDK/bte/hcicmds.c"),
            Object(Matching, "RVL_SDK/bte/hcisu_h2.c"),
            Object(Matching, "RVL_SDK/bte/hidd_api.c"),
            Object(Matching, "RVL_SDK/bte/hidd_conn.c"),
            Object(Matching, "RVL_SDK/bte/hidd_mgmt.c"),
            Object(Matching, "RVL_SDK/bte/hidd_pm.c"),
            Object(Matching, "RVL_SDK/bte/hidh_api.c"),
            Object(Matching, "RVL_SDK/bte/hidh_conn.c"),
            Object(Matching, "RVL_SDK/bte/l2c_api.c"),
            Object(Matching, "RVL_SDK/bte/l2c_csm.c"),
            Object(Matching, "RVL_SDK/bte/l2c_link.c"),
            Object(Matching, "RVL_SDK/bte/l2c_main.c"),
            Object(Matching, "RVL_SDK/bte/l2c_utils.c"),
            Object(Matching, "RVL_SDK/bte/port_api.c"),
            Object(Matching, "RVL_SDK/bte/port_rfc.c"),
            Object(Matching, "RVL_SDK/bte/port_utils.c"),
            Object(Matching, "RVL_SDK/bte/rfc_l2cap_if.c"),
            Object(Matching, "RVL_SDK/bte/rfc_mx_fsm.c"),
            Object(Matching, "RVL_SDK/bte/rfc_port_fsm.c"),
            Object(Matching, "RVL_SDK/bte/rfc_port_if.c"),
            Object(Matching, "RVL_SDK/bte/rfc_ts_frames.c"),
            Object(Matching, "RVL_SDK/bte/rfc_utils.c"),
            Object(Matching, "RVL_SDK/bte/sdp_api.c"),
            Object(Matching, "RVL_SDK/bte/sdp_db.c"),
            Object(Matching, "RVL_SDK/bte/sdp_discovery.c"),
            Object(Matching, "RVL_SDK/bte/sdp_main.c"),
            Object(Matching, "RVL_SDK/bte/sdp_server.c"),
            Object(Matching, "RVL_SDK/bte/sdp_utils.c"),
            Object(Matching, "RVL_SDK/bte/uusb_ppc.c"),
            Object(Matching, "RVL_SDK/bte/wbt_ext.c"),
        ],
    }
)

config.libs.append(
    {
        "lib": "RVL_SDK_msc",
        "mw_version": "GC/3.0a5",
        "src_dir": "libs/msc/src",
        "cflags": cflags_msc_sdk,
        "progress_category": "sdk",
        "objects": [
            Object(Matching, "RVL_SDK/ax/AX.c"),
            Object(Matching, "RVL_SDK/ax/AXAlloc.c"),
            Object(Matching, "RVL_SDK/ax/AXAux.c"),
            Object(Matching, "RVL_SDK/ax/AXCL.c"),
            Object(Matching, "RVL_SDK/ax/AXOut.c"),
            Object(Matching, "RVL_SDK/ax/AXSPB.c"),
            Object(Matching, "RVL_SDK/ax/AXVPB.c"),
            Object(Matching, "RVL_SDK/dvd/dvdfs.c"),
            Object(Matching, "RVL_SDK/dvd/dvd.c"),
            Object(Matching, "RVL_SDK/dvd/dvd_broadway.c"),
            Object(Matching, "RVL_SDK/exi/EXIBios.c", cflags=cflags_msc_sdk_O3),
            Object(Matching, "RVL_SDK/gx/GXMisc.c"),
            Object(Matching, "RVL_SDK/gx/GXLight.c"),
            Object(Matching, "RVL_SDK/gx/GXTransform.c"),
            Object(Matching, "RVL_SDK/mem/mem_unitHeap.c"),
            Object(Matching, "RVL_SDK/os/OSExec.c"),
            Object(Matching, "RVL_SDK/os/OSReboot.c"),
            Object(Matching, "RVL_SDK/vi/vi.c"),
            Object(Matching, "RVL_SDK/vi/i2c.c"),
            Object(Matching, "RVL_SDK/usb/usb.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXReverbHiExp.c"),
            Object(Matching, "RVL_SDK/fs/fs.c"),
            Object(Matching, "RVL_SDK/nand/NANDOpenClose.c"),
            Object(Matching, "RVL_SDK/nand/NANDCore.c"),
            Object(Matching, "RVL_SDK/vf/pf_clib.c"),
            Object(Matching, "RVL_SDK/vf/pf_code.c"),
            Object(Matching, "RVL_SDK/vf/pf_service.c"),
            Object(Matching, "RVL_SDK/vf/pf_str.c"),
            Object(Matching, "RVL_SDK/vf/pf_driver.c"),
            Object(Matching, "RVL_SDK/vf/pdm_dskmng.c"),
            Object(Matching, "RVL_SDK/vf/pdm_bpb.c"),
            Object(Matching, "RVL_SDK/vf/pdm_disk.c"),
            Object(NonMatching, "RVL_SDK/vf/pdm_mbr.c"),
            Object(NonMatching, "RVL_SDK/vf/pdm_partition.c"),
            Object(Matching, "RVL_SDK/vf/pf_cache.c"),
            Object(Matching, "RVL_SDK/vf/pf_cluster.c"),
            Object(Matching, "RVL_SDK/vf/pf_dir.c"),
            Object(Matching, "RVL_SDK/vf/pf_entry.c"),
            Object(Matching, "RVL_SDK/vf/pf_entry_iterator.c"),
            Object(Matching, "RVL_SDK/vf/pf_fat.c"),
            Object(Matching, "RVL_SDK/vf/pf_fat12.c"),
            Object(Matching, "RVL_SDK/vf/pf_fat16.c"),
            Object(Matching, "RVL_SDK/vf/pf_fat32.c"),
            Object(Matching, "RVL_SDK/vf/pf_fatfs.c"),
            Object(Matching, "RVL_SDK/vf/pf_file.c"),
            Object(Matching, "RVL_SDK/vf/pf_path.c"),
            Object(Matching, "RVL_SDK/vf/pf_sector.c"),
            Object(Matching, "RVL_SDK/vf/pf_volume.c"),
            Object(Matching, "RVL_SDK/vf/pf_api_util.c"),
            Object(Matching, "RVL_SDK/vf/pf_attach.c"),
            Object(Matching, "RVL_SDK/vf/pf_detach.c"),
            Object(Matching, "RVL_SDK/vf/pf_fclose.c"),
            Object(Matching, "RVL_SDK/vf/pf_finfo.c"),
            Object(Matching, "RVL_SDK/vf/pf_fopen.c"),
            Object(Matching, "RVL_SDK/vf/pf_fread.c"),
            Object(Matching, "RVL_SDK/vf/pf_fseek.c"),
            Object(Matching, "RVL_SDK/vf/pf_fsfirst.c"),
            Object(Matching, "RVL_SDK/vf/pf_fsnext.c"),
            Object(Matching, "RVL_SDK/vf/pf_unmount.c"),
            Object(Matching, "RVL_SDK/vf/pf_filelock.c"),
            Object(Matching, "RVL_SDK/vf/pf_system.c"),
            Object(Matching, "RVL_SDK/vf/vf_et_api.c"),
            Object(Matching, "RVL_SDK/vf/pfk.c"),
            Object(Matching, "RVL_SDK/vf/nand_drv.c"),
            Object(Matching, "RVL_SDK/vf/sd_drv.c"),
            Object(Matching, "RVL_SDK/arc/arc.c"),
        ],
    }
)


# Monster Games' engine and Excite Truck's own code (no reference; names are guesses).
config.libs.append(
    {
        "lib": "game",
        "mw_version": "GC/3.0a5",
        "cflags": cflags_game,
        "progress_category": "game",
        "objects": [
            Object(Matching, "game/app.cpp"),
            Object(Matching, "game/flags.cpp"),
            Object(Matching, "game/stubs.cpp"),
            Object(Matching, "game/version.cpp"),
            Object(Matching, "game/rep.cpp"),
            Object(Matching, "game/loadtimer.cpp"),
            Object(Matching, "game/fwd126b4.cpp"),
            Object(Matching, "game/race.cpp"),
            Object(Matching, "game/cmdline.cpp"),
            Object(Matching, "game/grades.cpp"),
            Object(Matching, "game/carstats.cpp"),
            Object(NonMatching, "game/unlocks.cpp"),
            Object(NonMatching, "game/ident.cpp"),
            Object(NonMatching, "game/achieve.cpp"),
            Object(Matching, "game/driver.cpp"),
            Object(NonMatching, "game/loading.cpp"),
            Object(NonMatching, "game/stats.cpp"),
            Object(NonMatching, "game/davmap.cpp"),
            Object(NonMatching, "game/eve.cpp"),
            Object(NonMatching, "game/stdoverlay.cpp"),
            Object(NonMatching, "game/overlay.cpp"),
            Object(NonMatching, "game/escape.cpp"),
            Object(NonMatching, "game/replay.cpp"),
            Object(NonMatching, "game/raceload.cpp"),
            Object(NonMatching, "game/gameloop.cpp"),
            Object(NonMatching, "game/racing.cpp"),
            Object(NonMatching, "game/dashhelper.cpp"),
            Object(NonMatching, "game/trackmap.cpp"),
            Object(NonMatching, "game/prof.cpp"),
            Object(Matching, "game/eventq.cpp"),
            Object(Matching, "game/bg.cpp"),
            Object(NonMatching, "game/mem.cpp"),
            Object(Matching, "game/spubank.cpp"),
            Object(Matching, "game/sndrate.cpp"),
            Object(Matching, "game/scratch.cpp"),
            Object(NonMatching, "game/joy.cpp"),
            Object(Matching, "game/sysinit.cpp"),
            Object(NonMatching, "game/random.cpp"),
            Object(NonMatching, "game/pool.cpp"),
            Object(NonMatching, "game/res.cpp"),
            Object(Matching, "game/strtab.cpp"),
            Object(Matching, "game/memstream.cpp"),
            Object(NonMatching, "game/quat.cpp"),
            Object(Matching, "game/base64.cpp"),
            Object(NonMatching, "game/locale.cpp"),
            Object(NonMatching, "game/valtable.cpp"),
            Object(Matching, "game/crc.cpp"),
            Object(Matching, "game/options.cpp"),
            Object(Matching, "game/loadpool.cpp"),
            Object(Matching, "game/strfunc.cpp"),
            Object(NonMatching, "game/aidriver.cpp"),
            Object(NonMatching, "game/idealline.cpp"),
            Object(NonMatching, "game/proxer.cpp"),
            Object(Matching, "game/league.cpp"),
            Object(NonMatching, "game/idealres.cpp"),
            Object(NonMatching, "game/catchup.cpp"),
            Object(Matching, "game/aiwidth.cpp"),
            Object(NonMatching, "game/radar.cpp"),
            Object(NonMatching, "game/arenabnd.cpp"),
            Object(NonMatching, "game/aistub.cpp"),
            Object(Matching, "game/pushpass.cpp"),
            Object(NonMatching, "game/aiskill.cpp"),
            Object(NonMatching, "game/catchupval.cpp"),
            Object(NonMatching, "game/lineinfo.cpp"),
            Object(Matching, "game/carclass.cpp"),
            Object(NonMatching, "game/subtask.cpp"),
            Object(NonMatching, "game/zone.cpp"),
            Object(NonMatching, "game/modeinl.cpp"),
            Object(NonMatching, "game/records.cpp"),
            Object(NonMatching, "game/ovlayout.cpp"),
            Object(NonMatching, "game/betarace.cpp"),
            Object(NonMatching, "game/ilresmgr.cpp"),
            Object(NonMatching, "game/stunts.cpp"),
            Object(NonMatching, "game/aiobst.cpp"),
            Object(Matching, "game/gradesched.cpp"),
            Object(Matching, "game/aiutil.cpp"),
            Object(NonMatching, "game/tutorial.cpp"),
            Object(NonMatching, "game/orbit.cpp"),
            Object(Matching, "game/stub5.cpp"),
            Object(NonMatching, "game/pickup.cpp"),
            Object(NonMatching, "game/ring.cpp"),
            Object(NonMatching, "game/ringchal.cpp"),
            Object(NonMatching, "game/techdemo.cpp"),
            Object(NonMatching, "game/crushmaster.cpp"),
            Object(NonMatching, "game/physics.cpp"),
            Object(NonMatching, "game/phob.cpp"),
            Object(NonMatching, "game/collvol.cpp"),
            Object(Matching, "game/physdriver.cpp"),
            Object(Matching, "game/stub6.cpp"),
            Object(Matching, "game/tirelist.cpp"),
            Object(Matching, "game/stub7.cpp"),
            Object(Matching, "game/phoblist.cpp"),
            Object(Matching, "game/pgfx.cpp"),
            Object(Matching, "game/spotter.cpp"),
            Object(NonMatching, "game/amaster.cpp"),
            Object(Matching, "game/alarm.cpp"),
            Object(Matching, "game/dmginfo.cpp"),
            Object(Matching, "game/csreplay.cpp"),
            Object(Matching, "game/surftype.cpp"),
            Object(Matching, "game/carlisten.cpp"),
            Object(Matching, "game/assoc.cpp"),
            Object(Matching, "game/updlist.cpp"),
            Object(Matching, "game/drawlist.cpp"),
            Object(NonMatching, "game/carelem.cpp"),
            Object(NonMatching, "game/cargear.cpp"),
            Object(Matching, "game/carmesh.cpp"),
            Object(NonMatching, "game/actseq.cpp"),
            Object(NonMatching, "game/mgroup.cpp"),
            Object(NonMatching, "game/weather.cpp"),
            Object(NonMatching, "game/joint.cpp"),
            Object(NonMatching, "game/dbglod.cpp"),
            Object(Matching, "game/fonttext.cpp"),
            Object(Matching, "game/ground.cpp"),
            Object(Matching, "game/gendrawlist.cpp"),
            Object(NonMatching, "game/stub17.cpp"),
            Object(Matching, "game/camproj.cpp"),
            Object(Matching, "game/carinfo.cpp"),
            Object(NonMatching, "game/animobj.cpp"),
            Object(Matching, "game/fwd1ea4b4.cpp"),
            Object(Matching, "game/event.cpp"),
            Object(Matching, "game/trackgrf.cpp"),
            Object(NonMatching, "game/terchunk.cpp"),
            Object(NonMatching, "game/terrpick.cpp"),
            Object(NonMatching, "game/tunedash.cpp"),
            Object(NonMatching, "game/difficulty.cpp"),
            Object(NonMatching, "game/sound.cpp"),
            Object(NonMatching, "game/soundmgr.cpp"),
            Object(NonMatching, "game/mixer.cpp"),
            Object(NonMatching, "game/engsnd.cpp"),
            Object(Matching, "game/tiresnd.cpp"),
            Object(NonMatching, "game/musicplr.cpp"),
            Object(NonMatching, "game/sounddash.cpp"),
            Object(NonMatching, "game/menumus.cpp"),
            Object(NonMatching, "game/roadsnd.cpp"),
            Object(Matching, "game/strlist.cpp"),
            Object(NonMatching, "game/autolyt.cpp"),
            Object(Matching, "game/uictx.cpp"),
            Object(Matching, "game/gtime.cpp"),
            Object(Matching, "game/uitext.cpp"),
            Object(NonMatching, "game/stub19.cpp"),
            Object(Matching, "game/stub10.cpp"),
            Object(Matching, "game/mmain.cpp"),
            Object(Matching, "game/gamedo.cpp"),
            Object(NonMatching, "game/setmenu.cpp"),
            Object(NonMatching, "game/nameent.cpp"),
            Object(NonMatching, "game/loadscrn.cpp"),
            Object(NonMatching, "game/unlkmenu.cpp"),
            Object(NonMatching, "game/mstats.cpp"),
            Object(NonMatching, "game/tunemenu.cpp"),
            Object(Matching, "game/tutmenu.cpp"),
            Object(Matching, "game/uiobjs.cpp"),
            Object(Matching, "game/stub16.cpp"),
            Object(Matching, "game/sortwrap.cpp"),
            Object(NonMatching, "game/jumpcam.cpp"),
            Object(NonMatching, "game/camframe.cpp"),
            Object(Matching, "game/cscript.cpp"),
            Object(NonMatching, "game/stub18.cpp"),
            Object(NonMatching, "game/carlayout.cpp"),
            Object(Matching, "game/lightset.cpp"),
            Object(Matching, "game/cartype.cpp"),
            Object(Matching, "game/camdefs.cpp"),
            Object(Matching, "game/carstate.cpp"),
            Object(NonMatching, "game/carsetup.cpp"),
            Object(NonMatching, "game/carrival.cpp"),
            Object(Matching, "game/carreg.cpp"),
            Object(Matching, "game/stub12.cpp"),
            Object(NonMatching, "game/fxseq.cpp"),
            Object(NonMatching, "game/shadowvol.cpp"),
            Object(Matching, "game/drawline.cpp"),
            Object(NonMatching, "game/fence.cpp"),
            Object(Matching, "game/stub13.cpp"),
            Object(NonMatching, "game/cartags.cpp"),
            Object(Matching, "game/trkefx.cpp"),
            Object(Matching, "game/simpletga.cpp"),
            Object(NonMatching, "game/umat.cpp"),
            Object(NonMatching, "game/lweight.cpp"),
            Object(Matching, "game/stub14.cpp"),
            Object(NonMatching, "game/faninfo.cpp"),
            Object(NonMatching, "game/sky.cpp"),
            Object(NonMatching, "game/boats.cpp"),
            Object(NonMatching, "game/swlmap.cpp"),
            Object(Matching, "game/preload.cpp"),
            Object(NonMatching, "game/vidsetup.cpp"),
            Object(Matching, "game/image.cpp"),
            Object(NonMatching, "game/mrender.cpp"),
            Object(Matching, "game/vidmode.cpp"),
            Object(NonMatching, "game/modelpool.cpp"),
            Object(NonMatching, "game/billboard.cpp"),
            Object(Matching, "game/discerr.cpp"),
            Object(NonMatching, "game/romfont.cpp"),
            Object(Matching, "game/stub15.cpp"),
            Object(Matching, "game/log.cpp"),
            Object(Matching, "game/file.cpp"),
            Object(Matching, "game/memarena.cpp"),
            Object(NonMatching, "game/filecd.cpp"),
            Object(Matching, "game/keybd.cpp"),
            Object(Matching, "game/vmem.cpp"),
            Object(NonMatching, "game/mcsys.cpp"),
            Object(Matching, "game/strmfile.cpp"),
            Object(Matching, "game/strmbank.cpp"),
            Object(Matching, "game/callhook.cpp"),
            Object(Matching, "game/sysconfig.cpp"),
            Object(Matching, "game/movieinfo.cpp"),
            Object(NonMatching, "game/rigid.cpp"),
            Object(NonMatching, "game/ball.cpp"),
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
