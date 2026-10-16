#ifndef _NMWEXCEPTION_H
#define _NMWEXCEPTION_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef short vbase_ctor_arg_type;
typedef char local_cond_type;

typedef void* (*ConstructorDestructor)(void*, short);

#define CTORARG_TYPE short
#define CTORARG_PARTIAL (0)
#define CTORARG_COMPLETE (1)

#define CTORCALL_COMPLETE(ctor, objptr) (((void (*)(void*, CTORARG_TYPE))ctor)(objptr, CTORARG_COMPLETE))

#define DTORARG_TYPE short
#define DTORARG_PARTIAL (0)
#define DTORARG_COMPLETE (-1)

#define DTORCALL_COMPLETE(dtor, objptr) (((void (*)(void*, DTORARG_TYPE))dtor)(objptr, DTORARG_COMPLETE))

#define ARRAY_HEADER_SIZE 16

void* __construct_new_array(void* block, ConstructorDestructor ctor, ConstructorDestructor dtor, size_t size, size_t n);
void __construct_array(void* ptr, ConstructorDestructor ctor, ConstructorDestructor dtor, size_t size, size_t n);
void __destroy_arr(void* block, ConstructorDestructor dtor, size_t size, size_t n);
void __destroy_new_array(void* block, ConstructorDestructor dtor);

#ifdef __cplusplus
}
#endif

#endif
