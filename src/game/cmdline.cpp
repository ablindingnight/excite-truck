// Command-line switches (0x80019898-0x80019BBC): each "-name" found sets a bit in the option word,
// and "-name:arg" passes arg to the option's handler. AppMain picks its mode from these bits.
// Names are guesses.
#include <string.h>
#include <ctype.h>
#include "game/common.h"

typedef void (*OptionHandler)(const char* arg);

struct Option {
    const char* name;
    u32 flag;
    OptionHandler handler;
};

void SetDirectToTrackName(const char* name);
void SetForceChassis(const char* arg);
void SetForcedRacers(const char* arg);
void SetDefaultColors(const char* arg);

extern "C" {
void fn_80203F0C(const char* arg);
void fn_8023BF68(const char* arg);
void fn_802BC328(const char* arg);
void fn_80131368(const char* arg);
}

ModuleTag lbl_8055D598;
u32 lbl_8055D5A0;

Option lbl_802FD528[] = {
    {"-direct_to_track", 0x1, SetDirectToTrackName},
    {"-model_test", 0x2, NULL},
    {"-pa_test", 0x4, NULL},
    {"-no_sound", 0x8, NULL},
    {"-no_music", 0x10, NULL},
    {"-autogen_envmap", 0x20, fn_80203F0C},
    {"-gx_test", 0x40, NULL},
    {"-profile", 0x80, NULL},
    {"-car_image", 0x100, SetForceChassis},
    {"-car_icon", 0x1000, SetForceChassis},
    {"-force_chassis", 0, SetForceChassis},
    {"-widescreen", 0x200, NULL},
    {"-e3_demo", 0x400, NULL},
    {"-players", 0, fn_8023BF68},
    {"-cars", 0, SetForcedRacers},
    {"-wmapgen", 0x800, fn_802BC328},
    {"-default_colors", 0, SetDefaultColors},
    {"-locale", 0, fn_80131368},
    {"", 0, NULL},
    {NULL, 0, NULL},
};

// Returns where `opt` starts a whitespace-separated token of `cmdline`, or NULL.
const char* FindOption(const char* cmdline, const char* opt) {
    const char* found = NULL;
    int len = strlen(opt);

    for (;;) {
        while (*cmdline != '\0' && isspace(*cmdline)) {
            cmdline++;
        }
        if (strncmp(cmdline, opt, len) == 0) {
            found = cmdline;
            break;
        }
        if (*cmdline == '\0') {
            break;
        }
        while (*cmdline != '\0' && !isspace(*cmdline)) {
            cmdline++;
        }
    }
    return found;
}

void ParseOptions(const char* cmdline) {
    Option* opt;
    const char* p;

    lbl_8055D5A0 = 0;
    for (opt = lbl_802FD528; opt->name != NULL; opt++) {
        p = FindOption(cmdline, opt->name);
        if (p == NULL) {
            continue;
        }
        lbl_8055D5A0 |= opt->flag;
        if (opt->handler == NULL) {
            continue;
        }
        {
            char arg[0x100] = {0};

            if (p[strlen(opt->name)] == ':') {
                const char* src = p + strlen(opt->name) + 1;
                int n = sizeof(arg);
                char* dst = arg;

                while ((*dst++ = *src++) != '\0') {
                    if (--n <= 0) {
                        dst[-1] = '\0';
                        break;
                    }
                }
                for (dst = arg; *dst != '\0' && !isspace(*dst); dst++) {
                }
                *dst = '\0';
            }
            opt->handler(arg);
        }
    }
    if (lbl_8055D5A0 & 0x24) {
        lbl_8055D5A0 |= 1;
    }
}

BOOL OptDirectToTrack(void) { return lbl_8055D5A0 & 1; }
BOOL OptModelTest(void) { return (lbl_8055D5A0 >> 1) & 1; }
BOOL OptPaTest(void) { return (lbl_8055D5A0 >> 2) & 1; }
BOOL OptNoSound(void) { return (lbl_8055D5A0 >> 3) & 1; }
BOOL OptGxTest(void) { return (lbl_8055D5A0 >> 6) & 1; }
BOOL OptProfile(void) { return (lbl_8055D5A0 >> 7) & 1; }
BOOL OptCarImage(void) { return (lbl_8055D5A0 >> 8) & 1; }
BOOL OptCarIcon(void) { return (lbl_8055D5A0 >> 12) & 1; }
BOOL OptWidescreen(void) { return (lbl_8055D5A0 >> 9) & 1; }
BOOL OptE3Demo(void) { return (lbl_8055D5A0 >> 10) & 1; }
