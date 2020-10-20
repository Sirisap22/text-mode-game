#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <math.h>

void gotoxy(int, int);
void setcursor(bool);
void setcolor(int, int);
void check_input();

void draw_ship(int, int);
void erase_ship(int, int);
void ship_movement();
bool check_wall_ship(int, int, char);

void draw_bullet(int, int);
void erase_bullet(int, int);
void bullet_movement();
bool check_wall_bullet(int, int);
int choose_empty_bullet_slot();

void draw_star(int, int);
void erase_star(int, int);
void update_stars();
int* random_star_position();
bool check_star_duplicate_position(int*, int);
bool check_bullet_star_collision(int, int);

/*------------- game config--------------- */

// default config
const int DEFAULT_COLOR[2] = { 7, 0 };
const int GAME_SPEED = 100; // the smaller the faster game are.

// ship config
const int SHIP_COLOR[2] = { 2, 4 };

// bullet config
const int BULLET_COLOR[2] = { 6, 6 };
const int BULLETS_SIZE = 5;

// star config
const int STAR_COLOR[2] = { 6, 0 };
const int STARS_NUMBER = 20;

/*-------------- gobal variables --------------*/

// input keyboard
char ch = '.';

// ship variables
int x = 38, y = 20;
char ship_direction = 'S';

// bullet variables
typedef struct Bullet {
	int x;
	int y;
	bool is_bullet_activate;
} Bullet;
Bullet bullets[BULLETS_SIZE];
int bullets_capacity = 0;

typedef struct Star {
	int x;
	int y;
	bool is_star_alive;
} Star;
Star stars[STARS_NUMBER];

int main() {
	/* initial state */

	// set a seed
	srand(time(NULL));

	// initialize ship
	setcolor(SHIP_COLOR[0], SHIP_COLOR[1]);
	draw_ship(x, y);		
	setcursor(0);
	// initialize bullets
	for (int i = 0; i < BULLETS_SIZE; i++) {
		bullets[i].x = 0;
		bullets[i].y = 0;
		bullets[i].is_bullet_activate = false;
	}
	// initialize stars
	for (int i = 0; i < STARS_NUMBER; i++) {
		bool duplicate = true;
		int* coord = new int[2];
		while (duplicate) {
			coord = random_star_position();
			if (!check_star_duplicate_position(coord, i)) {
				duplicate = false;
			}
		}
		stars[i].x = coord[0];
		stars[i].y = coord[1];
		stars[i].is_star_alive = false;
	}
	for (int i = 0; i < STARS_NUMBER; i++) {
		if (!stars[i].is_star_alive) {
			draw_star(stars[i].x, stars[i].y);
			stars[i].is_star_alive = true;
		}
	}

	/* game loop  */

	do {
		check_input();
		ship_movement();
		bullet_movement();
		update_stars();
		Sleep(GAME_SPEED);
	} while (ch != 'x');
	return 0;
}

void check_input() {
	if (_kbhit()) {
		ch = _getch();

		// ship direction
		
		if (ch == 'a') {
			ship_direction = 'L';
		}
		if (ch == 'd') {
			ship_direction = 'R';
		}
		if (ch == 's') {
			ship_direction = 'S';
		}

		// bullet state
		if (ch == ' ') {
			if (bullets_capacity < BULLETS_SIZE) {
				bullets_capacity = bullets_capacity + 1;
				int curr_bul = choose_empty_bullet_slot();
				bullets[curr_bul].is_bullet_activate = true;
				erase_bullet(bullets[curr_bul].x, bullets[curr_bul].y);
				bullets[curr_bul].x = x + 3;
				bullets[curr_bul].y = y - 1;
			}
		}
		fflush(stdin);
	}
}

// util functions

void gotoxy(int x, int y) {
	COORD c = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void setcursor(bool visible) {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = visible;
	lpCursor.dwSize = 20;
	SetConsoleCursorInfo(console, &lpCursor);
}

void setcolor(int fg, int bg) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, bg * 16 + fg);
}

// ship functions

void draw_ship(int x, int y) {
	setcolor(SHIP_COLOR[0], SHIP_COLOR[1]);
	gotoxy(x, y);
	printf(" <-0-> ");
	setcolor(DEFAULT_COLOR[0], DEFAULT_COLOR[1]);
}

void erase_ship(int x, int y) {
	gotoxy(x, y);
	setcolor(DEFAULT_COLOR[0], DEFAULT_COLOR[1]);
	printf("       ");
}

void ship_movement() {
	if (ship_direction != 'S') {
		if (ship_direction == 'L' && !check_wall_ship(x, y, 'L')) {
			erase_ship(x, y);
			draw_ship(--x, y);
		}
		else if (ship_direction == 'R' && !check_wall_ship(x, y, 'R')) {
			erase_ship(x, y);
			draw_ship(++x, y);
		}
	}
}

bool check_wall_ship(int x, int y, char dir) {
	switch (dir) {
		case 'R':
			if (x >= 80) 
				return 1;
			break;
		case 'L':
			if (x <= 0) 
				return 1;
			break;
	}
	return 0;
}

// bullet functions

void draw_bullet(int x, int y) {
	setcolor(BULLET_COLOR[0], BULLET_COLOR[1]);
	gotoxy(x, y);
	printf("^");
	setcolor(DEFAULT_COLOR[0], DEFAULT_COLOR[1]);
}

void erase_bullet(int x, int y) {
	gotoxy(x, y);
	setcolor(DEFAULT_COLOR[0], DEFAULT_COLOR[1]);
	printf(" ");
}

void bullet_movement() {
	for (int i = 0; i < BULLETS_SIZE; i++) {
		if (bullets[i].is_bullet_activate) {
			erase_bullet(bullets[i].x, bullets[i].y);
			if (check_bullet_star_collision(bullets[i].x, bullets[i].y)) { // check star collision
			bullets[i].is_bullet_activate = false;
			bullets_capacity = bullets_capacity - 1;
			Beep(700, 100);
			erase_bullet(bullets[i].x, bullets[i].y);
			}
			else if (!check_wall_bullet(bullets[i].x, bullets[i].y)) {
				draw_bullet(bullets[i].x, --bullets[i].y);
			}
			else if (check_wall_bullet(bullets[i].x, bullets[i].y)) {
				bullets[i].is_bullet_activate = false;
				bullets_capacity = bullets_capacity - 1;
				erase_bullet(bullets[i].x, bullets[i].y);
			} 

		}
	}
}

bool check_wall_bullet(int x, int y) {
	if (y <= 0) return 1;
	return 0;
}

int choose_empty_bullet_slot() {
	for (int i = 0; i < BULLETS_SIZE; i++) {
		if (!(bullets[i].is_bullet_activate)) {
			return i;
		}
	}
	return -1; // never reach!
}

// star functions

int* random_star_position() {
	int* coord = new int[2];
	coord[0] = round(((double)rand() / (double)RAND_MAX) * 60 + 10);
	coord[1] = round(((double)rand() / (double)RAND_MAX) * 3 + 2);
	return coord;
}

bool check_star_duplicate_position(int* coord, int current_position) {
	for (int i = 0; i < STARS_NUMBER; i++) {
		if (i == current_position)
			continue;
		if (coord[0] == stars[i].x && coord[1] == stars[i].y)
			return true;
	}
	return false;
}

void draw_star(int x, int y) {
	setcolor(STAR_COLOR[0], STAR_COLOR[1]);
	gotoxy(x, y);
	printf("*");
	setcolor(DEFAULT_COLOR[0], DEFAULT_COLOR[1]);
}

void erase_star(int x, int y) {
	gotoxy(x, y);
	setcolor(DEFAULT_COLOR[0], DEFAULT_COLOR[1]);
	printf(" ");
}

void update_stars() {
	for (int i = 0; i < STARS_NUMBER; i++) {
		if (!stars[i].is_star_alive) {
			bool duplicate = true;
			int* coord = new int[2];
			while (duplicate) {
				coord = random_star_position();
				if (!check_star_duplicate_position(coord, i)) {
					duplicate = false;
				}
			}
			stars[i].x = coord[0];
			stars[i].y = coord[1];
			stars[i].is_star_alive = true;
			draw_star(stars[i].x, stars[i].y);
		}
	}
}

// collision check

bool check_bullet_star_collision(int x, int y) {
		for (int j = 0; j < STARS_NUMBER; j++) {
			if (stars[j].is_star_alive &&
				x == stars[j].x &&
				y == stars[j].y) {
				stars[j].is_star_alive = false;
				return true;
			}
		}
	return false;
}



