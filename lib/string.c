#include "types.h"

uint32 strlen(const char* str) {
    uint32 ret = 0;
    while(str[ret] != 0) {
        ret++;
    }
    return ret;
}
