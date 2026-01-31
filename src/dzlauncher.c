#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses.h>

#include "server_query.h"
#include "a2s_parser.h"
#include "a2s_decoder.h"
#include "dzlauncher.h"


#define OPENDZL_LOG_FILEPATH "/tmp/open-dz-launcher.log"
#define OPENDZL_MAX_ERROR_MESSAGES 8
#define OPENDZL_ERRMSG_MAX_LENGTH 511

#define OPENDZL_TUI_ELEM_COLUMNS 16
#define OPENDZL_TUI_ELEM_ROWS 16

/*
void opendzl_setup(struct opendzl_ctx* ctx) {
    ctx->running = true;
    for(uint16_t i = 0; i < OPENDZL_MAX_ERROR_MESSAGES; i++) {
        ctx->error_messages[i] = create_string();
    }

    opendzltui_create(&ctx->tui, OPENDZL_TUI_ELEM_COLUMNS, OPENDZL_TUI_ELEM_ROWS);
}

void opendzl_free(struct opendzl_ctx* ctx) {
    for(uint16_t i = 0; i < OPENDZL_MAX_ERROR_MESSAGES; i++) {
        string_free(&ctx->error_messages[i]);
    }

    opendzltui_free(&ctx->tui);
}

void opendzl_open_logfile(struct opendzl_ctx* ctx) {
    ctx->logfile_fd = open(OPENDZL_LOG_FILEPATH, O_CREAT, S_IRUSR | S_IWUSR);
}

void opendzl_close_logfile(struct opendzl_ctx* ctx) {
    if(ctx->logfile_fd > 0) {
        close(ctx->logfile_fd);
        ctx->logfile_fd = -1;
    }
}
    
void opendzl_draw_errmsg(struct opendzl_ctx* ctx) {
    if(ctx->num_error_messages == 0) {
        return;   
    }

    attron(COLOR_PAIR(COLOR_RED));

    for(uint16_t i = 0; i < ctx->num_error_messages; i++) {
        mvaddstr(i, 0, ctx->error_messages[i].bytes);
    }

    attroff(COLOR_PAIR(COLOR_RED));
}

void opendzl_write_errmsg_ex
(
    struct opendzl_ctx* ctx,
    const char* msg,
    const char* from_file,
    int from_line,
    const char* from_func,
    ...
){
    va_list args;
    va_start(args);

    memset(ctx->errmsg_buffer, 0, sizeof(ctx->errmsg_buffer));
    const int errmsg_length = vsnprintf(ctx->errmsg_buffer, OPENDZL_ERRMSG_MAX_LENGTH, msg, args);

    bool error_messages_full = false;

    if(ctx->num_error_messages+1 > OPENDZL_MAX_ERROR_MESSAGES) {
        //asm("int3");
        for(uint16_t i = 0; i < OPENDZL_MAX_ERROR_MESSAGES-1; i++) {
            struct string_t* A = &ctx->error_messages[i];
            struct string_t* B = &ctx->error_messages[i+1];

            string_move(A, B->bytes, B->size);
        }

        error_messages_full = true;
    }

    if(!error_messages_full) {
        ctx->num_error_messages++;
    }
    
    struct string_t* errmsg_str = &ctx->error_messages[ctx->num_error_messages-1]; 
    string_move(errmsg_str, ctx->errmsg_buffer, errmsg_length);

    va_end(args);
}
*/

bool get_server_info(char* addr, uint16_t port, struct dayz_server* server) {
    bool result = false;

    struct string_t server_info = create_string();
    struct string_t server_mod_info = create_string();

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


    memset(server->name, 0, OPENDZL_MAX_SERVER_NAME);
    memset(server->map_name, 0, OPENDZL_MAX_SERVER_MAP_NAME);

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
    string_free(&server_info);
    string_free(&server_mod_info);

    return result;
}







