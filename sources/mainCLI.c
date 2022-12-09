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


//Remame this file to main.c 
//launche make CLASS=CLI
//if you need the librarie of n_curse can you install pls 
//./tbob_CLI



int main ( int argc, char ** argv)
{
    dungeon_rooms    dr     = {.n_rooms = 0, .rooms = NULL};
    dungeon_items    di     = {.n_items = 0, .items = NULL};
    dungeon_monsters dm    = {.n_monsters = 0, .monsters = NULL};

    dr = read_rtbob ( "test.rtbob");
    di = read_itbob ( "test.itbob");
    dm = read_mtbob ( "test.mtbob");

    srand ( 42);

    stage_t s = gen_stage ( dr, di, dm);

    print_stage_level ( &s);

    player_t p = { .y = D_HEIGHT / 2, .x = D_WIDTH/2, .name = "Briatte", .hpMax = 3.0, .dmg= 3.5, .shield = 0.0, .flight = 0};


    n_curses ( p, s);


    free_stage ( s);

    free_rtbob ( dr);
    free_itbob ( di);
    free_mtbob ( dm);

    return 0;
}