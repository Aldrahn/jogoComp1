#include "game.h"

int startGameScreen()
{
	//Criando Janela
	SDL_Window *window;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

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
	startMenu(window, renderer, texture);
	//gameLoop(window, renderer);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	Mix_Quit();
	SDL_Quit();
	return 0;
}

int startMenu(SDL_Window *window, SDL_Renderer *rend, SDL_Texture* bg)
{
	bool isRunning = true;
	SDL_Rect srcBG = {0,0,1280,720};
	SDL_Rect dstBG = {0,0,1280,720};
	SDL_Event ev;
	int scape = 0;
	bool touch = false;

	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);
	Mix_Music* bgm = Mix_LoadMUS("assets/sounds/bgm_title.mp3");
	Mix_Chunk* nav = Mix_LoadWAV("assets/sounds/sfx_navigation.wav");
	Mix_Chunk* select = Mix_LoadWAV("assets/sounds/sfx_selection.wav");
  	Mix_PlayMusic(bgm, -1);

	while(isRunning)
	{
		while (SDL_PollEvent(&ev) != 0)
		{

			//Fecha a janela se apertar Esc ou 'X'
			if(ev.type == SDL_QUIT || ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)
			{
				isRunning = false;
			}

			if(ev.type == SDL_KEYDOWN && ev.key.keysym.scancode == SDL_SCANCODE_UP)
			{
				if(touch == false)
				{
					Mix_PlayChannel(-1, nav, 0);
					srcBG.y = 0;
					touch = true;
				}
			}
			else if(ev.type == SDL_KEYDOWN && ev.key.keysym.scancode == SDL_SCANCODE_DOWN)
			{
				Mix_PlayChannel(-1, nav, 0);
				srcBG.y = 720;
			}
			else if(ev.type == SDL_KEYUP)
			{
				touch = false;
			}
			if(ev.key.keysym.scancode == SDL_SCANCODE_RETURN)
			{
				Mix_PlayChannel(-1, select, 0);
				scape = gameLoop(window, rend);
				if(scape != 0){isRunning = false;}
				Mix_PlayMusic(bgm, -1);
			}

			SDL_RenderCopy(rend, bg, &srcBG, &dstBG);
			SDL_RenderPresent(rend);
			SDL_UpdateWindowSurface(window);
		}
	}

	Mix_FreeChunk(nav);
	Mix_FreeChunk(select);
	Mix_FreeMusic(bgm);
	Mix_HaltMusic();
	return 0;
}

int gameLoop(SDL_Window *window, SDL_Renderer *rend)
{
	bool isRunning = true;
	bool pause = false;
	bool loadNextWave = true;
	int totalPoints = 0;
	int waveCounter = 0;
	int spawnedIndex = 0;
	//wave Register: Quantas naves existem por wave
	int waveRegister[20] = {7, 7, 8, 10, 18, 4, 8, 16, 10, 6, 20, 20, 15, 20, 20, 5, 8, 9, 6, 10};
	//As 5 ultimas waves, são as da Boss Fight (Precisam ficar em loop até o boss morrer)

	EnemyShip **arrayWave = (EnemyShip **)malloc(20 * sizeof(EnemyShip *));

	//Caregando Imagens
	SDL_Event ev;
	SDL_Texture *enemyShipsSheet = loadShipImage("assets/objects/enemiessheets.png", rend);
	SDL_Texture *explosions = loadShipImage("assets/objects/explosion.png", rend);
	SDL_Texture *pause_overlay = loadShipImage("assets/menu/pause_screen.png", rend);

	//Retângulos do BG
	SDL_Texture *bg = loadShipImage("assets/menu/bg4.png", rend);
	SDL_Rect a_BG = {0, 1080, 1920, 1080};
	SDL_Rect b_BG = {0, 0, 1280, 720};

	BulletVector *bulletVector = createBulletVector();
	PlayerShip *player = createPlayerShip(rend);

	blit(player->ally->texture, rend, player->ally->x_axis, player->ally->y_axis, player);
	
	//Carregando Sons
  	Mix_Music* bgm = Mix_LoadMUS("assets/sounds/bgm_kamikaze_mod.mp3");
  	Mix_Chunk* jetTurbine = Mix_LoadWAV("assets/sounds/jet_loop.wav");
  	Mix_Chunk* bulletSound = Mix_LoadWAV("assets/sounds/ship_bullet.wav");
  	Mix_PlayMusic(bgm, -1);
  	Mix_PlayChannel(-1, jetTurbine, -1);

	while (isRunning)
	{
		//Carrega prox wave
		if (loadNextWave == true)
		{
			arrayWave = waveLoader(arrayWave, waveRegister, waveCounter);
			loadNextWave = false;
		}

		//checa a invencibilidade do player
		bool invWindow = isInvincible(player->invincibilityTimeStart);
		
		if (player->invincible == true && invWindow == true)
		{
			printf("Player is no longer invincible!\n");
			player->invincibilityTimeStart = -1;
			player->invincible = false;
		}

		while (SDL_PollEvent(&ev) != 0)
		{
			//Fecha a janela se apertar Esc ou 'X'
			if (ev.type == SDL_QUIT || ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)
			{
				isRunning = false;
				return 1;
			}

			if(pause == false)
			{
				switch(ev.type)
				{
					case SDL_KEYDOWN:
						
						player = doKeyDown(&ev.key, player);
						
						if (player->ally->fire == 1)
						{
							Bullet *bullet = createBullet(player->ally, bulletVector, rend);
							//if(!(Mix_Playing(2)))
							{
								Mix_PlayChannel(2, bulletSound, 0);
							}	
						}
						
						break;

					case SDL_KEYUP:
					{
						player = doKeyUp(&ev.key, player);
						break;
					}
				}
			}

			if(ev.type == SDL_KEYDOWN && ev.key.keysym.scancode == SDL_SCANCODE_P)
			{
				if(pause == false)
				{
					pause = true;
					Mix_PauseMusic();
					Mix_Pause(-1);
					SDL_RenderCopy(rend, pause_overlay, NULL, NULL);
				}
				else
				{
					pause = false;
					Mix_ResumeMusic();
					Mix_Resume(-1);
				}
			}
			if(ev.type == SDL_KEYDOWN && ev.key.keysym.scancode == SDL_SCANCODE_D)
			{
				player->ally->hp -= 10;
			}
		}

		player = movePlayer(player, arrayWave, spawnedIndex);
		blit(player->ally->texture, rend, player->ally->x_axis, player->ally->y_axis, player);

		if(pause == false)
		{

			
			if(spawnedIndex == 0 && arrayWave[spawnedIndex]->spawnType == paralel){spawnedIndex++;}

			//Libera o spawn do próximo membro da wave
			if(spawnedIndex < waveRegister[waveCounter] - 1 && spawnRequest(arrayWave, spawnedIndex) == true)
			{
				if(spawnedIndex > 0 && arrayWave[spawnedIndex]->spawnType == paralel)
				{
					spawnedIndex++;
				}
				spawnedIndex++;
			}
			
			//Garante que todos os inimigos continuem se movendo
			arrayWave = moveEnemies(arrayWave, player, spawnedIndex);

			//Atualiza o BG
			if (a_BG.y == 0)
			{
				a_BG.y = 2160;
			}
			SDL_RenderCopy(rend, bg, &a_BG, &b_BG);
			a_BG.y -= 8;

			//Atualiza o Player
			blit(player->ally->texture, rend, player->ally->x_axis, player->ally->y_axis, player);

			//Atualiza a Wave Inimiga
			for (int j = 0; j < spawnedIndex + 1; j++)
			{
				if (arrayWave[j]->spawned == true)
				{
					SDL_RenderCopy(rend, enemyShipsSheet, &arrayWave[j]->enemy->srcrect, &arrayWave[j]->enemy->dstrect);
				}
				if (arrayWave[j]->spawned == false &&
					arrayWave[j]->enemy->x_axis >= 0 &&
					arrayWave[j]->enemy->y_axis >= 0 &&
					!(isOffScreen(arrayWave[j]->enemy->x_axis, arrayWave[j]->enemy->y_axis, NULL)))
				{
					arrayWave[j]->spawned = true;
				}
			}

			//Movimentação das balas
			bulletVector = moveBullet(bulletVector);
			for (int i = 0; i < bulletVector->firstEmpty; i++)
			{
				if (bulletVector->bullets[i] != NULL)
				{
					blit(bulletVector->bullets[i]->texture, rend, bulletVector->bullets[i]->x_axis, bulletVector->bullets[i]->y_axis, NULL);
				}
			}


			changeShipColor(player);
			if(player->ally->hp == 0)
			{
				//Game Over
				//break;
			} 
			

			//Comeca a proxima wave. Obs.:spawnedIndex +1 == Total de Naves inimigas spawnadas
			if (spawnedIndex + 1 == waveRegister[waveCounter] && checkWaveStatus(arrayWave, spawnedIndex + 1))
			{
				for (int i = 0; i < waveRegister[waveCounter]; i++) //Free na wave passada
				{
					if (arrayWave[i] != NULL)
					{
						printf("Free!!!! Wave %d\n", waveCounter);

						free(arrayWave[i]->enemy);
						free(arrayWave[i]);
					}
				}
				if (waveCounter == 19) //Se a wave chegou no fim
				{
					waveCounter -= 5; //Volta 5 posicoes
				}
				//Atualizando valores
				waveCounter++;
				loadNextWave = true;
				spawnedIndex = 0;
			}	
		}
		SDL_RenderPresent(rend);
		SDL_UpdateWindowSurface(window);
	}

	totalPoints += 20 * (player->ally->hp);

	//Descarregando Memória
	Mix_HaltMusic();

	SDL_DestroyTexture(bg);
	SDL_DestroyTexture(pause_overlay);
	SDL_DestroyTexture(player->ally->texture);
	SDL_DestroyTexture(enemyShipsSheet);
	SDL_DestroyTexture(explosions);

	for (int i = 0; i < waveRegister[0]; i++)
	{

		free(arrayWave[i]->enemy);
		free(arrayWave[i]);
	}
	free(arrayWave);
	free(player->ally);
	free(player);
	Mix_FreeMusic(bgm);
	Mix_FreeChunk(jetTurbine);
	Mix_FreeChunk(bulletSound);

	return 0;
}

EnemyShip *createEnemyShip(int enemyType, int x, int y, int movement, int upDown, int leftRight, int spawn)
{
	EnemyShip *new = (EnemyShip *)malloc(sizeof(EnemyShip));
	new->enemy = (Ship *)malloc(sizeof(Ship));

	if (enemyType == 1)
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
		new->spawnType = spawn;
	}

	else if (enemyType == 2)
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
		new->spawnType = spawn;
	}

	else if (enemyType == 3)
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
		new->spawnType = spawn;
	}

	else if (enemyType == 4)
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
		new->spawnType = spawn;
	}
	return new;
}

SDL_Texture *loadShipImage(char *shipFilename, SDL_Renderer *renderer)
{

	SDL_Texture *shipTexture;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", shipFilename);

	shipTexture = IMG_LoadTexture(renderer, shipFilename);

	return shipTexture;
}

void blit(SDL_Texture *texture, SDL_Renderer *renderer, int x, int y, PlayerShip* player)
{
	if(player == NULL)
	{
		SDL_Rect dest;

		dest.x = x;
		dest.y = y;
		SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
		SDL_RenderCopy(renderer, texture, NULL, &dest);
	}
	else
	{
		SDL_RenderCopy(renderer, texture, &player->ally->srcrect, &player->ally->dstrect);	
	}
}

PlayerShip *createPlayerShip(SDL_Renderer *renderer)
{
	
	PlayerShip *player = (PlayerShip *)malloc(sizeof(PlayerShip));

	player->invincible = false;
	player->ally = (Ship *)malloc(sizeof(Ship));
	player->ally->hp = 100;
	player->ally->x_axis = 590;
	player->ally->y_axis = 600;
	player->ally->speed = 8;
	player->ally->isPlayer = 1;
	player->up = 0;
	player->down = 0;
	player->left = 0;
	player->right = 0;
	player->ally->texture = loadShipImage("assets/objects/player_sheet.png", renderer);
	SDL_Rect a = {0,0, 230, 280};
	SDL_Rect b = {player->ally->x_axis, player->ally->y_axis, 100, 135};
	player->ally->srcrect = a;
	player->ally->dstrect = b;

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

PlayerShip *movePlayer(PlayerShip *player, EnemyShip **enemies, int spawnedIndex)
{

	if(player->invincible == true){

		if (player->up && player->ally->y_axis >= 0)
		{
			player->ally->y_axis -= player->ally->speed;
			player->ally->dstrect.y = player->ally->y_axis;
			player->ally->srcrect.x = 210; //Muda animacao para propulsao
		}

		else if (player->down && player->ally->y_axis <= HEIGHT - player->ally->dstrect.h)
		{
			player->ally->y_axis += player->ally->speed;
			player->ally->dstrect.y = player->ally->y_axis;
			player->ally->srcrect.x = 0;//Muda animacao para sem propulsao
		}

		else if (player->left && player->ally->x_axis >= 0)
		{
			player->ally->x_axis -= player->ally->speed;
			player->ally->dstrect.x = player->ally->x_axis;
			player->ally->srcrect.x = 630;//Muda animacao para desvio para esquerda
		}

		else if (player->right && player->ally->x_axis <= WIDTH - player->ally->dstrect.w)
		{
			player->ally->x_axis += player->ally->speed;
			player->ally->dstrect.x = player->ally->x_axis;
			player->ally->srcrect.x = 840;//Muda animacao para desvio para direita
		}
	}

	else{

		if (player->up && player->ally->y_axis >= 0 && shipColision(player, enemies, spawnedIndex) == false)
		{
			player->ally->y_axis -= player->ally->speed;
			player->ally->dstrect.y = player->ally->y_axis;
			player->ally->srcrect.x = 210; //Muda animacao para propulsao
		}

		else if (player->down && player->ally->y_axis <= HEIGHT - player->ally->dstrect.h && shipColision(player, enemies, spawnedIndex) == false)
		{
			player->ally->y_axis += player->ally->speed;
			player->ally->dstrect.y = player->ally->y_axis;
			player->ally->srcrect.x = 0;//Muda animacao para sem propulsao
		}

		else if (player->left && player->ally->x_axis >= 0 && shipColision(player, enemies, spawnedIndex) == false)
		{
			player->ally->x_axis -= player->ally->speed;
			player->ally->dstrect.x = player->ally->x_axis;
			player->ally->srcrect.x = 630;//Muda animacao para desvio para esquerda
		}

		else if (player->right && player->ally->x_axis <= WIDTH - player->ally->dstrect.w && shipColision(player, enemies, spawnedIndex) == false)
		{
			player->ally->x_axis += player->ally->speed;
			player->ally->dstrect.x = player->ally->x_axis;
			player->ally->srcrect.x = 840;//Muda animacao para desvio para direita
		}

		else if(shipColision(player, enemies, spawnedIndex))
		{
			/*player->ally->x_axis = 590;
			player->ally->y_axis = 600;
			player->ally->dstrect.x = 590;
			player->ally->dstrect.y = 600;
			player->ally->hp -= 20;
			player->ally->invincible = true;
			*/
			
			player->invincible = true;
			player->ally->hp -= 20;
			player->invincibilityTimeStart = clock() / CLOCKS_PER_SEC;
			printf("Player is invincible!\nstarted in second %d\n", player->invincibilityTimeStart);
		}
	}
	return player;
}

//funcionamento do inimigo
EnemyShip **moveEnemies(EnemyShip **arrayWave, PlayerShip *player, int i)
{
	for (int j = i; j >= 0; j--)
	{	
		if (arrayWave[j]->movement == diagonal && arrayWave[j]->upDown == cima && arrayWave[j]->leftRight == direita)
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
		else if (arrayWave[j]->movement == diagonal && arrayWave[j]->upDown == cima && arrayWave[j]->leftRight == esquerda)
		{
			arrayWave[j]->enemy->dstrect.x -= arrayWave[j]->enemy->speed;
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
		else if (arrayWave[j]->movement == diagonal && arrayWave[j]->upDown == baixo && arrayWave[j]->leftRight == direita)
		{
			arrayWave[j]->enemy->dstrect.x += arrayWave[j]->enemy->speed;
			arrayWave[j]->enemy->dstrect.y++;
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
		else if (arrayWave[j]->movement == diagonal && arrayWave[j]->upDown == baixo && arrayWave[j]->leftRight == esquerda)
		{

			arrayWave[j]->enemy->dstrect.x -= arrayWave[j]->enemy->speed;
			arrayWave[j]->enemy->dstrect.y++;
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
		else if (arrayWave[j]->movement == horizontal && arrayWave[j]->leftRight == direita)
		{
			arrayWave[j]->enemy->dstrect.x += arrayWave[j]->enemy->speed;
			arrayWave[j]->enemy->x_axis = arrayWave[j]->enemy->dstrect.x;

			if (arrayWave[j]->spawned == true &&
				((arrayWave[j]->enemy->y_axis < -1 * arrayWave[j]->enemy->dstrect.h) ||
				 (arrayWave[j]->enemy->x_axis < -1 * arrayWave[j]->enemy->dstrect.w) ||
				 isOffScreen(arrayWave[j]->enemy->x_axis, arrayWave[j]->enemy->y_axis, NULL)))
			{
				printf("Nave %d Fora da tela\n", j);
				arrayWave[j]->spawned = false;
			}
		}
		else if(arrayWave[j]->movement == horizontal && arrayWave[j]->leftRight == esquerda)
		{
			arrayWave[j]->enemy->dstrect.x -= arrayWave[j]->enemy->speed;
			arrayWave[j]->enemy->x_axis = arrayWave[j]->enemy->dstrect.x;

			if (arrayWave[j]->spawned == true &&
				((arrayWave[j]->enemy->y_axis < -1 * arrayWave[j]->enemy->dstrect.h) ||
				 (arrayWave[j]->enemy->x_axis < -1 * arrayWave[j]->enemy->dstrect.w) ||
				 isOffScreen(arrayWave[j]->enemy->x_axis, arrayWave[j]->enemy->y_axis, NULL)))
			{
				printf("Nave %d Fora da tela\n", j);
				arrayWave[j]->spawned = false;
			}
		}
		else if(arrayWave[j]->movement == vertical && arrayWave[j]->upDown == cima)
		{
			arrayWave[j]->enemy->dstrect.y--;
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
		else if (arrayWave[j]->movement == vertical && arrayWave[j]->upDown == baixo)
		{
			arrayWave[j]->enemy->dstrect.y++;
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
	}
	return arrayWave;
}

//funcionamento das balas
Bullet *createBullet(Ship *source, BulletVector *bulletVector, SDL_Renderer *renderer)
{
	Bullet *bullet = (Bullet *)malloc(sizeof(Bullet));

	bullet->speed = 5;
	bullet->damage = 1;
	bullet->x_axis = source->x_axis;
	bullet->y_axis = source->y_axis - 45;
	bullet->owner = source;
	bullet->texture = loadShipImage("assets/objects/shoot2.png", renderer);

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
	int firstEmpty = bulletVector->firstEmpty;
	int removed = 0;

	for (int i = 0; i < firstEmpty; i++)
	{
		if (bulletVector->bullets[i]->owner->isPlayer)
		{
			bulletVector->bullets[i]->y_axis -= bulletVector->bullets[i]->speed;
		}
		else
		{
			bulletVector->bullets[i]->y_axis += bulletVector->bullets[i]->speed;
		}

		if (bulletVector->bullets[i]->y_axis <= -HEIGHT_BULLET)
		{
			free(bulletVector->bullets[i]);
			bulletVector->bullets[i] = NULL;
			removed += 1;
		}
	}
	for (int j = 0; j < removed; j++)
	{
		for (int i = 0; i < firstEmpty; i++)
		{
			if (bulletVector->bullets[i] == NULL)
			{
				bulletVector->bullets[i] = bulletVector->bullets[firstEmpty - 1];
				bulletVector->bullets[firstEmpty - 1] = NULL;
				firstEmpty -= 1;
				break;
			}
		}
	}
	bulletVector->firstEmpty = firstEmpty;

	return bulletVector;
}

//funções auxiliares
bool shipColision(PlayerShip* player, EnemyShip** enemies, int spawnedIndex)
{
	for (int i = 0; i < spawnedIndex + 1; i++)
	{
		if (enemies[i]->spawned == true &&
			(((max(player->ally->x_axis, enemies[i]->enemy->x_axis) < min(player->ally->x_axis + player->ally->dstrect.w, enemies[i]->enemy->x_axis + enemies[i]->enemy->dstrect.w)) &&
			(max(player->ally->y_axis, enemies[i]->enemy->y_axis) < min(player->ally->y_axis + player->ally->dstrect.h, enemies[i]->enemy->y_axis + enemies[i]->enemy->dstrect.h)))))
		{
			
			printf("Colision!!!-----------\n");	
			return true;
		}
	}

	return false;
}

bool isOffScreen(int x, int y, EnemyShip *enemy)
{
	if (y >= HEIGHT || x >= WIDTH)
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

bool checkWaveStatus(EnemyShip **arrayWave, int total)
{
	int count = 0;
	for (int i = 0; i < total; i++)
	{
		if (arrayWave[i]->spawned == false)
		{
			count++;
		}
		else
		{
			break;
		}
	}

	if (count == total)
	{
		return true;
	}
	return false;
}

bool spawnRequest(EnemyShip **arrayWave, int spawnedIndex)
{
	
	/*for(int i = spawnedIndex; i >= 0; i--)
	{
		if(arrayWave[i]->spawnType == arrayWave[spawnedIndex + 1]->spawnType)
		{*/
			if ((arrayWave[spawnedIndex]->movement == horizontal || arrayWave[spawnedIndex]->movement == diagonal) &&
				arrayWave[spawnedIndex]->leftRight == direita &&
				arrayWave[spawnedIndex]->enemy->x_axis > arrayWave[spawnedIndex]->enemy->dstrect.w)
			{
				return true;
			}
			if ((arrayWave[spawnedIndex]->movement == horizontal || arrayWave[spawnedIndex]->movement == diagonal) &&
				arrayWave[spawnedIndex]->leftRight == esquerda &&
				arrayWave[spawnedIndex]->enemy->x_axis < 1280 - arrayWave[spawnedIndex]->enemy->dstrect.w)
			{
				return true;
			}
			if (arrayWave[spawnedIndex]->movement == vertical &&
				arrayWave[spawnedIndex]->upDown == cima &&
				arrayWave[spawnedIndex]->enemy->y_axis < 720 - arrayWave[spawnedIndex]->enemy->dstrect.h)
			{
				return true;
			}
			if (arrayWave[spawnedIndex]->movement == vertical &&
				arrayWave[spawnedIndex]->upDown == baixo &&
				arrayWave[spawnedIndex]->enemy->y_axis > arrayWave[spawnedIndex]->enemy->dstrect.h)
			{
				return true;
			}
		/*	break;
		}
		else
		{
			return true;
		}
	}*/

	return false;
}

void changeShipColor(PlayerShip* player)
{
	if(player->ally->hp < 33)
	{
		player->ally->srcrect.y = 600;
	}
	else if(player->ally->hp > 33 && player->ally->hp < 66)
	{
		player->ally->srcrect.y = 300;
	}
	else
	{
		player->ally->srcrect.y = 0;
	}
}

bool isPlayerMoving(PlayerShip* player)
{
	if(player->up == false && player->left == false && player->right == false && player->down == false)
	{
		return false;
	}
	return true;
}

EnemyShip** waveLoader(EnemyShip** arrayWave, int* waveRegister, int waveCounter)
{
	for (int i = 0; i < waveRegister[waveCounter]; i++)
	{
		if(waveCounter == 0)
		{
			arrayWave[i] = createEnemyShip(1, -2, 301, horizontal, -1, direita, sequential);
		}
		else if(waveCounter == 1)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
		else if(waveCounter == 2)
		{
			arrayWave[i] = createEnemyShip(2, 1280-90, 720, vertical, cima, direita, paralel);
			if(i+1 < waveRegister[waveCounter])
			{
				arrayWave[i+1] = createEnemyShip(2, 0, 720, vertical, cima, -1, paralel);
				i++;
			}
		}
		else if(waveCounter == 3)
		{
			if(i < 5)
			{
				arrayWave[i] = createEnemyShip(2, 1280-90, 720, vertical, cima, direita, paralel);
				if(i+1 < waveRegister[waveCounter])
				{
					arrayWave[i+1] = createEnemyShip(2, 0, 720, vertical, cima, -1, paralel);
					i++;
				}
			}
			else
			{
				arrayWave[i] = createEnemyShip(1, -2, 301, horizontal, -1, direita, sequential);	
			}
		}
		else if(waveCounter == 4)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
		else if(waveCounter == 5)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
		else if(waveCounter == 6)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
		else if(waveCounter == 7)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
		else if(waveCounter == 8)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
		else if(waveCounter == 9)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
		else if(waveCounter == 10)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
		else if(waveCounter == 11)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
		else if(waveCounter == 12)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
		else if(waveCounter == 13)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
		else if(waveCounter == 14)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
		else if(waveCounter == 15)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
		else if(waveCounter == 16)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
		else if(waveCounter == 17)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
		else if(waveCounter == 18)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
		else if(waveCounter == 19)
		{
			arrayWave[i] = createEnemyShip(1, 1280, 301, horizontal, -1, esquerda, sequential);
		}
	}
	return arrayWave;
}

PlayerShip* playerRule(PlayerShip* player)
{
	if(player->up == true)
	{
		if(player->ally->y_axis <= HEIGHT - player->ally->srcrect.h)
		{
			player->ally->y_axis += 20;
			player->ally->dstrect.y = player->ally->y_axis;
		}
	}
	else if(player->left == true)
	{
		if(player->ally->x_axis <= WIDTH - player->ally->srcrect.w)
		{
			player->ally->x_axis += 20;
			player->ally->dstrect.x = player->ally->x_axis;
		}
	}
	else if(player->right == true)
	{
		if(player->ally->x_axis > 0)
		{
			player->ally->x_axis -= 20;
			player->ally->dstrect.x = player->ally->x_axis;
		}
	}
	else if(player->down == true)
	{
		if(player->ally->y_axis > 0)
		{
			player->ally->y_axis -= 20;
			player->ally->dstrect.y = player->ally->y_axis;
		}
	}
	return player;
}

int gettime(){
	
	time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  //printf ( "second: %d", timeinfo->tm_sec );
  return timeinfo->tm_sec;

}

bool isInvincible(clock_t start)
{
	/*int endTime = gettime();
	
	int timeDiff = endTime - initialTime;
	
	if(timeDiff == 2){

		printf("difference is %d seconds!\n", timeDiff);
		return true;
	}

	return false;*/

	//clock_t start = clock();

	clock_t end = clock() / CLOCKS_PER_SEC;
	float seconds = (float)(end - start);

	if(seconds == 2){
		printf("difference is %d seconds!\n", seconds);
		return true;
	}

	return false;
}