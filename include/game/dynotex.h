#ifndef GAME_DYNOTEX_H
#define GAME_DYNOTEX_H

// A list of named textures drawn by id (gart.cpp; "dynotex" is its allocation tag). Shared by gart, evpath, multievt and
// widgets, which each had their own view. Self-contained (built-in types only): widgets.cpp has its own u32 typedef.
// Field names are guesses.
class DynoTex {
public:
    struct Entry {
        char name[0x24];  // offset 0x0
        long tex;         // offset 0x24
        int id;           // offset 0x28
    };

    DynoTex(int max, int flip);
    virtual ~DynoTex(void);
    void Add(int id, const char* name);
    void Draw(int id, int x, int y, int w, int h);

    Entry* entries;      // offset 0x4
    int max;             // offset 0x8
    int count;           // offset 0xC
    long current;        // offset 0x10
    unsigned char flip;  // offset 0x14
};

#endif
