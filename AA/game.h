#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <time.h>
#define horizontal 0
#define vertical 1
#define diagonal 2
#define esquerda 0 
#define direita 1
#define cima 0
#define baixo 1
#define sequential 0
#define paralel 1
#define true 1
#define false 0
#define colisionDamage 0
#define singleShot 0
#define BULLET_DAMAGE 5
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
	SDL_Rect srcrectExplosion;
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
	int spawnType;
	int dist;
	bool stopCriteria;
	int stopValue;
	time_t timeStop;
	time_t timeResume;
	bool timeRunning;
	bool doDmgAnimation;
	bool doDestroyingAnimation;
	int frameTime;
	Ship* enemy;
	
} EnemyShip;

typedef struct
{
	int up;
	int down;
	int left;
	int right;
	int points;
	int frameTime;
	bool invincible;
	bool doInvencibilityAnimation;
	bool doDmgAnimation;
	time_t invincibilityTimeStart;
	Ship *ally;
} PlayerShip;

typedef struct Bullet
{
	int x_axis;
	int y_axis;
	int speed;
	int damage;
	int type;
	int fireRate;
	int start_x;
	int start_y;
	time_t fireStart;
	time_t fireStop;
	bool isShooting;
	Ship *owner;
	SDL_Texture *texture;
	SDL_Rect srcrect;
	SDL_Rect dstrect;
} Bullet;

typedef struct BulletVector
{
	Bullet **bullets;
	int firstEmpty;
	int lenghtVector;
} BulletVector;

typedef struct Explosion
{
	SDL_Rect srcrect;
	SDL_Rect dstrect;
}Explosion;

int startGameScreen();
bool startMenu(SDL_Window*, SDL_Renderer*);
int gameLoop(SDL_Window *, SDL_Renderer *);
int gameOver(SDL_Window *, SDL_Renderer *);
void blit(SDL_Texture *, SDL_Renderer *, int, int, PlayerShip*);
SDL_Texture *loadShipImage(char *, SDL_Renderer *);
SDL_Point getSize(SDL_Texture *);

EnemyShip* createEnemyShip(int, int, int, int, int, int, int, int, int);
PlayerShip *createPlayerShip(SDL_Renderer *);
Bullet *createBullet(Ship *, BulletVector *, int, int);
BulletVector *createBulletVector(void);

EnemyShip **moveEnemies(EnemyShip **, PlayerShip*, BulletVector*, int);
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
bool spawnRequest(EnemyShip**, int, BulletVector*, Ship*);
void changeShipColor(PlayerShip*);
bool isPlayerMoving(PlayerShip*);
bool shouldEnemyMove(EnemyShip*);
EnemyShip** waveLoader(EnemyShip**, int*, int);
PlayerShip* playerRule(PlayerShip*);
bool isInvincible(time_t);
BulletVector* rearrangeBulletVector(BulletVector*);

void enemyDmgAnimation(EnemyShip*);
void playerInvencibilityAnimation(PlayerShip*);
void playerDmgAnimation(PlayerShip*);
void destructionAnimation(Ship* ship);

//funções de arquivo
void score(char*, int);
void organizeHighScore(int, char*);
int getOffset(FILE*);
void readData(FILE*, int, int*, char*, int);
void readScore(SDL_Renderer* renderer);
int writeThingsOnScreen(SDL_Renderer*, SDL_Texture*, int);
void  updateScreenWithText(SDL_Renderer*, TTF_Font*, char*, char*);
SDL_Texture* showScoreOnScreen(SDL_Renderer* renderer, SDL_Texture* bg, int);