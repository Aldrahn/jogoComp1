#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "SDL2/SDL.h" 
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#define true 1
#define false 0
#define singleShot 0

typedef int bool;

int startGameScreen()
{
	//Criando Janela
	SDL_Window* window; 

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0); 
  
	if(window==NULL)
	{   
		printf("Could not create window: %s\n", SDL_GetError());
    	return 1;
  	}

  	//Criando Renderer
  	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  	SDL_Renderer *rend = SDL_CreateRenderer(window, -1, render_flags);

  	if(!rend)
  	{
  		printf("error creating renderer: %s\n", SDL_GetError());
  		SDL_DestroyWindow(window);
  		SDL_Quit();
  		return 1;
  	}

  	//Criando Surface
  	SDL_Surface *surface = IMG_Load("assets/menu/game_screen.png");
  	if(!surface)
  	{
  		printf("error creating surface\n");
  		SDL_DestroyRenderer(rend);
  		SDL_DestroyWindow(window);
  		SDL_Quit();
  		return 1;
  	}

  	//Surface -> Textura
  	SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, surface);
  	SDL_FreeSurface(surface);
  	if(!tex)
  	{
  		printf("error creating texture\n");
  		SDL_DestroyRenderer(rend);
  		SDL_DestroyWindow(window);
  		SDL_Quit();
  		return 1;
  	}

  	//Clear na Janela
  	SDL_RenderClear(rend);

  	//Joga a imagem para a janela
  	SDL_RenderCopy(rend, tex, NULL, NULL);
  	SDL_RenderPresent(rend);

  	//Rodando
	gameLoop(window, rend, tex);

  	SDL_DestroyTexture(tex);
  	SDL_DestroyRenderer(rend);
  	SDL_DestroyWindow(window);
  	SDL_Quit();
	return 0;
}

int gameLoop(SDL_Window* window, SDL_Renderer* rend, SDL_Texture* bg)
{

	bool isRunning = true;
	SDL_Event ev;
	int waves = 1;
	EnemyShip** arrayWave = (EnemyShip**) malloc(7 * sizeof(EnemyShip*));

	for (int i = 0; i < waves; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			arrayWave[j] = createEnemyShip(1);
		}
	}

	SDL_Surface *surface = IMG_Load(arrayWave[0]->enemy->textureDir);
  	SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, surface);
  	SDL_FreeSurface(surface);

	int i = 0; 
	while(isRunning)
  	{
  		while(SDL_PollEvent(&ev) != 0)
  		{
  			//Fecha a janela se apertar Esc ou 'X'
  			if(ev.type == SDL_QUIT || ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)
  			{
  				isRunning = false;
  			}
  		}
  		
  		//Garante que todos os inimigos continuem se movendo
  		for (int j = i; j >= 0; j--)
  		{
  			arrayWave[j]->enemy->dstrect.x += arrayWave[j]->enemy->speed;
	  		arrayWave[j]->enemy->dstrect.y--;
	  		arrayWave[j]->enemy->x = arrayWave[j]->enemy->dstrect.x;
	  		arrayWave[j]->enemy->y = arrayWave[j]->enemy->dstrect.y;
  		}

  		//SDL_DestroyTexture(tex);
  		
  		if(i < 6 && arrayWave[i]->enemy->dstrect.x == 50)
  		{
  			i++;
  		}

  		SDL_RenderClear(rend);
  		
  		//Atualiza o BG
  		SDL_RenderCopy(rend, bg, NULL, NULL);
  		for (int j = 0; j <= i; j++)
  		{
  			SDL_RenderCopy(rend, tex, &arrayWave[j]->enemy->srcrect, &arrayWave[j]->enemy->dstrect);
  		}
  		SDL_RenderPresent(rend);
		
		SDL_UpdateWindowSurface(window);

  		//if(arrayWave[i]->enemy->dstrect.)
  		/*free(arrayWave[i]->enemy);
  		free(arrayWave[i]);*/
  	}

	return 0;
}

EnemyShip* createEnemyShip(int enemyType)
{
	EnemyShip* new = (EnemyShip*) malloc(sizeof(EnemyShip));
	new->enemy = (Ship*) malloc(sizeof(Ship));

	if(enemyType == 1)
	{
		new->enemy->bulletType = singleShot;
		new->enemy->speed = 2;
		new->enemy->x = -2;
		new->enemy->y = 300;
		new->type = 1;
		SDL_Rect a = {1, 1, 219, 243};
		SDL_Rect b = {new->enemy->x, new->enemy->y, 50, 50};
		new->enemy->srcrect = a;
		new->enemy->dstrect = b;
		strcpy(new->enemy->textureDir, "assets/ships/ship1.png");
	}

	return new;
}
