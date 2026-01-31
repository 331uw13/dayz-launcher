#ifndef DAYZ_MOD_H
#define DAYZ_MOD_H

#include <stdint.h>

#define DAYZ_MODNAME_MAX 256


struct dayz_mod {
    //uint64_t   workshop_id;
    char       workshop_id[32];
    char       name[DAYZ_MODNAME_MAX];
    uint8_t    name_length;
};


#endif
