#ifndef _GUI
#include <ncurses.h>
#endif

#include "tbob_logic.h"


// Separation des differents types d'actions
int tbob_get_action_type ( int key)
{
	//~ printf ( "%d | %d %d%d %d %d %d %d\n", key, TBOB_LEFT, TBOB_UP, TBOB_RIGHT || key == TBOB_DOWN
	if ( key == TBOB_LEFT || key == TBOB_UP || key == TBOB_RIGHT || key == TBOB_DOWN)	return 1 /*<<4*/;	// Action move
	if ( key == KEY_LEFT || key == KEY_UP || key == KEY_RIGHT || key == KEY_DOWN)	return 2/*1<<5*/;	// Action move

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

int tbob_get_next_room ( stage_t s, int row, int col)
{
	for (int i = 0; i < N_ROOMS; i++)
	{
		if ( s.p[i].y == row && s.p[i].x == col)
			return i;
	}
	return 0;
}
