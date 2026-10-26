// Release-build stand-ins for the MetroTRK debugger hooks: the console reads and TRK writes report
// failure (1) and the TRK init calls do nothing. The original file name is unknown.
#include <stddef.h>

typedef unsigned long __file_handle;
typedef void (*__idle_proc)(void);

int __read_console(__file_handle handle, unsigned char* buffer, size_t* count, __idle_proc idle_fn) {
    return 1;
}

int __TRK_write_console(__file_handle handle, unsigned char* buffer, size_t* count, __idle_proc idle_fn) {
    return 1;
}

void InitMetroTRK(void) {}

void EnableMetroTRKInterrupts(void) {}
