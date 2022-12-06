#include <time.h>
//~ #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <ncurses.h>

#include "rtbob.h"
#include "itbob.h"
#include "mtbob.h"
#include "crud_piece.h"

// TBOB CONSTANTS
#define N_ROOMS		14
#define D_HEIGHT	9
#define D_WIDTH		15
#define H_TITLE 	"The Binding of Briatte"
#define M_LEFT		"left"
#define M_RIGHT		"right"
#define M_UP		"up"
#define M_DOWN		"down"

// NCURSES VARS
#define W_MARGIN	6
#define H_MARGIN	6

// NCURSES KEYS
#define TBOB_QUIT	'w'
#define TBOB_UP 	'z'
#define TBOB_DOWN 	's'
#define TBOB_LEFT 	'q'
#define TBOB_RIGHT 	'd'

typedef struct
{
	int y, x;
	int card[4];
}point;

typedef struct
{
	// Position
	int y, x;
	// Stat
	char * name;
	float hpMax, dmg, shield;
	int flight;

} player_t;

typedef struct
{
	point p[14];
	int height, width;
	room_t *** mtx; // Matrice de pointeur
}stage_t;

double gettime () {
	struct timespec ts;
	clock_gettime ( CLOCK_MONOTONIC, &ts);
	return ts.tv_sec*1e0 + ts.tv_nsec*1e-9;
}

void print_stage_tab ( room_t * mtx[14][14], int height, int width)
{
	printf ("Size(%d, %d)\n+", height, width);
	for ( int i = 0; i < width; i++) printf ("++++++");
	printf ("+\n");
	for (int i = 0; i < height; i++)
	{
		printf ("+");
		for (int j = 0; j < width; j++)
		{
			if ( mtx[i][j] == NULL)
				printf ("      ");
			else
			{
				if ( mtx[i][j]->kind == NORMAL_ROOM)
					printf ( " [%2d] ", mtx[i][j]->id + 1);
				else if ( mtx[i][j]->kind == SPAWN_ROOM)
					printf ( " [SR] ");
				else if ( mtx[i][j]->kind == ITEM_ROOM)
					printf ( " [IR] ");
				else if ( mtx[i][j]->kind == ITEM_BONUS_ROOM)
					printf ( " [IB] ");
				else if ( mtx[i][j]->kind == BOSS_ROOM)
					printf ( " [BR] ");
			}
		}
		printf ("+\n");
	}
	printf ("+");
	for ( int i = 0; i < width; i++) printf ("++++++");
	printf ("+\n");
}

void print_stage ( stage_t s)
{
	printf ("(%d, %d)\n", s.height, s.width);
	for (int i = 0; i < s.height; i++)
	{
		printf ("|");
		for (int j = 0; j < s.width; j++)
		{
			if ( s.mtx[i][j] == NULL)
				printf ("      ");
			else
			{
				if ( s.mtx[i][j]->kind == NORMAL_ROOM)
					printf ( " [%2d] ", s.mtx[i][j]->id + 1);
				else if ( s.mtx[i][j]->kind == SPAWN_ROOM)
					printf ( " [SP] ");
				else if ( s.mtx[i][j]->kind == ITEM_ROOM)
					printf ( " [IR] ");
				else if ( s.mtx[i][j]->kind == ITEM_BONUS_ROOM)
					printf ( " [IB] ");
				else if ( s.mtx[i][j]->kind == BOSS_ROOM)
					printf ( " [BR] ");
			}
		}
		printf ("|\n");
	}
}

void print_stage_level ( stage_t * s)
{
	FILE * fout = fopen ( "test.out", "w");
	int i = 0, j = 0, k =0, l = 0;
	fprintf ( fout, "(%d, %d)\n", s->height, s->width);
	for ( i = 0; i < s->height; i++)
	{
		for ( j = 0; j < 9 /*s->mtx[i][k]->height*/; j++)
		{
			for ( k = 0; k < s->width; k++)
			{
				for ( l = 0; l < 15 /*s->mtx[i][k]->width*/; l++)
				{
					if ( s->mtx[i][k])
						fprintf ( fout, "%c ", s->mtx[i][k]->piece[j][l]);
					else
						fprintf ( fout, "  ");
				}
				if ( j == 4 && s->mtx[i][k])
				{
					if ( s->mtx[i][k]->piece[j][14] == 'D')
						fprintf ( fout, "D ");
					else
						fprintf ( fout, "  ");
				}
				else
					fprintf ( fout, "  ");
			}
			fprintf ( fout, "\n");
		}
		//~ fprintf ( fout, "\n");
		for ( k = 0; k < s->width; k++)
		{
			if ( !s->mtx[i][k])
				fprintf ( fout, "%*c", 32, ' ');//"                                "
			else
			{
				if ( s->mtx[i][k]->piece[8][7] == 'D')
					fprintf ( fout, "              D                 ");
				else
					fprintf ( fout, "%*c", 32, ' ');//"                                "
			}
		}
		fprintf ( fout, "\n");
	}
	fclose ( fout);
}

room_t * gen_empty_room ( int h, int w, piece_kind kind)
{
	room_t * r = (room_t *) malloc ( sizeof ( room_t));

	r->kind = kind;
	r->height = h;
	r->width = w;
	r->piece = (char **) malloc ( h * sizeof (char*));
	
	for (int i = 0; i < h; i++)
	{
		r->piece[i] = (char *) malloc ( w * sizeof (char));
	}
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			r->piece[i][j] = ' ';
	for (int i = 0; i < h; i++)
	{
		r->piece[i][0] = 'W';
		r->piece[i][w-1] = 'W';
	}
	for (int i = 0; i < w; i++)
	{
		r->piece[0][i] = 'W';
		r->piece[h-1][i] = 'W';
	}
	return r;	
}

room_t * rtbob_clone_room( room_t * r)
{
	room_t * ret = (room_t *) malloc ( sizeof ( room_t));
	ret->kind = r->kind;
	ret->width = r->width;
	ret->height = r->height;
	ret->piece = (char **) malloc ( r->height * sizeof ( char*));
	for (int i = 0; i < r->height; i++)
		ret->piece[i] = (char *) malloc ( r->width * sizeof ( char));
	for (int i = 0; i < r->height; i++)
		for (int j = 0; j < r->width; j++)
			ret->piece[i][j] = r->piece[i][j];
	return ret;
}


// Fonction d'ajout des rooms par insertion et decallage
void add_stage_room ( room_t * s[N_ROOMS][N_ROOMS], room_t * r, int h, int w, int row, int * height, int * width)
{
	//~ printf ("%d - h(%d) w(%d) [%d - %d]\n", row, h, w, (*height), (*width));
	if ( row == 0 && w == 0 && s[h][w] != NULL)
	{
		// Shift left
		for (int i = 0; i < (*height) ; i++)
			for (int j = (*width); j > w; j--)
				s[i][j] = s[i][j-1];
		(*width)++;
		for (int i = 0; i < (*height); i++)
			s[i][w] = NULL;		
	}
	else if ( row == 1 && h == 0 && s[h][w] != NULL)
	{
		// Shift down
		for (int i = (*height); i > 0 ; i--)
			for (int j = 0; j < (*width); j++)
				s[i][j] = s[i-1][j];
		(*height)++;
		for (int i = 0; i < (*width); i++)
			s[h][i] = NULL;		
	}
	else if ( row == 2 && w >= (*width)) (*width)++;
	else if ( row == 3 && h >= (*height)) (*height)++;

	if ( s[h][w] != NULL) printf ("Error !!! %d %d\n", w, h);
	else	s[h][w] = r;		
}

// Fonction de mise a jour des points
void update_stage_metadata ( room_t * mtx[][N_ROOMS], point * p, int * placed, int * h, int * w, int row, int next)
{
		// Update rooms metadata
		if ( row == 0) // Shift left
		{
			p[next].card[0] = 1;
			p[(*placed)].card[2] = 1;
			*h = p[next].y;
			*w = p[next].x == 0 ? 0 : p[next].x - 1;
			if ( p[next].x == 0)
			{
				for ( int j = 0; j < (*placed); j++)
					p[j].x++;
			}
		}
		if ( row == 1) // Shift down
		{
			p[next].card[1] = 1;
			p[(*placed)].card[3] = 1;
			*w = p[next].x;
			*h = p[next].y == 0 ? 0 : p[next].y - 1;
			if ( p[next].y == 0)
			{
				for ( int j = 0; j < (*placed); j++)
					p[j].y++;
			}
		}
		if ( row == 2) //
		{
			p[next].card[2] = 1;
			p[(*placed)].card[0] = 1;
			*h = p[next].y;
			*w = p[next].x + 1;
		}
		if ( row == 3) //
		{
			p[next].card[3] = 1;
			p[(*placed)].card[1] = 1;
			*h = p[next].y + 1;
			*w = p[next].x;			
		}
		p[(*placed)].x = *w;
		p[(*placed)].y = *h;
		/*if ( *placed < 11)*/	(*placed)++;	
		for (int i = 0; i < (*placed); i++)
		{
			for (int j = 0; j < (*placed); j++)
			{
				if ( p[i].x - p[j]. x ==  1 && p[i].y == p[j].y)	p[i].card[0] = 1;
				if ( p[i].y - p[j]. y ==  1 && p[i].x == p[j].x)	p[i].card[1] = 1;
				if ( p[i].x - p[j]. x == -1 && p[i].y == p[j].y)	p[i].card[2] = 1;
				if ( p[i].y - p[j]. y == -1 && p[i].x == p[j].x)	p[i].card[3] = 1;
			}
		}
}

void pick_next_pos ( point * p, int * row, int * next, int placed)
{
	// Choose next point
	do{
		*row = rand() % 4;			// Le sens compris entre 0 et 3
		*next = rand() % (placed);	// Le numero du sommet auquel sera ajouté un autre
	} while ( p[*next].card[*row] == 1);	// Tant que le booleen d'etat de la room choisi indique qu'il est occupé
}

void _init_rooms ( room_t * rooms[], dungeon_rooms dr)
{
	// Init
	for ( int i = 0; i < N_ROOMS; i++) rooms[i] = NULL;
	
	// Special Rooms
	rooms[0] = gen_empty_room ( 9, 15, SPAWN_ROOM);
	rooms[11] = gen_empty_room ( 9, 15, ITEM_ROOM);
	rooms[12] = gen_empty_room ( 9, 15, ITEM_BONUS_ROOM); 
	rooms[13] = gen_empty_room ( 9, 15, BOSS_ROOM);

	// Random rooms
	int id = 0;
	for (int i = 0; i < N_ROOMS; i++)
		if ( rooms[i] == NULL)
		{
			//~ printf ("%d - (%d) ->", i, id);
			int r = rand() % dr.n_rooms;
			rooms[i] = rtbob_clone_room ( dr.rooms[r]);
			rooms[i]->id = id++;
		}
	//~ printf ("\n");
	
	for (int i = 0; i < N_ROOMS; i++)
		for (int y = 0; y < rooms[i]->height; y++)
			for (int x = 0; x < rooms[i]->width; x++)
				if ( rooms[i]->piece[y][x] == 'D')
					rooms[i]->piece[y][x] = 'W';
	
}

void _init_mtx ( room_t * mtx[][N_ROOMS])
{
	// Stage buffer
	for (int i = 0; i < N_ROOMS; i++)
		for (int j = 0; j < N_ROOMS; j++)
			mtx[i][j] = NULL;
}

void _init_points ( point * p, int n_p)
{
	
	for ( int i = 0; i < n_p; i++)
	{
		p[i].x = 0, p[i].y = 0;
		p[i].card[0] = 0;
		p[i].card[1] = 0;
		p[i].card[2] = 0;
		p[i].card[3] = 0;
	}
}

void add_stage_rooms_paths ( room_t * rooms[N_ROOMS], int s_src, int s_dest, int row)
{
	char c = s_src > 10 || s_dest > 10 ? '$' : 'D';
	if ( row == 0)	rooms[s_src]->piece[rooms[s_src]->height / 2][0] = c,  rooms[s_dest]->piece[rooms[s_dest]->height / 2][rooms[s_dest]->width - 1] = c;
	if ( row == 1)	rooms[s_src]->piece[0][rooms[s_src]->width / 2] = c,  rooms[s_dest]->piece[rooms[s_dest]->height - 1][rooms[s_dest]->width / 2] = c;
	if ( row == 2)	rooms[s_src]->piece[rooms[s_src]->height/2][rooms[s_src]->width - 1] = c,  rooms[s_dest]->piece[rooms[s_dest]->height / 2][0] = c;
	if ( row == 3)	rooms[s_src]->piece[rooms[s_src]->height-1][rooms[s_src]->width/2] = c,  rooms[s_dest]->piece[0][rooms[s_dest]->width / 2] = c;
}

void add_stage_rooms ( room_t * mtx[][N_ROOMS], room_t * rooms[N_ROOMS], point * p, int * height, int * width)
{
	// Adding spawn to buffer
	mtx[0][0] = rooms[0];
	(*width)++;
	(*height)++;
	//~ print_stage_tab ( mtx, *height, *width);

	// Adding other rooms
	int h = 0, w = 0, row, next = -1, placed = 1;
	for (int i = 1; i < N_ROOMS; i++)
	{
		// On choisi la prochaine position a l'aide d'un next qui représente l'id de la room
		// 											d'un row compris entre 0 (gauche) 1 (haut) 2 (droite) et 3 (bas) pour 
		// On obtient une position en mixant le numero de room et le sens
		pick_next_pos ( p, &row, &next, placed);
		// On met a jour les positions des rooms, les booleans d'état pour savoir si un coté est disponible ou pas
		update_stage_metadata ( mtx, p, &placed, &h, &w, row, next);
		// On ajoute la room à la matrice à l'aide d'une fonction d'ajout
		add_stage_room ( mtx, rooms[i], h, w, row, height, width);
		// On ajoute une porte/chemin entre la room a placer et la rooms choisie
		add_stage_rooms_paths ( rooms, next, i, row);
		// On s'assure que les pieces speciales n'ont qu'une seule issue
		if ( i > 10) p[i].card[0] = 1, p[i].card[1] = 1, p[i].card[2] = 1, p[i].card[3] = 1;
		//~ print_stage_tab ( mtx, *height, *width);
	}	
}

room_t *** copy_stage_matrix ( room_t * b_mtx[][N_ROOMS], int height, int width)
{
	// Copy data
	room_t *** mtx = ( room_t ***) malloc ( height * sizeof ( room_t **));
	for (int i = 0; i < height; i++)
		mtx[i] = (room_t **) malloc ( width * sizeof ( room_t *));

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			mtx[i][j] = b_mtx[i][j];
		}
	}
	return mtx;
}

void free_stage ( stage_t s)
{
	for (int i = 0; i < s.height; i++)
	{
		for ( int j = 0; j < s.width; j++)
		{
			if ( s.mtx[i][j])
				free_rtbob_rooms_elem ( s.mtx[i][j]);
		}
		free ( s.mtx[i]);
	}
	free ( s.mtx);
}

stage_t gen_stage ( dungeon_rooms dr, dungeon_items di, dungeon_monsters dm)
{
	stage_t s = {.height = 0, .width = 0, .mtx = NULL};
	room_t * mtx [N_ROOMS][N_ROOMS];
	room_t * rooms[N_ROOMS];

	_init_rooms ( rooms, dr);		// A cette etape on a toutes les rooms a ajouter dans le stage
	_init_mtx ( mtx);				// La on definit une matrice pour y stocker els rooms
	_init_points ( s.p, N_ROOMS);	// Vecteur de point pour stocker les positions dis pieces dans la matrice

	add_stage_rooms ( mtx, rooms, s.p, &(s.height), &(s.width));	// Ajout des pieces generees
	s.mtx = copy_stage_matrix ( mtx, s.height, s.width);			// Compie de la variable temporaire mtx dans une variable alloué dynamiquement a al bonne taille

	return s;
}

void wprint_stage_level ( WINDOW * win, int lines, int cols, stage_t * s)
{
	int i = 0, j = 0, k =0, l = 0, ln = 1, cl = 1;
	for ( i = 0; i < s->height && ln < (lines - 3); i++)
	{
		for ( j = 0; j < 9  && ln < (lines - 1); j++)
		{
			for ( k = 0; k < s->width && cl < cols-1; k++)
			{
				for ( l = 0; l < 15 && cl < cols-1 ; l++)
				{
					if ( s->mtx[i][k])
						mvwprintw ( win, ln, cl, "%c ", s->mtx[i][k]->piece[j][l]);
					cl += 2;
				}
				if ( j == 4 && s->mtx[i][k] && cl < cols - 1)
					if ( s->mtx[i][k]->piece[j][14] == 'D')
						mvwprintw ( win, ln, cl, "D ");
				wrefresh ( win);
				cl += 2;
			}
			cl = 1;
			ln++;
		}

		for ( k = 0; k < s->width && cl < (cols - 2) && (32 * k) + 14 < cols; k++)
			if ( s->mtx[i][k])
				if ( s->mtx[i][k]->piece[8][7] == 'D')
					mvwprintw ( win, ln, (32 * k + 14) + 1, "D");
		ln++;
	}
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
	
	//~ mvwprintw ( win, *abs_y, *abs_x, "c : %d %d", *abs_y, *abs_x);
	//~ mvwprintw ( win, 2, 1, "H : %d W : %d | HGi : (%d, %d) | HGx : (%d, %d) | ni %d nj %d", lines, cols, hgi, hgj, hgy, hgx, n_i, n_j);
	//~ wrefresh ( win);

	//~ return;
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

void get_next (stage_t s, int * cur, int * y, int * x, int card)
{
	int _x = s.p[*cur].x, _y =  s.p[*cur].y;
	if ( card == 1)	_x--;
	if ( card == 2)	_y--;
	if ( card == 3)	_y++;
	if ( card == 4)	_x++;
	for (int i = 0; i < N_ROOMS; i++)
	{
		if ( s.p[i].x == _x && s.p[i].y == _y)
		{
			*cur = i;
			*y = D_HEIGHT / 2;
			*x = D_WIDTH / 2;
			break;
		}
	}	
}

int update_player_pos ( stage_t s, int key, int * y, int * x, int * cur)
{
	int ret = 0;
	
	if ( key == 'q')
	{
		if ( *y == D_HEIGHT / 2 && *x == 1 && (s.mtx[s.p[*cur].y][s.p[*cur].x]->piece[*y][0] == 'D' || s.mtx[s.p[*cur].y][s.p[*cur].x]->piece[*y][0] == '$'))		ret = 1;
		else						*x = (*x - 1) < 1 ? 1 : *x - 1, mvprintw ( LINES - 2, COLS / 2, "%s", M_LEFT);
	} else if ( key == 'z')
	{
		if ( *y == 1 && *x == D_WIDTH/2 && (s.mtx[s.p[*cur].y][s.p[*cur].x]->piece[0][*x] == 'D' || s.mtx[s.p[*cur].y][s.p[*cur].x]->piece[0][*x] == '$'))		ret = 2;
		else						*y = (*y - 1) < 1 ? 1 : *y - 1, mvprintw ( LINES - 2, COLS / 2, "%s", M_UP);
	} else if ( key == 's')
	{
		if ( *y == D_HEIGHT - 2 && *x == D_WIDTH/2 && (s.mtx[s.p[*cur].y][s.p[*cur].x]->piece[D_HEIGHT - 1][*x] == 'D' || s.mtx[s.p[*cur].y][s.p[*cur].x]->piece[D_HEIGHT - 1][*x] == '$'))		ret = 3;
		else					*y = (*y + 1) > (D_HEIGHT - 2) ? D_HEIGHT - 2 : *y + 1, mvprintw ( LINES - 2, COLS / 2, "%s", M_DOWN);
	} else if ( key == 'd')
	{
		if ( *y == D_HEIGHT / 2 && *x == D_WIDTH - 2 && (s.mtx[s.p[*cur].y][s.p[*cur].x]->piece[*y][D_WIDTH - 1] == 'D' || s.mtx[s.p[*cur].y][s.p[*cur].x]->piece[*y][D_WIDTH - 1] == '$'))		ret = 4;
		else					*x = (*x + 1) > (D_WIDTH - 2) ? D_WIDTH - 2 : *x + 1, mvprintw ( LINES - 2, COLS / 2, "%s", M_RIGHT);
	} else
		mvprintw ( LINES - 2, COLS / 2, "%s", "unknown");
	return ret;
}

// Fonction inutile vouée a disparaitre
// Combine a la fois logique et affichage
void wtbob_acion ( WINDOW * w_main, int key, int * x, int * y, int *abs_y, int *abs_x, int * cur, stage_t s)
{
	if ( key == ERR) return;	// Do nothing

	// Erase character
	mvwprintw ( w_main, (*abs_y) + (*y), (*abs_x) + 2 * (*x), "%c", s.mtx[s.p[*cur].y][s.p[*cur].x]->piece[*y][*x]);
	// Erase action bar
	mvprintw ( LINES - 2, COLS / 2, "%*c", 10, ' ');
	
	int r_update = update_player_pos( s, key, y, x, cur);

	if ( r_update)
	{
		get_next ( s, cur, y, x, r_update);
		wprint_game_win ( w_main, LINES, COLS, W_MARGIN, W_MARGIN, s, abs_y, abs_x, s.p[*cur]);
	}
	refresh();
	// Display character
	mvwprintw ( w_main,  *abs_y + *y, (*abs_x) + 2 * *x, "@");
	//~ wrefresh( w_main);
}

// Separation des differents types d'actions
int tbob_get_action_type ( int key)
{
	if ( key == TBOB_LEFT || key == TBOB_UP || TBOB_RIGHT || key == TBOB_DOWN)	return 1 /*<<4*/;	// Action move
	if ( key == KEY_LEFT || key == KEY_UP || KEY_RIGHT || key == KEY_DOWN)	return 2/*1<<5*/;	// Action move
	return 0;
}

// Utilisation d'un dx et dy pour représenter le déplacement du jouer
// -1 signifie qu'aucun deplacement n'est demandé
int tbob_get_action_move ( int key, int * dy, int * dx)
{
	int ret = - 1;
	if ( key == TBOB_LEFT)	(*dx)--, ret = 0;
	if ( key == TBOB_UP)	(*dy)--, ret = 1;
	if ( key == TBOB_RIGHT) (*dx)++, ret = 2;
	if ( key == TBOB_DOWN)	(*dy)++, ret = 3;
	return ret;
}

int tbob_is_valide_move ( stage_t s, int cur, int y, int x) {
	int i = s.p[cur].y,
		j = s.p[cur].x;
	// Ne devrait jamais se produire pa rla suite
	// Le changement de piece n'étant pas implémenté, c'est uen sécurité
	// Est la pour nous dire qu'un changement de piece doit avoir lieu
	if ( x < 0 || y < 0 ||
		 x > (D_WIDTH - 1) ||
		 y > (D_HEIGHT -1))
		 return 4;
	// Si on est sur une case qu'on ne peux pas traverser
	if ( s.mtx[i][j]->piece[y][x] == 'W' ||
		 s.mtx[i][j]->piece[y][x] == 'R' ||
		 s.mtx[i][j]->piece[y][x] == 'G' )
		 return 0;
	// Si on est sur une porte
	if ( s.mtx[i][j]->piece[y][x] == 'D' ||
		 s.mtx[i][j]->piece[y][x] == '$')
		 return 2;
	// Sinon le mouvement est valide
	return 1;
}

void ncurses_game_loop ( WINDOW * w_main, player_t p, stage_t s)
{
	int x = D_WIDTH / 2, y = D_HEIGHT / 2, abs_x = ((COLS - W_MARGIN - 2) / 2) - (D_WIDTH), abs_y = ((LINES - H_MARGIN - 2) / 2) - (D_HEIGHT / 2), cur = 0;
	int quit = 0, key = 410;
	do
	{
		if ( key == TBOB_QUIT)	quit = 1;
		if ( key == KEY_RESIZE)
		{
			clear();
			wprint_header ( H_TITLE, NULL);
			wprint_game_win ( w_main, LINES, COLS, W_MARGIN, W_MARGIN, s, &abs_y, &abs_x, s.p[cur]);
		}
		
		// Blink charachter
		attron (A_DIM);	
		mvwprintw ( w_main, abs_y + y, abs_x + 2 * x, "@");
		wmove ( w_main, abs_y+y, abs_x + 2 * x);
		attroff(A_DIM);	

		int t_act, a_act, a_move = -1, v_move = 0, _y = y, _x = x, dx = 0, dy = 0;

		t_act = tbob_get_action_type ( key);
		if ( t_act == 1)
		{
			a_move = tbob_get_action_move ( key, &dy, &dx);	// Action move
			_x += dx, _y += dy;
			v_move = tbob_is_valide_move ( s, cur, _y, _x);
			/*
			 *	I STOPED HERE: 
			 * 	 1 -	l'idée c'est de vérifier la valeur de v_move pour initier un simple mouvement (effacer, afficher, clignoter)
			 *   2 -	initier un changement de chambre
			 *   3 - 	éventuellement une attaque (si monstre adjascent)
			 */


		}
		/*
		else if ( t_act == 2)	a_act = -1;										// Action attack
		* Lancer un tir jusqu'a ce qu'il atteigne un obstacle
		* 	En gros il faut prévoir un max de tir et un pas de temps pour le tir (possibilité de rater ca cible ?
		* 	Donc mettre a vrai un booleen de tire si a faux
		*/
		
		if ( a_move != -1 && v_move == 1)
		{
			int i = s.p[cur].y,
				j = s.p[cur].x;
			mvwprintw ( w_main, (abs_y) + (y), (abs_x) + 2 * (x), "%c", s.mtx[i][j]->piece[y][x]);
			x = _x, y = _y;
			//~ mvwprintw ( w_main,  abs_y + y, (abs_x) + 2 * x, "@");
		}
		//~ wtbob_acion ( w_main, key, &x, &y, &abs_y, &abs_x, &cur, s);
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

int main ( int argc, char ** argv)
{
	dungeon_rooms	dr 	= {.n_rooms = 0, .rooms = NULL};
	dungeon_items	di 	= {.n_items = 0, .items = NULL};
	dungeon_monsters dm	= {.n_monsters = 0, .monsters = NULL};

	dr = read_rtbob ( "test.rtbob");
	di = read_itbob ( "test.itbob");
	dm = read_mtbob ( "test.mtbob");

	srand ( 42);
	
	stage_t s = gen_stage ( dr, di, dm);
	print_stage ( s);
	
	print_stage_level ( &s);

	player_t p = { .y = D_HEIGHT / 2, .x = D_WIDTH/2, .name = "Briatte", .hpMax = 3.0, .dmg= 3.5, .shield = 0.0, .flight = 0};

	n_curses( p, s);

	free_stage ( s);
	free_rtbob ( dr);
	free_itbob ( di);
	free_mtbob ( dm);

	return 0;
}
