typedef struct __va_list_struct {
    char gpr;
    char fpr;
    char reserved[2];
    char* input_arg_area;
    char* reg_save_area;
} __va_list_struct;

#define ALIGN(addr, amount) (((unsigned int)(addr) + ((amount) - 1)) & ~((amount) - 1))

void* __va_arg(__va_list_struct* list, int type) {
    char* addr;
    char* reg = &list->gpr;
    int g_reg = list->gpr;
    int maxsize = 8;
    int size = 4;
    int increment = 1;
    int even = 0;
    int fpr_offset = 0;
    int regsize = 4;

    if (type == 3) {
        reg = &list->fpr;
        g_reg = list->fpr;
        size = 8;
        fpr_offset = 32;
        regsize = 8;
    }

    if (type == 2) {
        size = 8;
        maxsize--;
        if (g_reg & 1) {
            even = 1;
        }
        increment = 2;
    }

    if (g_reg < maxsize) {
        g_reg += even;
        addr = list->reg_save_area + fpr_offset + (g_reg * regsize);
        *reg = g_reg + increment;
    } else {
        *reg = 8;
        list->input_arg_area = (char*)ALIGN(list->input_arg_area, size);
        addr = list->input_arg_area;
        list->input_arg_area += size;
    }

    if (type == 0) {
        addr = *((char**)addr);
    }

    return addr;
}
