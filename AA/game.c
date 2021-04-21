#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "SDL2/SDL.h"
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#define true 1
#define false 0

#define LEN_BULLET 1000

typedef int bool;

int startGameScreen()
{
	//Criando Janela
	SDL_Window *window;

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);

	if (window == NULL)
	{
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	//Criando Renderer
	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	SDL_Renderer *renderer;
	renderer = SDL_CreateRenderer(window, -1, render_flags);

	if (!renderer)
	{
		printf("error creating renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	//Criando Surface
	SDL_Surface *surface = IMG_Load("assets/menu/game_screen.png");
	if (!surface)
	{
		printf("error creating surface\n");
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	//Surface -> Textura
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	if (!texture)
	{
		printf("error creating texture\n");
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	//Clear na Janela
	SDL_RenderClear(renderer);

	//Joga a imagem para a janela
	SDL_RenderCopy(renderer, texture, NULL, NULL);

	SDL_RenderPresent(renderer);

	//Rodando
	gameLoop(window, renderer, texture);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

int gameLoop(SDL_Window *window, SDL_Renderer *rend, SDL_Texture *bg)
{

	bool isRunning = true;
	SDL_Event event;

	int waves = 1;
	EnemyShip **arrayWave = (EnemyShip **)malloc(7 * sizeof(EnemyShip *));
	for (int i = 0; i < waves; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			arrayWave[j] = createEnemyShip(1);
		}
	}
	arrayWave[0]->enemy->texture = loadShipImage("assets/objects/ship.png", rend);

	BulletVector *bulletVector = createBulletVector();
	PlayerShip *player = createPlayerShip(rend);

	blit(player->ally->texture, rend, player->ally->x_axis, player->ally->y_axis);

	int i = 0;

	while (isRunning)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			//Fecha a janela se apertar Esc ou 'X'
			if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
			{
				isRunning = false;
			}

			if (event.type == SDL_KEYDOWN)
			{
				player = movePlayer(doKeyDown(&event.key, player), arrayWave);
				blit(player->ally->texture, rend, player->ally->x_axis, player->ally->y_axis);
				
				if (player->ally->fire == 1)
				{
					Bullet *bullet = createBullet(player->ally, bulletVector, rend);
				}
			}
			
			if (event.type == SDL_KEYUP)
			{
				player = movePlayer(doKeyUp(&event.key, player), arrayWave);
			}
		}

		//Garante que todos os inimigos continuem se movendo
		arrayWave = moveEnemies(arrayWave, i);

		bulletVector = moveBullet(bulletVector);

		if (i < 6 && arrayWave[i]->enemy->dstrect.x == arrayWave[i]->enemy->dstrect.w)
		{
			i++;
		}

		//Atualiza o BG
		SDL_RenderCopy(rend, bg, NULL, NULL);

		//Atualiza o Player
		blit(player->ally->texture, rend, player->ally->x_axis, player->ally->y_axis);
		
		//Atualiza a Wave Inimiga
		for (int j = 0; j <= i; j++)
		{
			SDL_RenderCopy(rend, arrayWave[0]->enemy->texture, &arrayWave[j]->enemy->srcrect, &arrayWave[j]->enemy->dstrect);
			if (arrayWave[j]->spawned == false &&
				arrayWave[j]->enemy->x_axis > 0 &&
				arrayWave[j]->enemy->y_axis > 0 &&
				!(isOffScreen(arrayWave[j]->enemy->x_axis, arrayWave[j]->enemy->y_axis, NULL)))
			{
				arrayWave[j]->spawned = true;
			}
		}

		for (int i = 0; i < bulletVector->firstEmpty; i++)
		{
			if (bulletVector->bullets[i] != NULL)
			{
				blit(bulletVector->bullets[i]->texture, rend, bulletVector->bullets[i]->x_axis, bulletVector->bullets[i]->y_axis);
			}
		}

		//if(shipOffscreen() || shipIsDead()){free();}

		SDL_RenderPresent(rend);
		SDL_UpdateWindowSurface(window);
	}

	//Descarregando Memória

	SDL_DestroyTexture(arrayWave[0]->enemy->texture);
	SDL_DestroyTexture(bg);
	SDL_DestroyTexture(player->ally->texture);

	for (int i = 0; i < 7; i++)
	{
		free(arrayWave[i]->enemy);
		free(arrayWave[i]);
	}
	free(arrayWave);
	free(player->ally);
	free(player);

	return 0;
}

SDL_Texture *loadShipImage(char *shipFilename, SDL_Renderer *renderer)
{
	SDL_Texture *shipTexture;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", shipFilename);
	shipTexture = IMG_LoadTexture(renderer, shipFilename);

	return shipTexture;
}

void blit(SDL_Texture *texture, SDL_Renderer *renderer, int x, int y)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
	SDL_RenderCopy(renderer, texture, NULL, &dest);
}

//funcionamento do player
PlayerShip *createPlayerShip(SDL_Renderer *renderer)
{
	PlayerShip *player = (PlayerShip *)malloc(sizeof(PlayerShip));

	player->ally = (Ship *)malloc(sizeof(Ship));
	player->ally->x_axis = 590;
	player->ally->y_axis = 600;
	player->ally->speed = 8;
	player->ally->texture = loadShipImage("assets/objects/player.png", renderer);

	return player;
}

PlayerShip *doKeyDown(SDL_KeyboardEvent *event, PlayerShip *player)
{
	if (event->repeat == 0)
	{
		if (event->keysym.scancode == SDL_SCANCODE_UP)
		{
			player->up = 1;
		}

		if (event->keysym.scancode == SDL_SCANCODE_DOWN)
		{
			player->down = 1;
		}

		if (event->keysym.scancode == SDL_SCANCODE_LEFT)
		{
			player->left = 1;
		}

		if (event->keysym.scancode == SDL_SCANCODE_RIGHT)
		{
			player->right = 1;
		}

		if (event->keysym.scancode == SDL_SCANCODE_SPACE)
		{
			player->ally->fire = 1;
		}
	}

	return player;
}

PlayerShip *doKeyUp(SDL_KeyboardEvent *event, PlayerShip *player)
{
	if (event->repeat == 0)
	{
		if (event->keysym.scancode == SDL_SCANCODE_UP)
		{
			player->up = 0;
		}

		if (event->keysym.scancode == SDL_SCANCODE_DOWN)
		{
			player->down = 0;
		}

		if (event->keysym.scancode == SDL_SCANCODE_LEFT)
		{
			player->left = 0;
		}

		if (event->keysym.scancode == SDL_SCANCODE_RIGHT)
		{
			player->right = 0;
		}

		if (event->keysym.scancode == SDL_SCANCODE_SPACE)
		{
			player->ally->fire = 0;
		}
	}

	return player;
}

PlayerShip *movePlayer(PlayerShip *player, EnemyShip **enemies)
{

	if (player->up && player->ally->y_axis >= 0 && shipColision(player, enemies) == false)
	{
		player->ally->y_axis -= player->ally->speed;
	}

	if (player->down && player->ally->y_axis <= 720 - 46 && shipColision(player, enemies) == false)
	{
		player->ally->y_axis += player->ally->speed;
	}

	if (player->left && player->ally->x_axis >= 0 && shipColision(player, enemies) == false)
	{
		player->ally->x_axis -= player->ally->speed;
	}

	if (player->right && player->ally->x_axis <= 1280 - 48 && shipColision(player, enemies) == false)
	{
		player->ally->x_axis += player->ally->speed;
	}

	return player;
}

//funcionamento do inimigo
EnemyShip *createEnemyShip(int enemyType)
{
	EnemyShip *new = (EnemyShip *)malloc(sizeof(EnemyShip));
	new->enemy = (Ship *)malloc(sizeof(Ship));
	
	if (enemyType == 1)
	{
		new->enemy->speed = 1;
		new->enemy->x_axis = -2;
		new->enemy->y_axis = 301;
		new->type = 0;
		SDL_Rect a = {1, 1, 219, 243};
		SDL_Rect b = {new->enemy->x_axis, new->enemy->y_axis, 50, 50};
		new->enemy->srcrect = a;
		new->enemy->dstrect = b;
		new->spawned = false;
	}

	return new;
}

EnemyShip **moveEnemies(EnemyShip **arrayWave, int i)
{
	for (int j = i; j >= 0; j--)
	{
		arrayWave[j]->enemy->dstrect.x += arrayWave[j]->enemy->speed;
		arrayWave[j]->enemy->dstrect.y--;
		arrayWave[j]->enemy->x_axis = arrayWave[j]->enemy->dstrect.x;
		arrayWave[j]->enemy->y_axis = arrayWave[j]->enemy->dstrect.y;

		if (arrayWave[j]->spawned == true &&
			((arrayWave[j]->enemy->y_axis < -1 * arrayWave[j]->enemy->dstrect.h) ||
			 (arrayWave[j]->enemy->x_axis < -1 * arrayWave[j]->enemy->dstrect.w) ||
			 isOffScreen(arrayWave[j]->enemy->x_axis, arrayWave[j]->enemy->y_axis, NULL)))
		{
			printf("Nave %d Fora da tela\n", j);
			arrayWave[j]->spawned = false;
		}
	}

	return arrayWave;
}

//funcionamento das balas
Bullet *createBullet(Ship *source, BulletVector *bulletVector, SDL_Renderer *renderer)
{
	Bullet *bullet = (Bullet *)malloc(sizeof(Bullet));

	bullet->speed = 16;
	bullet->damage = 1;
	bullet->x_axis = source->x_axis;
	bullet->y_axis = source->y_axis - 45;
	bullet->owner = source;
	bullet->texture = loadShipImage("assets/objects/shoot.png", renderer);

	addBulletInVector(bullet, bulletVector);

	return bullet;
}

void addBulletInVector(Bullet *bullet, BulletVector *bulletVector)
{
	int firstEmpty = bulletVector->firstEmpty;

	if (firstEmpty < bulletVector->lenghtVector)
	{
		bulletVector->bullets[firstEmpty] = bullet;
		bulletVector->firstEmpty += 1;
	}
}

BulletVector *createBulletVector(void)
{
	BulletVector *bulletVector = (BulletVector *)malloc(sizeof(BulletVector));
	bulletVector->bullets = (Bullet **)malloc(LEN_BULLET * sizeof(Bullet *));

	for (int i = 0; i < LEN_BULLET; i++)
	{
		bulletVector->bullets[i] = NULL;
	}

	bulletVector->firstEmpty = 0;
	bulletVector->lenghtVector = LEN_BULLET;

	return bulletVector;
}

BulletVector *moveBullet(BulletVector *bulletVector)
{
	for (int i = 0; i < bulletVector->firstEmpty; i++)
	{
		bulletVector->bullets[i]->y_axis -= bulletVector->bullets[i]->speed;
		// if(bulletVector->bullets[i]->y_axis < 720)
		// {	
		// 	free(bulletVector->bullets[i]);
		// 	printf("deu free %d\n", i);
		// }
	}
	
	return bulletVector;
}

//funções auxiliares
bool shipColision(PlayerShip *player, EnemyShip **enemies)
{
	SDL_Rect recPlayer;
	SDL_Rect recEnemy;

	SDL_QueryTexture(player->ally->texture, NULL, NULL, &recPlayer.w, &recPlayer.h);

	for (int i = 0; i < 7; i++)
	{
		if (enemies[i]->spawned == true &&
			(max(player->ally->x_axis, enemies[i]->enemy->x_axis) < min(player->ally->x_axis + recPlayer.w, enemies[i]->enemy->x_axis + enemies[i]->enemy->dstrect.w)) &&
			(max(player->ally->y_axis, enemies[i]->enemy->y_axis) < min(player->ally->y_axis + recPlayer.h, enemies[i]->enemy->y_axis + enemies[i]->enemy->dstrect.h)))
		{
			printf("Colision!!!-----------\n");
			player->ally->y_axis = player->ally->y_axis + 1;
			return true;
		}
	}

	return false;
}

bool isOffScreen(int x, int y, EnemyShip *enemy)
{
	if (y > 720 || x > 1280)
	{
		return true;
	}
	return false;
}

int max(int x, int y)
{
	if (x > y)
	{
		return x;
	}
	return y;
}

int min(int x, int y)
{
	if (x < y)
	{
		return x;
	}
	return y;
}