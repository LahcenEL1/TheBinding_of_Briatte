#ifndef _H_TBOB_LOGIC_
#define _H_TBOB_LOGIC_

#include "stage.h"

// TBOB CONSTANTS
#define N_ROOMS		14
#define D_HEIGHT	9
#define D_WIDTH		15
#define TBOB_QUIT	'w'
#define TBOB_UP 	'z'
#define TBOB_DOWN 	's'
#define TBOB_LEFT 	'q'
#define TBOB_RIGHT 	'd'

#ifdef _GUI
#include <SDL2/SDL.h>

#define KEY_UP		SDLK_UP
#define KEY_DOWN	SDLK_DOWN
#define KEY_LEFT	SDLK_LEFT
#define KEY_RIGHT	SDLK_RIGHT
#endif

typedef struct
{
	// Position
	int y, x;
	// Stat
	char * name;
	float hpMax, dmg, shield;
	int flight;

} player_t;

// Separation des differents types d'actions
int tbob_get_action_type ( int key);

// Utilisation d'un dx et dy pour représenter le déplacement du jouer
// -1 signifie qu'aucun deplacement n'est demandé
int tbob_get_action_move ( int key, int * dy, int * dx);

int tbob_get_shoot_way ( int key, int * dy, int * dx);

int tbob_is_valide_move ( stage_t s, int cur, int y, int x);

int tbob_get_next_room ( stage_t s, int row, int col);

void tbob_get_new_coord ( int a_move, int * x, int * y);

//~ void tbob_engine ( stage_t s, int cur, int * shoot_f, int * bullet_pos_y, int * bullet_pos_x);

#endif
