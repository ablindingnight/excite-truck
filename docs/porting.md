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
