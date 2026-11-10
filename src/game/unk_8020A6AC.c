// Code 0x8020A6AC-0x8020BCB8, imported verbatim from the upstream project (github.com/orangedude27/excite-truck,
// CC0; file src/auto/text_8020A6AC_.c), where its functions match exactly. Unit name is provisional.
// CLEANUP: automated-decompiler source with raw offsets and placeholder types; rewrite with real types
// before the PC port. Tricks are tagged MATCHING-AID.

extern void fn_802E317C(const char *, ...);
extern const char lbl_8033C510[];
extern void fn_802E61E4(void *, float);
extern float fn_80209028(void), fn_802091CC(void);
extern int fn_802090CC(void), fn_802E2C20(void);
extern void fn_802E625C(int);
extern void fn_802E61EC(void *, float), fn_802E620C(void *, float), fn_802E6234(void *, float), fn_802E6238(void *, float), fn_802E623C(void *, float), fn_802E622C(void *, float), fn_802E6240(void *, int);
extern float lbl_8055B590, lbl_8055B594;
extern float lbl_80558DC4[];
extern int lbl_8055EDF8;
extern unsigned int lbl_8055EE00;

extern void fn_802E6264(void);
extern int lbl_8055EE0C;
extern void *lbl_8055EE10;
extern char lbl_8033C35C[];
extern char lbl_8033C368[];
extern char lbl_804892D0[];
extern int __nw__FUlPCc(int, void *);
extern void *fn_8012A13C(void *, int, int);
extern int fn_8012A2D8(void *);
extern int fn_8012A364(void *, void *);
typedef struct { void **vtable; } Unk8012A570;
    extern void *fn_8020A860();
  extern int fn_80208E90(void *, int);
  extern float lbl_8055B598;

  extern void *lbl_8055EE04;
extern char lbl_8033C37C[];
extern void *lbl_8055EE08;
extern unsigned char lbl_8033C520[];
extern unsigned char lbl_80489BD0[];
extern int __dl__FPv(void);
extern char lbl_8033C458[];
typedef struct { char *name; int state; } UnkStruct_8020A860;
extern char lbl_8033C338[];
extern int strcasecmp(const char *, const char *);

// Looks a sample up by name in the loaded sample table (0x24 bytes each); unknown names get the default entry.
void *fn_8020A6AC(void *name) {
    char *entry = lbl_804892D0;
    int i;
    for (i = 0; i < (int)lbl_8055EE08; i++) {
        if (strcasecmp((const char *)name, entry) == 0) {
            return entry;
        }
        entry += 0x24;
    }
    return lbl_8033C338;
}

void fn_8020A72C(void) {
    void *m = (void *)__nw__FUlPCc(0xC, lbl_8033C35C);
    if (m != 0) {
        m = fn_8020A860(m);
    }
    fn_80208E90(m, 0);
}

void* fn_8020A76C(void) {
    return lbl_8055EE08;
}

void *fn_8020A774(int arg0) { return lbl_804892D0 + arg0 * 0x24; }

#pragma push
#pragma cplusplus on
void* operator new(unsigned long size, const char* tag);

// A fixed-size block pool (joint.cpp declares it too).
class Pool {
public:
    Pool(const char* name, long count, long size);
    virtual ~Pool(void);
    void* Alloc(void);
    void Free(void* p);

    unsigned char pad4[0x1C];
};

// Blocks of 0x54 bytes from a shared pool, created on first use and deleted with its last block.
// This is the voice class's operator new; size and tag are fixed by the pool.
extern "C" void* fn_8020A788(unsigned long size, const char* tag) {
    if (lbl_8055EE0C == 0) {
        lbl_8055EE10 = new (lbl_8033C368) Pool(lbl_8033C368, 0x200, 0x54);
    }
    void* p = ((Pool*)lbl_8055EE10)->Alloc();
    if (p) {
        lbl_8055EE0C++;
    }
    return p;
}

extern "C" void fn_8020A800(void* p) {
    ((Pool*)lbl_8055EE10)->Free(p);
    if (--lbl_8055EE0C == 0) {
        delete (Pool*)lbl_8055EE10;
        lbl_8055EE10 = 0;
    }
}
#pragma cplusplus reset
#pragma pop

void *fn_8020A860(UnkStruct_8020A860 *arg0) {
    arg0->name = lbl_8033C458;
    arg0->state = 0;
    return arg0;
}
#pragma scheduling on

extern int OptNoSound__Fv(void);
extern void *__nwa__FUlPCc(unsigned int, void *);
extern void __dla__FPv(void *);
extern void *memset(void *, int, unsigned int);
extern void fn_802E6B04(void);
extern void fn_802E6B34(void);
extern void fn_802E6264(void);
extern char lbl_8033C388[];

typedef struct VoiceSoundMixer {
    void **vtable;
    void **m_voices;
} VoiceSoundMixer;

int VoiceSoundMixer_vf18(VoiceSoundMixer *self) {
    lbl_8055B598 = OptNoSound__Fv() ? 0.0f : 1.0f;
    lbl_8055EE04 = self;
    self->m_voices = (void **)__nwa__FUlPCc(0x800, lbl_8033C37C);
    memset(self->m_voices, 0, 0x800);
    fn_802E6B04();
    return 1;
}

void VoiceSoundMixer_vf24(VoiceSoundMixer *self) {
    int i;
    fn_802E6B34();
    for (i = 0; i < 0x200; i++) {
        if (self->m_voices[i] != 0) {
            fn_802E317C(lbl_8033C388, self->m_voices[i]);
        }
    }
    __dla__FPv(self->m_voices);
    self->m_voices = 0;
    lbl_8055EE04 = 0;
}

#pragma push
#pragma cplusplus on
// A mixer voice; only the two virtual slots used here are named.
class MixerVoice {
public:
    virtual void vf08(void);
    virtual void vf0C(void);
    virtual void vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void Stop(void);
    virtual int GetState(void);
};

// Stops every voice that is not already finished (state 3).
extern "C" void VoiceSoundMixer_vf28(VoiceSoundMixer *self) {
    for (int i = 0; i < 0x200; i++) {
        if (self->m_voices[i] != 0 && ((MixerVoice *)self->m_voices[i])->GetState() != 3) {
            ((MixerVoice *)self->m_voices[i])->Stop();
        }
    }
    fn_802E6264();
}
#pragma cplusplus reset
#pragma pop

extern void fn_802E59BC(void *);
extern void fn_802E5950(int, int);
extern void fn_802E6138(void *);
extern void fn_802E61D8(void *);
void fn_8020AA0C(void *arg0) {
    void *value = *(void **)((char *)arg0 + 8);
    if (value != 0) {
        fn_802E61D8(value);
        if (*(void **)((char *)arg0 + 0xC) != 0)
            fn_802E61D8(*(void **)((char *)arg0 + 0xC));
        if (*(void **)((char *)arg0 + 0xC) != 0) {
            fn_802E6138(*(void **)((char *)arg0 + 0xC));
            *(void **)((char *)arg0 + 0xC) = 0;
        }
        if (*(void **)((char *)arg0 + 8) != 0) {
            fn_802E6138(*(void **)((char *)arg0 + 8));
            *(void **)((char *)arg0 + 8) = 0;
        }
    }
    *(int *)((char *)arg0 + 4) = 3;
    *((unsigned char *)arg0 + 0x3E) = 0;
}

// MATCHING-AID?: volatile from upstream (check: hardware or trick); revisit.
int fn_8020AA98(void *arg0) { volatile int *p = (volatile int *)((char *)arg0 + 4); if (*p == 1 && *((unsigned char *)arg0 + 0x3E) != 0) return 2; return *p; }

extern const char lbl_8033C3B4[];
extern const char lbl_8033C3C0[];
extern const char lbl_8055B5A0[];
extern const char lbl_8055B5A8[];
extern const char lbl_8055B5B0[];
extern void *fn_80131808(const char *);
extern void fn_80131818(void *);
extern void fn_802E58B8(int);
extern int strcmp(const char *, const char *);
#pragma push
#pragma cplusplus on
// A node of a parsed config list (as returned by fn_80131808).
class ConfigNode {
public:
    virtual const char *GetName(void);
    virtual ConfigNode *GetAt(int index);
    virtual void vf10(void);
    virtual const char *GetString(const char *key, int fallback);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void vf20(void);
    virtual void vf24(void);
    virtual float GetValue(const char *key, float fallback);
};

// Loads the "samples" table (name and volume of each voice sample) from the sound config.
extern "C" void VoiceSoundMixer_vf2C(int unused, int arg1) {
    ConfigNode *list = (ConfigNode *)fn_80131808(lbl_8033C3B4);
    if (list != 0) {
        ConfigNode *item;
        int index;
        for (index = 0; (item = list->GetAt(index)) != 0; index++) {
            const char *name = item->GetName();
            if (strcmp(name, "samples") == 0) {
                int n = (int)lbl_8055EE08;
                char *entry = lbl_804892D0 + n * 0x24;
                int left;
                const char *s;
                char *dest;
                lbl_8055EE08 = (void *)(n + 1);
                s = item->GetString("name", n);
                left = 0x20;
                dest = entry;
                while ((*dest++ = *s++) != 0) {
                    if (--left <= 0) {
                        dest[-1] = 0;
                        break;
                    }
                }
                *(float *)(entry + 0x20) = item->GetValue("volume", 1.0f);
            }
        }
        fn_80131818(list);
    } else {
        fn_802E317C(lbl_8033C3C0);
    }
    fn_802E58B8(arg1);
}
#pragma cplusplus reset
#pragma pop

extern void fn_802E5950(int, int); void VoiceSoundMixer_vf30(int arg0, int arg1, int arg2) { fn_802E5950(arg1, arg2); }

extern void fn_802E59BC(void *); void VoiceSoundMixer_vf34(void *arg0, int arg1) { fn_802E59BC((void *)arg1); }

extern void fn_802E5A54(void);
void VoiceSoundMixer_vf38(void) {
    lbl_8055EE08 = 0;
    fn_802E5A54();
}

#pragma push
#pragma processor 750
#pragma push
#pragma cplusplus on
// The slots of the voice's vtable that the mixer update calls.
class MixerVoiceSlots {
public:
    virtual ~MixerVoiceSlots(void);
    virtual void vf0C(void);
    virtual int vf10(void);
    virtual void vf14(void);
    virtual void vf18(void);
    virtual void vf1C(void);
    virtual void Expire(void);  // 0x20
};

extern "C" void VoiceSoundMixer_vf20(void *arg0) {
 void *p; int i,now; void *handle; float volume,x,v;
 lbl_8055B590=fn_80209028();
 lbl_8055B594=fn_802091CC();
 lbl_8055EDF8++;
 now=fn_802E2C20();
 lbl_8055EE00=now-(*(int *)((char *)(arg0)+(8)));
 if(lbl_8055EE00>200U) lbl_8055EE00=200;
 (*(int *)((char *)(arg0)+(8)))=now;
 for(i=0;i<512;i++) {
 p=(*(void ** *)((char *)(arg0)+(4)))[i];
 if(p) {
 int mode=1;
 switch(fn_802090CC()) {case 0: mode=0;break;case 1:break;case 2:mode=2;break;}
 fn_802E625C(mode);
 handle=(*(void * *)((char *)(p)+(8)));
 if(handle) {
 int j;
 v=(*(float *)((char *)(p)+(0x1c)))*(*(float *)((char *)((*(void * *)((char *)(p)+(0x18))))+(0x20)));
 if((*(unsigned char *)((char *)(p)+(0x3e)))) volume=0.0f;
 else {float w; if((*(unsigned char *)((char *)(p)+(0x3c)))) w=v; else w=v*lbl_8055B590;volume=lbl_8055B598*w;}
 if((*(void * *)((char *)(p)+(0xc)))) {
 x=1.0f+-(*(float *)((char *)(p)+(0x20)));
 if(1.0f<x)x=1.0f;
 x=x>0.0f?x:0.0f;
 ((void (*)(void *,float))fn_802E61E4)(handle,volume*x);
 fn_802E61EC((*(void * *)((char *)(p)+(8))),(-1.0f));
 x=1.0f+(*(float *)((char *)(p)+(0x20)));
 if(1.0f<x)x=1.0f;
 x=x>0.0f?x:0.0f;
 ((void (*)(void *,float))fn_802E61E4)((*(void * *)((char *)(p)+(0xc))),volume*x);
 fn_802E61EC((*(void * *)((char *)(p)+(0xc))),1.0f);
 }else{
 ((void (*)(void *,float))fn_802E61E4)(handle,volume);
 fn_802E61EC((*(void * *)((char *)(p)+(8))),(*(float *)((char *)(p)+(0x20))));
 }
 for(j=0;j<2;j++){
 void *h;
 if(j==0)h=(*(void * *)((char *)(p)+(8)));else h=(*(void * *)((char *)(p)+(0xc)));
 if(h) {
 fn_802E620C(h,(*(float *)((char *)(p)+(0x28))));
 fn_802E6234(h,(*(float *)((char *)(p)+(0x2c))));
 fn_802E6238(h,(*(float *)((char *)(p)+(0x30))));
 fn_802E623C(h,(*(float *)((char *)(p)+(0x34))));
 fn_802E622C(h,(*(float *)((char *)(p)+(0x24))));
 fn_802E6240(h,(*(int *)((char *)(p)+(0x38))));
 }}
 }
 if((*(int *)((char *)(p)+(4)))==1) {
 (*(int *)((char *)(p)+(0x44)))++;
 if(!(*(unsigned char *)((char *)(p)+(0x3e))) && !((float)__fabs((*(float *)((char *)(p)+(0x24))))<=lbl_80558DC4[0])) (*(unsigned int *)((char *)(p)+(0x50)))+=lbl_8055EE00;
 if(!(*(unsigned char *)((char *)(p)+(0x3d))) && (*(unsigned int *)((char *)(p)+(0x50)))>(*(unsigned int *)((char *)(p)+(0x4c))))
 ((MixerVoiceSlots *)p)->Expire();
 }else (*(unsigned int *)((char *)(p)+(0x50)))=0;
 }
 }
 fn_802E6264();
}
#pragma pop
#pragma pop

extern char lbl_8033C3F4[];
typedef struct MixerVoiceData {
    void **vtable;
    int state;
    void *handle0;
    void *handle1;
    void *stream0;
    void *stream1;
    char *sample;
    float volume;
    float pan;
    float param24;
    float param28;
    float param2C;
    float param30;
    float param34;
    int param38;
    unsigned char unscaled;
    unsigned char paused;
    unsigned char muted;
    unsigned char pad3F;
    int slot;
    int unk44;
    int pad48[2];
    int unk50;
} MixerVoiceData;

extern char lbl_8033C3D8[];
extern char lbl_8033C3E4[];
extern char lbl_8033C4A0[];
extern char lbl_8033C4C8[];
extern char lbl_8055B5B8[8];
extern char lbl_8055B5C0[5];
extern void *fn_802E5AE8(void *);
extern void *fn_8020A6AC(void *);
extern float fn_802E6040(void *);
extern char *strchr(const char *, int);
extern int strcasecmp(const char *, const char *);
extern void fn_802E30C8(const char *, ...);

#pragma push
#pragma cplusplus on
// The voice object as the mixer sees it: a deleting destructor and an Init that opens it.
class MixerVoiceObj {
public:
    virtual ~MixerVoiceObj(void);
    virtual void vf0C(void);
    virtual int Init(void);
};

// Creates a voice in the first free slot: opens the left stream (name) and the matching
// right stream (name with its extension replaced), then initialises it.
// Sets up a new voice: the left stream is the named file, the right stream the same name
// with its extension replaced.
static inline MixerVoiceData *MixerVoice_Construct(MixerVoiceData *voice, char *name, int slot, unsigned char unscaled) {
    char buf[0x40];
    char *dest;
    int left;
    const char *src;
    char *ext;

    voice->vtable = (void **)lbl_8033C3F4;
    *(float *)((char *)voice + 0x48) = 0.0f;
    voice->slot = slot;
    voice->unk44 = 0;
    voice->state = 3;
    voice->handle1 = 0;
    voice->handle0 = 0;
    voice->stream1 = 0;
    voice->stream0 = 0;
    voice->paused = 0;
    voice->muted = 0;
    voice->unscaled = unscaled;
    voice->stream0 = fn_802E5AE8(name);
    if (voice->stream0 == 0) {
        return voice;
    }
    left = 0x40;
    src = name;
    dest = buf;
    while ((*dest++ = *src++) != 0) {
        if (--left <= 0) {
            dest[-1] = 0;
            break;
        }
    }
    ext = strchr(buf, '.');
    if (ext != 0) {
        if (strcasecmp(ext, lbl_8055B5C0) != 0) {
            fn_802E317C(lbl_8033C4C8, name);
        }
        *ext = 0;
        {
            char *d = buf;
            int n = 0x40;
            while (*d != 0) {
                n--;
                d++;
            }
            if (n > 0) {
                const char *s = lbl_8055B5B8;
                while ((*d++ = *s++) != 0) {
                    if (--n <= 0) {
                        d[-1] = 0;
                        break;
                    }
                }
            }
        }
    } else {
        fn_802E317C(lbl_8033C4A0, name);
    }
    voice->stream1 = fn_802E5AE8(buf);
    voice->sample = (char *)fn_8020A6AC(name);
    *(float *)((char *)voice + 0x48) = fn_802E6040(voice->stream0);
    *(unsigned long *)((char *)voice + 0x4C) = 1000.0f * (0.1f + 1.02f * *(float *)((char *)voice + 0x48));
    voice->pan = 0.0f;
    voice->volume = 0.0f;
    voice->param24 = 1.0f;
    voice->param28 = 1.0f;
    voice->param2C = 0.0f;
    voice->param30 = 0.0f;
    voice->param34 = 0.0f;
    voice->param38 = 0;
    return voice;
}

// Creates a voice in the first free slot and initialises it.
extern "C" MixerVoiceObj *VoiceSoundMixer_vf1C(VoiceSoundMixer *self, char *name, int unused, unsigned char unscaled) {
    MixerVoiceData *voice;
    int i;
    for (i = 0; i < 0x200; i++) {
        if (self->m_voices[i] == 0) {
            voice = (MixerVoiceData *)fn_8020A788(0x54, lbl_8033C3D8);
            if (voice != 0) {
                voice = MixerVoice_Construct(voice, name, i, unscaled);
            }
            if (!((MixerVoiceObj *)voice)->Init()) {
                delete (MixerVoiceObj *)voice;
                return 0;
            }
            self->m_voices[i] = voice;
            return (MixerVoiceObj *)voice;
        }
    }
    fn_802E30C8(lbl_8033C3E4);
    return 0;
}
#pragma cplusplus reset
#pragma pop

extern void fn_802E5D5C(void *);
void fn_8020B2BC(void *arg0, int arg1, int arg2);

// Voice destructor: releases both sound handles and the two streams, then frees the pool block if asked.
void *fn_8020B1C8(void *self, int flags) {
    if (self != 0) {
        *(void **)self = lbl_8033C3F4;
        if (*(void **)((char *)self + 8) != 0)
            fn_802E61D8(*(void **)((char *)self + 8));
        if (*(void **)((char *)self + 0xC) != 0)
            fn_802E61D8(*(void **)((char *)self + 0xC));
        if (*(void **)((char *)self + 0xC) != 0) {
            fn_802E6138(*(void **)((char *)self + 0xC));
            *(void **)((char *)self + 0xC) = 0;
        }
        if (*(void **)((char *)self + 8) != 0) {
            fn_802E6138(*(void **)((char *)self + 8));
            *(void **)((char *)self + 8) = 0;
        }
        if (*(void **)((char *)self + 0x14) != 0)
            fn_802E5D5C(*(void **)((char *)self + 0x14));
        if (*(void **)((char *)self + 0x10) != 0) {
            fn_802E5D5C(*(void **)((char *)self + 0x10));
            fn_8020B2BC(lbl_8055EE04, (int)self, *(int *)((char *)self + 0x40));
        }
        if (flags > 0)
            fn_8020A800(self);
    }
    return self;
}

int fn_8020B2A8(void *arg0) {
    return *(int *)((char *)arg0 + 0x10) != 0;
}

void fn_8020B2BC(void *arg0, int arg1, int arg2) {
    int *ptr = *(int **)((unsigned char *)arg0 + 4);
    ptr[arg2] = 0;
}

void VoiceSoundMixer_vf3C(void) {

}

extern unsigned char lbl_8033C520[];
extern unsigned char lbl_80489BD0[];
extern int __dl__FPv(void);
int VoiceSoundMixer_vf08(int arg0, int arg1) {
    if (arg0 != 0 && arg1 > 0) {
        __dl__FPv();
    }
    return arg0;
}

void *VoiceSoundMixer_vf0C(void) {
    return lbl_8033C520;
}

void VoiceSoundMixer_vf10(void) {
    unsigned char *p = lbl_80489BD0;
    p[0] = 0;
    *(int *)(p + 4) = 0;
    *(int *)(p + 8) = 0x20;
}

void VoiceSoundMixer_vf40(void) {
    fn_802E5824();
}

void VoiceSoundMixer_vf44(void) {
    fn_802E5828();
}

#pragma push
#pragma cplusplus on
extern "C" void fn_8020B348(void *arg0) {
    class Obj {
    public:
        virtual void f(int);
    };
    if (arg0 == 0) return;
    ((Obj *)arg0)->f(1);
}
#pragma cplusplus reset
#pragma pop


extern void *fn_802E6058(void *);
extern void fn_802E6ABC(void *);
extern void fn_80207938(void);
extern void fn_802E61BC(void *);
extern void fn_802E61A8(void *);

// Opens a handle for each stream that does not have one yet.
static inline void MixerVoice_Open(MixerVoiceData *voice) {
    if (voice->stream0 != 0 && voice->handle0 == 0) {
        voice->handle0 = fn_802E6058(voice->stream0);
        if (voice->handle0 == 0) {
            fn_802E6ABC(voice->stream0);
            fn_80207938();
        } else if (voice->stream1 != 0) {
            voice->handle1 = fn_802E6058(voice->stream1);
            if (voice->handle1 == 0) {
                fn_802E6138(voice->handle0);
                voice->handle0 = 0;
            }
        }
    }
}

// Applies volume, pan and effect parameters to the open handles, then plays or pauses them.
static inline void MixerVoice_Apply(MixerVoiceData *voice) {
    int i;
    float gain;
    float v;
    float side;

    v = voice->volume * *(float *)(voice->sample + 0x20);
    if (voice->muted) {
        gain = 0.0f;
    } else {
        gain = lbl_8055B598 * (voice->unscaled ? v : v * lbl_8055B590);
    }
    if (voice->handle1 != 0) {
        side = 1.0f + -voice->pan;
        side = 1.0f < side ? 1.0f : side;
        side = side > 0.0f ? side : 0.0f;
        fn_802E61E4(voice->handle0, gain * side);
        fn_802E61EC(voice->handle0, (-1.0f));
        side = 1.0f + voice->pan;
        side = 1.0f < side ? 1.0f : side;
        side = side > 0.0f ? side : 0.0f;
        fn_802E61E4(voice->handle1, gain * side);
        fn_802E61EC(voice->handle1, 1.0f);
    } else {
        fn_802E61E4(voice->handle0, gain);
        fn_802E61EC(voice->handle0, voice->pan);
    }
    for (i = 0; i < 2; i++) {
        void *handle = i == 0 ? voice->handle0 : voice->handle1;
        if (handle != 0) {
            fn_802E620C(handle, voice->param28);
            fn_802E6234(handle, voice->param2C);
            fn_802E6238(handle, voice->param30);
            fn_802E623C(handle, voice->param34);
            fn_802E622C(handle, voice->param24);
            fn_802E6240(handle, voice->param38);
        }
    }
    if (voice->paused) {
        fn_802E61BC(voice->handle0);
        if (voice->handle1 != 0) fn_802E61BC(voice->handle1);
    } else {
        fn_802E61A8(voice->handle0);
        if (voice->handle1 != 0) fn_802E61A8(voice->handle1);
    }
}

static inline void MixerVoice_Start(MixerVoiceData *voice) {
    MixerVoice_Open(voice);
    if (voice->handle0 != 0) {
        voice->unk50 = 0;
        MixerVoice_Apply(voice);
    }
    voice->unk44 = 0;
    voice->state = 1;
}

// Starts a voice playing.
void fn_8020B368(MixerVoiceData *voice) {
    voice->paused = 0;
    MixerVoice_Start(voice);
}

// Starts a voice paused (ready to resume).
void fn_8020B5EC(MixerVoiceData *voice) {
    voice->paused = 1;
    MixerVoice_Start(voice);
}
extern void *fn_802E5AE8(void *);
extern void fn_802E5D5C(void *);
extern void *fn_8020A6AC(void *);
extern void fn_802E30C8(const char *, ...);
extern const char lbl_8033C4F0[];
int fn_8020B874(void *object, void *name) {
    int result = 0;
    void *created = fn_802E5AE8(name);
    if (created != 0) {
        fn_802E5D5C(*(void **)((char *)object + 0x10));
        *(void **)((char *)object + 0x10) = created;
        *(void **)((char *)object + 0x18) = fn_8020A6AC(name);
        result = 1;
    } else {
        fn_802E30C8(lbl_8033C4F0, name);
    }
    return result;
}

extern void fn_802E61E4(void *, float);
extern int lbl_8055EDFC;
void fn_8020B908(void *arg0) {
    void *p;
    if (*((unsigned char *)arg0 + 0x3E) == 0 && *(int *)((char *)arg0 + 4) != 3) {
        *((unsigned char *)arg0 + 0x3E) = 1;
        lbl_8055EDFC++;
        p = *(void **)((char *)arg0 + 8);
        if (p != 0) {
            fn_802E61D8(p);
            p = *(void **)((char *)arg0 + 0xC);
            if (p != 0)
                fn_802E61D8(p);
            p = *(void **)((char *)arg0 + 0xC);
            if (p != 0) {
                fn_802E6138(p);
                *(void **)((char *)arg0 + 0xC) = 0;
            }
            p = *(void **)((char *)arg0 + 8);
            if (p != 0) {
                fn_802E6138(p);
                *(void **)((char *)arg0 + 8) = 0;
            }
        }
        p = *(void **)((char *)arg0 + 8);
        if (p != 0)
            fn_802E61E4(p, 0.0f);
        p = *(void **)((char *)arg0 + 0xC);
        if (p != 0)
            fn_802E61E4(p, 0.0f);
    }
}

// Unmutes a voice; a playing voice that lost its handles is reopened and restarted.
void fn_8020B9D4(MixerVoiceData *voice) {
    if (voice->muted) {
        voice->muted = 0;
        if (voice->state == 1 && voice->handle0 == 0) {
            MixerVoice_Open(voice);
            if (voice->handle0 != 0) {
                MixerVoice_Apply(voice);
            }
        }
    }
}

void fn_8020BC50(void *arg0, float farg0) { *(float *)((char *)arg0 + 0x24) = farg0; }

typedef float f32;
typedef signed int s32;
void fn_8020BC58(void *arg0, f32 farg0) { *(f32 *)((char *)arg0 + 0x1C) = farg0; }

s32 fn_8020BC60(void) { return 0; }

void fn_8020BC68(void *arg0, f32 farg0) { *(f32 *)((char *)arg0 + 0x20) = farg0; }

void fn_8020BC70(void *arg0, f32 farg0) {
    *(f32 *)((signed char *)arg0 + 0x28) = farg0;
}

void fn_8020BC78(void *arg0, f32 farg0) {
    *(f32 *)((signed char *)arg0 + 0x2C) = farg0;
}

void fn_8020BC80(void *arg0, float arg1) {
    *(float *)((char *)arg0 + 0x30) = arg1;
}

void fn_8020BC88(void *arg0, float arg1) {
    *(float *)((char *)arg0 + 0x34) = arg1;
}

void fn_8020BC90(void *arg0, int arg1) {
    *(int *)((char *)arg0 + 0x38) = arg1;
}

void fn_8020BC98(void) {
    fn_802E317C(lbl_8033C510);
}

float fn_8020BCA8(void *arg0) {
    return *(float *)((char *)arg0 + 0x48);
}

void fn_8020BCB0(void) {

}

void fn_8020BCB4(void) {

}

extern void fn_802E6264(void);
extern void fn_802E6B34(void);
extern void __dla__FPv(void *);
extern int OptNoSound__Fv(void);
extern void *__nwa__FUlPCc(unsigned int, void *);
extern void *memset(void *, int, unsigned int);
extern char lbl_8033C388[];
extern char lbl_8033C37C[];
extern float lbl_8055B598;
extern void *lbl_8055EE04;
extern int lbl_8055EE0C;
extern void *lbl_8055EE10;
extern int fn_8012A364(void *, void *);
extern int __nw__FUlPCc(int, void *);
extern void *fn_8012A13C(void *, int, int);
extern int fn_8012A2D8(void *);
extern char lbl_8033C368[];
extern char lbl_8033C35C[];
extern char lbl_804892D0[];
extern void *fn_8020A860();
extern int fn_80208E90(void *, int);

/* .bss owned by this unit (0x804892D0-0x80489BE0), in address order. */
char lbl_804892D0[0x900];
unsigned char lbl_80489BD0[0x10];
