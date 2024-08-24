// emojitui Copyright (C) 2024 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

/*
        emojitui.h
*/

#include <ncurses.h>

#ifndef __EMOJITUI_H_
#define __EMOJITUI_H_
typedef enum { MAIN_VIEW, ABOUT_VIEW, SEARCH_VIEW } ActiveView;
// function prototypes
void draw_menu(int highlight, int start_col, char **menu_choices,
               int menu_choices_count);
void clear_menu(int start_col, int menu_choices_count);
void handle_resize();
void about_window(void);
int search_emoji(const char *query);
void redraw_black_bar(WINDOW *win, int middle_row, int bar_start_x,
                      int bar_width);
void search_window(void);
void init_screen(void);

// File menu options
#define FILE_MENU_CHOICES 2
char *file_menu_choices[] = {"Search", "Exit"};

// Edit menu options
#define EDIT_MENU_CHOICES 1
char *edit_menu_choices[] = {"About"};

const char *emoji_list[] = {"smile", "wink", "thumbs up", "heart",
                            "laugh", "cry",  "angry",     "surprised"};
const char *emojis[] = {"😊", "😉", "👍" "❤️","😂", "😢", "😠", "😲"};
#define EMOJI_LIST_SIZE (sizeof(emoji_list) / sizeof(emoji_list[0]))

#define VERSION "0.6"

#define ABOUT_WINDOW_HEIGHT 10
#define ABOUT_WINDOW_WIDTH 40
#define SEARCH_WINDOW_HEIGHT 20
#define SEARCH_WINDOW_WIDTH 80
#define WAYLAND 1
#define X11 2

#endif
