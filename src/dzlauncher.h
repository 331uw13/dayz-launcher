#ifndef DZLAUNCHER_H
#define DZLAUNCHER_H

#include "app_config.h"
#include "dayz_mod.h"
#include "dayz_server.h"


bool query_dayz_server(char* addr, uint16_t port, struct dayz_server* server);
void launch_dayz(struct app_config* cfg, char* game_addr, char* game_port, struct dayz_server* server);




#endif
