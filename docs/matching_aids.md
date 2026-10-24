# Matching aids to revisit

Some functions match only with compiler tricks the original developers did not write: `#pragma`
switches (`scheduling invalid_target`, `opt_propagation off`, ...), `register` pinning, `volatile`
locals, or odd temporaries. They are fine for byte-matching but should be replaced by natural source
when we find the real shape (also better for the PC port).

Each one carries a `MATCHING-AID:` comment. List them with:

    git grep -n "MATCHING-AID"

| Function | File | Aid | Source |
|---|---|---|---|
| fn_8018A2C0 (physics tuning sliders) | src/game/carbody.cpp | register pinning, scheduling + opt_propagation pragmas | upstream orangedude27/excite-truck |
| fn_80198488 (pick waiting car) | src/game/aicar.cpp | volatile reads of the two counts | upstream orangedude27/excite-truck |
| Shuffle<T> (fn_802B2504, fn_802B28BC) | src/game/objplace.cpp | byte-offset counter divided back to an index | upstream orangedude27/excite-truck |
| Paint::Add (Add__5PaintFP4TVecP4TVecf) | src/game/trail.cpp | position copied through x/y/z temporaries in a fixed order | stmtmove sweep 2026-10-24 |
| fn_801B3E24 (collision sound play) | src/game/collisionsound.cpp | volatile zero vector forces stack stores | Codex (tagged in COLLISIONSOUND-23) |
| UI API unit (9 sites: dont_inline/scheduling pragmas, 1 register) | src/game/uiapi.cpp | imported raw from upstream; whole file needs a typed rewrite | upstream orangedude27/excite-truck |
| whole unit (3 tagged sites) | src/game/unk_802E4D84.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (8 tagged sites) | src/game/unk_8020A6AC.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (6 tagged sites) | src/game/unk_8020DE74.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (6 tagged sites) | src/game/unk_8020F3B4.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (3 tagged sites) | src/game/unk_8020CF68.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (1 tagged sites) | src/game/unk_801274C0.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (1 tagged sites) | src/game/unk_801BD620.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (1 tagged sites) | src/game/unk_8020BCB8.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (1 tagged sites) | src/game/unk_8020F3AC.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (1 tagged sites) | src/game/unk_8026E224.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (1 tagged sites) | src/game/unk_802E6B48.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (2 tagged sites) | src/game/unk_80215E68.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (2 tagged sites) | src/game/unk_80243460.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (3 tagged sites) | src/game/unk_80262F8C.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (1 tagged sites) | src/game/unk_8026B844.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (43 tagged sites) | src/game/unk_80116BD4.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (1 tagged sites) | src/game/unk_800241DC.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (1 tagged sites) | src/game/unk_80116644.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (1 tagged sites) | src/game/unk_80107180.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (1 tagged sites) | src/game/unk_8002B0A0.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| whole unit (1 tagged sites) | src/game/unk_8002B0B0.c | upstream pragmas/register/volatile; typed rewrite pending | upstream orangedude27/excite-truck |
| fn_80236848 (pop anim state) | src/game/uianime.cpp | address of a local taken to steer load order | NucBox permuter |

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
| src/game/widgets.cpp | 1398 | register |
| src/game/zone.cpp | 2341 | pragma |
| src/game/zone.cpp | 2495 | pragma |
