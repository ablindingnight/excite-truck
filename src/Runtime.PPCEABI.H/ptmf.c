/* Pointer-to-member-function support for the Metrowerks C++ ABI. */

typedef struct __ptmf {
    long this_delta;
    long vtbl_offset;
    union {
        void* func_addr;
        long ventry_offset;
    } func_data;
} __ptmf;

const __ptmf __ptmf_null = {0, 0, 0};

asm void __ptmf_cmpr(void) {
    nofralloc
    lwz r5, 0x0(r3)
    lwz r6, 0x0(r4)
    lwz r7, 0x4(r3)
    lwz r8, 0x4(r4)
    lwz r9, 0x8(r3)
    lwz r10, 0x8(r4)
    li r3, 0x1
    cmpw r5, r6
    cmpw cr6, r7, r8
    cmpw cr7, r9, r10
    bnelr
    bnelr cr6
    bnelr cr7
    li r3, 0x0
    blr
}

asm void __ptmf_scall(void) {
    nofralloc
    lwz r0, 0x0(r12)
    lwz r11, 0x4(r12)
    lwz r12, 0x8(r12)
    add r3, r3, r0
    cmpwi r11, 0x0
    blt L_8002F788
    lwzx r12, r3, r12
    lwzx r12, r12, r11
L_8002F788:
    mtctr r12
    bctr
}

