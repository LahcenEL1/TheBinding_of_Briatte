#include "tbob_gui.h"

#include <time.h>


double gettime () {
	struct timespec ts;
	clock_gettime ( CLOCK_MONOTONIC, &ts);
	return ts.tv_sec*1e0 + ts.tv_nsec*1e-9;
}

SDL_Texture * sdl_tbob_piece_texture ( SDL_Renderer * r, char ** piece, SDL_Texture * t_gap, SDL_Texture * t_door, SDL_Texture * t_rock, SDL_Texture * t_wall, SDL_Texture * t_spike)
{
	int x = 0, y = 0;
	SDL_Texture * t_level = SDL_CreateTexture ( r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, W_WIDTH - 50, W_HEIGHT - 50);
	SDL_SetRenderTarget ( r, t_level);
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			SDL_Rect hg = { .y = y + i * 50, .x = x + j * 50,.h = 50, .w = 50};
			if ( piece[i][j] == 'R')
			{
				if ( SDL_SetRenderDrawColor ( r, 0xA0, 0xA0, 0xA0, 0) < 0) printf ("Error renderer\n");
				SDL_RenderFillRect ( r, &hg);
				SDL_RenderCopy ( r, t_rock, NULL, &hg);
			}
			else if ( piece[i][j] == 'W')
			{
				if ( SDL_SetRenderDrawColor ( r, 0x58, 0x29, 0x00, 0) < 0) printf ("Error renderer\n");
				SDL_RenderFillRect ( r, &hg);
				SDL_RenderCopy ( r, t_wall, NULL, &hg);
			}
			else if ( piece[i][j] == 'G')
			{
				if ( SDL_SetRenderDrawColor ( r, 0xA0, 0xA0, 0xA0, 0) < 0) printf ("Error renderer\n");
				SDL_RenderFillRect ( r, &hg);
				SDL_RenderCopy ( r, t_gap, NULL, &hg);
			}
			else if ( piece[i][j] == 'D')
			{
				if ( SDL_SetRenderDrawColor ( r, 0xA0, 0xA0, 0xA0, 0) < 0) printf ("Error renderer\n");
				SDL_RenderFillRect ( r, &hg);
				SDL_RenderCopy ( r, t_door, NULL, &hg);
			}
			else if ( piece[i][j] == 'S')
			{
				if ( SDL_SetRenderDrawColor ( r, 0xA0, 0xA0, 0xA0, 0) < 0) printf ("Error renderer\n");
				SDL_RenderFillRect ( r, &hg);
				SDL_RenderCopy ( r, t_spike, NULL, &hg);
			}
			else
			{
				if ( SDL_SetRenderDrawColor ( r, 0xA0, 0xA0, 0xA0, 0) < 0) printf ("Error renderer\n");				
				SDL_RenderFillRect ( r, &hg);
			}
		}
	}

	SDL_SetRenderTarget ( r, NULL);
	return t_level;
}

void sdl_tbob_move_player ( SDL_Renderer * r, SDL_Texture ** t_level, SDL_Texture * t_perso, int cur, int y, int x, int _y, int _x)
{
	SDL_Rect src = { .x = _x * 50, .y = _y * 50, .h = 50, .w = 50},
			 dest = { .x = W_LEFT + _x * S_BOX, .y = W_TOP + _y * S_BOX, .h = 50, .w = 50},
			 hg = { .x = W_LEFT + x * S_BOX, .y = W_TOP + y * S_BOX, .w = S_BOX, .h = S_BOX};
	
	SDL_RenderCopy ( r, t_level[cur], &src, &dest);
	SDL_RenderCopy ( r, t_perso, NULL, &hg);
	SDL_RenderPresent ( r);
}

void sdl_display_game_win ( SDL_Renderer * r, SDL_Texture * t_level, SDL_Texture * t_perso, int y, int x)
{
	// Chargement du niveau
	SDL_Rect level_pos = { .x = W_LEFT, .y = W_TOP, .w = W_WIDTH - 50, .h = W_HEIGHT - 50};
	SDL_RenderCopy ( r, t_level, NULL, &level_pos);

	// Chargement du personnage
	SDL_Rect perso_pos = {.x = W_LEFT + x * S_BOX, .y = W_TOP + y * S_BOX, .w = S_BOX, .h = S_BOX};
	SDL_RenderCopy ( r, t_perso, NULL, &perso_pos);
	
	//~ SDL_RenderPresent ( r);	
}

void tbob_engine ( int * shot_f, int * y, int * x, int *dy, int * dx, double * t_ref, double * t_timer, int * updated, game_t g)
{
	// Engine du tir
	if ( *shot_f && !(*updated))
	{
		if ( (*t_timer) - (*t_ref) > 0.05)
		{
			(*x) += (*dx);
			(*y) += (*dy);				

			// Detecter l'impact
			if ( (*x) == 0 || (*y) == 0 || (*x) == D_WIDTH - 1 || (*y) == D_HEIGHT - 1 )
			{
				(*shot_f) = 0;
				// Booleen pour dire qu'il y a eu contact
				// Int pour dire qui a ete touche
			}
			(*t_ref) = gettime();
			*updated = 1;
		}
	}
	// Gestion des monstres
	// Gestion des dégat perso et monstre
	// Gestion des items
	// Requete de changement de niveau
	//
}

void sdl_tbob_get_event ( SDL_Event e, int * key, int * quit)
{
	// tbob_get_event
	switch(e.type)
	{
		case SDL_QUIT: // Clic sur la croix
			*quit = 1;
			break;
		case SDL_KEYUP:
			if ( e.key.keysym.sym == SDLK_d) 		*key = (int) 'd';
			if ( e.key.keysym.sym == SDLK_s) 		*key = (int) 's';
			if ( e.key.keysym.sym == SDLK_z)	 	*key = (int) 'z';
			if ( e.key.keysym.sym == SDLK_q) 		*key = (int) 'q';
			if ( e.key.keysym.sym == SDLK_UP) 		*key = KEY_UP;
			if ( e.key.keysym.sym == SDLK_DOWN) 	*key = KEY_DOWN;
			if ( e.key.keysym.sym == SDLK_LEFT) 	*key = KEY_LEFT;
			if ( e.key.keysym.sym == SDLK_RIGHT)	*key = KEY_RIGHT;
			if ( e.key.keysym.sym == SDLK_w) 		*quit = 1; 
			break;
		default:
			break;
	}	
}


void sdl_game_loop ( game_t g)
{
	g.quit = 0;
	g.p.x = D_WIDTH / 2;
	g.p.y = D_HEIGHT / 2;
	g.cur = 0; g.cur_x = g.s.p[0].x; g.cur_y = g.s.p[0].y;
	g.w_updated_f = 0;

	g.t_glob = gettime();
	g.t_ref = gettime();
	g.t_timer = 0.0f;
	int  t_act, a_move = -1, v_move = 0;

	for ( int i = 0; i < 7; i++) 
	{
		g.ts[i].x = 0;
		g.ts[i].y = 0;
		g.ts[i].dx = 0;
		g.ts[i].dy = 0;
		g.ts[i].shot_f = 0;
		g.ts[i].updated_f = 0;
	}

	sdl_display_game_win ( g.r, g.bg[g.cur], g.tp, g.p.y, g.p.x);
	SDL_RenderPresent ( g.r);
	do
	{
		SDL_Event e;
		int key = -1;
		a_move = -1, v_move = 0;
		g.w_updated_f = 0, g.dx = 0, g.dy = 0, g.room_f = 0, g.move_f = 0, g.t_timer = gettime();
		while ( SDL_PollEvent(&e)){

			sdl_tbob_get_event ( e, &key, &(g.quit));
			t_act = tbob_get_action_type ( key);
			if ( t_act == 1)
			{
				a_move = tbob_get_action_move ( key, &(g.dy), &(g.dx));	// Action move
				v_move = tbob_is_valide_move ( g.s, g.cur, g.p.y + g.dy, g.p.x + g.dx);
				if ( v_move == 1)
				{
					g.p.x += g.dx, g.p.y += g.dy, g.move_f = 1;
				}
				else if ( v_move == 2)
				{
					g.cur = tbob_get_next_room ( g.s, g.s.p[g.cur].y + g.dy, g.s.p[g.cur].x + g.dx);
					tbob_get_new_coord ( a_move, &(g.p.x), &(g.p.y));
					g.room_f = 1;
				}
			}
			else if ( t_act == 2 && !g.ts[0].shot_f)
			{
				g.ts[0].dy = 0;
				g.ts[0].dx = 0;
				tbob_get_shoot_way ( key, &(g.ts[0].dy), &(g.ts[0].dx));
				g.ts[0].x = g.p.x;
				g.ts[0].y = g.p.y;
				g.ts[0].shot_f = 1;
			}
			if ( g.move_f || g.room_f)	g.w_updated_f = 1;
			break;
		}
		for ( int i = 0; i < 7; i++)
			tbob_engine ( &(g.ts[i].shot_f), &(g.ts[i].y), &(g.ts[i].x), &(g.ts[i].dy), &(g.ts[i].dx), &(g.t_ref), &(g.t_timer), &(g.ts[i].updated_f), g);
		
		for ( int i = 0; i < 7; i++)
		{
			if ( g.ts[i].shot_f == 1 && g.ts[i].updated_f == 1)	g.w_updated_f = 1;
			if ( g.ts[i].shot_f == 0 && g.ts[i].updated_f == 1)	g.w_updated_f = 1;
		}
		
		if ( g.w_updated_f)
		{
			sdl_display_game_win ( g.r, g.bg[g.cur], g.tp, g.p.y, g.p.x);

			for ( int i = 0; i < 7; i++)
			{
				SDL_Rect tl = { .x = W_LEFT + g.ts[i].x * S_BOX, .y = W_TOP + g.ts[i].y * S_BOX, .w = S_BOX, .h = S_BOX};
				if ( g.ts[i].shot_f == 1 && g.ts[i].updated_f == 1)
				{
					SDL_RenderCopy ( g.r, g.fb, NULL, &tl);
					g.ts[i].updated_f = 0;
				}
			}
			SDL_RenderPresent ( g.r);	
			g.w_updated_f = 0;
		}
	}
	while ( !g.quit);
}

void sdl ( player_t p, stage_t s)
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window * main_w = SDL_CreateWindow(	W_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W_WIDTH, W_HEIGHT,	SDL_WINDOW_SHOWN);
	SDL_Renderer * main_w_r = SDL_CreateRenderer ( main_w, -1, SDL_RENDERER_ACCELERATED);

	SDL_Surface	* gap	= IMG_Load ( "./res/gap.png"),
				* door	= IMG_Load ( "./res/door.png"),
				* rock	= IMG_Load ( "./res/roche.png"),
				* wall	= IMG_Load ( "./res/wall.png"),
				* spike = IMG_Load ( "./res/spike.png"),
				* fire	= IMG_Load ( "./res/fire.png"),
				* item	= IMG_Load ( "./res/item.png"),
				* perso = IMG_Load ( "./res/character.png");

	SDL_Texture * t_gap		= SDL_CreateTextureFromSurface ( main_w_r, gap),
				* t_door	= SDL_CreateTextureFromSurface ( main_w_r, door),
				* t_rock	= SDL_CreateTextureFromSurface ( main_w_r, rock),
				* t_wall	= SDL_CreateTextureFromSurface ( main_w_r, wall),
				* t_spike	= SDL_CreateTextureFromSurface ( main_w_r, spike),
				* t_fire	= SDL_CreateTextureFromSurface ( main_w_r, fire),
				* t_item	= SDL_CreateTextureFromSurface ( main_w_r, item),
				* t_perso 	= SDL_CreateTextureFromSurface ( main_w_r, perso);

	SDL_Texture*  t_level[N_ROOMS];
	for (int i = 0; i < N_ROOMS; i++)
		t_level[i]= sdl_tbob_piece_texture ( main_w_r, s.mtx[s.p[i].y][s.p[i].x]->piece, t_gap, t_door, t_rock, t_wall, t_spike);

	game_t g = {.t_glob= 0.0f, .t_ref = 0.0f, .t_timer = 0.0f, .quit = 1, .cur = 0, .cur_y = s.p[0].y, .cur_x = s.p[0].x, .w_updated_f = 0, .p = p, .s = s, .r = main_w_r, .tp = t_perso, .ti = t_item, .fb = t_fire, .bg = t_level};

	sdl_game_loop ( g);
	
	SDL_FreeSurface ( gap);
	SDL_FreeSurface ( door);
	SDL_FreeSurface ( rock);
	SDL_FreeSurface ( wall);
	SDL_FreeSurface ( spike);
	SDL_FreeSurface ( fire);
	SDL_FreeSurface ( item);
	SDL_FreeSurface ( perso);
	
	SDL_DestroyTexture ( t_gap);
	SDL_DestroyTexture ( t_door);
	SDL_DestroyTexture ( t_rock);
	SDL_DestroyTexture ( t_wall);
	SDL_DestroyTexture ( t_spike);
	SDL_DestroyTexture ( t_fire);
	SDL_DestroyTexture ( t_item);
	SDL_DestroyTexture ( t_perso);
	
	for ( int i = 0; i < N_ROOMS; i ++)
		SDL_DestroyTexture ( t_level[i]);
	
	SDL_DestroyRenderer ( main_w_r);
	
	SDL_DestroyWindow( main_w);
	
	SDL_Quit();	
}
