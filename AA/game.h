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
	int movement;
	int upDown;
	int leftRight;
	int pointsWorth;
	int type;
	bool spawned;
	Ship* enemy;
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
bool startMenu(SDL_Window*, SDL_Renderer*, SDL_Texture*);
int gameLoop(SDL_Window *, SDL_Renderer *);
int gameMenu();
void blit(SDL_Texture *, SDL_Renderer *, int, int, PlayerShip*);
SDL_Texture *loadShipImage(char *, SDL_Renderer *);
SDL_Point getSize(SDL_Texture *);

EnemyShip* createEnemyShip(int, int, int, int, int, int);
PlayerShip *createPlayerShip(SDL_Renderer *);
Bullet *createBullet(Ship *, BulletVector *, SDL_Renderer *);
BulletVector *createBulletVector(void);

EnemyShip **moveEnemies(EnemyShip **, PlayerShip*, int);
PlayerShip *movePlayer(PlayerShip *, EnemyShip **, int);
BulletVector *moveBullet(BulletVector *,EnemyShip** , PlayerShip* , int );
PlayerShip *doKeyDown(SDL_KeyboardEvent *, PlayerShip *);
PlayerShip *doKeyUp(SDL_KeyboardEvent *, PlayerShip *);

void addBulletInVector(Bullet *, BulletVector *);

bool bulletCollision(Ship*, Bullet* );
bool shipColision(PlayerShip*, EnemyShip**, int);
bool isOffScreen(int, int, EnemyShip*);

int max(int, int);
int min(int, int);

bool checkWaveStatus(EnemyShip**, int);
bool spawnRequest(EnemyShip**, int);
void changeShipColor(PlayerShip*);
bool isPlayerMoving(PlayerShip*);
void score(int*);