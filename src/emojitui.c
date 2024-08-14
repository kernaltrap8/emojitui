// emojitui Copyright (C) 2024 kernaltrap8
// This program comes with ABSOLUTELY NO WARRANTY
// This is free software, and you are welcome to redistribute it
// under certain conditions

/*
        emojitui.c
*/

#include "emojitui.h"
#include <ctype.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION "0.5a"

#define ABOUT_WINDOW_HEIGHT 10
#define ABOUT_WINDOW_WIDTH 40
#define SEARCH_WINDOW_HEIGHT 20
#define SEARCH_WINDOW_WIDTH 80

void draw_menu(int highlight, int start_col, char **menu_choices,
               int menu_choices_count) {
  for (int i = 0; i < menu_choices_count; ++i) {
    if (highlight == i + 1) {
      attron(COLOR_PAIR(4)); // Highlight color
      mvprintw(1 + i, start_col, "%s", menu_choices[i]);
      attroff(COLOR_PAIR(4)); // Turn off highlight color
    } else {
      mvprintw(1 + i, start_col, "%s", menu_choices[i]);
    }
  }
}

void clear_menu(int start_col, int menu_choices_count) {
  attron(COLOR_PAIR(2)); // Use the color pair for black background
  for (int i = 0; i < menu_choices_count; ++i) {
    mvprintw(1 + i, start_col, "                "); // Clear with spaces
  }
  attroff(COLOR_PAIR(2)); // Turn off the color pair after clearing
}

void about_window(void) {
  int row, col;
  getmaxyx(stdscr, row, col); // Get screen dimensions

  // Calculate window position
  int win_height = ABOUT_WINDOW_HEIGHT;
  int win_width = ABOUT_WINDOW_WIDTH;
  int win_start_y = (row - win_height) / 2;
  int win_start_x = (col - win_width) / 2;

  // Create a new window
  WINDOW *info_win = newwin(win_height, win_width, win_start_y, win_start_x);

  // Set the background color of the window to white
  wbkgd(info_win, COLOR_PAIR(1)); // Assuming COLOR_PAIR(1) is white on black

  // Draw a box around the window
  box(info_win, 0, 0);

  // Display text with appropriate color pair for text
  char version1[35] = "emojitui version ";
  char version2[] = VERSION;
  strcat(version1, version2);
  wattron(info_win,
          COLOR_PAIR(
              4)); // Assuming COLOR_PAIR(4) is black text on white background
  mvwprintw(info_win, 1, (win_width - strlen(version1)) / 2, version1);
  mvwprintw(info_win, 2,
            (win_width - strlen("Copyright (C) 2024 kernaltrap8")) / 2,
            "Copyright (C) 2024 kernaltrap8");
  mvwprintw(info_win, 3,
            (win_width - strlen("Licensed under BSD-2-Clause")) / 2,
            "Licensed under BSD-2-Clause");

  // Create OK button
  mvwprintw(info_win, win_height - 2, (win_width - 6) / 2, "[ OK ]");
  wattroff(info_win, COLOR_PAIR(4)); // Turn off text color after use

  // Refresh the window to show content
  wrefresh(info_win);

  int ch;
  while ((ch = wgetch(info_win)) !=
         '\n') {          // Wait for Enter key to close the window
    if (ch == KEY_F(1)) { // Handle F1 for exit
      endwin();
      exit(0);
    }
  }

  // Close the window
  delwin(info_win);

  // Clear the area where the window was
  attron(COLOR_PAIR(2)); // Use background color pair (e.g., black background)
  for (int y = win_start_y; y < win_start_y + win_height; ++y) {
    mvhline(y, win_start_x, ' ', win_width); // Clear the window area
  }
  attroff(COLOR_PAIR(2)); // Turn off the background color pair

  // Refresh the main screen to clear the old window area
  refresh(); // Ensure the screen updates
}

int search_emoji(const char *query) {
  for (unsigned long i = 0; i < EMOJI_LIST_SIZE; ++i) {
    if (strcmp(query, emoji_list[i]) == 0) {
      return i; // Return index of the match
    }
  }
  return -1; // No match found
}

void redraw_black_bar(WINDOW *win, int middle_row, int bar_start_x,
                      int bar_width) {
  // Draw the black bar
  wattron(win, COLOR_PAIR(2)); // Ensure COLOR_PAIR(2) is set to black
  mvwhline(win, middle_row, bar_start_x, ' ', bar_width); // Draw the black bar
  wattroff(win, COLOR_PAIR(2)); // Turn off color pair after use
}

void search_window(void) {
  int row, col;
  getmaxyx(stdscr, row, col); // Get screen dimensions

  // Calculate window position
  int win_height = SEARCH_WINDOW_HEIGHT;
  int win_width = SEARCH_WINDOW_WIDTH;
  int win_start_y = (row - win_height) / 2;
  int win_start_x = (col - win_width) / 2;

  // Create a new window
  WINDOW *search_win = newwin(win_height, win_width, win_start_y, win_start_x);

  // Set the background color of the window
  wbkgd(search_win,
        COLOR_PAIR(1)); // Ensure COLOR_PAIR(1) is black or appropriate

  // Draw a box around the window
  box(search_win, 0, 0);

  // Display text with appropriate color pair for text
  wattron(search_win,
          COLOR_PAIR(
              4)); // Assuming COLOR_PAIR(4) is black text on white background
  mvwprintw(search_win, 1, (win_width - strlen("Search")) / 2, "Search");
  wattroff(search_win, COLOR_PAIR(4)); // Turn off text color after use

  char query[100] = {0}; // Buffer for user input
  int query_length = 0;  // Length of the current query
  int scroll_offset = 0; // Current scroll offset
  int middle_row = win_height / 2;

  mvwprintw(search_win, middle_row, 1, "Enter search query: ");

  // Draw the black bar before user input
  int bar_start_x =
      1 + strlen("Enter search query: ");      // Start position of the bar
  int bar_width = win_width - bar_start_x - 1; // Width of the bar

  // Draw the initial black bar
  redraw_black_bar(search_win, middle_row, bar_start_x, bar_width);

  // Display the initial state with an empty bar
  mvwprintw(search_win, middle_row, bar_start_x, "%s", query);
  wrefresh(search_win);

  int ch;
  while ((ch = wgetch(search_win)) !=
         '\n') {          // Wait for Enter key to close the window
    if (ch == KEY_F(1)) { // Handle F1 for exit
      endwin();
      exit(0);
    } else if (ch == KEY_BACKSPACE ||
               ch == 127) { // Handle backspace (127 for some terminals)
      if (query_length > 0) {
        query_length--;
        query[query_length] = '\0'; // Remove last character from query
        if (scroll_offset > query_length) {
          scroll_offset = query_length; // Adjust scroll offset
        }
      }
    } else if (isprint(ch) &&
               query_length <
                   (int)sizeof(query) - 1) { // Handle printable characters
      // Add character to the query buffer
      if (query_length - scroll_offset >= bar_width) {
        scroll_offset++; // Scroll the text if it exceeds visible area
      }
      query[query_length++] = (char)ch;
      query[query_length] = '\0'; // Null-terminate the string
    }

    // Clear the previous input
    mvwprintw(search_win, middle_row, bar_start_x, "%*s", bar_width,
              " "); // Clear the bar

    // Redraw the black bar
    redraw_black_bar(search_win, middle_row, bar_start_x, bar_width);

    // Display the scrolled portion of the query in the bar
    if (scroll_offset > query_length) {
      scroll_offset = query_length; // Adjust scroll offset if needed
    }
    mvwprintw(search_win, middle_row, bar_start_x, "%s", query + scroll_offset);

    // Refresh window to show updates
    wrefresh(search_win);
  }

  // Perform search only when Enter key is pressed
  int index = search_emoji(query);
  mvwprintw(search_win, middle_row + 2, 1, "Search result: ");
  if (index != -1) {
    mvwprintw(search_win, middle_row + 3, 1, "Found '%s' at index %d",
              emoji_list[index], index);
  } else {
    mvwprintw(search_win, middle_row + 3, 1, "No match found for '%s'", query);
  }

  // Create OK button
  mvwprintw(search_win, win_height - 2, (win_width - 6) / 2, "[ OK ]");
  wrefresh(search_win);

  // Wait for user to press Enter or F1 key to exit
  while ((ch = wgetch(search_win)) != '\n') {
    if (ch == KEY_F(1)) { // Handle F1 for exit
      endwin();
      exit(0);
    }
  }

  // Close the window
  delwin(search_win);

  // Clear the area where the window was
  attron(COLOR_PAIR(2)); // Use background color pair (e.g., black background)
  for (int y = win_start_y; y < win_start_y + win_height; ++y) {
    mvhline(y, win_start_x, ' ', win_width); // Clear the window area
  }
  attroff(COLOR_PAIR(2)); // Turn off the background color pair

  // Refresh the main screen to clear the old window area
  refresh(); // Ensure the screen updates
}

void init_screen(void) {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  if (has_colors()) {
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE); // For white background
    init_pair(2, COLOR_BLACK, COLOR_BLACK); // For clearing the menu to black
    init_pair(3, COLOR_BLACK, COLOR_WHITE); // For dropdown menu background
    init_pair(4, COLOR_BLACK, COLOR_WHITE); // For highlighted text
  }

  int row, col;
  getmaxyx(stdscr, row, col);

  attron(COLOR_PAIR(1));

  // Top banner
  mvhline(0, 0, ' ', col);          // Fill the top line with spaces
  mvprintw(0, 0, "emojitui alpha"); // Banner text
  mvprintw(0, 20, "File");          // File option
  mvprintw(0, 27, "Edit");          // Edit option

  // Bottom banner
  mvhline(row - 1, 0, ' ', col);
  mvprintw(row - 1, (col - 16) / 2, "Press F1 to Exit");

  refresh();

  int ch;
  int highlight = 1;
  int menu_active = 0;
  int start_col = 20;
  int file_menu_col = 20; // File menu start column
  int edit_menu_col = 27; // Edit menu start column
  int file_menu_highlight = 1;
  int edit_menu_highlight = 1;

  while ((ch = getch()) != KEY_F(1)) {
    switch (ch) {
    case KEY_LEFT:
      if (menu_active && start_col == edit_menu_col) {
        clear_menu(edit_menu_col, EDIT_MENU_CHOICES); // Clear the "Edit" menu
        start_col = file_menu_col;
        highlight = file_menu_highlight;
        draw_menu(highlight, start_col, file_menu_choices, FILE_MENU_CHOICES);
      }
      break;

    case KEY_RIGHT:
      if (menu_active && start_col == file_menu_col) {
        clear_menu(file_menu_col, FILE_MENU_CHOICES); // Clear the "File" menu
        start_col = edit_menu_col;
        highlight = edit_menu_highlight;
        draw_menu(highlight, start_col, edit_menu_choices, EDIT_MENU_CHOICES);
      }
      break;

    case KEY_UP:
      if (menu_active) {
        if (start_col == file_menu_col && highlight > 1) {
          highlight--;
        } else if (start_col == edit_menu_col && highlight > 1) {
          highlight--;
        }
        clear_menu(start_col, (start_col == file_menu_col) ? FILE_MENU_CHOICES
                                                           : EDIT_MENU_CHOICES);
        draw_menu(highlight, start_col,
                  (start_col == file_menu_col) ? file_menu_choices
                                               : edit_menu_choices,
                  (start_col == file_menu_col) ? FILE_MENU_CHOICES
                                               : EDIT_MENU_CHOICES);
      }
      break;

    case KEY_DOWN:
      if (menu_active) {
        if (start_col == file_menu_col && highlight < FILE_MENU_CHOICES) {
          highlight++;
        } else if (start_col == edit_menu_col &&
                   highlight < EDIT_MENU_CHOICES) {
          highlight++;
        }
        clear_menu(start_col, (start_col == file_menu_col) ? FILE_MENU_CHOICES
                                                           : EDIT_MENU_CHOICES);
        draw_menu(highlight, start_col,
                  (start_col == file_menu_col) ? file_menu_choices
                                               : edit_menu_choices,
                  (start_col == file_menu_col) ? FILE_MENU_CHOICES
                                               : EDIT_MENU_CHOICES);
      }
      break;

    case 10: // Enter key
      if (menu_active) {
        clear_menu(start_col, (start_col == file_menu_col)
                                  ? FILE_MENU_CHOICES
                                  : EDIT_MENU_CHOICES); // Clear the menu before
                                                        // showing the selection
        if (start_col == file_menu_col) {
          switch (highlight) {
          case 1: // "Search"
            search_window();
            break;
          case 2: // "Exit"
            endwin();
            exit(0); // Exit the program
            break;
          }
        } else if (start_col == edit_menu_col) {
          switch (highlight) {
          case 1: // "About"
            about_window();
            break;
          }
        }
        menu_active = 0;
      } else {
        menu_active = 1;
      }
      break;

    case 27: // Escape key to exit
      menu_active = 0;
      clear_menu(file_menu_col, FILE_MENU_CHOICES); // Clear the "File" menu
      clear_menu(edit_menu_col, EDIT_MENU_CHOICES); // Clear the "Edit" menu
      break;

    default:
      break;
    }

    if (menu_active) {
      attron(COLOR_PAIR(3)); // Set background color for menu
      clear_menu(start_col, (start_col == file_menu_col)
                                ? FILE_MENU_CHOICES
                                : EDIT_MENU_CHOICES); // Clear the menu area
      draw_menu(
          highlight, start_col,
          (start_col == file_menu_col) ? file_menu_choices : edit_menu_choices,
          (start_col == file_menu_col) ? FILE_MENU_CHOICES : EDIT_MENU_CHOICES);
      attroff(COLOR_PAIR(3)); // Turn off background color
    }

    refresh();
  }

  attroff(COLOR_PAIR(1)); // Turn off the color pair after usage
  endwin();
}

int main(void) {
  init_screen();
  return 0;
}
