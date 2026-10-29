# Matching aids to revisit

Some functions match only with compiler tricks the original developers did not write: `#pragma`
switches (`scheduling invalid_target`, `opt_propagation off`, ...), `register` pinning, `volatile`
locals, or odd temporaries. They are fine for byte-matching but should be replaced by natural source
when we find the real shape (also better for the PC port).

Each one carries a `MATCHING-AID:` comment. List them with:

    git grep -n "MATCHING-AID"

| Function | File | Aid | Source |
|---|---|---|---|
| fn_8018A2C0 (physics tuning sliders) | src/game/carbody.cpp | register pinning, scheduling + opt_propagation pragmas (natural form known: literals + unit-string locals; apply once the unit's earlier strings exist) | upstream orangedude27/excite-truck |
| ut::Color::~Color weak copy (end of db_console) | libs/msc/src/RVL_SDK/hbm/nw4hbm/db/db_console.cpp | unused ut::Color local in Console_GetTotalLines | TP decomp approach, 2026-10-25 |
| fn_8011A388 (Bink stream read) | src/game/unk_80116BD4.c | volatile reads of position 0x90 and cap 0x40 (probably genuine async stream state, like 0xC0 in fn_8011A45C) | Claude, 2026-10-26 |
| Shuffle<T> (fn_802B2504, fn_802B28BC) | src/game/objplace.cpp | byte-offset counter divided back to an index | upstream orangedude27/excite-truck |
| Gate::operator= (__as__4GateFRC4Gate) | src/game/zone.cpp | end points copied as one GateSeg block; the original likely grouped a/b in a segment struct | Claude 2026-10-24 |
| MultiEvtTestingCheck (stripped by the linker) | src/game/multievt.cpp | reconstructed body for the orphan "Testing mode only allowed from Single Race path" literal so the .data pool keeps the original offsets (unlocks fn_8024A6EC / fn_8024ACB4) | Claude, 2026-10-27 |
| GXCheckEnvMap, GXScreenshotNumbered, GXPrintRow (stripped by the linker) | src/game/gx.cpp | reconstructed bodies for orphan pool literals ("dr: custom envmap ...", "tmp_screenshot%02d.tga", " %5.2f ..."), so later literals keep their offsets (fn_802D09E8 exact) | Claude, 2026-10-27 |
| sSurfTex (unused static) | src/game/water.cpp | keeps the orphan "gen_surf.tex" literal at its .data pool slot (fn_802BEDFC exact) | Claude, 2026-10-27 |
| ContestDescribe (stripped by the linker) | src/game/eventpath.cpp | reconstructed contest twin of fn_8027CBF8 for the orphan "EventPath:GOAL_GRADE_%s" / "EventPath:LOCKED_CHALLENGE" literals (pool order now matches) | Claude, 2026-10-27 |
| fn_8025AA90, fn_8025AE18 (achview) | src/game/achview.cpp | explicit calls to `StatsUpdate__Fv(v)` standing in for vector default constructors. The target function at 0x8001BA68 (labelled StatsUpdate) is an empty out-of-line default constructor of a shared vector type, called on fresh stack vectors in achview, panelvol, groundvis, ppsys, masslist, xlobj, trackmod, boneset and aiobst. Natural fix: one shared vector type whose ctor is declared (not defined) and the symbol renamed to its mangled name | found 2026-10-27 |
| fn_801B3E24 (collision sound play) | src/game/collisionsound.cpp | volatile zero vector forces stack stores | Codex (tagged in COLLISIONSOUND-23) |
| ~LinearDroneTMH, ~HermiteDroneTMH (drone), ~XFaderTMH (trackmod), ~AnimDesc (trackobj) | src/game/drone.cpp, trackmod.cpp, trackobj.cpp | empty inline "teardown" call in an otherwise empty dtor body. The target keeps the dtor's own vtable store, which MWCC drops for a truly empty body but keeps once the body had any (later inlined-away) call; the original was probably a compiled-out debug call (same shape as smoke SmokeNop / panelvol DbgLine). Every dtor with that store in the game (actseq, opsound, roadsnd, stunts, tiresnd) has a real body | found 2026-10-28 |
| fn_802682B0 (cammgr) | src/game/cammgr.cpp | `m[row = 0][*rowp]` (permuter) makes the first store an unfoldable index, which yields the target's `stfsu` to the global matrix. Same store-update shape still open in cammgr fn_80267680 and mrender fn_802C83CC; struct copies and pointer-walking loops produce it naturally elsewhere (camfov fn_80270484), but loop/pointer forms are 5 rows off here | tagged 2026-10-28 |
| UI API unit (9 sites: dont_inline/scheduling pragmas, 1 register) | src/game/uiapi.cpp | imported raw from upstream; whole file needs a typed rewrite | upstream orangedude27/excite-truck |
| Spu sound layer (3 tagged sites) | src/game/unk_802E4D84.c | 3 `msg` base pointers into the message pool (fn_802E5478, fn_802E5AE8, fn_802E6264) standing in for pooled string literals. The pool also holds strings of linker-stripped functions, so literals can't reproduce its offsets until the unit owns its .data | upstream orangedude27/excite-truck; Claude 2026-10-24 (its scheduling/dont_inline pragmas were no-ops, removed 2026-10-27) |
| whole unit (8 tagged sites) | src/game/unk_8020A6AC.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (6 tagged sites) | src/game/unk_8020DE74.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (6 tagged sites) | src/game/unk_8020F3B4.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (3 tagged sites) | src/game/unk_8020CF68.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (1 tagged sites) | src/game/unk_8020BCB8.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (1 tagged sites) | src/game/unk_8020F3AC.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (2 tagged sites) | src/game/unk_80215E68.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (1 site) | src/game/unk_80243460.c | `#pragma cplusplus on` so the C file can make two virtual calls; really a C++ menu item (typed rewrite pending). The dont_inline and use_lmw_stmw pragmas were no-ops and are gone | upstream orangedude27/excite-truck |
| whole unit | src/game/unk_80262F8C.c | none left: the volatile local was the stack-passed 10th argument of fn_8022BAD0 and the scheduling pragma is gone (DecideItem::Draw now 2 rows off instead of 16); typed C++ rewrite pending | upstream orangedude27/excite-truck; Claude 2026-10-27 |
| whole unit | src/game/unk_80116BD4.c | volatile stream/DVD state only (likely genuine async fields); its unroll / opt_unroll_loops / use_lmw_stmw pragmas produced identical code for all 154 functions and were removed 2026-10-27; typed rewrite pending | upstream orangedude27/excite-truck |

These upstream files had only the generic "Tricks are tagged MATCHING-AID" header and no trick construct
(checked 2026-10-27): unk_80127398, unk_801BD620, unk_8026E224, unk_802E6B48, unk_8026B844, unk_800241DC,
unk_80116644, unk_80107180, unk_8002B0A0, unk_8002B0B0. They still need typed rewrites, but nothing to undo.

## Evidence-based reclassifications

- Widget selection draw (`fn_80223280`): its unused screen conversions are retained as a natural reconstruction of conversion setup/stores present in the original, consistently with gauge span (`fn_8021CB4C`) and choice height (`fn_8021DB58`). Plain locals, discarded casts and const locals reproduce the same code; integer-only controls remove the observed work. The exact original spelling and purpose remain unknown. Reviewed by Claude and Codex, 2026-10-29; see `claude_reply_widgets391_owner_contracts_and_unused_conversions_2026-10-29.md`. This is a classification correction, not new code or a new match.

## Replaced by natural source

| Function | Aid that was used | Natural form | When |
|---|---|---|---|
| fn_80198488 (aicar, pick waiting car) | volatile reads of the two counts | SideHit struct (valid + Hit + best) with a const `Car()` accessor; a const method's read is not CSE'd with the earlier plain read | Claude 2026-10-27 |
| CloseCallTracker::Rate (stunts) | const f32& bound to 10.0f | `f32 turboLimit = 10.0f;` local | Claude 2026-10-27 |
| fn_8025AD3C (achview lerp) | two single-use helpers | `SetVec3(out, x, y, z)` inline: arguments evaluate right to left | Claude 2026-10-27 |
| Paint::Add (trail) | copy through x/y/z temporaries | `const TVec*` params + `m->pos = *pos;` (symbol renamed to Add__5PaintFPC4TVecPC4TVecf) | Claude 2026-10-27 |
| fn_80236848 (uianime pop) | address of a local | compare the head global directly; load the walk pointer in the else | Claude 2026-10-27 |

## Looked at, kept (likely original or no natural form yet), 2026-10-27

- volatile sweep (each removed in turn, every function compared): bonemgr size/used, cammgr id[12], physics state
  globals, sdmp3 ring fields, crushmaster/ringchal `state` all change code without it (needed / likely genuine
  shared state). camproj's `const volatile` 61.5f: defining the constant after fn_801EA1FC matches the function
  without volatile but reorders .sdata2 (61.5 must precede the DegToRad literal), so it stays.
- eventpath FixedArray::RemoveAt `volatile s32&` replaced by a const `Count()` accessor (same code, 2026-10-27);
  sdmp3 fn_802EAB28's `volatile u8 decoded` was wrong (removing it improves the function) and is gone.

- zone ZoneMaster::vf0C `#pragma dont_inline`: in the target nothing at all is inlined into vf0C, while every other
  function (and the weak copies emitted after it) inlines the same helpers; inline-size pragmas don't reproduce it.
  Most likely an original per-function pragma.
- stunts fn_801572A4 / betarace `#pragma inline_depth(1)`: the factory inlines exactly one level (TrackerArray ctor,
  Add/Get/Num) and calls everything below; also likely original.
- aiobst fn_8015FBCC region and betarace BetaCar::Update `#pragma dont_inline`: both change the weak copies and
  calls (35 / 37 functions differ without them), the same pattern as zone vf0C. betarace's second
  `inline_depth(1)` (before DrawResults) was redundant with the first and was removed.
- driver PickVehicleColor `#pragma scheduling 604`: without it, one store/compare swap (2 rows); 8 natural spellings
  tried, none move it.
- objplace Shuffle<T>: natural loops give a pointer induction variable (6 rows); the target keeps a byte offset.
  16 loop shapes tried.

## Existing sites (inventory 2026-10-23, before tagging)

Found by scanning `src/game/*.cpp` for `register`, `volatile` and optimiser pragmas
(`scheduling`, `opt_*`, `dont_inline`, `inline_depth`, ...). Not all are fakes: `volatile` on hardware
registers or shared/interrupt data (gx, sdmp3, kernel) is likely original. Each needs a look; tag the
real aids with `MATCHING-AID:` as they are reviewed. Data-placement pragmas (`section`, `force_active`,
`pack`) are a separate, mostly legitimate category and are not listed.

| File | Line | Kind |
|---|---|---|
| src/game/aiobst.cpp | 627 | pragma |
| src/game/aiobst.cpp | 748 | pragma |
| src/game/betarace.cpp | 1763 | pragma |
| src/game/betarace.cpp | 2029 | pragma |
| src/game/betarace.cpp | 2150 | pragma |
| src/game/betarace.cpp | 2265 | pragma |
| src/game/betarace.cpp | 2275 | pragma |
| src/game/bonemgr.cpp | 48 | volatile |
| src/game/bonemgr.cpp | 49 | volatile |
| src/game/cammgr.cpp | 874 | volatile |
| src/game/camproj.cpp | 30 | volatile |
| src/game/carbody.cpp | 499 | pragma |
| src/game/carbody.cpp | 500 | pragma |
| src/game/carbody.cpp | 502 | register |
| src/game/carbody.cpp | 503 | register |
| src/game/carbody.cpp | 504 | register |
| src/game/carbody.cpp | 505 | register |
| src/game/carbody.cpp | 506 | register |
| src/game/carbody.cpp | 507 | register |
| src/game/carbody.cpp | 508 | register |
| src/game/carbody.cpp | 509 | register |
| src/game/carbody.cpp | 510 | register |
| src/game/carbody.cpp | 511 | register |
| src/game/collisionsound.cpp | 160 | volatile |
| src/game/credits.cpp | 673 | volatile |
| src/game/crushmaster.cpp | 416 | volatile |
| src/game/driver.cpp | 613 | pragma |
| src/game/driver.cpp | 630 | pragma |
| src/game/drsurf.cpp | 79 | volatile |
| src/game/drsurf.cpp | 1114 | register |
| src/game/eventpath.cpp | 83 | volatile |
| src/game/glare.cpp | 61 | volatile |
| src/game/gndtex.cpp | 94 | volatile |
| src/game/gx.cpp | 244 | volatile |
| src/game/ident.cpp | 166 | register |
| src/game/kernel.cpp | 83 | volatile |
| src/game/log.cpp | 47 | volatile |
| src/game/musicplr.cpp | 91 | volatile |
| src/game/physics.cpp | 138 | volatile |
| src/game/physics.cpp | 141 | volatile |
| src/game/physics.cpp | 143 | volatile |
| src/game/physics.cpp | 662 | register |
| src/game/ringchal.cpp | 606 | volatile |
| src/game/sdmp3.cpp | 127 | volatile |
| src/game/sdmp3.cpp | 128 | volatile |
| src/game/sdmp3.cpp | 147 | volatile |
| src/game/sdmp3.cpp | 155 | volatile |
| src/game/sdmp3.cpp | 171 | volatile |
| src/game/sdmp3.cpp | 172 | volatile |
| src/game/sdmp3.cpp | 174 | volatile |
| src/game/sdmp3.cpp | 178 | volatile |
| src/game/sdmp3.cpp | 179 | volatile |
| src/game/sdmp3.cpp | 542 | volatile |
| src/game/stunts.cpp | 2194 | pragma |
| src/game/stunts.cpp | 2219 | pragma |
| src/game/zone.cpp | 2341 | pragma |
| src/game/zone.cpp | 2495 | pragma |
| fn_802200AC (widgets gauge draw) | src/game/widgets.cpp | zero-input half getter (four uses); unproved original boundary, controls FP operand/CSE choices; plain literal/static/local constants and Mul/Half/centering forms regress (all-/2.0f form leaves one operand-order word at +0x16c) | unresolved; Codex/Claude source-policy review 2026-10-29 |
| fn_80221544, fn_80221970 (widgets scroll) | src/game/widgets.cpp | one-int ScrollLocalWord/ScrollEdgeWord local carriers; unproved original types; plain-value removals change both functions | unresolved; Codex/Claude source-policy review 2026-10-29 |
