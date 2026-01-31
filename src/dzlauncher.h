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

bool get_server_info(char* addr, uint16_t port, struct dayz_server* server);

/*
struct dzl_ctx {
    bool            running;
    char            errmsg_buffer[OPENDZL_ERRMSG_MAX_LENGTH+1];
    struct string_t error_messages[OPENDZL_MAX_ERROR_MESSAGES];
    uint16_t        num_error_messages;
    int             logfile_fd;

};



void dzl_setup(struct opendzl_ctx* ctx);
void dzl_free(struct opendzl_ctx* ctx);
void dzl_open_logfile(struct opendzl_ctx* ctx);
void dzl_close_logfile(struct opendzl_ctx* ctx);
void dzl_draw_errmsg(struct opendzl_ctx* ctx);
*/



/*
#define opendzl_write_errmsg(ctx, msg, ...)\
    opendzl_write_errmsg_ex(ctx, msg, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

void opendzl_write_errmsg_ex
(
    struct opendzl_ctx* ctx,
    const char* msg,
    const char* from_file,
    int from_line,
    const char* from_func,
    ...
);
*/




#endif
