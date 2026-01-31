#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dzlauncher.h"









int main() {

    const char* server_addr = "192.168.1.109";
    const char* game_port = "2302";
    const uint16_t query_port = 27016;

    struct config cfg;
    cfg.dayz_workshop_dir =
        strdup("/home/user/.local/share/Steam/steamapps/workshop/content/221100/");
    
    cfg.dayz_steam_dir =
        strdup("/home/user/.local/share/Steam/steamapps/common/DayZ/");


    struct dayz_server server_info;
    if(!query_dayz_server(server_addr, query_port, &server_info)) {
        return 1;
    }
   

    for(uint8_t i = 0; i < server_info.num_mods; i++) {
        printf("%s - %li\n", server_info.mods[i].name, server_info.mods[i].workshop_id);
    }

    launch_dayz(&cfg, server_addr, game_port, &server_info);

    free(cfg.dayz_workshop_dir);
    free(cfg.dayz_steam_dir);
    return 0;
}
