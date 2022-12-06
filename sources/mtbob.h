#ifndef _H_MTBOB_
#define _H_MTBOB_

typedef struct
{
	char * name;
	float hpMax;
	int shoot,
		ss,
		flight;
	
}monster_t;

typedef struct 
{
	int n_monsters;
	monster_t ** monsters;
}dungeon_monsters;

void free_mtbob_monsters ( monster_t ** monsters, int n_monsters);
void free_mtbob ( dungeon_monsters dm);


dungeon_monsters read_mtbob ( char * filename);
void write_mtbob ( dungeon_monsters dm, char * path);

#endif
