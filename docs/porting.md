# Porting plan

Once the decompilation reaches 100%, the plan is a **legal recompile / PC port** of Excite Truck. The port will contain code only. Players supply their own copy of the game, and an extractor pulls the assets from it.

A matching decompilation is necessary for this, but not sufficient. The work below makes the eventual port much easier, so prefer it during decomp work whenever a match allows.

## 1. Real types instead of raw offsets

Many NonMatching units use raw-offset helpers such as `F32(p, 0x1C)` and `PTR(p, 0x24)` to reach matches quickly. They compile identically, but they lock the code to the Wii's 32-bit memory layout. On a 64-bit target, pointers grow and every later offset shifts.

- Replace raw offsets with real `struct`/`class` definitions as layouts become known. Ghidra struct recovery and cross-references help here.
- Give recovered types and fields descriptive names. Keep an `// offset 0xNN` comment on each field.
- When a raw-offset form is still needed for a match, note the intended type nearby.

## 2. A shiftable build

NonMatching units still link the original assembly at fixed addresses. The goal is a build where functions and data can change size and everything still links, because every reference is a symbol.

- Split and name data sections (`.data`, `.rodata`, `.sdata`, `.sdata2`, `.bss`) for each unit, not just `.text`. Today several units are text-only splits.
- Settle data ownership for the open tails (controls 72 bytes, credits and profile 96 bytes each).
- Avoid hard-coded addresses in source; reference symbols.

## 3. A clean platform boundary

A port replaces the Wii SDK (GX graphics, AX/DSP audio, WPAD controllers, DVD, NAND/memory card, OS) with PC equivalents and keeps the game code.

- Keep SDK calls named and routed through their real APIs. Don't inline SDK internals into game units.
- Keep held SDK internals (e.g. the AX/SPU range) behind their interfaces.

## 4. Byte order and size assumptions

The Wii is big-endian with 32-bit pointers; typical PC targets are little-endian with 64-bit pointers.

- Mark code that reads raw data files or byte-swaps, e.g. the collision volume constructors in `sphcoll.cpp` that byte-swap fields after loading.
- Mark code that stores pointers in integer fields or assumes `sizeof(void*) == 4`.

## 5. Legal hygiene

- Never commit Nintendo binaries, disc images or extracted assets. `orig/` and extracted files stay local.
- The port reads assets from the player's own disc through an extractor tool.

## 6. Two routes to a PC build

- **Decomp port (our main route).** The finished decompilation is compiled natively for the PC, with the Wii SDK
  replaced by a host layer (section 7). The result is readable, moddable source.
- **Static recompilation (a possible earlier route).** A tool translates the game's machine code (PowerPC, from the
  player's DOL) into C automatically and compiles it with the same host layer. Hookshot
  (https://git.notrexed.dev/Hookshot/Hookshot, a Switch 2 game recompiled to Windows/Linux) and woohoo-flight-school
  (Pilotwings Resort, 3DS) work this way. It does not need 100% decomp, but the code is not human-readable.
  The two routes can share one host layer, so recompiled functions could be swapped for decompiled ones one at a time.

## 7. Distribution and architecture (modelled on Hookshot)

- **No game data in the repository.** No DOL, no assets, no keys. The player supplies their own Excite Truck disc
  (REXE01) dump. A builder program verifies the dump (hash of the expected version), extracts what it needs, and
  builds or installs locally. It never modifies the player's files.
- **Host layer** (replaces the Wii SDK, keeps the game code unchanged):
  - Graphics: GX calls recorded and replayed on a modern API (Vulkan or OpenGL), TEV stages turned into shaders.
  - Audio: AX/DSP voices mixed on the CPU and played through SDL; the game's own MP3 (TMCC) and Bink audio decoders
    are part of the decompiled code and run natively.
  - Video: RAD Bink 1 decoder is in the decompiled source (unk_80116BD4) and runs natively.
  - Input: WPAD (Wii Remote tilt steering, buttons) mapped to controllers through SDL - tilt from a gamepad stick or
    gyro, plus keyboard. Tilt steering is the central control, so its mapping needs real playtesting.
  - Files: DVD reads served from the extracted disc folder; NAND saves stored in a per-user folder.
- **Releases.** Nothing is published until the port is complete and tested end to end (all modes, saves, audio,
  video, controls). Licence to be decided before release (Hookshot uses GPL-3.0-or-later).
- **README shape** (follow Hookshot's): what it is, "no game files included", status table, install and play,
  supplying your dump, troubleshooting, controls/options, build from source, how it works, credits, licence.
