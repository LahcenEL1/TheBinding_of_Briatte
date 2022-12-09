#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "rtbob.h"
#include "itbob.h"
#include "mtbob.h"
#include "stage.h"
#include "crud_piece.h"
#include "tbob_logic.h"

#if defined (_GUI)
#include "tbob_gui.h"
#elif defined (_CLI)
#include "tbob_cli.h"
#endif


#define HELP	100

void print_title ()
{	
	printf ("\033[H\033[2J");
	//~ printf ("\033[3J");
	printf ("\033[1m"); // Bold
	printf ("┌────────────────────────┐\n");
	printf ("│ The Binding of Briatte │\n");
	printf ("└────────────────────────┘\n");
	printf ("\033[0m");	// Disable esacp seqs

	printf ("\n");	
}

void print_press_any_key ()
{
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	print_title();
	printf ("\033[%dH", w.ws_row / 2);	
	printf ("\033[%ldG", w.ws_col / 2 - strlen ("Press any key to continue") / 2);
	printf ("Press any key to continue");	
	printf ("\033[%dH", w.ws_row / 2 + 1);	
	printf ("\033[%ldG", w.ws_col / 2 - strlen ("type \"help\" to print help message") / 2);
	printf ("type \"help\" to print help message");
	printf ("\033[%dH", w.ws_row); fflush ( stdout);
}

void print_actions ()
{
	printf ( "  ┌───────────────────────────────────────┐\n");
	printf ( "  │ATCIONS                         :      │\n");
	printf ( "  ├───────────────────────────────────────┤\n");
	printf ( "  │  C.R.U.D                              │\n");
	printf ( "  │    crud_r : crud for rooms            │\n");
	printf ( "  │    crud_i : crud for items            │\n");
	printf ( "  │    crud_m : crud for monsters         │\n");
	printf ( "  ├───────────────────────────────────────┤\n");
	printf ( "  │  VIEW                                 │\n");
	printf ( "  │    view_r : view mode for rooms       │\n");
	printf ( "  │    view_i : view mode for items       │\n");
	printf ( "  │    view_m : view mode for monsters    │\n");
	printf ( "  ├───────────────────────────────────────┤\n");
	printf ( "  │  LEVEL                                │\n");
	printf ( "  │    gen_l  : generate a new level      │\n");
	printf ( "  ├───────────────────────────────────────┤\n");
	printf ( "  │  GAME                                 │\n");
	printf ( "  │    r_cli  : run game in CLI mode      │\n");
	printf ( "  │    r_gui  : run game in GUI mode      │\n");
	printf ( "  ├───────────────────────────────────────┤\n");
	printf ( "  │    help   : print this message        │\n");
	printf ( "  │    quit   : quit TBOB                 │\n");
	printf ( "  └───────────────────────────────────────┘\n");
}

int get_event ( char * buf)
{
	if ( !strncmp ( buf, "quit", 4) ||
		 !strncmp ( buf, "q", 1)) 		return 0;
	if ( !strncmp ( buf, "crud_r", 6))	return 1;
	if ( !strncmp ( buf, "crud_i", 6))	return 2;
	if ( !strncmp ( buf, "crud_m", 6))	return 3;
	if ( !strncmp ( buf, "view_r", 6))	return 11;
	if ( !strncmp ( buf, "view_i", 6))	return 12;
	if ( !strncmp ( buf, "view_m", 6))	return 13;
	if ( !strncmp ( buf, "gen_l", 4))	return 21;
	if ( !strncmp ( buf, "r_cli", 4))	return 31;
	if ( !strncmp ( buf, "r_gui", 4))	return 41;
	if ( !strncmp ( buf, "help", 4))	return HELP;
	return -1;
}

int get_crud ( char * buf)
{
	if ( !strncmp ( buf, "C", 1)) return 0;
	if ( !strncmp ( buf, "R", 1)) return 1;
	if ( !strncmp ( buf, "U", 1)) return 2;
	if ( !strncmp ( buf, "D", 1)) return 3;
	return -1;
}

int main ( int argc, char ** argv)
{

	dungeon_rooms	dr 	= {.n_rooms = 0, .rooms = NULL};
	dungeon_items	di 	= {.n_items = 0, .items = NULL};
	dungeon_monsters dm	= {.n_monsters = 0, .monsters = NULL};

	stage_t s = { .height = 0, .width = 0, .mtx = NULL};
	player_t p = { .y = D_HEIGHT / 2, .x = D_WIDTH/2, .name = "Briatte", .hpMax = 3.0, .dmg= 3.5, .shield = 0.0, .flight = 0};
	
	print_press_any_key();

	struct winsize w;
	int quit = 0, event = HELP, crud_t = -1;
	 while ( !quit)
	{
		// Get event from string action
		char buf[128] = "\0";
		fgets ( buf, 128, stdin);		
		if ( event == -1)	event = get_event ( buf);
		
			
		crud_t = -1;
		// DO actions
		if ( event == 0)
			quit = 1;
		if (   0 < event && event < 4)
		{
			ioctl(0, TIOCGWINSZ, &w);
			print_title();
			printf ( "\033[6H");	// On se place a la 6e ligne
			printf ("C.R.U.D type (expected : \"C\", \"R\", \"U\" or \"D\")");
			printf ("\033[%dH", w.ws_row);	// On se place en bas a gauche
			fgets ( buf, 128, stdin);		
			crud_t = get_crud ( buf);
			if ( crud_t != -1)	print_title();
			
			if ( event == 1)		// CRUD Rooms
			{
				printf ( "\033[6H");
				if ( crud_t == 0) crud_piece_create ( &dr);
				if ( crud_t == 1) crud_piece_read ( &dr);
				if ( crud_t == 2 || crud_t == 3)
				{
					printf ( "\033[6H");	// On se place a la 6e ligne
					printf ("\tRoom's index (between [0:%d]", dr.n_rooms);
					printf ("\033[%dH", w.ws_row);	// On se place en bas a gauche
					fgets ( buf, 128, stdin);

					int index = -1;
					sscanf ( buf, "%d", &index);
					if ( crud_t == 2) crud_piece_update ( &dr, index);
					if ( crud_t == 3) crud_piece_delete ( &dr, index);
				}
			}
			//~ else if ( event == 2)	// CRUD Items
			//~ {}
			//~ else if ( event == 3)	// CRUD Monsters
			//~ {}
			
			if ( crud_t != -1)	event = HELP;
		}
		if ( event == 11)					// Afficher les rooms
		{
			print_title();
			printf ( "\033[6H");	// On se place a la 6e ligne
			printf ("\tRoom's index (between [0:%d]", dr.n_rooms);
			printf ("\033[%dH", w.ws_row);	// On se place en bas a gauche
			fgets ( buf, 128, stdin);
			
			int index = -1;
			sscanf ( buf, "%d", &index);
			
			print_title();
			write_rtbob_rooms ( stdout, index < dr.n_rooms ? dr.rooms[index] : NULL);

			printf ("\033[%dH", w.ws_row);
			printf ("type \'help\' to get help");		// Printf press any key to continue
			event = -1;
		}
		//~ else if ( event == 12)			// Afficher les items
		//~ else if ( event == 13)			// Afficher les monstres
		else if ( event == 21)
		{
			
			print_title();
			srand ( 42);
			s = gen_stage ( dr, di, dm);
			printf ("Stage generated\n");
			event = -1;
		}
		//~ else if ( event == 31)
		else if ( event == 41)
		{
			
			print_title();
			printf ("Lanching GUI\n");
			sdl ( p, s);
			event = -1;	
		}
		if ( event == HELP)				// Print HELP
		{
			print_title();
			print_actions ();
			event = -1;
		}
		printf ("\033[%dH", w.ws_row);	// Move cursor to bottom
	}

	free_rtbob ( dr);
	free_itbob ( di);
	free_mtbob ( dm);

	return 0;
}


int _main ( int argc, char ** argv)
{
	dungeon_rooms	dr 	= {.n_rooms = 0, .rooms = NULL};
	dungeon_items	di 	= {.n_items = 0, .items = NULL};
	dungeon_monsters dm	= {.n_monsters = 0, .monsters = NULL};

	dr = read_rtbob ( "test.rtbob");
	di = read_itbob ( "test.itbob");
	dm = read_mtbob ( "test.mtbob");

	srand ( 42);
	
	stage_t s = gen_stage ( dr, di, dm);
	
	print_stage_level ( &s);

	player_t p = { .y = D_HEIGHT / 2, .x = D_WIDTH/2, .name = "Briatte", .hpMax = 3.0, .dmg= 3.5, .shield = 0.0, .flight = 0};

#if defined (_GUI)
	sdl ( p, s);
#else
	n_curses ( p, s);
#endif

	free_stage ( s);

	free_rtbob ( dr);
	free_itbob ( di);
	free_mtbob ( dm);

	return 0;
}
  
