#ifndef _H_STAGE_
#define _H_STAGE_

#define N_ROOMS		14
#define D_HEIGHT	9
#define D_WIDTH		15

#include "rtbob.h"
#include "itbob.h"
#include "mtbob.h"

typedef struct
{
	int y, x;
	int card[4];	// int is_left_available - card[0], is_right_available card[2], is_up_available - card[1], is_down_available - card[3] 
}point;

typedef struct
{
	point p[14];
	int height, width;
	room_t *** mtx; // Matrice de pointeur
}stage_t;


void print_stage_tab ( room_t * mtx[14][14], int height, int width);
void print_stage ( stage_t s);
void print_stage_level ( stage_t * s);
void add_stage_room ( room_t * s[N_ROOMS][N_ROOMS], room_t * r, int h, int w, int row, int * height, int * width);
void update_stage_metadata ( room_t * mtx[][N_ROOMS], point * p, int * placed, int * h, int * w, int row, int next);
void pick_next_pos ( point * p, int * row, int * next, int placed);
void _init_rooms ( room_t * rooms[], dungeon_rooms dr);
void _init_mtx ( room_t * mtx[N_ROOMS][N_ROOMS]);
void _init_points ( point * p, int n_p);
void add_stage_rooms_paths ( room_t * rooms[N_ROOMS], int s_src, int s_dest, int row);
void add_stage_rooms ( room_t * mtx[][N_ROOMS], room_t * rooms[N_ROOMS], point * p, int * height, int * width);
room_t *** copy_stage_matrix ( room_t * b_mtx[][N_ROOMS], int height, int width);
void free_stage ( stage_t s);
stage_t gen_stage ( dungeon_rooms dr, dungeon_items di, dungeon_monsters dm);

#endif
