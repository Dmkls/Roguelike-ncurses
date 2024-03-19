#include <curses.h>
#include <vector>
#include <stdlib.h>
#include <time.h>

int px, py; // player coords

bool t_placed = 0;
bool p_placed = 0;
int r_placed = 0;
int p_gold = 0;
int dlvl = 1;

class Monster {
	public:
		int y;
		int x;
		int hp;
};

Monster monsters[10];

void battle(std::vector<std::vector<char>>&map, int dir_y, int dir_x)
{
	for (int m = 0; m < 10; m++)
	{
		if (dir_y == monsters[m].y && dir_x == monsters[m].x)
		{
			if (monsters[m].hp <= 0)
			{
				map[dir_y][dir_x] = ' ';
				p_gold += rand() % 10 + 1;
			}
			else if (monsters[m].hp >= 1)
			{
				monsters[m].hp -= 1;
			}
			break;
		}
	}
}

void corridor(int y, int x, int y2, int x2, std::vector<std::vector<char>>&map)
{
	int y_saved = y;
	for (y_saved = y; y_saved != y2; )
	{
		if (map[y_saved][x] != '%')
			map[y_saved][x] = ' ';
		
		if (y < y2)
			y_saved++;
		else
			y_saved--;
	}
	
	for (int xx = x; xx != x2; )
	{
		if (map[y_saved][xx] != '%')
			map[y_saved][xx] = ' ';
		
		if (x < x2)
			xx++;
		else
			xx--;
	}
}

void bigcorridor(int y, int x, int y2, int x2, std::vector<std::vector<char>>&map)
{
	for (int yy = std::min(y, y2); yy <= std::max(y, y2); yy++)
	{
		for (int xx = std::min(x, x2); xx <= std::max(x, x2); xx++)
			map[yy][xx] = ' ';
	}
}

int p_action(int c, std::vector<std::vector<char>>&map)
{
	int dir_y = py, dir_x = px;
	
	if		(c == KEY_UP)
		dir_y--;
	else if (c == KEY_DOWN)
		dir_y++;
	else if (c == KEY_RIGHT)
		dir_x++;
	else if (c == KEY_LEFT)
		dir_x--;
	else if (c == '>' && map[py][px] == '>')
	{
		r_placed = 0;
		p_placed = 0;
		t_placed = 0;
		return 1;
	}
	
	if (map[dir_y][dir_x] == ' ' || map[dir_y][dir_x] == '>')
	{
		py = dir_y;
		px = dir_x;
	}
	else if (map[dir_y][dir_x] == 't')
		battle(map, dir_y, dir_x);
	return 0;
	
}

void dungeon_draw(int rows, int cols, std::vector<std::vector<char>>&map)
{
	for (int y = 1; y <= rows-1; y++)
	{
		for (int x = 0; x <= cols; x++)
		{
			mvaddch(y, x, map[y][x]);
		}
	}
	
	mvprintw(rows, 0, "Gold: %d \tDlvl - %d", p_gold, dlvl);
}

void dungeon_gen(int rows, int cols, std::vector<std::vector<char>>&map)
{
	if (!r_placed)
	{	
		int rx, ry;
		int r_sizeY, r_sizeX;
		int room_num = rand() % 5 + 5;
		bool collision = 0;
		int r_centerY, r_centerX = -1;
		int r2_centerY, r2_centerX = -1;
		int sy, sx;
		// fill dungeon with walls and borders
		for (int y = 0; y <= rows; y++)
		{
			for (int x = 0; x <= cols; x++)
			{
				// borders
				if (y == 0 || y == 1 || x == 0 || y == rows || x == cols || y == (rows - 1))
					map[y][x] = '%';
				// walls
				else
					map[y][x] = '#';
			}	
		}
		
		while (r_placed < room_num)
		{
			
			int try_counter = 0;
			do 
			{
			collision = 0;
			// generate room coords
			ry = rand() % (rows - 5) + 2;
			rx = rand() % (cols - 7) + 1;
			
			// generate room size
			r_sizeY = rand() % 5 + 7;
			r_sizeX = rand() % 10 + 10;
			if (try_counter > 10)
			{
				r_sizeY = 10 - try_counter / 20;
				r_sizeX = 20 - try_counter / 10;
			}
			
			for (int y = ry; y <= ry+r_sizeY; y++)
			{
				if (y >= rows-1) {break;}
				for (int x = rx; x <= rx+r_sizeX; x++)
				{
					if (x >= cols) {break;}
					if (!(x + 2 > cols || x < 2 ||
						y + 2 > rows || y < 2))
					{
						if (map[y][x] == ' ' ||
							map[y+2][x] == ' ' || map[y-2][x] == ' ' ||
							map[y][x+2] == ' ' || map[y][x-2] == ' ')
						{
							collision = 1;
							try_counter++;
							y = ry+r_sizeY+1;
							break;
						}
					}
				}
			}
			}
			while (collision);
						
			for (int y = ry; y <= ry+r_sizeY; y++)
			{
				if (y >= rows-1) {break;}
				for (int x = rx; x <= rx+r_sizeX; x++)
				{
					if (x >= cols) {break;}
					map[y][x] = ' ';
				}
			}
			
			r_placed++;
			
			// corridors
			if (r_placed > 1)
			{				
				r2_centerY = r_centerY;
				r2_centerX = r_centerX;
			}
			
			r_centerY = ry + r_sizeY/2;
			r_centerX = rx + r_sizeX/2;
			
			if (r_centerY >= rows-1)
				r_centerY = rows-2;
			if (r_centerX >= cols)
				r_centerX = cols-1;
			
			if (r_placed > 1)
				corridor(r_centerY, r_centerX, r2_centerY, r2_centerX, map);
			
			// for final dungeon
			//if (r_centerX != -1)
			//{
			//	r2_centerY = ry + r_sizeY/2;
			//	r2_centerX = rx + r_sizeX/2;
			//	corridor(r_centerY, r_centerX, r2_centerY, r2_centerX, map);
			//	printf("corridors +1\n" );
			//}
			//if (r_centerX == -1)
			//{
			//	r_centerY = ry + r_sizeY/2;
			//	r_centerX = rx + r_sizeX/2;
			//}
			//if (r2_centerX != -1)
			//{
			//	r_centerY = r2_centerY;
			//	r_centerX = r2_centerX;
			//}
		};
	do
	{
		sy = rand() % rows;
		sx = rand() % cols;
	}
	while(map[sy][sx] != ' ');
	map[sy][sx] = '>';
	}
}

void place_p(int rows, int cols, std::vector<std::vector<char>>&map)
{
	do 
	{
		py = rand() % rows;
		px = rand() % cols;
	}
	while (map[py][px] != ' ');
	p_placed = 1;
}

void place_t(int rows, int cols, std::vector<std::vector<char>>&map)
{
	for (int m = 0; m < 10; m++)
	{
		int my, mx; // goblin coords
		do 
		{
			my = rand() % rows;
			mx = rand() % cols;
		}
		while (map[my][mx] != ' ');
		
		monsters[m].y = my;
		monsters[m].x = mx;
		monsters[m].hp = rand() % 5 + 1;
		map[my][mx] = 't';
		
	}
	t_placed = 1;
}

int game_loop(int c, int rows, int cols, std::vector<std::vector<char>>&map)
{
	
	int new_lvl = 0;
	srand(time(NULL));
		
	// coords for room
	dungeon_gen(rows, cols, map);
	
	if (!t_placed)
		place_t(rows, cols, map);
	
	if (!p_placed)
		place_p(rows, cols, map);
	
	if (c != 0)
		new_lvl = p_action(c, map); // +battle()
	
	dungeon_draw(rows, cols, map);
		
	mvaddch(py, px, '@'); // print cursor
	
	if (new_lvl)
	{	
		// mvprintw(0, 0, "Welcome to level %d (Press any button to continue)", ++dlvl);
		// or
		clear();
		mvprintw(rows/2, cols/2 - 9, "Welcome to level %d", ++dlvl);
	}
	
	c = getch();
	
	return c;
}

int main()
{
	int c = 0;
	int cols, rows;

	initscr();
	noecho(); // don't display input
	curs_set(0); // hide cursor
	keypad(stdscr, 1); // allow arrows

	getmaxyx(stdscr, rows, cols);
	
	std::vector<std::vector<char>> map (rows, std::vector<char>(cols, '#'));

	do 
	{
		c = game_loop(c, rows-1, cols-1, map);

	}
	while (c!= 27); // 27 - ESC

	endwin();

	return 0;
}