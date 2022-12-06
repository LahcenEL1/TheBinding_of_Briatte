#ifndef _H_CRUD_PIECE_
#define _H_CRUD_PIECE_

#include "rtbob.h"

void crud_piece_create ( dungeon_rooms * dr );

void crud_piece_read ( dungeon_rooms * dr);

void crud_piece_delete ( dungeon_rooms * dr, int pos);

void crud_piece_update ( dungeon_rooms * dr, int pos);

#endif
