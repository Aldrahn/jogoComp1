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
	int x;
	int y;
	char textureDir[50];
	SDL_Rect srcrect;
	SDL_Rect dstrect;

} Ship;

typedef struct EnemyShip
{
	int type;
	Ship* enemy;

} EnemyShip;

int gameLoop(SDL_Window*, SDL_Renderer*, SDL_Texture*);
int gameMenu();	
int startGameScreen();
EnemyShip* createEnemyShip(int);