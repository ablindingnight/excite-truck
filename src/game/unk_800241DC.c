// Code 0x800241DC-0x800242CC, imported verbatim from the upstream project (github.com/orangedude27/excite-truck,
// CC0; file src/auto/text_800241DC_.c), where its functions match exactly. Unit name is provisional.
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.

int fn_800241DC(int arg0, int arg1, unsigned int arg2, unsigned int arg3, int (*arg4)()) {
    int *candidate;
    unsigned int low;
    unsigned int high;
    unsigned int mid;
    int result;
    if (arg0 == 0 || arg1 == 0 || arg2 == 0 || arg3 == 0 || arg4 == 0)
        return 0;
    result = arg4();
    if (result == 0)
        return arg1;
    if (result < 0)
        return 0;
    high = arg2 - 1;
    low = 1;
    while (low <= high) {
        mid = (low + high) >> 1;
        candidate = (int *)(arg1 + arg3 * mid);
        result = arg4(arg0, candidate);
        if (result == 0)
            return (int)candidate;
        if (result < 0)
            high = mid - 1;
        else
            low = mid + 1;
    }
    return 0;
}
