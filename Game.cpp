#include <curses.h>
#include <vector>
#include <stdlib.h>
#include <time.h>

int px, py; // player coords
int tx, ty; // goblin coords
int rx, ry, r_sizeY, r_sizeX;

bool t_placed = 0;
bool p_placed = 0;
bool r_placed = 0;
int p_gold = 0;

void dungeon(int c, int rows, int cols, std::vector<std::vector<char>>&map)
{

	srand(time(NULL));
	
	// coords for room
	if (!r_placed)
	{	
		ry = rand() % (rows - 5) + 1;
		rx = rand() % (cols - 7) + 1;
		
		r_sizeY = rand() % 5 + 5;
		r_sizeX = rand() % 10 + 15;
		
		r_placed = 1;
	}
	
	// draw walls and borders
	for (int y = 0; y <= rows; y++)
	{
		for (int x = 0; x <= cols; x++)
		{
			if (y == 0 || x == 0 || y == rows || x == cols)
			{
				map[y][x] = '%';
				mvaddch(y, x, '%');
			}
			else
			{
				map[y][x] = '#';
				mvaddch(y, x, '#');
			}
		}	
	}
	
	// draw room
	for (int y = ry; y <= ry+r_sizeY; y++)
	{
		if (y >= rows) {break;}
		for (int x = rx; x <= rx+r_sizeX; x++)
		{
			if (x >= cols) {break;}
			{
				map[y][x] = ' ';
				mvaddch(y, x, ' ');
			}
		}
	}

	if (c == KEY_UP && map[py-1][px] != '#' && map[py-1][px] != '%') py--;
	else if (c == KEY_DOWN && map[py+1][px] != '#' && map[py+1][px] != '%') py++;
	else if (c == KEY_RIGHT && map[py][px+1] != '#' && map[py][px+1] != '%') px++;
	else if (c == KEY_LEFT && map[py][px-1] != '#' && map[py][px-1] != '%') px--;
	
	if (!p_placed)
	{
		do 
		{
			py = rand() % rows;
			px = rand() % cols;
		}
		while (map[py][px] != ' ');
		p_placed = 1;
	}
	
	if (!t_placed)
	{
		do 
		{
			ty = rand() % rows;
			tx = rand() % cols;
		}
		while (map[ty][tx] != ' ');
		t_placed = 1;
	}
	
	if ((px == tx) && (py == ty))
	{
		t_placed = 0;
		p_gold += rand() % 10 + 1;
	}
	
	mvaddch(ty, tx, 't'); // print goblin
	mvaddch(py, px, '@'); // print cursor
	mvprintw(rows, 0, "Gold: %d", p_gold);
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
	
	std::vector<std::vector<char>> map (rows, std::vector<char>(cols, ' '));

	do 
	{
		dungeon(c, rows-1, cols-1, map);
	}
	while ((c = getch()) != 27); // 27 - ESC

	endwin();

	return 0;
}