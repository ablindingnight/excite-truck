#ifndef GAME_UISTYLE_H
#define GAME_UISTYLE_H
// Input contract read by the native-matching style producer. Unknown fields remain unnamed.
struct UIStyleInputFields {
    const char* fontName; float height; unsigned long field08;
    unsigned long words0C[11];
    unsigned long field38,field3C,field40;
    const char* resource; unsigned long field48; float field4C,field50; unsigned long field54;
};
typedef char UIStyleInputSize88[sizeof(UIStyleInputFields)==0x58?1:-1];
#endif
