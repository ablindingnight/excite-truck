#ifndef GAME_STRCOPY_H
#define GAME_STRCOPY_H

// The game's bounded string copy: copies src into dst up to and including the terminator, but writes at most n
// bytes; when the limit is hit the last byte written becomes the terminator. Retail has one out-of-line copy (vmem's
// CopyStringWithLimit(char* dst, const char* src, int n), called from res) and expands it inline in about 40 units; those
// units used to carry ~40 private copies under invented names (StrCopyN, CopyName, StrNCopy, ...).
// Helper name unproven. The local copies of dst and n are retail evidence (aids audit 2, 2026-11-16): with them the
// shared inline is code-identical in 43 of the 47 former call sites; without them in none.
inline void SafeStrCopy(char* dst, const char* src, int n) {
    char* d = dst;
    int left = n;
    while ((*d++ = *src++) != 0) {
        if (--left <= 0) {
            d[-1] = 0;
            return;
        }
    }
}

#endif
