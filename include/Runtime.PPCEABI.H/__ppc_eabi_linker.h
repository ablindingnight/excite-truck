#ifndef __PPC_EABI_LINKER_H
#define __PPC_EABI_LINKER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __eti_init_info {
    void* eti_start;
    void* eti_end;
    void* code_start;
    unsigned long code_size;
} __eti_init_info;

__declspec(section ".init") extern __eti_init_info _eti_init_info[];

int __register_fragment(struct __eti_init_info* info, char* TOC);
void __unregister_fragment(int fragmentID);

#ifdef __cplusplus
}
#endif

#endif
