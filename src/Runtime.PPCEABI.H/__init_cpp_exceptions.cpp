#include "Runtime.PPCEABI.H/__ppc_eabi_linker.h"

#ifdef __cplusplus
extern "C" {
#endif

void __init_cpp_exceptions(void);
void __fini_cpp_exceptions(void);

#ifdef __cplusplus
}
#endif

static int fragmentID = -2;

static inline char* GetR2(void) {
    register char* temp;
    asm { mr temp, r2 }
    return temp;
}

extern "C" void __init_cpp_exceptions(void) {
    if (fragmentID == -2) {
        char* R2 = GetR2();
        fragmentID = __register_fragment(_eti_init_info, R2);
    }
}

extern "C" void __fini_cpp_exceptions(void) {
    if (fragmentID != -2) {
        __unregister_fragment(fragmentID);
        fragmentID = -2;
    }
}

extern "C" void __destroy_global_chain(void);

#pragma section ".ctors$10"
__declspec(section ".ctors$10") extern void* const __init_cpp_exceptions_reference = __init_cpp_exceptions;
#pragma section ".dtors$10"
__declspec(section ".dtors$10") extern void* const __destroy_global_chain_reference = __destroy_global_chain;
#pragma section ".dtors$15"
__declspec(section ".dtors$15") extern void* const __fini_cpp_exceptions_reference = __fini_cpp_exceptions;
