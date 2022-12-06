#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rtbob.h"
#include "itbob.h"
#include "mtbob.h"
#include "crud_piece.h"

typedef struct
{
	dungeon_items * inventaire;
	dungeon_monsters caracteristique;
	int x, y, z;
}player_t;

typedef struct
{
	dungeon_rooms /*rtbob_t*/ * dr;
	dungeon_items /*itbob_t*/ * di;
	dungeon_monsters /*mtbob_t*/ * dm;
}dungeon_t;

typedef enum
{
	ACT_CREATE,
	ACT_READ,
	ACT_UPDATE,
	ACT_DELETE,
	ACT_QUIT,
	ACT_TOTAL
}action_t;

action_t get_action_type ( char * e)
{
	if ( strlen ( e) == 6 && !strncmp ( "create", e, 6))	return ACT_CREATE;
	if ( strlen ( e) == 1 && !strncmp ( "c", e, 1))			return ACT_CREATE;
	if ( strlen ( e) == 4 && !strncmp ( "read", e, 4))		return ACT_READ;
	if ( strlen ( e) == 1 && !strncmp ( "r", e, 1))			return ACT_READ;
	if ( strlen ( e) == 6 && !strncmp ( "update", e, 6))	return ACT_UPDATE;
	if ( strlen ( e) == 1 && !strncmp ( "u", e, 1))			return ACT_UPDATE;
	if ( strlen ( e) == 6 && !strncmp ( "delete", e, 6))	return ACT_DELETE;
	if ( strlen ( e) == 1 && !strncmp ( "d", e, 1))			return ACT_DELETE;
	if ( strlen ( e) == 4 && !strncmp ( "quit", e, 4))		return ACT_QUIT;
	if ( strlen ( e) == 1 && !strncmp ( "q", e, 1))			return ACT_QUIT;
	return ACT_TOTAL;
}

typedef enum
{
	CRUD_PIECE,
	CRUD_ITEM,
	CRUD_MONSTER,
	CRUD_TOTAL
}crud_t;

crud_t get_crud_type ( char * e)
{
	if ( strlen ( e) == 5 && !strncmp ( "piece", e, 5))		return CRUD_PIECE;
	if ( strlen ( e) == 1 && !strncmp ( "p", e, 1))			return CRUD_PIECE;
	if ( strlen ( e) == 4 && !strncmp ( "item", e, 4))		return CRUD_ITEM;
	if ( strlen ( e) == 1 && !strncmp ( "i", e, 1))			return CRUD_ITEM;
	if ( strlen ( e) == 7 && !strncmp ( "monster", e, 7))	return CRUD_MONSTER;
	if ( strlen ( e) == 1 && !strncmp ( "m", e, 1))			return CRUD_MONSTER;
	return CRUD_TOTAL;
}



void action_oriented_loop ( action_t a, crud_t ct, dungeon_rooms * dr, char * filename)
{
	if ( a == ACT_CREATE)
	{
		printf ("CRUD Creation :\n");
		if ( ct == CRUD_PIECE)
		{
			printf (" ... piece\n");
			crud_piece_create ( dr);
		}
	}
}

void type_oriented_loop ( action_t a, crud_t ct, dungeon_rooms * dr/*, char * filename*/)
{
	if ( ct == CRUD_PIECE)
	{
		printf ("CRUD Piece : \n");
		if ( a == ACT_CREATE)
		{
			printf (" ... creation.;\n");
			crud_piece_create ( dr);
			// write_back_after_creaction			
		}
		else if ( a == ACT_READ)
		{
			printf (" ... read.;\n");
			crud_piece_read ( dr);		
		}
		else if ( a == ACT_UPDATE)
		{
			printf (" ... update.;\n");
			crud_piece_update ( dr, 1);		
		}
		else if ( a == ACT_DELETE)
		{
			printf (" ... delete.;\n");
			
			//~ write_rtbob ( NULL, *dr);
			
			crud_piece_delete ( dr, 2);

			write_rtbob ( NULL, *dr);
			// write_back_for_modif
		}
		// write_back_anyway
/*		else if ( a == ACT_SAVE)
		{
			write_back_if_asked
		}*/
	}
}

void test_crud_piece ( int argc, char ** arv)
{
	dungeon_rooms dr = {0, NULL};
	
	
	action_t a = ACT_TOTAL;
	crud_t ct = CRUD_TOTAL;
	int loop = 1;
	char crud[256] = "\0";
	char t_crud[256] = "\0";
	//~ char filename[256] = "\0";	
	

	while ( loop)
	{
		get_entry_str ( crud, 256, "Action CRUD (c, create, r, read, u, update, d, delete) :\n\t");
		get_entry_str ( t_crud, 256, "Type de CRUD (piece, p, item, i, monster, m) :\n\t");

		a = get_action_type ( crud);
		ct = get_crud_type ( t_crud);

		if ( a & ACT_QUIT)
		{
			loop = 0;
			write_rtbob ( NULL, dr);
			free_rtbob ( dr);				
		}
		else
		{
			//~ action_oriented_loop ( a, ct, &dr, filename);
			type_oriented_loop ( a, ct, &dr);
		}
	}
	
}

void test_crud_item ( int argc, char ** argv)
{
	dungeon_items di = { 0, NULL};
	
	printf ("Test of Reading ...\n");
	di = read_itbob ( "test.itbob");
	print_as_table ( &di);


	printf ("\nTest of Creating (adding) ...\n");
/*
	add_itbob_item ( di.items, di.n_items, (item_t) {.name="Totoro", .hpMax=2.0, .shield=0.0, .dmg=1.0, .ps=0, .ss=0, .flight=1});
	di.n_items++;
*/
	add_itbob ( &di, (item_t) {.name="Totoro", .hpMax=2.0, .shield=0.0, .dmg=1.0, .ps=0, .ss=0, .flight=1});
	print_as_table ( &di);

	printf ("\nTest of Deleting (removing) ...\n");
/*
	remove_itbob_item ( di.items, di.n_items, 1);
	di.n_items--;
*/
	remove_itbob ( &di, "Bucket of Lard");
	print_as_table ( &di);


	
	printf ("\nTest of Updating (updating) ...\n");
	modify_itbob ( &di, "Totoro", (item_t) {.name=NULL, .hpMax=10.0, .shield=0.0, .dmg=8.7, .ps=0, .ss=0, .flight=0});
	print_as_table ( &di);

	// Free all this
}

void test_crud_monstes ( int argc, char ** argv)
{

	dungeon_monsters dm = { .n_monsters = 0, .monsters = NULL};
	
	dm = read_mtbob ( "test.mtbob");
	
	
	
	write_mtbob ( dm, NULL);
	
	
	free_mtbob ( dm);
		
}

void test_stage_gen ( int argc, char ** argv)
{
	dungeon_rooms	dr 	= {.n_rooms = 0, .rooms = NULL};
	dungeon_items	di 	= {.n_items = 0, .items = NULL};
	dungeon_monsters dm	= {.n_monsters = 0, .monsters = NULL};


	dr = read_rtbob ( "test.rtbob");
	di = read_itbob ( "test.itbob");
	dm = read_mtbob ( "test.mtbob");	
}

int add ()
{
	printf ("(%d, %d\n", s->height, s->width);
	// Alloc the matrix
	if ( h != 0 && w != 0 && shift_l != 0 && shift_d != 0)
	{	
		if ( !realloc ( s->mtx, (s->height+1) * sizeof ( room_t **))) return 0;
		for (int i = 0; i < (s->height+1); i++)
			if ( !realloc ( s->mtx[i], (s->width+1) * sizeof ( room_t *))) return 0;
	}
	else
	{
		s->mtx = malloc ( (s->height+1) * sizeof ( room_t **));
		for (int i = 0; i < (s->height+1); i++)
		{
			s->mtx[i] = malloc ( (s->width+1) * sizeof ( room_t *));
		}
	}
	// Shift left
	if ( shift_d)
	{
		for (int i = 0; i < s->height; i++)
			for (int j = s->width; j > w; j--)
				s->mtx[i][j] = s->mtx[i][j-1];
		printf ("We shifted\n");
	}
	// Shift down
	if ( shift_l)
	{
		for (int i = s->height; i > h ; i--)
			for (int j = 0; j < s->width; j++)
				s->mtx[i][j] = s->mtx[i-1][j];
	}
	// Increment size
	
	// Adding the room
	
	if ( shift_d)
	{
		s->width++;
		//~ for (int i = 0; i < s->height; i++)
		//~ s->mtx[i][w] = NULL;
	}
	if ( shift_l)
	{
		s->height++;
		//~ for (int i = 0; i < s->width; i++)
			//~ s->mtx[h][i] = NULL;
	}
	s->mtx[h][w] = r;
	// Adding doors
			
	// Init stage :
	//   Gen spaw
	s.mtx = malloc ( (s.height+1) * sizeof ( room_t **));
	for (int i = 0; i < (s.height+1); i++)
	{
		s.mtx[i] = malloc ( (s.width+1) * sizeof ( room_t *));
	}	
	s.mtx[0][0] = spawn_r; 
	s.height++;
	s.width++;
	// !
	print_stage ( s);
	
	srand ( 42);
	int h = 0, w = 0, row;
	for (int i = 0; i < 5; i++)
	{
		int r = rand() % dr.n_rooms;
		row = 1;//rand() % 4;
		if ( row == 0) w = w > 0 ? w - 1 : 0;
		if ( row == 1) h++;
		if ( row == 2) w++;
		if ( row == 3) h = h > 0 ? h - 1 : 0;
		row = row % 2;
		//~ add_stage_room ( &s, rtbob_clone_room ( dr.rooms[r]), h, w, row);
		//~ printf ("Iter (%d) : \n", i);
		//~ print_stage ( s);
	}	
	return 1;
}
