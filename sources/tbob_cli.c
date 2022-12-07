#include <string.h>
#include <time.h>

#include "tbob_cli.h"


double gettime () {
	struct timespec ts;
	clock_gettime ( CLOCK_MONOTONIC, &ts);
	return ts.tv_sec*1e0 + ts.tv_nsec*1e-9;
}

void wprint_stage ( WINDOW * win, int lines, int cols, stage_t * s, int * abs_y, int * abs_x, point p)
{
	*abs_x = ((cols - 2) / 2) - (D_WIDTH),
	*abs_y = ((lines - 2) / 2) - (D_HEIGHT / 2);

	int hgx = *abs_x - (D_WIDTH * 2 + 2) > 0 ? *abs_x - (D_WIDTH * 2 + 2) : *abs_x,
		hgy = *abs_y - (D_HEIGHT+1) > 0 ? *abs_y - (D_HEIGHT+1) : *abs_y;
	
	int hgi = p.y - (*abs_y / (D_HEIGHT+1)),
		hgj = p.x - (*abs_x / (D_WIDTH*2 + 2));
	
	int n_j = (cols - 2) / (D_WIDTH * 2 + 2),//((cols  - hgx) - 2) / (D_WIDTH * 2 + 2),
		n_i = (lines - 2) / (D_HEIGHT+1);//((lines - hgy) - 2) / (D_HEIGHT + 1);
	
	for ( int i = 0; i < n_i; i++)
		for ( int j = 0; j < n_j; j++)
		{
			for ( int k = 0; k < D_HEIGHT; k++)
				for ( int l = 0; l < D_WIDTH; l++)
				{
					int y = hgy + ( i * (D_HEIGHT + 1)) + k,
						x = hgx + j*((D_WIDTH + 1) * 2) + (l*2),
						_i = i+hgi,
						_j = j+hgj;
					if ( _i < 0 || _i > s->height - 1 || _j < 0 || _j > s->width - 1)
						continue;
					if ( s->mtx[_i][_j] && x < cols - 2 && y < lines - 1 && x > 0 && y > 0 && _i < s->height && _j < s->width)
					{
						// Enable "CSS"
						if ( _i == p.y && _j == p.x && ((k == 0 || k == D_HEIGHT -1) || (l == 0 || l == D_WIDTH - 1)))	wattron ( win, A_STANDOUT);
						mvwprintw ( win, y, x, "%c ", s->mtx[_i][_j]->piece[k][l]);
						wrefresh ( win);
						// Disable "CSS"
						if ( _i == p.y && _j == p.x && ((k == 0 || k == D_HEIGHT -1) || (l == 0 || l == D_WIDTH - 1)))	wattroff ( win, A_STANDOUT);

					}
				}
		}
	return;
}

void wprint_header ( char * h_title, char * h_msg)
{
	if ( h_title)	mvprintw ( 0, (COLS / 2) - (strlen(h_title) / 2), "%s", h_title);
	if ( h_msg)		mvprintw ( 1, (COLS / 2) - (strlen(h_msg) / 2), "%s", h_msg);


	mvprintw ( LINES - 2, COLS/2 - 9, "action :");

	attron (A_STANDOUT);
	mvprintw ( LINES - 1, (COLS / 2) - 45 < 0 ? 0 : (COLS / 2) - 45, "  MOVE - q : left  d : right  z : up  s : down - ATTACK - up, down, left, right - QUIT : w  ");
	attroff (A_STANDOUT);
	refresh();
}

void wprint_game_intro ( WINDOW * w_main)
{
	double t1 = gettime();
	double t2 = t1;
	int i = 0;
	char * tab [4] = {"Briatte and his pets lived alone in a small house on a hill.", "Briatte kept to himself, drawing pictures and playing with his toys as his pets watched Christian broadcasts on the television.", "Life was simple, and they were both happy.That was, until the day Isaac's pets heard a voice from above :", "\"Your handler has become corrupted by sin!He needs to be saved!\""};
	
	while ( i < 4)
	{
		t2 = gettime();
		if ( t2 - t1 > 2)
		{
			mvwprintw ( w_main, 3+i, 2*i+2, "%s", tab[i]);
			t1 = gettime();
			wrefresh ( w_main);
			i++;
		}		
	}	
}

void wprint_game_win (WINDOW * win, int _LINES, int _COLS, int h_margin, int w_margin, stage_t s, int * y, int *x, point p)
{
	int lines = _LINES - h_margin, cols = _COLS - w_margin;
	wclear(win);
	wresize ( win, lines, cols);
	box ( win, ACS_VLINE, ACS_HLINE);
	wprint_stage ( win, lines, cols, &s, y, x, p);
	//~ wrefresh ( win);	
}


void wtbob_blink_character ( WINDOW * win, int top, int left, int y, int x, char c)
{
	// Blink charachter
	mvwprintw ( win, top + y, left + 2 * x, "%c", c);
	wmove ( win, top + y, left + 2 * x);
}

void ncurses_game_loop ( WINDOW * w_main, player_t p, stage_t s)
{
	int x = D_WIDTH / 2, y = D_HEIGHT / 2, abs_x = ((COLS - W_MARGIN - 2) / 2) - (D_WIDTH), abs_y = ((LINES - H_MARGIN - 2) / 2) - (D_HEIGHT / 2), cur = 0;
	int quit = 0, key = 410;
	do
	{
		if ( key == TBOB_QUIT)		quit = 1;
		if ( key == KEY_RESIZE)		clear(), wprint_header ( H_TITLE, NULL), wprint_game_win ( w_main, LINES, COLS, W_MARGIN, W_MARGIN, s, &abs_y, &abs_x, s.p[cur]);

		int t_act, a_act, a_move = -1, v_move = 0, _y = y, _x = x, dx = 0, dy = 0, room_f = 0, move_f = 0;

		t_act = tbob_get_action_type ( key);
		if ( t_act == 1)
		{
			a_move = tbob_get_action_move ( key, &dy, &dx);	// Action move
			v_move = tbob_is_valide_move ( s, cur, y + dy, x + dx);
			if ( v_move == 1)
			{
				x += dx, y += dy;
				move_f = 1;
			}
			else if ( v_move == 2)
			{
				int i = s.p[cur].y + dy,	j = s.p[cur].x + dx;
				cur = tbob_get_next_room ( s, i, j);
				
				if 		( a_move == 0)	y = D_HEIGHT / 2, x = D_WIDTH - 2;
				else if ( a_move == 1)	y = D_HEIGHT - 2, x = D_WIDTH / 2;
				else if ( a_move == 2)	y = D_HEIGHT / 2, x = 1;
				else if ( a_move == 3)	y = 1, x = D_WIDTH / 2;
				room_f = 1;
			}
		}
		if ( move_f)	mvwprintw ( w_main, (abs_y) + (_y), (abs_x) + 2 * (_x), "%c", s.mtx[s.p[cur].y][s.p[cur].x]->piece[_y][_x]);
		if ( room_f)	wprint_game_win ( w_main, LINES, COLS, H_MARGIN, W_MARGIN, s, &abs_y, &abs_x, s.p[cur]);
		wtbob_blink_character ( w_main, abs_y, abs_x, y, x, 'P');
		wrefresh ( w_main);
		// Get input/event
		key = getch();
	} while ( !quit);	
}

void n_curses ( player_t p, stage_t s)
{
	// Init n_curses
	initscr();
	noecho();
	cbreak();
	nodelay ( stdscr, TRUE);
	keypad ( stdscr, TRUE);

	// Init main game window
	int lines = LINES - H_MARGIN, cols = COLS - W_MARGIN;
	WINDOW * w_main = subwin ( stdscr, lines, cols, H_MARGIN / 2, W_MARGIN / 2);

	ncurses_game_loop ( w_main, p, s);

	delwin ( w_main);
	// Closing ncurses
	nodelay ( stdscr, FALSE);
	nocbreak();
	echo();
	endwin();	
}
