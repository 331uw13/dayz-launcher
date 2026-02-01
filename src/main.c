#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <raylib.h>


#include "dzlauncher.h"
#include "common.h"

#define RAYGUI_IMPLEMENTATION
#include "thirdparty/raygui.h"


int main() {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(700, 400, "DayZ-Launcher");
    SetTargetFPS(30);

    struct app_config appcfg;
    bool config_read_ok = load_app_config(&appcfg, "dzlauncher.ini");
    
    if(!config_read_ok) {
        fprintf(stderr, "Failed to read app config.\n");
    }

        
    int fav_server_name_list_active_index = 0;
    int fav_server_name_list_focus = 0;
    int fav_server_name_list_scroll_index = 0;

    char* server_game_address = NULL;
    char* server_game_port = NULL;
    struct dayz_server server;
    bool server_selected = false;

    char** server_mod_name_list = malloc(SERVER_MODS_MAX * sizeof *server_mod_name_list);
    for(size_t i = 0; i < SERVER_MODS_MAX; i++) {
        server_mod_name_list[i] = NULL;
    }
   
    uint32_t server_mod_name_list_count = 0;

    const int NAME_LIST_WIDTH = 150;
    const int NAME_LIST_PADDING_Y = 10;
    const int NAME_LIST_PADDING_X = 10;

    while(!WindowShouldClose()) {
        int win_w = GetScreenWidth();
        int win_h = GetScreenHeight();

        BeginDrawing();
        ClearBackground((Color){ 200, 190, 180, 255 });

        GuiDrawText("#100#https://github.com/331uw13/dayz-launcher",
                (Rectangle){ 5, win_h-20, 300, 20 }, 0, (Color){ 0, 0, 0, 255 });

        const int panels_height = win_h - 50;
        
        if(GuiListViewEx((Rectangle){
                    NAME_LIST_PADDING_X, 
                    NAME_LIST_PADDING_Y, 
                    NAME_LIST_WIDTH, 
                    panels_height }, 
                (const char**)appcfg.fav_servers_name_list,
                appcfg.fav_servers_name_list_count, 
                &fav_server_name_list_scroll_index, 
                &fav_server_name_list_active_index,
                &fav_server_name_list_focus,
                TEXT_ALIGN_CENTER)) {
            
            struct dayz_fav_server* fav_server = &appcfg.fav_servers[fav_server_name_list_active_index];


            server_selected = query_dayz_server(fav_server->address, atoi(fav_server->query_port), &server);
            server_mod_name_list_count = 0;
            server_game_address = NULL;
            server_game_port = NULL;
            if(server_selected) {
                for(uint8_t i = 0; i < server.num_mods; i++) {
                    freeif(server_mod_name_list[i]);
                    server_mod_name_list[i] = strdup(server.mods[i].name);
                    server_mod_name_list_count++;
                }
                server_game_address = fav_server->address;
                server_game_port = fav_server->game_port;
            }
        }

        if(server_selected) {
            Rectangle panel_rect = (Rectangle){ NAME_LIST_PADDING_X+NAME_LIST_WIDTH+5, 10, 300, panels_height };
            panel_rect.width = win_w - panel_rect.x-10;
            GuiPanel(panel_rect,
                    server.name);
            
            GuiDrawText(TextFormat("Mods:", server.map_name),
                    (Rectangle){ panel_rect.x+10, 60, 150, 20 }, 0, (Color){ 0, 0, 0, 255 });

            static int scroll_index = 0;
            static int active_index = 0;
            static int focus = 0;
           
            GuiListViewEx((Rectangle){
                        panel_rect.x+10, 80,
                        200, 
                        panels_height - 80-40
                    },
                    (const char**)server_mod_name_list,
                    server_mod_name_list_count,
                    &scroll_index,
                    &active_index,
                    &focus,
                    TEXT_ALIGN_LEFT);

            if(GuiButton((Rectangle){ panel_rect.x+10, panel_rect.y+panel_rect.height-40, 80, 30 }, "#119#Connect")) {
                launch_dayz(&appcfg, server_game_address, server_game_port, &server);
            }

            
            GuiDrawText(TextFormat("Map: %s", server.map_name),
                    (Rectangle){ panel_rect.x+10, panel_rect.y+30, 500, 20 }, 0, (Color){ 0, 0, 0, 255 });
        }


        EndDrawing();
    }

    free_app_config(&appcfg);
    CloseWindow();
    return 0;
}


/*
int main() {

    const char* server_addr = "192.168.1.109";
    const char* game_port = "2302";
    const uint16_t query_port = 27016;

    struct app_config cfg;
    cfg.dayz_workshop_dir =
        strdup("/home/eeiuwie/.local/share/Steam/steamapps/workshop/content/221100/");
    
    cfg.dayz_steam_dir =
        strdup("/home/eeiuwie/.local/share/Steam/steamapps/common/DayZ/");


    struct dayz_server server_info;
    if(!query_dayz_server(server_addr, query_port, &server_info)) {
        return 1;
    }

    for(uint8_t i = 0; i < server_info.num_mods; i++) {
        printf("%s - %s\n", server_info.mods[i].name, server_info.mods[i].workshop_id);
    }

    launch_dayz(&cfg, server_addr, game_port, &server_info);

    free(cfg.dayz_workshop_dir);
    free(cfg.dayz_steam_dir);
    return 0;
}
*/
