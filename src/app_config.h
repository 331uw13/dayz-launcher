#ifndef DZLAUNCHER_APP_CONFIG_H
#define DZLAUNCHER_APP_CONFIG_H

#include "dayz_server.h"


#define FAVOURITE_SERVERS_MAX 256

struct app_config {
    // Directory paths are guranteed to end with '/'
    char* dayz_workshop_dir; // ...steamapps/workshop/content/221100/
    char* dayz_steam_dir;    // ...steamapps/common/Dayz/


    struct dayz_fav_server fav_servers [FAVOURITE_SERVERS_MAX];
    uint32_t               num_fav_servers;

    char**   fav_servers_name_list;
    uint32_t fav_servers_name_list_count;

};


bool load_app_config(struct app_config* cfg, const char* filepath);
void free_app_config(struct app_config* cfg);


#endif
