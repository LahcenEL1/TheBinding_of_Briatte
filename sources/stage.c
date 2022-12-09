#include "stage.h"

void print_stage_tab ( room_t * mtx[14][14], int height, int width)
{
	printf ("Size(%d, %d)\n+", height, width);
	for ( int i = 0; i < width; i++) printf ("++++++");
	printf ("+\n");
	for (int i = 0; i < height; i++)
	{
		printf ("+");
		for (int j = 0; j < width; j++)
		{
			if ( mtx[i][j] == NULL)
				printf ("      ");
			else
			{
				if ( mtx[i][j]->kind == NORMAL_ROOM)
					printf ( " [%2d] ", mtx[i][j]->id + 1);
				else if ( mtx[i][j]->kind == SPAWN_ROOM)
					printf ( " [SR] ");
				else if ( mtx[i][j]->kind == ITEM_ROOM)
					printf ( " [IR] ");
				else if ( mtx[i][j]->kind == ITEM_BONUS_ROOM)
					printf ( " [IB] ");
				else if ( mtx[i][j]->kind == BOSS_ROOM)
					printf ( " [BR] ");
			}
		}
		printf ("+\n");
	}
	printf ("+");
	for ( int i = 0; i < width; i++) printf ("++++++");
	printf ("+\n");
}

void print_stage ( stage_t s)
{
	printf ("(%d, %d)\n", s.height, s.width);
	for (int i = 0; i < s.height; i++)
	{
		printf ("|");
		for (int j = 0; j < s.width; j++)
		{
			if ( s.mtx[i][j] == NULL)
				printf ("      ");
			else
			{
				if ( s.mtx[i][j]->kind == NORMAL_ROOM)
					printf ( " [%2d] ", s.mtx[i][j]->id + 1);
				else if ( s.mtx[i][j]->kind == SPAWN_ROOM)
					printf ( " [SP] ");
				else if ( s.mtx[i][j]->kind == ITEM_ROOM)
					printf ( " [IR] ");
				else if ( s.mtx[i][j]->kind == ITEM_BONUS_ROOM)
					printf ( " [IB] ");
				else if ( s.mtx[i][j]->kind == BOSS_ROOM)
					printf ( " [BR] ");
			}
		}
		printf ("|\n");
	}
}

void print_stage_level ( stage_t * s)
{
	FILE * fout = fopen ( "test.out", "w");
	int i = 0, j = 0, k =0, l = 0;
	fprintf ( fout, "(%d, %d)\n", s->height, s->width);
	for ( i = 0; i < s->height; i++)
	{
		for ( j = 0; j < 9 /*s->mtx[i][k]->height*/; j++)
		{
			for ( k = 0; k < s->width; k++)
			{
				for ( l = 0; l < 15 /*s->mtx[i][k]->width*/; l++)
				{
					if ( s->mtx[i][k])
						fprintf ( fout, "%c ", s->mtx[i][k]->piece[j][l]);
					else
						fprintf ( fout, "  ");
				}
				if ( j == 4 && s->mtx[i][k])
				{
					if ( s->mtx[i][k]->piece[j][14] == 'D')
						fprintf ( fout, "D ");
					else
						fprintf ( fout, "  ");
				}
				else
					fprintf ( fout, "  ");
			}
			fprintf ( fout, "\n");
		}
		//~ fprintf ( fout, "\n");
		for ( k = 0; k < s->width; k++)
		{
			if ( !s->mtx[i][k])
				fprintf ( fout, "%*c", 32, ' ');//"                                "
			else
			{
				if ( s->mtx[i][k]->piece[8][7] == 'D')
					fprintf ( fout, "              D                 ");
				else
					fprintf ( fout, "%*c", 32, ' ');//"                                "
			}
		}
		fprintf ( fout, "\n");
	}
	fclose ( fout);
}
// Fonction d'ajout des rooms par insertion et decallage
void add_stage_room ( room_t * s[N_ROOMS][N_ROOMS], room_t * r, int h, int w, int row, int * height, int * width)
{
	//~ printf ("%d - h(%d) w(%d) [%d - %d]\n", row, h, w, (*height), (*width));
	if ( row == 0 && w == 0 && s[h][w] != NULL)
	{
		// Shift left
		for (int i = 0; i < (*height) ; i++)
			for (int j = (*width); j > w; j--)
				s[i][j] = s[i][j-1];
		(*width)++;
		for (int i = 0; i < (*height); i++)
			s[i][w] = NULL;		
	}
	else if ( row == 1 && h == 0 && s[h][w] != NULL)
	{
		// Shift down
		for (int i = (*height); i > 0 ; i--)
			for (int j = 0; j < (*width); j++)
				s[i][j] = s[i-1][j];
		(*height)++;
		for (int i = 0; i < (*width); i++)
			s[h][i] = NULL;		
	}
	else if ( row == 2 && w >= (*width)) (*width)++;
	else if ( row == 3 && h >= (*height)) (*height)++;

	if ( s[h][w] != NULL) printf ("Error !!! %d %d\n", w, h);
	else	s[h][w] = r;		
}

// Fonction de mise a jour des points
void update_stage_metadata ( room_t * mtx[][N_ROOMS], point * p, int * placed, int * h, int * w, int row, int next)
{
		// Update rooms metadata
		if ( row == 0) // Shift left
		{
			p[next].card[0] = 1;
			p[(*placed)].card[2] = 1;
			*h = p[next].y;
			*w = p[next].x == 0 ? 0 : p[next].x - 1;
			if ( p[next].x == 0)
			{
				for ( int j = 0; j < (*placed); j++)
					p[j].x++;
			}
		}
		if ( row == 1) // Shift down
		{
			p[next].card[1] = 1;
			p[(*placed)].card[3] = 1;
			*w = p[next].x;
			*h = p[next].y == 0 ? 0 : p[next].y - 1;
			if ( p[next].y == 0)
			{
				for ( int j = 0; j < (*placed); j++)
					p[j].y++;
			}
		}
		if ( row == 2) //
		{
			p[next].card[2] = 1;
			p[(*placed)].card[0] = 1;
			*h = p[next].y;
			*w = p[next].x + 1;
		}
		if ( row == 3) //
		{
			p[next].card[3] = 1;
			p[(*placed)].card[1] = 1;
			*h = p[next].y + 1;
			*w = p[next].x;			
		}
		p[(*placed)].x = *w;
		p[(*placed)].y = *h;
		/*if ( *placed < 11)*/	(*placed)++;	
		for (int i = 0; i < (*placed); i++)
		{
			for (int j = 0; j < (*placed); j++)
			{
				if ( p[i].x - p[j]. x ==  1 && p[i].y == p[j].y)	p[i].card[0] = 1;
				if ( p[i].y - p[j]. y ==  1 && p[i].x == p[j].x)	p[i].card[1] = 1;
				if ( p[i].x - p[j]. x == -1 && p[i].y == p[j].y)	p[i].card[2] = 1;
				if ( p[i].y - p[j]. y == -1 && p[i].x == p[j].x)	p[i].card[3] = 1;
			}
		}
}

void pick_next_pos ( point * p, int * row, int * next, int placed)
{
	// Choose next point
	do{
		*row = rand() % 4;			// Le sens compris entre 0 et 3
		*next = rand() % (placed);	// Le numero du sommet auquel sera ajouté un autre
	} while ( p[*next].card[*row] == 1);	// Tant que le booleen d'etat de la room choisi indique qu'il est occupé
}

void _init_rooms ( room_t * rooms[], dungeon_rooms dr)
{
	// Init
	for ( int i = 0; i < N_ROOMS; i++) rooms[i] = NULL;
	
	// Special Rooms
	rooms[0] = gen_empty_room ( 9, 15, SPAWN_ROOM);
	rooms[11] = gen_empty_room ( 9, 15, ITEM_ROOM);
	rooms[12] = gen_empty_room ( 9, 15, ITEM_BONUS_ROOM); 
	rooms[13] = gen_empty_room ( 9, 15, BOSS_ROOM);

	// Random rooms
	int id = 0;
	for (int i = 0; i < N_ROOMS; i++)
		if ( rooms[i] == NULL)
		{
			//~ printf ("%d - (%d) ->", i, id);
			int r = rand() % dr.n_rooms;
			rooms[i] = rtbob_clone_room ( dr.rooms[r]);
			rooms[i]->id = id++;
		}
	//~ printf ("\n");
	
	for (int i = 0; i < N_ROOMS; i++)
		for (int y = 0; y < rooms[i]->height; y++)
			for (int x = 0; x < rooms[i]->width; x++)
				if ( rooms[i]->piece[y][x] == 'D')
					rooms[i]->piece[y][x] = 'W';
	
}

void _init_mtx ( room_t * mtx[N_ROOMS][N_ROOMS])
{
	// Stage buffer
	for (int i = 0; i < N_ROOMS; i++)
		for (int j = 0; j < N_ROOMS; j++)
			mtx[i][j] = NULL;
}

void _init_points ( point * p, int n_p)
{
	
	for ( int i = 0; i < n_p; i++)
	{
		p[i].x = 0, p[i].y = 0;
		p[i].card[0] = 0;
		p[i].card[1] = 0;
		p[i].card[2] = 0;
		p[i].card[3] = 0;
	}
}

void add_stage_rooms_paths ( room_t * rooms[N_ROOMS], int s_src, int s_dest, int row)
{
	char c = s_src > 10 || s_dest > 10 ? '$' : 'D';
	if ( row == 0)	rooms[s_src]->piece[rooms[s_src]->height / 2][0] = c,  rooms[s_dest]->piece[rooms[s_dest]->height / 2][rooms[s_dest]->width - 1] = c;
	if ( row == 1)	rooms[s_src]->piece[0][rooms[s_src]->width / 2] = c,  rooms[s_dest]->piece[rooms[s_dest]->height - 1][rooms[s_dest]->width / 2] = c;
	if ( row == 2)	rooms[s_src]->piece[rooms[s_src]->height/2][rooms[s_src]->width - 1] = c,  rooms[s_dest]->piece[rooms[s_dest]->height / 2][0] = c;
	if ( row == 3)	rooms[s_src]->piece[rooms[s_src]->height-1][rooms[s_src]->width/2] = c,  rooms[s_dest]->piece[0][rooms[s_dest]->width / 2] = c;
}

void add_stage_rooms ( room_t * mtx[][N_ROOMS], room_t * rooms[N_ROOMS], point * p, int * height, int * width)
{
	// Adding spawn to buffer
	mtx[0][0] = rooms[0];
	(*width)++;
	(*height)++;
	//~ print_stage_tab ( mtx, *height, *width);

	// Adding other rooms
	int h = 0, w = 0, row, next = -1, placed = 1;
	for (int i = 1; i < N_ROOMS; i++)
	{
		// On choisi la prochaine position a l'aide d'un next qui représente l'id de la room
		// 											d'un row compris entre 0 (gauche) 1 (haut) 2 (droite) et 3 (bas) pour 
		// On obtient une position en mixant le numero de room et le sens
		pick_next_pos ( p, &row, &next, placed);
		// On met a jour les positions des rooms, les booleans d'état pour savoir si un coté est disponible ou pas
		update_stage_metadata ( mtx, p, &placed, &h, &w, row, next);
		// On ajoute la room à la matrice à l'aide d'une fonction d'ajout
		add_stage_room ( mtx, rooms[i], h, w, row, height, width);
		// On ajoute une porte/chemin entre la room a placer et la rooms choisie
		add_stage_rooms_paths ( rooms, next, i, row);
		// On s'assure que les pieces speciales n'ont qu'une seule issue
		if ( i > 10) p[i].card[0] = 1, p[i].card[1] = 1, p[i].card[2] = 1, p[i].card[3] = 1;
		//~ print_stage_tab ( mtx, *height, *width);
	}	
}

room_t *** copy_stage_matrix ( room_t * b_mtx[][N_ROOMS], int height, int width)
{
	// Copy data
	room_t *** mtx = ( room_t ***) malloc ( height * sizeof ( room_t **));
	for (int i = 0; i < height; i++)
		mtx[i] = (room_t **) malloc ( width * sizeof ( room_t *));

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			mtx[i][j] = b_mtx[i][j];
		}
	}
	return mtx;
}

void free_stage ( stage_t s)
{
	for (int i = 0; i < s.height; i++)
	{
		for ( int j = 0; j < s.width; j++)
		{
			if ( s.mtx[i][j])
				free_rtbob_rooms_elem ( s.mtx[i][j]);
		}
		free ( s.mtx[i]);
	}
	free ( s.mtx);
}

void _init_item_vectors ( stage_t * s)
{
	for (int i = 0; i < 2; i++)
	{
		s->p_i[i].x = D_WIDTH / 2;
		s->p_i[i].y = D_HEIGHT / 2;
		s->itr_id[i] = 11 + i;
		s->v_i[i] = NULL;
	}
}

stage_t gen_stage ( dungeon_rooms dr, dungeon_items di, dungeon_monsters dm)
{
	stage_t s = {.height = 0, .width = 0, .mtx = NULL};
	room_t * mtx [N_ROOMS][N_ROOMS];
	room_t * rooms[N_ROOMS];

	_init_rooms ( rooms, dr);		// A cette etape on a toutes les rooms a ajouter dans le stage
	_init_mtx ( mtx);				// La on definit une matrice pour y stocker els rooms
	_init_points ( s.p, N_ROOMS);	// Vecteur de point pour stocker les positions dis pieces dans la matrice

	add_stage_rooms ( mtx, rooms, s.p, &(s.height), &(s.width));	// Ajout des pieces generees
	s.mtx = copy_stage_matrix ( mtx, s.height, s.width);			// Copie de la variable temporaire mtx dans une variable alloué dynamiquement a al bonne taille

	_init_item_vectors ( &s);
	if ( di.n_items > 0)
	{
		s.v_i[0] = itbob_copy_item ( di.items [ rand() % di.n_items]);
		s.v_i[1] = itbob_copy_item ( di.items [ rand() % di.n_items]);
	}

	return s;
}
