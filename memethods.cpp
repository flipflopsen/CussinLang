#include "memethods.h"

char* safe_strcpy(char* dest, size_t size, char* src) {
    if (size > 0) {
        size_t i;
        for (i = 0; i < size - 1 && src[i]; i++) {
            dest[i] = src[i];
        }
        dest[i] = '\0';
    }
    return dest;
}