#include <stdlib.h>

#include "common.h"




void freeif(void* ptr) {
    if(ptr) { free(ptr); }
}
