#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "itbob.h"

item_t * get_item_by_name ( item_t ** vect, int n_vect, char * name)
{
	item_t * item = NULL;
	for (int i = 0; i < n_vect; i++)
	{
		if ( !strcmp ( name, vect[i]->name))
		{
			item = vect[i];
			break;
		}
	}
	return item;
}

int get_item_pos ( item_t ** vect, int n_vect, char * name)
{
	int cpt = 0;
	for ( ; cpt < n_vect; cpt++)
	{
		if ( !strcmp ( name, vect[cpt]->name))
		{
			break;
		}
	}
	return cpt;
}

void affect_item_values ( item_t * item, item_t data)
{
	item->hpMax		= data.hpMax;
	item->shield	= data.shield;
	item->dmg		= data.dmg;
	item->ss		= data.ss;
	item->ps		= data.ps;
	item->flight	= data.flight;
}

item_t * add_itbob_item ( item_t ** vect, int n, item_t data)
{
	vect = realloc ( vect, (n+1) * sizeof ( item_t *));
	vect[n] = malloc ( sizeof ( item_t *));
	
	vect[n]->name = (char * ) malloc ( (strlen(data.name)+1) * sizeof ( char));
	strcpy ( vect[n]->name, data.name);
	
	affect_item_values ( vect[n], data);
	
	return vect[n];
}

item_t * add_itbob ( dungeon_items * di, item_t data)
{
	item_t * a_item = add_itbob_item ( di->items, di->n_items, data);
	di->n_items++;
	return a_item;
}

item_t * remove_itbob_item ( item_t ** vect, int n, int pos)
{
	if ( pos > n) return NULL;

	item_t * r_item = vect[pos];
	for (int i = pos; i < (n-1); i++)
		vect[i] = vect[i+1];

	vect = realloc ( vect, (n-1) * sizeof ( item_t *));

	return r_item;
}

item_t * remove_itbob ( dungeon_items * di, char * i_name)
{
	int pos =  get_item_pos ( di->items, di->n_items, i_name);
	item_t * m_item = remove_itbob_item ( di->items, di->n_items, pos);
	if ( m_item) di->n_items--;
	return m_item;
}

void free_itbob ( dungeon_items di)
{
	for (int i = 0; i < di.n_items; i++)
	{
		//~ if ( di.items[i]->name)
		free (di.items[i]->name);
		free ( di.items[i]);
	}
	free ( di.items);
}

void modify_itbob_item ( item_t * item, item_t data, char * n_name)
{
	if ( !item) return;
	if ( n_name)
	{
		if ( item->name) free (item->name);
		item->name = ( char *) malloc ( (strlen(n_name)+1)* sizeof ( char));
		strcpy ( item->name, n_name);
	}
	item->hpMax = data.hpMax;
	affect_item_values ( item, data);
}

item_t * modify_itbob ( dungeon_items * di, char * i_name, item_t data)
{
	item_t * m_item = get_item_by_name ( di->items, di->n_items, i_name);
	modify_itbob_item ( m_item, data, data.name);
	return m_item;
} 

dungeon_items read_itbob ( char * filename)
{

	dungeon_items di = { .n_items = 0, .items = NULL};
	FILE * fin = fopen ( filename, "r");
	char buff[256] = "\0";
	fgets ( buff, 200, fin);
	sscanf ( buff, "{%d}", &(di.n_items));
	
	di.items = ( item_t **) malloc ( di.n_items * sizeof ( item_t*));
		
	fgets ( buff,  256, fin); // tu lis "---"
	for (int i = 0; i < di.n_items; i++)
	{
		//~ printf ("Iter : %d\n", i);
		char temp[128] = "\0";
		di.items[i] = (item_t *) malloc ( sizeof ( item_t));
		do
		{
			if ( fgets ( buff,  256, fin) == NULL) break; // tu lis une entree
			//~ printf ( "   BUFF==[%s] (item:%p) (next:%p)\n", buff, item, item->next);
			if ( !strncmp ( buff, "name=",5))
			{
				strcpy ( temp, buff+5);
				temp[strlen(temp) - 1] = '\0';
				di.items[i]->name = malloc ( (strlen ( temp) + 1) * sizeof ( char));
				strcpy ( di.items[i]->name, temp);
			}
			else if ( !strncmp ( buff, "ps=", 3))		di.items[i]->ps = (strncmp ( buff, "ps=true",7) == 0 ? 1 : 0);
			else if ( !strncmp ( buff, "ss=", 3))		di.items[i]->ss = (strncmp ( buff, "ss=true",7) == 0 ? 1 : 0);
			else if ( !strncmp ( buff, "flight=", 7))	di.items[i]->flight = (strncmp ( buff, "flight=true",7) == 0 ? 1 : 0);
			else if ( !strncmp ( buff, "hpMax=", 6)) 	sscanf ( buff, "hpMax=%f", &(di.items[i]->hpMax));
			else if ( !strncmp ( buff, "shield=", 7)) 	sscanf ( buff, "shield=%f", &(di.items[i]->shield));
			else if ( !strncmp ( buff, "dmg=", 4)) 		sscanf ( buff, "dmg=%f", &(di.items[i]->dmg));
		} while ( strncmp ( buff, "---", 3));
	}
	fclose ( fin);
	return di;
}

void write_itbob ( dungeon_items di, char * path)
{
	FILE * fout = path ? fopen ( path, "w") : stdout;
	
	fprintf ( fout, "{%d}\n", di.n_items);
	fprintf ( fout, "---\n");
	
	for (int i = 0; i < di.n_items; i++)
	{
		fprintf ( fout, "name=%s\n", di.items[i]->name);
		if ( di.items[i]->hpMax) fprintf ( fout, "hpMax=%1.f\n", di.items[i]->hpMax);
		if ( di.items[i]->shield) fprintf ( fout, "shield=%1.f\n", di.items[i]->shield);
		if ( di.items[i]->dmg) fprintf ( fout, "dmg=%g\n", di.items[i]->dmg);
		if ( di.items[i]->ps) fprintf ( fout, "ps=%s\n", di.items[i]->ps ? "true" : "false");
		if ( di.items[i]->ss) fprintf ( fout, "ss=%s\n", di.items[i]->ss ? "true" : "false");
		if ( di.items[i]->flight) fprintf ( fout, "flight=%s\n", di.items[i]->flight ? "true" : "false");
		fprintf ( fout, "---\n");
	}
	
	path ? fclose ( fout) : /*fprintf ( fout, "Message : %s", "test_printing\n")*/0;
}

void print_as_table_bool ( int val)
{
	if ( val) printf (" 1 "); 
	else printf (" 0 ");	
}

void print_as_table_double ( double val)
{
	if ( val*val < 10e-8) printf ("      "); 
	else printf (" %*g ", 4,val);	
}

void print_as_table ( dungeon_items * di)
{
	printf ("    PS SS FL   HPM   SHD   DMG  --- NAME\n");
	for ( int i = 0; i < di->n_items; i++)
	{
		printf ("%2d) ", i+1);
		print_as_table_bool ( di->items[i]->ps);
		print_as_table_bool ( di->items[i]->ss);
		print_as_table_bool ( di->items[i]->flight);
		print_as_table_double ( di->items[i]->hpMax);
		print_as_table_double ( di->items[i]->shield);
		print_as_table_double ( di->items[i]->dmg);
		printf (" --- %s\n", di->items[i]->name);
		//~ printf ("%2d)  %1d  %1d  %1d % 1.1f  % 1.1f  % 1.1f  --- %s\n", i, di->items[i].ps, di->items[i].ss, di->items[i].flight, di->items[i].hpMax, di->items[i].shield, di->items[i].dmg, di->items[i].name);
	}
}

void print_as_file ( dungeon_items * di)
{
	write_itbob ( *di, NULL);
}
