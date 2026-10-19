// Movie info (0x802ECCE4-0x802ED050): a stack of opened movies (the current one on top) with a "missing"
// fallback, plus standalone movie handles. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, u32 n);
char* strrchr(const char* s, int c);
char* strncpy(char* dst, const char* src, u32 n);
int strcasecmp(const char* a, const char* b);
s32 fn_802E11A4(const char* name, int flag);  // open movie
void fn_802E1290(s32 h);                      // close movie
void fn_802E12DC(s32 h);
void fn_802E13D8(s32 h);
void fn_802E1718(s32 h);
}

void* operator new(size_t size, const char* tag);

struct MovieInfo {
    char name[0x40];  // offset 0x0
    s32 handle;       // offset 0x40
};

struct MovieStackEntry : MovieInfo {
    MovieStackEntry* next;  // offset 0x44
};

const char* lbl_8055D300 = "uisingle";
ModuleTag lbl_805600F0;
MovieStackEntry* lbl_805600F8;

extern "C" {

void fn_802ECCE4(char* dst, const char* src, int n) {
    const char* dot = strrchr(src, '.');
    if (dot) {
        int len = dot + 1 - src;
        if (len < n) {
            n = len;
        }
    }
    strncpy(dst, src, n);
    dst[n - 1] = 0;
}

int fn_802ECD68(MovieInfo* m, const char* name, int flag) {
    s32 h;
    memset(m, 0, sizeof(MovieInfo));
    fn_802ECCE4(m->name, name, 0x40);
    h = fn_802E11A4(m->name, flag);
    if (!h) {
        h = fn_802E11A4("missing", flag);
    }
    m->handle = h;
    return h != 0;
}

int fn_802ECDF4(void) {
    if (lbl_805600F8) {
        MovieStackEntry* next;
        fn_802E1290(lbl_805600F8->handle);
        next = lbl_805600F8->next;
        delete lbl_805600F8;
        lbl_805600F8 = next;
        return 1;
    }
    return 0;
}

int fn_802ECE48(const char* name) {
    MovieStackEntry* m = (MovieStackEntry*)operator new(sizeof(MovieStackEntry), "single_movie_info");
    if (fn_802ECD68(m, name, 1)) {
        m->next = lbl_805600F8;
        lbl_805600F8 = m;
        return 1;
    }
    delete m;
    return 0;
}

int fn_802ECEC0(void) { return fn_802ECDF4(); }

void fn_802ECEC4(void) {
    fn_802ECEC0();
    fn_802ECE48(lbl_8055D300);
}

int fn_802ECEEC(void) { return lbl_805600F8 != 0; }

int fn_802ECF00(const char* name) { return fn_802ECE48(name); }

int fn_802ECF04(void) { return fn_802ECDF4(); }

MovieInfo* fn_802ECF08(const char* name, int flag) {
    MovieInfo* m;
    if (fn_802ECEEC()) {
        return lbl_805600F8;
    }
    m = (MovieInfo*)operator new(sizeof(MovieInfo), "MoviePlayInfo");
    if (!fn_802ECD68(m, name, flag)) {
        delete m;
        m = 0;
    }
    return m;
}

int fn_802ECF90(const char* want, const char* name) {
    char buf[0x40];
    if (lbl_805600F8) {
        return 1;
    }
    fn_802ECCE4(buf, name, 0x40);
    return strcasecmp(buf, want) == 0;
}

void fn_802ECFEC(MovieInfo* m) { fn_802E12DC(m->handle); }

void fn_802ECFF4(MovieInfo* m) {
    if (!lbl_805600F8) {
        fn_802E1290(m->handle);
        delete m;
    }
}

void fn_802ED038(MovieInfo* m) { fn_802E13D8(m->handle); }

void fn_802ED040(MovieInfo* m) { fn_802E1718(m->handle); }

}  // extern "C"
