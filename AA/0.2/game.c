#include <stdio.h>
#include <stdlib.h>
#include "game.h"
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

int startGameScreen()
{
  //Criando Janela
  SDL_Window* window; 

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

  window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0); 
  
  if(window==NULL)
  {   
    printf("Could not create window: %s\n", SDL_GetError());
      return 1;
  }

  //Criando Renderer
  Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  SDL_Renderer* renderer;
  renderer = SDL_CreateRenderer(window, -1, render_flags);

  if(!renderer)
  {
    printf("error creating renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

  //Criando Surface
  SDL_Surface *surface = IMG_Load("assets/menu/bg3.png");
  if(!surface)
  {
    printf("error creating surface\n");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  //Surface -> Textura
  SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  if(!tex)
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
  SDL_RenderCopy(renderer, tex, NULL, NULL);

  SDL_RenderPresent(renderer);

  //Rodando
  gameLoop(window, renderer, tex);
  SDL_DestroyTexture(tex);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  Mix_Quit();
  SDL_Quit();
  return 0;
}

int gameLoop(SDL_Window* window, SDL_Renderer* rend, SDL_Texture* bg)
{

	//Novidades
	/*Waves carregadas dinâmicamente no game loop*/
	/*Modularização da movimentação das naves
	Background */
	bool isRunning = true;
	bool loadNextWave = true;
	int totalPoints = 0;
	int waveCounter = 0;
	int spawnedIndex = 0;
	//wave Register: Quantas naves existem por wave
	int waveRegister[20] = {7, 7, 14, 10, 18, 4, 8, 16, 10, 6, 20, 20, 15, 20, 20, 5, 8, 9, 6, 10}; 
	//As 5 ultimas waves, são as da Boss Fight (Precisam ficar em loop até o boss morrer)

	EnemyShip** arrayWave = (EnemyShip**) malloc(20 * sizeof(EnemyShip*));

	SDL_Event ev;
	SDL_Texture* enemyShipsSheet = loadShipImage("assets/objects/enemiessheets.png", rend);

	//Retângulos do BG
  	SDL_Rect a_BG = {0, 1080, 1920, 1080};	
	SDL_Rect b_BG = {0, 0, 1280, 720};


  	PlayerShip* player = createPlayerShip(rend);

  	blit(player->ally->texture, rend, player->ally->x_axis, player->ally->y_axis);


  	/*Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);
  	Mix_Music* bgm = Mix_LoadMUS("assets/sounds/bgm_kamikaze.mp3");
  	Mix_PlayMusic(bgm, -1);*/

	while(isRunning)
	{
		//Carrega prox wave
		if(loadNextWave == true)
		{
			for (int i = 0; i < waveRegister[waveCounter]; i++)
			{	
				if(waveCounter == 0)
				{
					arrayWave[i] = createEnemyShip(1, -2, 301, horizontal, -1, direita);
				}
				else if(waveCounter == 1)
				{
					if(i < waveRegister[waveCounter]/2){arrayWave[i] = createEnemyShip(1, -2, 301, diagonal, cima, direita);}
					else{arrayWave[i] = createEnemyShip(2, 0, 720, vertical, cima, -1);}
				}
				else
				{
					arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda);	
				}
			}
			loadNextWave = false;
		}
		
		while(SDL_PollEvent(&ev) != 0)
		{

      		//Fecha a janela se apertar Esc ou 'X'
		      if(ev.type == SDL_QUIT || ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)
		      {
		        isRunning = false;
		      }

		      if(ev.type == SDL_KEYDOWN)
		      {
		        player = movePlayer(doKeyDown(&ev.key, player), arrayWave);
		        
		        blit(player->ally->texture, rend, player->ally->x_axis, player->ally->y_axis);
		      }

		      if(ev.type == SDL_KEYUP)
		      {
		        
		        player = movePlayer(doKeyUp(&ev.key, player), arrayWave);
		        
		      }
		}
  	
	    //Garante que todos os inimigos continuem se movendo
	  	arrayWave = moveEnemies(arrayWave, spawnedIndex);
	  
    	//Libera o spawn do próximo membro da wave
	  	if(spawnedIndex < waveRegister[waveCounter]-1 && spawnRequest(arrayWave, spawnedIndex) == true)
	  	{
	  		spawnedIndex++;
	  	}
	  	
	  	//Atualiza o BG
	  	if(a_BG.y == 0){a_BG.y = 2160;}
	  	SDL_RenderCopy(rend, bg, &a_BG, &b_BG);
	  	a_BG.y -= 8;

	  	//Atualiza o Player
	  	blit(player->ally->texture, rend, player->ally->x_axis, player->ally->y_axis);  

	  	//Atualiza a Wave Inimiga
	    for (int j = 0; j < spawnedIndex+1; j++)
	  	{
	  		if(arrayWave[j]->spawned == true)
	  		{
	  			SDL_RenderCopy(rend, enemyShipsSheet, &arrayWave[j]->enemy->srcrect, &arrayWave[j]->enemy->dstrect);
	  		}
	  		if(arrayWave[j]->spawned == false && 
	  			arrayWave[j]->enemy->x_axis >= 0 && 
	  			arrayWave[j]->enemy->y_axis >= 0 &&
	  			!(isOffScreen(arrayWave[j]->enemy->x_axis, arrayWave[j]->enemy->y_axis, NULL)))
	  		{
	  			arrayWave[j]->spawned = true;
	  		}
	  	}

	  	//Comeca a proxima wave. Obs.:spawnedIndex +1 == Total de Naves inimigas spawnadas
	  	if(spawnedIndex+1 == waveRegister[waveCounter] && checkWaveStatus(arrayWave, spawnedIndex+1))
	  	{
	  		for (int i = 0; i < waveRegister[waveCounter]; i++) //Free na wave passada
	  		{
	  			if(arrayWave[i] != NULL)
				{
					printf("Free!!!! Wave %d\n", waveCounter);
					free(arrayWave[i]->enemy->bullet);
			 		free(arrayWave[i]->enemy);
			 		free(arrayWave[i]);
				}
	  		}
	  		if(waveCounter == 19)//Se a wave chegou no fim
	  		{
	  			waveCounter -= 5;//Volta 5 posicoes
	  		}
	  		//Atualizando valores
	  		waveCounter++;
	  		loadNextWave = true;
	  		spawnedIndex = 0;
	  	}

	  	SDL_RenderPresent(rend);
	    SDL_UpdateWindowSurface(window);
 	}

 	totalPoints += 20*(player->ally->hp);

 	//Descarregando Memória
 	
  	SDL_DestroyTexture(bg);
  	SDL_DestroyTexture(player->ally->texture);
  	SDL_DestroyTexture(enemyShipsSheet);
  	//Mix_FreeMusic(bgm);

  	for (int i = 0; i < waveRegister[0]; i++)
 	{
 		free(arrayWave[i]->enemy->bullet);
 		free(arrayWave[i]->enemy);
 		free(arrayWave[i]);
 	}
 	free(arrayWave);
 	free(player->ally);
 	free(player);

	return 0;
}

EnemyShip* createEnemyShip(int enemyType, int x, int y, int movement, int upDown, int leftRight)
{
	EnemyShip* new = (EnemyShip*) malloc(sizeof(EnemyShip));
	new->enemy = (Ship*) malloc(sizeof(Ship));
  
	if(enemyType == 1)
	{
		new->enemy->hp = 100;
		new->enemy->speed = 4;
		new->enemy->x_axis = x;
		new->enemy->y_axis = y;
		new->pointsWorth = 25;
		new->movement = movement;
		new->upDown = upDown;
		new->leftRight = leftRight;
		SDL_Rect a = {1, 1, 350, 390};
		SDL_Rect b = {new->enemy->x_axis, new->enemy->y_axis, 50, 50};
		new->enemy->srcrect = a;
		new->enemy->dstrect = b;
		new->spawned = false;
	}
	
	else if(enemyType == 2)
	{
		new->enemy->hp = 250;
		new->enemy->speed = 3;
		new->enemy->x_axis = x;
		new->enemy->y_axis = y;
		new->pointsWorth = 50;
		new->movement = movement;
		new->upDown = upDown;
		new->leftRight = leftRight;
		SDL_Rect a = {1, 460, 450, 480};
		SDL_Rect b = {new->enemy->x_axis, new->enemy->y_axis, 90, 90};
		new->enemy->srcrect = a;
		new->enemy->dstrect = b;
		new->spawned = false;
	}

	else if(enemyType == 3)
	{
		new->enemy->hp = 500;
		new->enemy->speed = 2;
		new->enemy->x_axis = x;
		new->enemy->y_axis = y;
		new->pointsWorth = 100;
		new->movement = movement;
		new->upDown = upDown;
		new->leftRight = leftRight;
		SDL_Rect a = {1, 950, 350, 700};
		SDL_Rect b = {new->enemy->x_axis, new->enemy->y_axis, 65, 130};
		new->enemy->srcrect = a;
		new->enemy->dstrect = b;
		new->spawned = false;
	}

	else if(enemyType == 4)
	{
		new->enemy->hp = 1000;
		new->enemy->speed = 1;
		new->enemy->x_axis = x;
		new->enemy->y_axis = y;
		new->pointsWorth = 200;
		new->movement = movement;
		new->upDown = upDown;
		new->leftRight = leftRight;
		SDL_Rect a = {1, 1700, 800, 700};
		SDL_Rect b = {new->enemy->x_axis, new->enemy->y_axis, 130, 110};
		new->enemy->srcrect = a;
		new->enemy->dstrect = b;
		new->spawned = false;
	}
	return new;
}

SDL_Texture *loadShipImage(char *shipFilename, SDL_Renderer* renderer)
{

  SDL_Texture *shipTexture;

  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", shipFilename);

  shipTexture = IMG_LoadTexture(renderer, shipFilename);

  return shipTexture;
}

void blit(SDL_Texture *texture, SDL_Renderer* renderer, int x, int y)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
	SDL_RenderCopy(renderer, texture, NULL, &dest);
}

PlayerShip* createPlayerShip(SDL_Renderer* renderer)
{
	PlayerShip* player = (PlayerShip*)malloc(sizeof(PlayerShip));
	player->ally = (Ship*)malloc(sizeof(Ship)); 
	player->ally->x_axis = 590;
	player->ally->y_axis = 600;
	player->ally->speed = 15;
	player->ally->texture = loadShipImage("assets/objects/player.png", renderer);

	return player;
}

PlayerShip* doKeyDown(SDL_KeyboardEvent *event, PlayerShip* player)
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
  }

  return player;
}

PlayerShip* doKeyUp(SDL_KeyboardEvent *event, PlayerShip* player)
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
  }

  return player;
}

PlayerShip* movePlayer(PlayerShip* player, EnemyShip** enemies)
{

  if (player->up && player->ally->y_axis >= 0 && shipColision(player, enemies) == false)
  {
    //printf("Movendo-se para cima\n");
    player->ally->y_axis -= player->ally->speed;
  }
  
  if (player->down && player->ally->y_axis <= 720 - 46 && shipColision(player, enemies) == false)
  {
    //printf("Movendo-se para baixo\n");
    player->ally->y_axis += player->ally->speed;

  }
  
  if (player->left && player->ally->x_axis >= 0 && shipColision(player, enemies) == false)
  {
    //printf("Movendo-se para esquerda\n");
   player->ally->x_axis -= player->ally->speed;
  }
  
  if (player->right && player->ally->x_axis <=  1280 - 48 && shipColision(player, enemies) == false)
  {
    //printf("Movendo-se para direita\n");
    player->ally->x_axis += player->ally->speed;
  }

  return player;
}

bool shipColision(PlayerShip* player, EnemyShip** enemies)
{
	SDL_Rect recPlayer;
	SDL_Rect recEnemy;

	SDL_QueryTexture(player->ally->texture, NULL, NULL, &recPlayer.w, &recPlayer.h);

	for (int i = 0; i < 7; i++)
	{
		if(enemies[i]->spawned == true && 
			(max(player->ally->x_axis, enemies[i]->enemy->x_axis) < min(player->ally->x_axis + recPlayer.w, enemies[i]->enemy->x_axis + enemies[i]->enemy->dstrect.w)) &&
			(max(player->ally->y_axis, enemies[i]->enemy->y_axis) < min(player->ally->y_axis + recPlayer.h, enemies[i]->enemy->y_axis + enemies[i]->enemy->dstrect.h)))
		{
			player->ally->y_axis = player->ally->y_axis + 1;
			return true;
		}

	}

	return false;
}

bool isOffScreen(int x, int y, EnemyShip* enemy)
{
	if(y >= 720 || x >= 1280)
	{
		return true;
	}
	return false;
}
  
EnemyShip** moveEnemies(EnemyShip** arrayWave, int i)
{

	for (int j = i; j >= 0; j--)
	{
		if(arrayWave[j]->movement == diagonal && arrayWave[j]->upDown == cima && arrayWave[j]->leftRight == direita)
		{
			arrayWave[j]->enemy->dstrect.x += arrayWave[j]->enemy->speed;
	  		arrayWave[j]->enemy->dstrect.y--;
	      	arrayWave[j]->enemy->x_axis = arrayWave[j]->enemy->dstrect.x;
	      	arrayWave[j]->enemy->y_axis = arrayWave[j]->enemy->dstrect.y;

	      	if(arrayWave[j]->spawned == true && 
	      		((arrayWave[j]->enemy->y_axis < -1*arrayWave[j]->enemy->dstrect.h) ||
	      		 (arrayWave[j]->enemy->x_axis < -1*arrayWave[j]->enemy->dstrect.w) ||
	      		 isOffScreen(arrayWave[j]->enemy->x_axis, arrayWave[j]->enemy->y_axis, NULL)
	      		 ))
	      	{
	      		printf("Nave %d Fora da tela\n", j);
	      		arrayWave[j]->spawned = false;
	      	}
		}
		else if(arrayWave[j]->movement == diagonal && arrayWave[j]->upDown == cima && arrayWave[j]->leftRight == esquerda)
		{ 
	  		arrayWave[j]->enemy->dstrect.x -= arrayWave[j]->enemy->speed;
	  		arrayWave[j]->enemy->dstrect.y--;
	      	arrayWave[j]->enemy->x_axis = arrayWave[j]->enemy->dstrect.x;
	      	arrayWave[j]->enemy->y_axis = arrayWave[j]->enemy->dstrect.y;

	      	if(arrayWave[j]->spawned == true && 
	      		((arrayWave[j]->enemy->y_axis < -1*arrayWave[j]->enemy->dstrect.h) ||
	      		 (arrayWave[j]->enemy->x_axis < -1*arrayWave[j]->enemy->dstrect.w) ||
	      		 isOffScreen(arrayWave[j]->enemy->x_axis, arrayWave[j]->enemy->y_axis, NULL)
	      		 ))
	      	{
	      		printf("Nave %d Fora da tela\n", j);
	      		arrayWave[j]->spawned = false;
	      	}	
		}
		else if(arrayWave[j]->movement == diagonal && arrayWave[j]->upDown == baixo && arrayWave[j]->leftRight == direita)
		{ 
	  		arrayWave[j]->enemy->dstrect.x += arrayWave[j]->enemy->speed;
	  		arrayWave[j]->enemy->dstrect.y++;
	      	arrayWave[j]->enemy->x_axis = arrayWave[j]->enemy->dstrect.x;
	      	arrayWave[j]->enemy->y_axis = arrayWave[j]->enemy->dstrect.y;

	      	if(arrayWave[j]->spawned == true && 
	      		((arrayWave[j]->enemy->y_axis < -1*arrayWave[j]->enemy->dstrect.h) ||
	      		 (arrayWave[j]->enemy->x_axis < -1*arrayWave[j]->enemy->dstrect.w) ||
	      		 isOffScreen(arrayWave[j]->enemy->x_axis, arrayWave[j]->enemy->y_axis, NULL)
	      		 ))
	      	{
	      		printf("Nave %d Fora da tela\n", j);
	      		arrayWave[j]->spawned = false;
	      	}
	  	
		}
		else if(arrayWave[j]->movement == diagonal && arrayWave[j]->upDown == baixo && arrayWave[j]->leftRight == esquerda)
		{ 
			
	  		arrayWave[j]->enemy->dstrect.x -= arrayWave[j]->enemy->speed;
	  		arrayWave[j]->enemy->dstrect.y++;
	      	arrayWave[j]->enemy->x_axis = arrayWave[j]->enemy->dstrect.x;
	      	arrayWave[j]->enemy->y_axis = arrayWave[j]->enemy->dstrect.y;

	      	if(arrayWave[j]->spawned == true && 
	      		((arrayWave[j]->enemy->y_axis < -1*arrayWave[j]->enemy->dstrect.h) ||
	      		 (arrayWave[j]->enemy->x_axis < -1*arrayWave[j]->enemy->dstrect.w) ||
	      		 isOffScreen(arrayWave[j]->enemy->x_axis, arrayWave[j]->enemy->y_axis, NULL)
	      		 ))
	      	{
	      		printf("Nave %d Fora da tela\n", j);
	      		arrayWave[j]->spawned = false;
	      	}
	  	
		}
		else if(arrayWave[j]->movement == horizontal && arrayWave[j]->leftRight == direita)
		{ 		
	  		arrayWave[j]->enemy->dstrect.x += arrayWave[j]->enemy->speed;
	      	arrayWave[j]->enemy->x_axis = arrayWave[j]->enemy->dstrect.x;

	      	if(arrayWave[j]->spawned == true && 
	      		((arrayWave[j]->enemy->y_axis < -1*arrayWave[j]->enemy->dstrect.h) ||
	      		 (arrayWave[j]->enemy->x_axis < -1*arrayWave[j]->enemy->dstrect.w) ||
	      		 isOffScreen(arrayWave[j]->enemy->x_axis, arrayWave[j]->enemy->y_axis, NULL)
	      		 ))
	      	{
	      		printf("Nave %d Fora da tela\n", j);
	      		arrayWave[j]->spawned = false;
	      	}
	  	
		}
		else if(arrayWave[j]->movement == horizontal && arrayWave[j]->leftRight == esquerda)
		{ 
	  		arrayWave[j]->enemy->dstrect.x -= arrayWave[j]->enemy->speed;
	      	arrayWave[j]->enemy->x_axis = arrayWave[j]->enemy->dstrect.x;

	      	if(arrayWave[j]->spawned == true && 
	      		((arrayWave[j]->enemy->y_axis < -1*arrayWave[j]->enemy->dstrect.h) ||
	      		 (arrayWave[j]->enemy->x_axis < -1*arrayWave[j]->enemy->dstrect.w) ||
	      		 isOffScreen(arrayWave[j]->enemy->x_axis, arrayWave[j]->enemy->y_axis, NULL)
	      		 ))
	      	{
	      		printf("Nave %d Fora da tela\n", j);
	      		arrayWave[j]->spawned = false;
	      	}
		}
		else if(arrayWave[j]->movement == vertical && arrayWave[j]->upDown == cima)
		{ 
	  		arrayWave[j]->enemy->dstrect.y--;
	      	arrayWave[j]->enemy->y_axis = arrayWave[j]->enemy->dstrect.y;

	      	if(arrayWave[j]->spawned == true && 
	      		((arrayWave[j]->enemy->y_axis < -1*arrayWave[j]->enemy->dstrect.h) ||
	      		 (arrayWave[j]->enemy->x_axis < -1*arrayWave[j]->enemy->dstrect.w) ||
	      		 isOffScreen(arrayWave[j]->enemy->x_axis, arrayWave[j]->enemy->y_axis, NULL)
	      		 ))
	      	{
	      		printf("Nave %d Fora da tela\n", j);
	      		arrayWave[j]->spawned = false;
	      	}
	  	
		}
		else if(arrayWave[j]->movement == vertical && arrayWave[j]->upDown == baixo)
		{ 
		
	  		arrayWave[j]->enemy->dstrect.y++;
	      	arrayWave[j]->enemy->y_axis = arrayWave[j]->enemy->dstrect.y;

	      	if(arrayWave[j]->spawned == true && 
	      		((arrayWave[j]->enemy->y_axis < -1*arrayWave[j]->enemy->dstrect.h) ||
	      		 (arrayWave[j]->enemy->x_axis < -1*arrayWave[j]->enemy->dstrect.w) ||
	      		 isOffScreen(arrayWave[j]->enemy->x_axis, arrayWave[j]->enemy->y_axis, NULL)
	      		 ))
	      	{
	      		printf("Nave %d Fora da tela\n", j);
	      		arrayWave[j]->spawned = false;
	      	}
		  	
		}
	}

	return arrayWave;
}

int max(int x, int y)
{
	if(x > y){return x;}
	return y;
}

int min(int x, int y)
{
	if(x < y){return x;}
	return y;
}

bool checkWaveStatus(EnemyShip** arrayWave, int total)
{
	int count = 0;
	for (int i = 0; i < total; i++)
	{
		if(arrayWave[i]->spawned == false)
		{
			count++;
		}
		else 
		{
			break;
		}
	}

	if(count == total)
	{
		return true;
	}
	return false;
}

bool spawnRequest(EnemyShip** arrayWave, int spawnedIndex)
{
	if((arrayWave[spawnedIndex]->movement == horizontal || arrayWave[spawnedIndex]->movement == diagonal) && 
		arrayWave[spawnedIndex]->leftRight == direita && 
		arrayWave[spawnedIndex]->enemy->x_axis > arrayWave[spawnedIndex]->enemy->dstrect.w)
	{
		return true;	
	}
	if((arrayWave[spawnedIndex]->movement == horizontal || arrayWave[spawnedIndex]->movement == diagonal) && 
		arrayWave[spawnedIndex]->leftRight == esquerda && 
		arrayWave[spawnedIndex]->enemy->x_axis < 1280 - arrayWave[spawnedIndex]->enemy->dstrect.w)
	{
		return true;
	}
	if(arrayWave[spawnedIndex]->movement == vertical && 
		arrayWave[spawnedIndex]->upDown == cima && 
		arrayWave[spawnedIndex]->enemy->y_axis < 720 - arrayWave[spawnedIndex]->enemy->dstrect.h)
	{
		return true;
	}
	if(arrayWave[spawnedIndex]->movement == vertical && 
		arrayWave[spawnedIndex]->upDown == baixo && 
		arrayWave[spawnedIndex]->enemy->y_axis > arrayWave[spawnedIndex]->enemy->dstrect.h)
	{
		return true;
	}

	return false;
}