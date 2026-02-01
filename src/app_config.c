#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

#include "app_config.h"
#include "common.h"
#include "string.h"

#include "thirdparty/ini.h"


#define STR_MATCH(a, b) (strcmp(a, b) == 0)

static
int appconfig_read_handler
(
    void* userptr,
    const char* section,
    const char* name,
    const char* value
){
    struct app_config* appcfg = (struct app_config*)userptr;


    if(STR_MATCH(section, "favourite_servers")) {
        if(appcfg->num_fav_servers+1 >= FAVOURITE_SERVERS_MAX) {
            fprintf(stderr, 
                    "WARNING: %s: Read too many favourite servers. Not reading more.\n",
                    __func__);
            return 1;
        }

        struct dayz_fav_server* fav_server = &appcfg->fav_servers[appcfg->num_fav_servers];
        
        const size_t name_len = strlen(name);
        if(name_len >= SERVER_NAME_MAX-1) {
            fprintf(stderr,
                    "ERROR: %s: Too long server name: %s\n",
                    __func__,
                    name);
            return 1;
        }

        strcpy(fav_server->name, name);

        memset(fav_server->address, 0, sizeof(fav_server->address));
        memset(fav_server->game_port, 0, sizeof(fav_server->game_port));
        memset(fav_server->query_port, 0, sizeof(fav_server->query_port));

        char* value_ch = (char*)&value[0];
        const size_t value_len = strlen(value);
        const char* value_max = value + value_len;

        size_t addr_len = 0;
        size_t game_port_len = 0;
        size_t query_port_len = 0;

        // Read server address.
        while(value_ch < value_max) {
            if(*value_ch == ',') {
                value_ch++;
                break;
            }

            if(addr_len+1 >= SERVER_ADDRSTR_MAX) {
                fprintf(stderr, "ERROR: %s: Value contains too long server address: %s\n", 
                        __func__, value);
                return 1;
            }

            fav_server->address[addr_len++] = *value_ch;
            value_ch++;
        }
   
        // Read game port.
        while(value_ch < value_max) {
            if(*value_ch == ',') {
                value_ch++;
                break;
            }

            if(game_port_len+1 >= SERVER_PORTSTR_MAX) {
                fprintf(stderr, "ERROR: %s: Value contains too long game port: %s\n", 
                        __func__, value);
                return 1;
            }

            fav_server->game_port[game_port_len++] = *value_ch;
            value_ch++;
        }
   
        // Read query port.
        while(value_ch < value_max) {
            if(*value_ch == ',') {
                value_ch++;
                break;
            }

            if(query_port_len+1 >= SERVER_PORTSTR_MAX) {
                fprintf(stderr, "ERROR: %s: Value contains too long query port: %s\n", 
                        __func__, value);
                return 1;
            }

            fav_server->query_port[query_port_len++] = *value_ch;
            value_ch++;
        }

        appcfg->num_fav_servers++;
    }
    else
    if(STR_MATCH(section, "settings")) {
    }
    else {
        fprintf(stderr, "WARNING: Unhandled config section: %s\n", section);
    }

    return 1;
}

bool load_app_config(struct app_config* appcfg, const char* filepath) {
    appcfg->num_fav_servers = 0;
    if(ini_parse(filepath, appconfig_read_handler, appcfg)) {
        return false;
    }


    appcfg->fav_servers_name_list = NULL;
    appcfg->fav_servers_name_list_count = 0;

    if(appcfg->num_fav_servers > 0) {
        appcfg->fav_servers_name_list_count = appcfg->num_fav_servers;
        appcfg->fav_servers_name_list = malloc(appcfg->num_fav_servers * sizeof *appcfg->fav_servers_name_list);
        for(uint32_t i = 0; i < appcfg->num_fav_servers; i++) {
            appcfg->fav_servers_name_list[i] = strdup(appcfg->fav_servers[i].name);
        }
    }


    char* home = getenv("HOME");
    if(home == NULL) {
        home = getpwuid(getuid())->pw_dir;
    }


    struct string_t path = string_create(0);
    string_append(&path, home, -1);
    if(string_lastbyte(&path) != '/') {
        string_pushbyte(&path, '/');
    }
    string_append(&path, ".local/share/Steam/steamapps/workshop/content/221100/", -1);
    appcfg->dayz_workshop_dir = strdup(path.bytes);

    string_clear(&path);
    string_append(&path, home, -1);
    if(string_lastbyte(&path) != '/') {
        string_pushbyte(&path, '/');
    }
    string_append(&path, ".local/share/Steam/steamapps/common/DayZ/", -1);
    appcfg->dayz_steam_dir = strdup(path.bytes);


    free_string(&path);
    return true;
}

void free_app_config(struct app_config* appcfg) {
    freeif(appcfg->dayz_workshop_dir);
    freeif(appcfg->dayz_steam_dir);

    if(appcfg->fav_servers_name_list) {
        for(uint32_t i = 0; i < appcfg->num_fav_servers; i++) {
            freeif(appcfg->fav_servers_name_list[i]);
        }

        freeif(appcfg->fav_servers_name_list);
    }
}
