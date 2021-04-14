#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h" 
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#define true 1
#define false 0
#define singleShot 0

typedef struct Ship
{
	int hp;
	int speed;
	int bulletType;
	int x_axis;
	int y_axis;
	SDL_Rect srcrect;
	SDL_Rect dstrect;
	SDL_Texture* texture;
} Ship;

typedef struct EnemyShip
{
	int type;
	Ship* enemy;
} EnemyShip;

typedef struct{
	
	int up;
	int down;
	int left;
	int right;
	Ship* ally;
} PlayerShip;

int gameLoop(SDL_Window*, SDL_Renderer*, SDL_Texture*);
int gameMenu();	
int startGameScreen();
EnemyShip* createEnemyShip(int enemyType);