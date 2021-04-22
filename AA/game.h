#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <string.h>

#define true 1
#define false 0
#define singleShot 0
#define LEN_BULLET 1000
#define WIDTH 1280
#define HEIGHT 720
#define HEIGHT_BULLET 45

typedef int bool;

typedef struct Ship
{
	int hp;
	int speed;
	int x_axis;
	int y_axis;
	int fire;
	bool isPlayer;
	SDL_Rect srcrect;
	SDL_Rect dstrect;
	SDL_Texture *texture;
} Ship;

typedef struct EnemyShip
{
	int type;
	bool spawned;
	Ship *enemy;
} EnemyShip;

typedef struct
{
	int up;
	int down;
	int left;
	int right;
	Ship *ally;
} PlayerShip;

typedef struct Bullet
{
	int x_axis;
	int y_axis;
	int speed;
	int damage;
	Ship *owner;
	SDL_Texture *texture;
} Bullet;

typedef struct BulletVector
{
	Bullet **bullets;
	int firstEmpty;
	int lenghtVector;
} BulletVector;

int startGameScreen();
int gameLoop(SDL_Window *, SDL_Renderer *, SDL_Texture *);
int gameMenu();
void blit(SDL_Texture *, SDL_Renderer *, int, int);
SDL_Texture *loadShipImage(char *, SDL_Renderer *);
SDL_Point getSize(SDL_Texture *);

EnemyShip *createEnemyShip(int);
PlayerShip *createPlayerShip(SDL_Renderer *);
Bullet *createBullet(Ship *, BulletVector *, SDL_Renderer *);
BulletVector *createBulletVector(void);

EnemyShip **moveEnemies(EnemyShip **, int);
PlayerShip *movePlayer(PlayerShip *, EnemyShip **);
BulletVector *moveBullet(BulletVector *);
PlayerShip *doKeyDown(SDL_KeyboardEvent *, PlayerShip *);
PlayerShip *doKeyUp(SDL_KeyboardEvent *, PlayerShip *);

void addBulletInVector(Bullet *, BulletVector *);

bool shipColision(PlayerShip *, EnemyShip **);
bool isOffScreen(int, int, EnemyShip *);

int max(int, int);
int min(int, int);