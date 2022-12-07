//~ #ifndef _GUI
//~ #define _GUI

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


#define W_TOP	125
#define W_LEFT	25
#define S_BOX	50
#define W_WIDTH 800
#define W_HEIGHT 600
#define W_TITLE "The Binding of Briatte"
#include "stage.h"
#include "tbob_logic.h"

SDL_Texture * sdl_tbob_piece_texture ( SDL_Renderer * r, char ** piece, SDL_Texture * t_gap, SDL_Texture * t_door, SDL_Texture * t_rock, SDL_Texture * t_wall, SDL_Texture * t_spike);

void sdl_tbob_move_player ( SDL_Renderer * r, SDL_Texture ** t_level, SDL_Texture * t_perso, int cur, int y, int x, int _y, int _x);

void sdl_display_game_win ( SDL_Renderer * r, SDL_Texture * t_level, SDL_Texture * t_perso, int y, int x);

void sdl_game_loop ( SDL_Renderer * main_w_r, SDL_Texture ** t_level, SDL_Texture * t_perso, stage_t s);

void sdl ( player_t p, stage_t s);

//~ #endif
