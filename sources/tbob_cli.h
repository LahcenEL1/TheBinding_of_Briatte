//~ #ifndef _CLI
//~ #define _CLI

#include <ncurses.h>

#include "tbob_logic.h"
#include "stage.h"


// TBOB CONSTANTS
// NCURSES VARS
#define H_TITLE 	"The Binding of Briatte"
#define M_LEFT		"left"
#define M_RIGHT		"right"
#define M_UP		"up"
#define M_DOWN		"down"

#define W_MARGIN	6
#define H_MARGIN	6

void wprint_stage ( WINDOW * win, int lines, int cols, stage_t * s, int * abs_y, int * abs_x, point p);

void wprint_header ( char * h_title, char * h_msg);

void wprint_game_intro ( WINDOW * w_main);

void wprint_game_win (WINDOW * win, int _LINES, int _COLS, int h_margin, int w_margin, stage_t s, int * y, int *x, point p);

void wtbob_blink_character ( WINDOW * win, int top, int left, int y, int x, char c);

void ncurses_game_loop ( WINDOW * w_main, player_t p, stage_t s);

void n_curses ( player_t p, stage_t s);

//~ #endif
