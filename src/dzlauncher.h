#ifndef OPENDZ_LAUNCHER_H
#define OPENDZ_LAUNCHER_H

#include "dayz_mod.h"
#include "string.h"

#define OPENDZL_MAX_SERVER_MODS 32
#define OPENDZL_MAX_SERVER_NAME 256
#define OPENDZL_MAX_SERVER_MAP_NAME 256

#define OPENDZL_ERRMSG_MAX 512

// NCurses color pairs
#define COLOR_CURSOR 8


#define OPENDZL_MAX_ERROR_MESSAGES 8
#define OPENDZL_ERRMSG_MAX_LENGTH 511


struct dayz_server {

    struct dayz_mod mods[OPENDZL_MAX_SERVER_MODS];
    uint8_t         num_mods;

    char     name[OPENDZL_MAX_SERVER_NAME];
    uint8_t  name_length;

    char     map_name[OPENDZL_MAX_SERVER_MAP_NAME];
    uint8_t  map_name_length;

};


struct config {
    // Directory paths are guranteed to end with '/'
    char* dayz_workshop_dir; // ...steamapps/workshop/content/221100/
    char* dayz_steam_dir;    // ...steamapps/common/Dayz/
};

bool query_dayz_server(char* addr, uint16_t port, struct dayz_server* server);
void launch_dayz(struct config* cfg, char* game_addr, char* game_port, struct dayz_server* server);




#endif
