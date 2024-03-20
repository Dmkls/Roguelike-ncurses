#include <curses.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <fstream>

#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define WHITE 7

bool t_placed = 0;
bool p_placed = 0;
int r_placed = 0;
int dlvl = 1;
int m_num = 0;
int max_gold = 0;
int m_defeated = 0;

class Player {
	public:
		int x;
		int y;
		int gold = 0;
		int att = 1;
		int hp = 0;
};

Player p;

class Monster {
	public:
		int y;
		int x;
		int lvl;
		int type;
		bool awake;
};

int best_result()
{
	std::string line = "";
	
	std::ifstream in("db.txt");
	
	int res, bres = 0;
	
	if (in.is_open())
    {
        while (std::getline(in, line))
        {
			int end = line.find(';');
			res = stoi(line.substr(1, end-1));
			if (res > bres)
				bres = res;
        }
    }
    in.close(); 
	return bres;
}

void results_write()
{
	std::ofstream out("db.txt", std::ios::app);
	if (out.is_open())
	{
		out << '{' << p.gold << ';' << m_defeated << ';' << dlvl << '}' << std::endl;
	}
	out.close();   
}

void read_config()
{
	std::string line = "";
	
	std::ifstream in("config.txt");
	if (in.is_open())
    {
        while (std::getline(in, line))
        {
            int n = line.find(':');
			int end = line.find(';');
			if (!m_num)
				m_num = stoi(line.substr(n+2, end-n-2));
			else if (!max_gold)
				max_gold = stoi(line.substr(n+2, end-n-2));
			else if (!p.hp)
				p.hp = stoi(line.substr(n+2, end-n-2));
        }
    }
    in.close(); 
}

void monster_turn(std::vector<std::vector<char>>&map, Monster* monsters)
{
	int dist_y, dist_x;
	
	for (int m = 0; m < m_num; m++)
	{
		
		if (monsters[m].lvl <= 0)
			continue;
		
		dist_y = abs(p.y - monsters[m].y);
		dist_x = abs(p.x - monsters[m].x);
		
		if (dist_y < 5 && dist_x < 5)
			monsters[m].awake = 1;
		else
			monsters[m].awake = 0;
		
		if (!monsters[m].awake)
			continue;
		
		int dir_y = monsters[m].y, dir_x = monsters[m].x;
		
		if (dist_y > dist_x)
		{
			if (dir_y < p.y)
				dir_y++;
			else
				dir_y--;
		}
		else
		{
			if (dir_x < p.x)
				dir_x++;
			else
				dir_x--;
		}
		
		
		if (map[dir_y][dir_x] == '#' || map[dir_y][dir_x] == '%' || map[dir_y][dir_x] == '>' || map[dir_y][dir_x] == 't')
		{
			dir_y = monsters[m].y;
			dir_x = monsters[m].x;
			if (dir_y < p.y)
				dir_y++;
			else
				dir_y--;
			
			if (dir_x < p.x)
				dir_x++;
			else
				dir_x--;
		}
		
		if (map[dir_y][dir_x] == '#' || map[dir_y][dir_x] == '%' ||  map[dir_y][dir_x] == '>' || map[dir_y][dir_x] == 't')
		{
			dir_y = monsters[m].y;
			dir_x = monsters[m].x;
			if (dist_y > dist_x)
			{
				if (dir_y > p.y)
					dir_y++;
				else
					dir_y--;
			}
			else
			{
				if (dir_x > p.x)
					dir_x++;
				else
					dir_x--;
			}
		}
		
		if (dir_y == p.y && dir_x == p.x)
		{
			p.hp -= dlvl/2 + 1;
		}
		else if (map[dir_y][dir_x] == ' ')
		{
			map[monsters[m].y][monsters[m].x] = ' ';
			monsters[m].y = dir_y;
			monsters[m].x = dir_x;
			map[monsters[m].y][monsters[m].x] = 't';
		}
	}
}

void battle(std::vector<std::vector<char>>&map, int dir_y, int dir_x, Monster* monsters)
{
	for (int m = 0; m < m_num; m++)
	{
		if (dir_y == monsters[m].y && dir_x == monsters[m].x)
		{
			if (monsters[m].lvl >= 1)
			{
				monsters[m].lvl -= p.att;
			}
			if (monsters[m].lvl <= 0)
			{
				map[dir_y][dir_x] = ' ';
				m_defeated++;
				p.gold += rand() % max_gold + 1;
			};
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

int p_action(int c, std::vector<std::vector<char>>&map, Monster* monsters)
{
	int dir_y = p.y, dir_x = p.x;
	
	if		(c == KEY_UP)
		dir_y--;
	else if (c == KEY_DOWN)
		dir_y++;
	else if (c == KEY_RIGHT)
		dir_x++;
	else if (c == KEY_LEFT)
		dir_x--;
	else if (c == '>' && map[p.y][p.x] == '>')
	{
		r_placed = 0;
		p_placed = 0;
		t_placed = 0;
		return 1;
	}
	
	if (map[dir_y][dir_x] == ' ' || map[dir_y][dir_x] == '>')
	{
		p.y = dir_y;
		p.x = dir_x;
	}
	else if (map[dir_y][dir_x] == 't')
		battle(map, dir_y, dir_x, monsters);
	return 0;
	
}

void dungeon_draw(int rows, int cols, std::vector<std::vector<char>>&map, Monster* monsters)
{
	for (int y = 0; y <= rows; y++)
	{
		for (int x = 0; x <= cols; x++)
		{
			if (y == 0)
			{
				mvaddch(y, x, ' ');
			}
			else if (y == rows)
			{
				mvaddch(y, x, ' ');
			}
			else if (map[y][x] == 't')
			{
				for (int m = 0; m < m_num; m++)
				{
					if (monsters[m].y == y && monsters[m].x == x)
					{
						if (monsters[m].lvl < (((monsters[m].type - 96)*3) + (monsters[m].type-97)*3)/3)
						{
							attron(COLOR_PAIR(RED));
								mvaddch(y, x, monsters[m].type);
							standend();
						}
						else if (monsters[m].lvl < (((monsters[m].type - 96)*3) + (monsters[m].type-97)*3)/2)
						{
							attron(COLOR_PAIR(YELLOW));
								mvaddch(y, x, monsters[m].type);
							standend();
						}
						else
							mvaddch(y, x, monsters[m].type);
						
						break;
					}
				}
			}
			else if (map[y][x] == '>')
			{
				attron(A_BOLD);
					mvaddch(y, x, map[y][x]);
				standend();
			}
			else
				mvaddch(y, x, map[y][x]);
		}
	}
	
	mvprintw(rows, 0, "HP: %d \tAtt: %d \tGold: %d \tDlvl - %d",p.hp, p.att, p.gold, dlvl);
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
		p.y = rand() % rows;
		p.x = rand() % cols;
	}
	while (map[p.y][p.x] != ' ');
	p_placed = 1;
}

void place_t(int rows, int cols, std::vector<std::vector<char>>&map, Monster* monsters)
{
	for (int m = 0; m < m_num; m++)
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
		monsters[m].type = rand() % dlvl + 97;
		monsters[m].lvl = rand() % ((monsters[m].type - 96)*3) + (monsters[m].type-97)*3;
		monsters[m].awake = 0;
		
		map[my][mx] = 't';
		
	}
	t_placed = 1;
}

int game_loop(int c, int rows, int cols, std::vector<std::vector<char>>&map, Monster* monsters)
{
	
	int new_lvl = 0;
	srand(time(NULL));
		
	// coords for room
	dungeon_gen(rows, cols, map);
	
	if (!t_placed)
		place_t(rows, cols, map, monsters);
	
	if (!p_placed)
		place_p(rows, cols, map);
	
	if (c != 0)
		new_lvl = p_action(c, map, monsters); // +battle()
	
	monster_turn(map, monsters);
	
	dungeon_draw(rows, cols, map, monsters);
	
	attron(A_BOLD);
		mvaddch(p.y, p.x, '@'); // print cursor
	attroff(A_BOLD);
	
	if (p.hp <= 0)
	{
		clear();
		mvprintw(rows/2, cols/2, R"(
	_.---,._,'
       /' _.--.<
         /'     `'
       /' _.---._____
       \.'   ___, .-'`
           /'    \\             .
         /'       `-.          -|-
        |                       |
        |                   .-'~~~`-.
        |                 .'         `.
        |                 |  R  I  P  |
        |                 |           |
        |                 |           |
         \              \\|           |//)");
   
		for (int c = 0; c < cols; c++)
		{
			mvaddch(rows/2+14, c, '^');
		}
		
		
		mvprintw(rows/2, cols/2 - 10, "RIP. You had %d gold", p.gold);
		mvprintw(rows/2+2, cols/2 - 10, "Best result is - %d", best_result());
		results_write();		
	}
	if (new_lvl)
	{	
		mvprintw(0, 0, "Welcome to level %d (Press any button to continue)", ++dlvl);
		// or
		// clear();
		// mvprintw(rows/2, cols/2 - 9, "Welcome to level %d", ++dlvl);
	}
	
	c = getch();
	
	return c;
}

int main()
{
	int c = 0;
	int cols, rows;

	initscr();
	start_color();
    use_default_colors();

    init_pair(BLACK, COLOR_BLACK, COLOR_WHITE);
    init_pair(RED, COLOR_RED, COLOR_BLACK);
    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(WHITE, COLOR_WHITE, COLOR_WHITE);
	
	noecho(); // don't display input
	curs_set(0); // hide cursor
	keypad(stdscr, 1); // allow arrows

	getmaxyx(stdscr, rows, cols);
	
	read_config();
	Monster monsters[m_num];
	
	std::vector<std::vector<char>> map (rows, std::vector<char>(cols, '#'));

	do 
	{
		c = game_loop(c, rows-1, cols-1, map, monsters);
	}
	while (c!= 27); // 27 - ESC
	
	endwin();

	return 0;
}