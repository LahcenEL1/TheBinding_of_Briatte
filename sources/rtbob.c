#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rtbob.h"

void get_entry_str ( char * buff, int size, char * msg)
{
	fprintf ( stdout, "%s", msg);
	fgets ( buff, size, stdin);
	buff [ strlen ( buff) -1] = '\0';
}

char ** read_rtbob_rooms_data ( FILE * fin, int n_w, int n_h)
{
	char ** piece = (char **) malloc ( n_h * sizeof(char*));
	
	for ( int h = 0; h < n_h; h++)
	{
		piece[h] = (char *) malloc ( n_w * sizeof(char));
		for (int w = 0; w < n_w; w++)
		{
			char c = fgetc ( fin);
			piece[h][w] = c;
			fgetc ( fin);
		}
	}
	return piece;
}

room_t * read_rtbob_rooms ( FILE * fin, int n_w, int n_h)
{
	
	room_t * rooms = malloc ( sizeof ( room_t));
	
	rooms->piece = read_rtbob_rooms_data ( fin, n_w, n_h);
	rooms->height = n_h;
	rooms->width = n_w;
	rooms->kind = NORMAL_ROOM;
	return rooms;
}

void add_rtbob_rooms ( dungeon_rooms * dr, room_t * r)
{
	dr->rooms = realloc ( dr->rooms, (dr->n_rooms+1) * sizeof (room_t **));
	dr->rooms[dr->n_rooms] = r;
	dr->n_rooms++;
	
	return;
}

/*
room_t * get_rtbob_rooms ( dungeon_rooms * dr, int indice)
{
	if ( dr->n_rooms < 1) return NULL;
	
	room_t * r_p = NULL, * r_c = dr->rooms; 
	
	for (int i = 1; i < indice ; i++)
	{
		r_p = r_c;
		r_c = r_c->next;
	}
	return r_p;
}
*/
dungeon_rooms read_rtbob ( char * filename)
{
	dungeon_rooms dr = { 0, NULL};
	
	FILE * fin = fopen ( filename, "r");
	char buff[200] = "\0";
	int n_h, n_w, in;
	fgets ( buff, 200, fin);
	sscanf ( buff, "{%d}", &(dr.n_rooms));

	for (int i = 0; i < dr.n_rooms; i++)
	{
		// Mettre en dehors de la fonction pour plus de genericite	
		fgets ( buff, 200, fin);
		sscanf ( buff, "[%d|%d]%d", &n_h, &n_w, &in);
		// !
		room_t * r = read_rtbob_rooms ( fin, n_w, n_h);
		dr.rooms = realloc ( dr.rooms, (i+1) * sizeof (room_t **));
		dr.rooms[i] = r;
	}
	fclose ( fin);
	return dr;
}

void write_rtbob_rooms (FILE * fout, room_t * r)
{
	if ( r == NULL)
	{
		fprintf ( fout, "No room to display\n");
		return;
	}
	static int in = 0;
	fprintf ( fout, "[%d|%d]%d\n", r->height, r->width, in);

	for ( int h = 0; h < r->height; h++)
	{
		for (int w = 0; w < r->width; w++)
		{
			fprintf ( fout, "%c ", r->piece[h][w]);
		}
		fprintf ( fout, "\n");
	}
	in++;
}

void write_rtbob ( char * filename, dungeon_rooms dr)
{
	FILE * fout = stdout;
	if (filename )
		fout = fopen ( filename, "w");

	fprintf ( fout, "{%d}\n", dr.n_rooms);
	
	for (int i = 0; i < dr.n_rooms; i++)
	{
		write_rtbob_rooms ( fout, dr.rooms[i]);
	}
	
	if ( filename)
		fclose ( fout);

	return;
}
//dungeon_rooms create_rbob () {}

void free_rtbob_rooms_elem ( room_t * rooms)
{
	for (int h = 0; h < rooms->height; h++)
	{
		free ( rooms->piece[h]);
	}	
	free ( rooms->piece);
	free ( rooms);
}


void free_rtbob_rooms ( room_t ** rooms, int n_rooms)
{
	for ( int i = 0; i < n_rooms; i++)
	{
		free_rtbob_rooms_elem ( rooms[i]);
	}
}

void free_rtbob ( dungeon_rooms dr)
{
	free_rtbob_rooms ( dr.rooms, dr.n_rooms);
	free ( dr.rooms);
}

room_t * gen_empty_room ( int h, int w, piece_kind kind)
{
	room_t * r = (room_t *) malloc ( sizeof ( room_t));

	r->kind = kind;
	r->height = h;
	r->width = w;
	r->piece = (char **) malloc ( h * sizeof (char*));
	
	for (int i = 0; i < h; i++)
	{
		r->piece[i] = (char *) malloc ( w * sizeof (char));
	}
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
			r->piece[i][j] = ' ';
	for (int i = 0; i < h; i++)
	{
		r->piece[i][0] = 'W';
		r->piece[i][w-1] = 'W';
	}
	for (int i = 0; i < w; i++)
	{
		r->piece[0][i] = 'W';
		r->piece[h-1][i] = 'W';
	}
	return r;	
}

room_t * rtbob_clone_room( room_t * r)
{
	room_t * ret = (room_t *) malloc ( sizeof ( room_t));
	ret->kind = r->kind;
	ret->width = r->width;
	ret->height = r->height;
	ret->piece = (char **) malloc ( r->height * sizeof ( char*));
	for (int i = 0; i < r->height; i++)
		ret->piece[i] = (char *) malloc ( r->width * sizeof ( char));
	for (int i = 0; i < r->height; i++)
		for (int j = 0; j < r->width; j++)
			ret->piece[i][j] = r->piece[i][j];
	return ret;
}
