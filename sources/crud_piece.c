#include <stdio.h>
#include <stdlib.h>

#include "rtbob.h"


void crud_piece_create ( dungeon_rooms * dr )
{
	char buff[256];
	int x, y;
	get_entry_str ( buff, 256, " Veuillez saisir la taille de la salle (h,w) :\n\t");
	sscanf ( buff, "(%d,%d)", &x, &y);

	room_t * nr = read_rtbob_rooms ( stdin, y, x);
	add_rtbob_rooms ( dr, nr);	
	dr->n_rooms++;
}

void crud_piece_read ( dungeon_rooms * dr)
{
	char filename[256] = "\0";
	
	get_entry_str ( filename, 256, " Veuillez saisir le nom du fichier à ouvrir : ");
	
	*dr = read_rtbob ( filename);
	
	printf ("CRUD Fin\n");
}

void crud_piece_delete ( dungeon_rooms * dr, int pos)
{
	if ( dr->n_rooms < pos) return;
	room_t * r = dr->rooms[pos];
	
	for (int i = pos; i < (dr->n_rooms - 1); i++)
	{
		dr->rooms[pos] = dr->rooms[pos+1];
	}
	free_rtbob_rooms_elem(r);
	dr->n_rooms--;
	
}

void crud_piece_update ( dungeon_rooms * dr, int pos)
{
	free_rtbob_rooms_elem ( dr->rooms[pos]);
	printf ("Veuillez écrire la topologie de votre room : \n");
	dr->rooms[pos]->piece = read_rtbob_rooms_data ( stdin, dr->rooms[pos]->width, dr->rooms[pos]->height);
}
