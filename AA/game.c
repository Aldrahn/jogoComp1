#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "SDL2/SDL.h" 
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
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
	gameLoop(window);

  	SDL_DestroyTexture(tex);
  	SDL_DestroyRenderer(rend);
  	SDL_DestroyWindow(window);
  	SDL_Quit();
	return 0;
}

int gameLoop(SDL_Window* window)
{

	bool isRunning = true;
	SDL_Event ev;

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
  		SDL_UpdateWindowSurface(window);
  	}

	return 0;
}

Ship* createShip()
{
	Ship* new = (Ship*) malloc(sizeof(Ship));
	new->bulletType = singleShot;

	return new;
}
