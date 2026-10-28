## Excite Truck (Wii) Decompilation Project
This is a work-in-progress matching decompilation of the Nintendo Wii game Excite Truck (USA, `REXE01`).

This project is for educational and research purposes only. It contains no game assets, executables or
disc images; you need your own legally obtained copy of the game to build it.

### About this fork

This is an independent fork of [orangedude27/excite-truck](https://github.com/orangedude27/excite-truck),
shared in case the work here is useful to the original project. It is not affiliated with, or endorsed by,
the original author.

- The `runtime-decomp` branch started from orangedude27's project setup (the dtk-template configuration,
  his first commits up to June 2025) and was developed separately from there. Its history is **not** based
  on the original repository's current `main`, which has continued independently since; nothing here
  replaces or rewrites his work, and his commits keep their original authorship.
- Timeline: when this work began on 2026-10-15, the newest commit in our copy of the original repository was
  from 2025-06-10, so the project appeared inactive and this branch was started from that state. On
  2026-10-23 we noticed the original repository had resumed active development (its newer commits are dated
  from August 2026 on) and have credited everything taken from it since.
- Some units and symbol names were later imported from the original repository's newer `main` (CC0). Those
  commits say "imported from upstream (orangedude27/excite-truck, CC0)" or "via upstream" / "with upstream".
- Progress is measured with objdiff (`build/REXE01/report.json`, exact matches only). Every commit on this
  branch rebuilds the game and verifies the DOL's SHA-1 against the original before it is accepted.
- Contact: Discord `ablindingnight`.

### Progress

Exact matches only (objdiff `report.json`), as of 2026-10-28:

| | Exactly matched code | Functions |
| :--- | :--- | :--- |
| **Whole game** | 65.0% (1,982,748 / 3,050,236 bytes) | 12,314 / 13,779 (89.4%) |
| Excite Truck's own code | 49.7% (1,009,380 / 2,030,956 bytes) | 8,708 / 10,141 |
| Nintendo SDK libraries | 96.9% (973,368 / 1,004,796 bytes) | 3,606 / 3,632 |

The SDK row now also counts SDK code that has no source file yet (it used to count only built SDK files),
so its total grew and the percentage is lower even though more SDK code matches.

### Goals

- A 100% matching decompilation: every function compiles back to the original bytes.
- Code that reads like the original source: real structures and names, natural control flow, and no matching
  "tricks" (no register pinning, no `#pragma` tweaks, no `volatile` or fake temporaries added only to steer the
  compiler). `volatile` is used only where the data really is shared with callbacks or interrupts.
- That goal is not reached yet. The tricks still in the code (about 100, most of them inherited with imported
  code) are marked `// MATCHING-AID:` and listed in [docs/matching_aids.md](docs/matching_aids.md), and are
  being replaced by natural code over time.
- Logic bugs found while matching are fixed rather than worked around: a function only counts once it does
  what the original does.

### Credits

Code adapted from other projects (all released under CC0):

- [orangedude27/excite-truck](https://github.com/orangedude27/excite-truck): the original project and
  setup this fork builds on, plus units and symbol names imported from its newer work (see above).
- [ogws](https://github.com/kiwi515/ogws), the Wii Sports decompilation: Revolution SDK sources and headers
  (`include/revolution`, `include/MSL`, `include/decomp`, `src/revolution`). Excite Truck ships SDK builds
  from the same 2006 release.
- [zeldaret/tp](https://github.com/zeldaret/tp), the Twilight Princess decompilation: sources under `libs/`
  (Revolution SDK, MSL and runtime headers). Twilight Princess for Wii (RZDE01 rev 0) ships the identical
  September 2006 SDK build.
- [mscharged-decomp](https://github.com/yannicksuter/mscharged-decomp), the Mario Strikers Charged
  decompilation: the MSL C library and Home Button Menu (nw4hbm) sources and headers under `libs/msc/`.
- [wii-ipl](https://github.com/koopthekoopa/wii-ipl), the Wii System Menu decompilation: OSSemaphore and parts
  of the VF / PrFILE2 file-system layer, adapted to Excite Truck's older versions.

Consulted for comparison only (no code copied): [Garigari](https://github.com/SMGCommunity/Garigari),
[Rhae](https://github.com/akiramusic000/Rhae), [bba-wd](https://github.com/vabold/bba-wd),
[pbr-dtk](https://github.com/bgsamm/pbr-dtk), [spm-decomp](https://github.com/SeekyCt/spm-decomp),
[wii-news-channel](https://github.com/hotlandsoftware/wii-news-channel),
[ForecastChannel](https://github.com/WiiLink24/ForecastChannel) and
[muscle-march-recomp](https://github.com/tmcleroy/muscle-march-recomp).

Tools: [dtk-template](https://github.com/encounter/dtk-template),
[decomp-toolkit](https://github.com/encounter/decomp-toolkit) and [objdiff](https://github.com/encounter/objdiff)
(build and diffing), [m2c](https://github.com/matt-kempster/m2c) and
[decomp-permuter](https://github.com/simonlindholm/decomp-permuter) (drafting and permuting functions; their
output was used as a starting point, their code is not included), and Ghidra.

Much of this fork's code was written with AI assistance (Anthropic's Claude, with contributions from
OpenAI's Codex); commits co-authored that way say so. Every change was verified against the original game
binary before being committed.
