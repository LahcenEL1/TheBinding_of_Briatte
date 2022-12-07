#ifndef _H_RTBOB_
#define _H_RTBOB_

#include <stdio.h>
#include <stdlib.h>

typedef enum
{
	SPAWN_ROOM,
	ITEM_ROOM,
	ITEM_BONUS_ROOM,
	BOSS_ROOM,
	NORMAL_ROOM
} piece_kind;

typedef struct
{
	int width, height, id;
	char ** piece;
	piece_kind kind;
}room_t;

typedef struct
{
	int n_rooms;
	room_t ** rooms;
} dungeon_rooms; // rtbob_t

void get_entry_str ( char * buff, int size, char * msg);

char ** read_rtbob_rooms_data ( FILE * fin, int n_w, int n_h);

dungeon_rooms read_rtbob ( char * filename);

void write_rtbob_rooms (FILE * fout, room_t * r);

void write_rtbob ( char * filename, dungeon_rooms dr);
//dungeon_rooms create_rbob ();

void free_rtbob_rooms_elem ( room_t * rooms);

void free_rtbob ( dungeon_rooms dr);

room_t * read_rtbob_rooms ( FILE * fin, int n_w, int n_h);

void add_rtbob_rooms ( dungeon_rooms * dr, room_t * r);

room_t * gen_empty_room ( int h, int w, piece_kind kind);

room_t * rtbob_clone_room( room_t * r);

//room_t * get_rtbob_rooms ( dungeon_rooms * dr, int indice);
#endif
