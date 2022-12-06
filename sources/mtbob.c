#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mtbob.h"

monster_t init_monster ( char * b_name, float hpMax, int shoot, int ss, int flight)
{
	char * name = (char*) malloc ( (strlen(b_name)+1) * sizeof(char));
	strcpy ( name, b_name);
	return (monster_t) {.name = name, .hpMax = hpMax, .shoot = shoot, .ss = ss, .flight = flight};
}

void free_monster ( monster_t * m)
{
	if ( m->name)
	{
		free (m->name);
		m->name = NULL;
	}
	free ( m);
}

void free_mtbob_monsters ( monster_t ** monsters, int n_monsters)
{
	for (int i = 0; i < n_monsters; i++)
	{
		free_monster ( monsters[i]);
	}
	free ( monsters);
}

void free_mtbob ( dungeon_monsters dm)
{
	free_mtbob_monsters ( dm.monsters, dm.n_monsters);
}

monster_t * get_monster_by_name ( monster_t ** monsters, int n_monsters, char * name)
{
	if ( !name) return NULL;
	for (int i = 0; i < n_monsters; i++)
	{
		if ( !strcmp ( name, monsters[i]->name))
			return monsters[i];
	}
	return NULL;
}

int get_monster_id ( monster_t ** monsters, int n_monsters, char * name)
{
	if ( !name) return -1;
	int cpt = 0;
	for ( ; cpt < n_monsters; cpt++)
	{
		if ( !strcmp ( name, monsters[cpt]->name))
			break;
	}
	return cpt;
}

monster_t ** read_mtbob_monster ( FILE * fin, int n_monsters)
{
	monster_t ** monsters = (monster_t **) malloc ( n_monsters * sizeof (monster_t*));
	char buff[256] = "\0";

	fgets ( buff,  256, fin); // tu lis "---"

	for (int i = 0; i < n_monsters; i++)
	{

		char temp[128] = "\0";
		monsters[i] = (monster_t *) malloc ( sizeof ( monster_t));
		do
		{
			if ( fgets ( buff,  256, fin) == NULL) break; // tu lis une entree

			if ( !strncmp ( buff, "name=",5))
			{
				strcpy ( temp, buff+5);
				temp[strlen(temp) - 1] = '\0';
				monsters[i]->name = malloc ( (strlen ( temp) + 1) * sizeof ( char));
				strcpy ( monsters[i]->name, temp);
			}
			else if ( !strncmp ( buff, "shoot=", 6))	monsters[i]->shoot 	= (strncmp ( buff, "shoot=true",7) == 0 ? 1 : 0);
			else if ( !strncmp ( buff, "ss=", 3))		monsters[i]->ss 	= (strncmp ( buff, "ss=true",7) == 0 ? 1 : 0);
			else if ( !strncmp ( buff, "flight=", 7))	monsters[i]->flight = (strncmp ( buff, "flight=true",7) == 0 ? 1 : 0);
			else if ( !strncmp ( buff, "hpMax=", 6)) 	sscanf ( buff, "hpMax=%f", &(monsters[i]->hpMax));
		} while ( strncmp ( buff, "---", 3));
	}
	return monsters;
}

dungeon_monsters read_mtbob ( char * filename)
{
	dungeon_monsters dm = {.n_monsters = 0, .monsters = NULL};
	
	FILE * fin = fopen ( filename, "r");
	
	fscanf ( fin, "{%d}\n", &(dm.n_monsters));
	
	dm.monsters = read_mtbob_monster ( fin, dm.n_monsters);

	fclose ( fin);
	
	return dm;
}

void write_mtbob ( dungeon_monsters dm, char * path)
{
	FILE * fout = path ? fopen ( path, "w") : stdout;
	
	fprintf ( fout, "{%d}\n", dm.n_monsters);
	fprintf ( fout, "---\n");
	
	for (int i = 0; i < dm.n_monsters; i++)
	{
		fprintf ( fout, "name="),  dm.monsters[i]->name ? fprintf ( fout, "%s", dm.monsters[i]->name) : fprintf ( fout, "No name %d", i), fprintf ( fout, "\n");
		if ( dm.monsters[i]->hpMax)		fprintf ( fout, "hpMax=%g\n", dm.monsters[i]->hpMax);
		if ( dm.monsters[i]->shoot)		fprintf ( fout, "shoot=true\n");
		if ( dm.monsters[i]->ss)		fprintf ( fout, "ss=true\n");
		if ( dm.monsters[i]->flight)	fprintf ( fout, "flight=true\n");
		fprintf ( fout, "---\n");
	}
	
	path ? fclose ( fout) : /*fprintf ( fout, "Message : %s", "test_printing\n")*/0;
}
