#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "server_query.h"
#include "a2s_parser.h"
#include "a2s_decoder.h"
#include "dzlauncher.h"
#include "string.h"


bool query_dayz_server(char* addr, uint16_t port, struct dayz_server* server) {
    bool result = false;

    struct string_t server_info = string_create(0);
    struct string_t server_mod_info = string_create(0);

    if(!get_server_a2s_responses(addr, port, &server_info, &server_mod_info)) {
        fprintf(stderr, "Failed to get correct server A2S responses.\n");
        goto out;
    }


    if(server_info.size < 4) {
        fprintf(stderr, "Failed to get good server info response. Got %i bytes.\n",
                server_info.size);
        goto out;
    }

    if(server_mod_info.size < 4) {
        fprintf(stderr, "Failed to get good server mod info response. Got %i bytes.\n",
                server_mod_info.size);
        goto out;
    }


    uint8_t decoded[4096] = { 0 };
    if(!decode_a2s_escapes((uint8_t*)server_mod_info.bytes, server_mod_info.size,
            decoded, sizeof(decoded))) {
        fprintf(stderr, "Failed to decode A2S escape sequences.\n");
        goto out;
    }

    if(!parse_a2s_rules(decoded, sizeof(decoded), server->mods, &server->num_mods)) {
        server->num_mods = 0;
        //goto out;
    }


    memset(server->name, 0, SERVER_NAME_MAX);
    memset(server->map_name, 0, SERVER_MAP_NAME_MAX);

    server->name_length = 0;
    server->map_name_length = 0;

    //
    //   Steam header |???| Server name | Game map name | 
    //                |   |             |               |
    //     FF FF FF FF I11 EXAMPLE NAME0 chernarusplus0 ...
    // 
    // First 6 bytes can be ignored.
    

    // Read server name.
    for(uint32_t i = 6; i < server_info.size; i++) {
        char ch = server_info.bytes[i];
        if(ch == 0) {
            break;
        }
        server->name[server->name_length++] = ch;
    }

    // Read server map name.
    for(uint32_t i = 7+server->name_length; i < server_info.size; i++) {
        char ch = server_info.bytes[i];
        if(ch == 0) {
            break;
        }
        server->map_name[server->map_name_length++] = ch;
    }

    /*
    for(size_t i = 0; i < server_info.size; i++) {
        char byte = server_info.bytes[i];
        if(byte >= 0x20 && byte <= 0x7E) {
            printf("%c", byte);
        }
        else {
            printf("\033[34m%02X\033[0m ", (uint8_t)byte);
        }
    }

    printf("\n");
    */

    result = true;

out:
    free_string(&server_info);
    free_string(&server_mod_info);

    return result;
}

static
bool all_mods_have_been_installed(struct app_config* cfg, struct dayz_server* server) {
    bool result = false;
    struct string_t path = string_create(0);

    // Check first all mods have been installed.
    for(uint8_t i = 0; i < server->num_mods; i++) {
        struct dayz_mod* mod = &server->mods[i];

        string_clear(&path);
        string_append(&path, cfg->dayz_workshop_dir, -1);
        string_append(&path, mod->workshop_id, -1);
        string_nullterm(&path);

        if(access(path.bytes, F_OK) != 0) {
            goto out;
        }
    }

    result =true;
out:
    free_string(&path);
    return result;
}

void launch_dayz(struct app_config* cfg, char* game_addr, char* game_port, struct dayz_server* server) {
    struct string_t cmd = string_create(0);
    struct string_t symlink_name = string_create(0);
    struct string_t symlink_dst = string_create(0);
    struct string_t launch_params = string_create(0);


    if(!all_mods_have_been_installed(cfg, server)) {
        printf("Not all mods have been installed.\n");
        return;
    }

    printf("\033[32m + All mods have been installed! +\033[0m\n");


    // TODO: Do this with dirent instead, much safer.

    // Reset mod symlinks.
    string_append(&cmd, "rm -rf ", -1);
    string_append(&cmd, cfg->dayz_steam_dir, -1);
    string_append(&cmd, "\\@*", -1);
    string_nullterm(&cmd);
   
    printf("Execute: %s\n", cmd.bytes);
    system(cmd.bytes);
    string_clear(&cmd);

    string_append(&launch_params, "steam -applaunch 221100 -nolauncher ", -1);

    // When creating symlinks to mods, we can collect the mod launch parameters at the same time.
    string_append(&launch_params, "-mod='", -1);

    for(uint8_t i = 0; i < server->num_mods; i++) {
        struct dayz_mod* mod = &server->mods[i];

        string_clear(&symlink_name);
        string_clear(&symlink_dst);

        string_append(&symlink_name, cfg->dayz_steam_dir, -1);
        string_append(&symlink_name, "@", 1);
        string_append(&symlink_name, mod->workshop_id, -1);
        string_nullterm(&symlink_name);


        string_append(&symlink_dst, cfg->dayz_workshop_dir, -1);
        string_append(&symlink_dst, mod->workshop_id, -1);
        string_nullterm(&symlink_dst);

        printf("%s -> %s\n",
                symlink_name.bytes,
                symlink_dst.bytes);

        if(symlink(symlink_dst.bytes, symlink_name.bytes) != 0) {
            fprintf(stderr, "%s: Failed to create symlink | %s\n", __func__, strerror(errno));
            goto error;
        }

        // Add mod to launch parameters.
        string_append(&launch_params, "@", 1);
        string_append(&launch_params, mod->workshop_id, -1);
        if(i+1 < server->num_mods) {
            string_append(&launch_params, ";", 1);
        }
    }

    string_append(&launch_params, "' ", -1);
    string_append(&launch_params, "-connect=", -1);
    string_append(&launch_params, game_addr, -1);
    string_append(&launch_params, " -port=", -1);
    string_append(&launch_params, game_port, -1);
    //string_append(&launch_params, " &", -1);
    printf("%s\n", launch_params.bytes);
    system(launch_params.bytes);

error:
    free_string(&launch_params);
    free_string(&symlink_name);
    free_string(&symlink_dst);
    free_string(&cmd);
}

