#include <ncurses.h>
#include <vector>

int x = 12, y = 10;

char map [37][100];
void dungeon(int c, int rows, int cols)
{

	for (int yy = 0; yy < rows; yy++)
	{
		for (int xx = 0; xx < cols; xx++)
		{
			map[yy][xx] = '#';
			mvaddch(yy, xx, '#');
		}	
	}
	
	for (int yy = 7; yy < rows/2; yy++)
	{
		for (int xx = 11; xx < cols/2; xx++)
		{
			map[yy][xx] = ' ';
			mvaddch(yy, xx, ' ');
		}
	}


	if (c == KEY_UP && map[y-1][x] != '#') y--;
	else if (c == KEY_DOWN && map[y+1][x] != '#') y++;
	else if (c == KEY_RIGHT && map[y][x+1] != '#') x++;
	else if (c == KEY_LEFT && map[y][x-1] != '#') x--;
	
	mvaddch(y, x, '@'); // print cursor
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

	do 
	{
		dungeon(c, rows, cols);

	}
	while ((c = getch()) != 27); // 27 - ESC

	endwin();

	return 0;
}
