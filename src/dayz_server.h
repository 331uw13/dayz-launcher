#ifndef DAYZ_SERVER_H
#define DAYZ_SERVER_H

#include <stdint.h>

#include "dayz_mod.h"


#define SERVER_MODS_MAX 32
#define SERVER_NAME_MAX 256
#define SERVER_MAP_NAME_MAX 256

#define SERVER_ADDRSTR_MAX 32
#define SERVER_PORTSTR_MAX 16



struct dayz_server {
    struct dayz_mod mods [SERVER_MODS_MAX];
    uint8_t         num_mods;

    char     name [SERVER_NAME_MAX];
    uint8_t  name_length;

    char     map_name [SERVER_MAP_NAME_MAX];
    uint8_t  map_name_length;
};


// Favourite server.
struct dayz_fav_server {
    char  name [SERVER_NAME_MAX];
    char  address [SERVER_ADDRSTR_MAX];
    char  game_port [SERVER_PORTSTR_MAX];
    char  query_port [SERVER_PORTSTR_MAX];
};

#endif
