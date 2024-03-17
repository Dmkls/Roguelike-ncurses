#include <ncurses.h>

int main()
{
	int c;
	int x = 10, y = 10;
	int cols, rows;

	initscr();
	noecho(); // don't display input
	curs_set(0); // hide cursor
	keypad(stdscr, 1); // allow arrows

	getmaxyx(stdscr, rows, cols);
	char map[rows][cols];
	do 
	{
		for (int yy = 0; yy <= rows; yy++)
		{
			for (int xx = 0; xx <= cols; xx++)
			{
				map[yy][xx] = '#';
				mvaddch(yy, xx, '#');
			}
		}

		for (int yy = 7; yy <= rows/2; yy++)
		{
			for (int xx = 11; xx <= cols/2; xx++)
			{
				map[yy][xx] = '0';
				mvaddch(yy, xx, ' ');
			}
		}
		if (c == KEY_UP) y--;
		else if (c == KEY_DOWN) y++;
		else if (c == KEY_RIGHT) x++;
		else if (c == KEY_LEFT) x--;
		mvaddch(y, x, '@'); // print cursor
	}
	while ((c = getch()) != 27); // 27 - ESC

	endwin();

	return 0;
}
