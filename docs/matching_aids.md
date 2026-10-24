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
