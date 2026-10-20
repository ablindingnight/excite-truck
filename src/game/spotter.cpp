// Spotter phrases (0x801B664C-0x801B6670 and the sinit): the radio spotter's call table (sound
// files, subtitle, repeat delay), a leftover of the stock-car game this engine came from. The code
// that used it was stripped, leaving empty functions. Names are guesses apart from the strings.
#include "game/common.h"

extern "C" {
void* memset(void* dst, int c, size_t n);
void fn_802E317C(const char* fmt, ...);  // fatal: print, then exit() -> PPCHalt (never returns)
}

struct Phrase {
    Phrase(void) {
        memset(this, 0, sizeof(*this));
        id = 0;
    }
    Phrase(int id, const char* file, const char* text, f32 delay) {
        this->id = id;
        this->file = file;
        this->text = text;
        last = 0;
        count = 0;
        this->delay = delay;
        range = 1000.0f;
    }
    s32 id;            // offset 0x0
    const char* file;  // offset 0x4
    const char* text;  // offset 0x8
    f32 delay;         // offset 0xC
    s32 last;          // offset 0x10
    s32 count;         // offset 0x14
    f32 range;         // offset 0x18
};

ModuleTag lbl_8055E728;

// Never called, so the linker strips these; their message and constants are left behind in the
// string and float pools (the pool order needs them before the table).
void SpotterCheckCount(int n) {
    if (n >= 129) {
        fn_802E317C("spotter: too many phrases");
    }
}

int SpotterActive(f32 t) { return t > 0.0f; }

f32 SpotterRepeatTime(void) { return 2.5f; }

f32 SpotterScale(f32 t) { return 2.0f * t; }

f32 SpotterDefaultTime(void) { return 12.0f; }

Phrase lbl_80435EE8[129] = {
    Phrase(1, "carlo.sfx carleft.sfx", "Car low", 3.0f),
    Phrase(2, "carhi.sfx carrt.sfx", "Car high", 3.0f),
    Phrase(3, "3wide.sfx", "*Three wide", 1.5f),
    Phrase(4, "clrlo.sfx clrleft.sfx", "Clear low", 2.0f),
    Phrase(5, "clrhi.sfx clrrt.sfx", "Clear high", 2.0f),
    Phrase(6, "clrclr.sfx", "Clear", 1.2f),
    Phrase(7, "clrall.sfx", "All clear", 2.5f),
    Phrase(10, "stilther.sfx", "Still there", 0.0f),
    Phrase(11, "staylo.sfx stayleft.sfx", "Stay low", 0.0f),
    Phrase(12, "stayhi.sfx stayrt.sfx", "Stay high", 0.0f),
    Phrase(13, "holdline.sfx", "Hold your line", 0.0f),
    Phrase(14, "watchit.sfx", "*Watch it!", 0.0f),
    Phrase(8, "xcarlo.sfx", "* is low", 0.0f),
    Phrase(9, "xcarhi.sfx", "* is high", 0.0f),
    Phrase(15, "grnflag.sfx", "Green flag!", 0.0f),
    Phrase(16, "yelflag.sfx", "Caution flag!", 0.0f),
    Phrase(18, "clrpitln.sfx", "Clear of pitlane", 0.0f),
    Phrase(19, "wrongpit.sfx", "*Wrong pitlane", 3e+01f),
    Phrase(20, "slowdown.sfx", "*Slow down!", 0.0f),
    Phrase(21, "lowfuel.sfx", "You're low on fuel!", 35.0f),
    Phrase(22, "wtchtirs.sfx", "Watch your tires!", 4e+01f),
    Phrase(25, "tireblwn.sfx", "You blew a tire!", 1e+01f),
    Phrase(23, "blewengn.sfx", "You've blown your engine!", 0.0f),
    Phrase(24, "rdtrdmg.sfx", "Radiator damaged", 0.0f),
    Phrase(26, "brkbrake.sfx", "Brake damage", 0.0f),
    Phrase(27, "brkcltch.sfx", "Clutch failure", 0.0f),
    Phrase(28, "brkradtr.sfx", "Debris on grille", 0.0f),
    Phrase(29, "brksteer.sfx", "Power steering failure", 0.0f),
    Phrase(30, "recrdlap.sfx", "That was the best lap ever!", 0.0f),
    Phrase(31, "bestlap.sfx", "That was your best lap!", 0.0f),
    Phrase(32, "lastlap.sfx", "Last lap!", 0.0f),
    Phrase(33, "2togo.sfx", "Two to go!", 0.0f),
    Phrase(34, "5togo.sfx", "Five to go!", 0.0f),
    Phrase(35, "10togo.sfx", "Ten to go!", 0.0f),
    Phrase(36, "20togo.sfx", "Twenty to go!", 0.0f),
    Phrase(37, "halfway.sfx", "Half way!", 0.0f),
    Phrase(38, "won.sfx", "You won", 0.0f),
    Phrase(39, "$top5.sfx", "Top five", 0.0f),
    Phrase(40, "$top10.sfx", "Top ten", 0.0f),
    Phrase(41, "$top20.sfx", "Top twenty", 0.0f),
    Phrase(42, "top43.sfx", "Maybe next time", 0.0f),
    Phrase(44, "quallap.sfx", "This is the one that counts", 0.0f),
    Phrase(43, "donequal.sfx", "Let's see where we place on the grid!", 0.0f),
    Phrase(45, NULL, "", 0.0f),
    Phrase(46, NULL, "", 0.0f),
    Phrase(47, "pitcar.sfx", "Pitting car ahead!", 2e+01f),
    Phrase(49, "xpitcar.sfx", " is pitting!", 2e+01f),
    Phrase(48, "ldrpitng.sfx", "Leader is pitting", 2e+01f),
    Phrase(50, NULL, "", 0.0f),
    Phrase(51, NULL, "", 0.0f),
    Phrase(52, NULL, "", 0.0f),
    Phrase(53, "blackflg.sfx", "", 0.0f),
    Phrase(54, "wrongway.sfx", "Wrong way", 1e+02f),
    Phrase(55, "gogo.sfx", "Go, go", 0.0f),
    Phrase(56, "getset.sfx", "*Get set", 1e+01f),
    Phrase(57, "getready.sfx", "*Get ready", 1e+01f),
    Phrase(58, NULL, "", 0.0f),
    Phrase(59, "pcleave.sfx", "Pacecar leaving track", 0.0f),
    Phrase(60, "pcout.sfx", "Pacecar's out", 0.0f),
    Phrase(61, "xhascrsh.sfx", " has crashed", 1e+01f),
    Phrase(63, "aspin.sfx", "There's a spin ", 12.0f),
    Phrase(62, "awreck.sfx", "There's a wreck ", 12.0f),
    Phrase(114, "turn1.sfx", " in turn 1", 0.0f),
    Phrase(115, "turn2.sfx", " in turn 1", 0.0f),
    Phrase(116, "turn3.sfx", " in turn 1", 0.0f),
    Phrase(117, "turn4.sfx", " in turn 1", 0.0f),
    Phrase(64, "prefuz1.sfx", "*[Pre-spotter fuzz 1]", 0.0f),
    Phrase(65, "prefuz2.sfx", "*[Pre-spotter fuzz 2]", 0.0f),
    Phrase(66, "prefuz3.sfx", "*[Pre-spotter fuzz 3]", 0.0f),
    Phrase(67, "postfuz1.sfx", "*[Post-spotter fuzz 1]", 0.0f),
    Phrase(68, "postfuz2.sfx", "*[Post-spotter fuzz 2]", 0.0f),
    Phrase(69, "postfuz3.sfx", "*[Post-spotter fuzz 3]", 0.0f),
    Phrase(0, NULL, NULL, 0.0f),
};

extern "C" {

void fn_801B664C(void) {}

void fn_801B6650(void) {}

void fn_801B6654(void) {}

void fn_801B6658(void) {}

void fn_801B665C(void) {}

void fn_801B6660(void) {}

void fn_801B6664(void) {}

int fn_801B6668(void) { return 0; }

}  // extern "C"
