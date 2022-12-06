#ifndef _H_ITBOB_
#define _H_ITBOB_

typedef struct item_t
{
	char * name;
	float hpMax,
		shield,
		dmg;
	int	ps,
		ss,
		flight;
}item_t;

typedef struct
{
	int n_items;
	item_t ** items;
}dungeon_items;


item_t * get_item_by_name ( item_t ** vect, int n_vect, char * name);

int get_item_pos ( item_t ** vect, int n_vect, char * name);

void affect_item_values ( item_t * item, item_t data);

item_t * add_itbob_item ( item_t ** vect, int n, item_t data);

item_t * add_itbob ( dungeon_items * di, item_t data);

item_t * remove_itbob_item ( item_t ** vect, int n, int pos);

item_t * remove_itbob ( dungeon_items * di, char * i_name);

void free_itbob ( dungeon_items di);

void modify_itbob_item ( item_t * item, item_t data, char * n_name);

item_t * modify_itbob ( dungeon_items * di, char * i_name, item_t data);

dungeon_items read_itbob ( char * filename);

void write_itbob ( dungeon_items di, char * path);

void print_as_table_bool ( int val);

void print_as_table_double ( double val);

void print_as_table ( dungeon_items * di);

void print_as_file ( dungeon_items * di);
#endif
