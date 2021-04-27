#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h" 
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <string.h>
#define horizontal 0
#define vertical 1
#define diagonal 2
#define esquerda 0 
#define direita 1
#define cima 0
#define baixo 1
#define true 1
#define false 0
#define singleShot 0

typedef int bool;

typedef struct Bullet
{
	int bulletType;
	int x_axis;
	int y_axis;
	
} Bullet;

typedef struct Ship
{
	int hp;
	int speed;
	int x_axis;
	int y_axis;
	SDL_Rect srcrect;
	SDL_Rect dstrect;
	SDL_Texture* texture;
	Bullet* bullet;
} Ship;

typedef struct EnemyShip
{
	int movement;
	int upDown;
	int leftRight;
	int pointsWorth;
	bool spawned;
	Ship* enemy;
} EnemyShip;

typedef struct
{
	int up;
	int down;
	int left;
	int right;
	Ship* ally;

} PlayerShip;


int startGameScreen();
int gameLoop(SDL_Window*, SDL_Renderer*, SDL_Texture*);
int gameMenu();	
void blit(SDL_Texture*, SDL_Renderer*, int, int);
SDL_Texture* loadShipImage(char*, SDL_Renderer*);


EnemyShip* createEnemyShip(int, int, int, int, int, int);
PlayerShip* createPlayerShip(SDL_Renderer*);

EnemyShip** moveEnemies(EnemyShip**, int); 
PlayerShip* movePlayer(PlayerShip*, EnemyShip**);
PlayerShip* doKeyDown(SDL_KeyboardEvent*, PlayerShip*);
PlayerShip* doKeyUp(SDL_KeyboardEvent*, PlayerShip*);

bool shipColision(PlayerShip*, EnemyShip**);
bool isOffScreen(int, int, EnemyShip*);

int max(int, int);
int min(int, int);

bool checkWaveStatus(EnemyShip**, int);
bool spawnRequest(EnemyShip**, int);