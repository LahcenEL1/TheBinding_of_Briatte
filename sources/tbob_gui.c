#include "tbob_gui.h"

//~ #include <SDL2/SDL.h>
//~ #include <SDL2/SDL_image.h>

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
	SDL_Rect level_pos = { .x = 25, .y = 125, .w = W_WIDTH - 50, .h = W_HEIGHT - 50};
	SDL_RenderCopy ( r, t_level, NULL, &level_pos);

	// Chargement du personnage
	SDL_Rect perso_pos = {.x = W_LEFT + x * S_BOX, .y = W_TOP + y * S_BOX, .w = S_BOX, .h = S_BOX};
	SDL_RenderCopy ( r, t_perso, NULL, &perso_pos);
	
	SDL_RenderPresent ( r);	
}

void sdl_game_loop ( SDL_Renderer * main_w_r, SDL_Texture ** t_level, SDL_Texture * t_perso, stage_t s)
{
	int quit = 0, x = D_WIDTH / 2, y = D_HEIGHT / 2, cur = 0, cpt = 0;

	sdl_display_game_win ( main_w_r, t_level[cur], t_perso, y, x);

	do
	{
		int key = -1, t_act, a_act, a_move = -1, v_move = 0, _y = y, _x = x, dx = 0, dy = 0, room_f = 0, move_f = 0;
		SDL_Event e;
		while ( SDL_PollEvent(&e)){
			switch(e.type)
			{
				case SDL_QUIT: // Clic sur la croix
					quit = 1;
					break;
				case SDL_KEYUP:
					if ( e.key.keysym.sym == SDLK_d) key = (int) 'd';//, t_act = 1, dx++;
					if ( e.key.keysym.sym == SDLK_s) key = (int) 's';//, t_act = 1,  dy++;
					if ( e.key.keysym.sym == SDLK_z) key = (int) 'z';//, t_act = 1,  dy--;
					if ( e.key.keysym.sym == SDLK_q) key = (int) 'q';//, t_act = 1,  dx--;
					if ( e.key.keysym.sym == SDLK_w) quit = 1;
					//~ SDL_Log ("%d ", cpt++);
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
					if ( move_f)	sdl_tbob_move_player ( main_w_r, t_level, t_perso, cur, y, x, _y, _x);
					if ( room_f)	sdl_display_game_win ( main_w_r, t_level[cur], t_perso, y, x);
					break;
			}
		}
	}	
	while ( !quit);
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
				* perso = IMG_Load ( "./res/character.png");

	SDL_Texture * t_gap		= SDL_CreateTextureFromSurface ( main_w_r, gap),
				* t_door	= SDL_CreateTextureFromSurface ( main_w_r, door),
				* t_rock	= SDL_CreateTextureFromSurface ( main_w_r, rock),
				* t_wall	= SDL_CreateTextureFromSurface ( main_w_r, wall),
				* t_spike	= SDL_CreateTextureFromSurface ( main_w_r, wall),
				* t_perso 	= SDL_CreateTextureFromSurface ( main_w_r, perso);

	SDL_Texture*  t_level[N_ROOMS];
	for (int i = 0; i < N_ROOMS; i++)
		t_level[i]= sdl_tbob_piece_texture ( main_w_r, s.mtx[s.p[i].y][s.p[i].x]->piece, t_gap, t_door, t_rock, t_wall, t_spike);

	sdl_game_loop ( main_w_r, t_level, t_perso, s);
	
	SDL_FreeSurface ( gap);
	SDL_FreeSurface ( door);
	SDL_FreeSurface ( rock);
	SDL_FreeSurface ( wall);
	SDL_FreeSurface ( spike);
	SDL_FreeSurface ( perso);
	
	SDL_DestroyTexture ( t_gap);
	SDL_DestroyTexture ( t_door);
	SDL_DestroyTexture ( t_rock);
	SDL_DestroyTexture ( t_wall);
	SDL_DestroyTexture ( t_spike);
	SDL_DestroyTexture ( t_perso);
	
	for ( int i = 0; i < N_ROOMS; i ++)
		SDL_DestroyTexture ( t_level[i]);
	
	SDL_DestroyRenderer ( main_w_r);
	
	SDL_DestroyWindow( main_w);
	
	SDL_Quit();	
}
