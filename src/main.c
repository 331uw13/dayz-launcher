
#include <ncurses.h>

#include "dzlauncher.h"




void init_curses() {
    initscr();
    raw();    // Do not generate any interupt signal.
    noecho(); // Do not print characters that user types.
    keypad(stdscr, 1); // Enable arrow, backspace keys etc...
    curs_set(0); // Hide cursor.
    start_color();
    use_default_colors();

    init_pair(COLOR_BLACK,   COLOR_BLACK, -1);
    init_pair(COLOR_RED,     COLOR_RED, -1);
    init_pair(COLOR_GREEN,   COLOR_GREEN, -1);
    init_pair(COLOR_YELLOW,  COLOR_YELLOW, -1);
    init_pair(COLOR_BLUE,    COLOR_BLUE, -1);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, -1);
    init_pair(COLOR_CYAN,    COLOR_MAGENTA, -1);
    init_pair(COLOR_CURSOR,  COLOR_BLACK, COLOR_CYAN);

}


/*
void user_interaction_loop() {
    while(ctx->running) {
        move(0, 0);
        clrtobot();
        printw("Open-DZ-Launcher  -  Press [q] to exit.");



        refresh();
    }    
}
*/

int main() {

    /*
    struct opendzl_ctx ctx;

    init_curses();
    opendzl_setup(&ctx);
    opendzl_open_logfile(&ctx);

    construct_tui(&ctx);
    user_interaction_loop(&ctx);


    opendzl_free(&ctx);
    opendzl_close_logfile(&ctx);
    endwin();
    */
    
    struct dayz_server server_info;

    if(get_server_info("192.168.1.109", 27016, &server_info)) {
        for(uint8_t i = 0; i < server_info.num_mods; i++) {
            printf("%s - %u\n", server_info.mods[i].name, server_info.mods[i].workshop_id);
        }
    }
    

    /*
    init_curses();


    // TODO: Maybe add signal handler if something goes wrong.
    curs_set(1);
    endwin();
    */
    return 0;
}
