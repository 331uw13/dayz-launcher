#ifndef DAYZ_MOD_H
#define DAYZ_MOD_H

#include <stdint.h>

#define DAYZ_MODNAME_MAX 256


struct dayz_mod {
    uint32_t   workshop_id;
    char       name[DAYZ_MODNAME_MAX];
    uint8_t    name_length;
};


#endif
