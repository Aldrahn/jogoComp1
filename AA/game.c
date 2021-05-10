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

	//criando uma fonte
	if(TTF_Init()==-1) 
	{
	    printf("TTF_Init: %s\n", TTF_GetError());
	    exit(2);
	}

	//Rodando
	startMenu(window, renderer);
	//gameLoop(window, renderer);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	Mix_Quit();
	SDL_Quit();
	return 0;
}

int startMenu(SDL_Window *window, SDL_Renderer *rend)
{
	bool isRunning = true;
	SDL_Rect srcBG = {0,0,1280,720};
	SDL_Rect dstBG = {0,0,1280,720};
	SDL_Event ev;
	int scape = 0;
	bool touch = false;

	bool flagEnter = true;

	SDL_Texture *bg = loadShipImage("assets/menu/game_screen.png", rend);

	SDL_Rect a_BG = {0, 1080, 1920, 1080};
	SDL_Rect b_BG = {0, 0, 1280, 720};

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

			if(ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_RETURN)
			{
				Mix_PlayChannel(-1, select, 0);

				if(srcBG.y != 720){
					//scape = gameLoop(window, rend, 1);
					scape = selectionScreen(window, rend);
				}

				else
				{
					readScore(rend, window);
				}

				Mix_PlayMusic(bgm, -1);
			}

			if(scape == 1)
			{
				isRunning = false;
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

int selectionScreen(SDL_Window *window, SDL_Renderer *rend)
{

	bool isRunning = true;
	SDL_Rect srcBG = {0,0,1280,720};
	SDL_Rect dstBG = {0,0,1280,720};
	SDL_Event ev;
	int scape = 0;
	bool touch = false;
	int retornoGameLoop;
	int playerType = 0;

	bool flagEnter = true;

	SDL_Texture *bg = loadShipImage("assets/menu/selection_screen.png", rend);

	Mix_Chunk* nav = Mix_LoadWAV("assets/sounds/sfx_navigation.wav");
	Mix_Chunk* select = Mix_LoadWAV("assets/sounds/sfx_selection.wav");


	//SDL_RenderClear(rend);
	// SDL_RenderCopy(rend, bg, &srcBG, &dstBG);
	// SDL_RenderPresent(rend);
	// SDL_UpdateWindowSurface(window);

  /*	while(isRunning)
  	{
  		while(SDL_PollEvent(&ev) != 0)
  		{
  			if(ev.type == SDL_QUIT || ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)
			{
				isRunning = false;
			}

			if(ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_RIGHT && srcBG.y <= 1440)
			{
				srcBG.y += 720;

				if(touch == false)
				{
					Mix_PlayChannel(-1, nav, 0);
					touch = true;
				}
			}

			else if(ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_LEFT && srcBG.y >= 0)
			{
				srcBG.y -= 720;

				if(touch == false)
				{
					Mix_PlayChannel(-1, nav, 0);
					touch = true;
				}
			}

			else if(ev.type == SDL_KEYUP)
			{
				touch = false;
			}

			if(ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_RETURN)
			{
				Mix_PlayChannel(-1, select, 0);

				if(srcBG.y == 0)
				{
					retornoGameLoop = gameLoop(window, rend, 1);
					playerType = 1;
					break;	
				}

				else if(srcBG.y == 720)
				{
					retornoGameLoop = gameLoop(window, rend, 2);
					playerType = 2;
					break;
				}

				else if(srcBG.y == 1440)
				{
					retornoGameLoop = gameLoop(window, rend, 3);
					playerType = 3;
					break;
				}
			}

			SDL_RenderCopy(rend, bg, &srcBG, &dstBG);
			SDL_RenderPresent(rend);
			SDL_UpdateWindowSurface(window);
		}

  		if(retornoGameLoop == 2)
		{	
			retornoGameLoop = gameLoop(window, rend, playerType);
		}
  	}*/

  	while(isRunning)
	{
		while (SDL_PollEvent(&ev) != 0)
		{

			//Fecha a janela se apertar Esc ou 'X'
			if(ev.type == SDL_QUIT || ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)
			{
				isRunning = false;
			}

			if(ev.type == SDL_KEYDOWN && ev.key.keysym.scancode == SDL_SCANCODE_LEFT)
			{
				if(/*touch == false &&*/ srcBG.y >= 0)
				{
					Mix_PlayChannel(-1, nav, 0);
					srcBG.y -= 720;
					touch = true;
				}
			}
			else if(ev.type == SDL_KEYDOWN && ev.key.keysym.scancode == SDL_SCANCODE_RIGHT)
			{
				if(srcBG.y <= 1440)
				{
					Mix_PlayChannel(-1, nav, 0);
					srcBG.y += 720;
				}
			}
			/*else if(ev.type == SDL_KEYUP)
			{
				touch = false;
			}*/

			if(ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_RETURN)
			{
				Mix_PlayChannel(-1, select, 0);

				if(srcBG.y == 0){
					retornoGameLoop = gameLoop(window, rend, 1);
					//scape = selectionScreen(window, rend);
				}

				else if(srcBG.y == 720){
					retornoGameLoop = gameLoop(window, rend, 2);
					//scape = selectionScreen(window, rend);
				}

				else if(srcBG.y == 1440){
					retornoGameLoop = gameLoop(window, rend, 3);
					//scape = selectionScreen(window, rend);
				}

				//Mix_PlayMusic(bgm, -1);
			}

			if(retornoGameLoop == 1)
			{
				isRunning = false;
			}

			SDL_RenderCopy(rend, bg, &srcBG, &dstBG);
			SDL_RenderPresent(rend);
			SDL_UpdateWindowSurface(window);
		}
	}

	Mix_FreeChunk(nav);
	Mix_FreeChunk(select);
	Mix_HaltMusic();

	return retornoGameLoop;
}

int gameLoop(SDL_Window *window, SDL_Renderer *rend, int playerType)
{
	const int FPS = 60;
	int frameTime;
	bool isRunning = true;
	bool pause = false;
	bool loadNextWave = true;
	int totalPoints = 0;
	int waveCounter = 0;
	int spawnedIndex = 0;
	
	//wave Register: Quantas naves existem por wave
	int waveRegister[20] = {7, 7, 6, 10, 18, 16, 4, 4, 16, 8, 20, 1, 20, 15, 20, 2, 8, 9, 2, 1};
	//As 5 ultimas waves, são as da Boss Fight (Precisam ficar em loop até o boss morrer)

	EnemyShip** arrayWave = (EnemyShip **)malloc(20 * sizeof(EnemyShip *));
	//Caregando Imagens
	SDL_Event ev;
	SDL_Texture *enemyShipsSheet = loadShipImage("assets/objects/enemiessheets.png", rend);
	SDL_Texture *explosions = loadShipImage("assets/objects/explosion.png", rend);
	SDL_Texture *bossSheet = loadShipImage("assets/objects/boss_1.png", rend);
	SDL_Texture *bullet_1 = loadShipImage("assets/objects/shoot2.png", rend);
	SDL_Texture *pause_overlay = loadShipImage("assets/menu/pause_screen.png", rend);
	SDL_Texture* textureWithScoring;
	//Retângulos do BG
	SDL_Texture *bg = loadShipImage("assets/menu/bg4.png", rend);
	SDL_Rect a_BG = {0, 1080, 1920, 1080};
	SDL_Rect b_BG = {0, 0, 1280, 720};

	BulletVector *bulletVectorPlayer = createBulletVector();
	BulletVector *bulletVectorEnemies = createBulletVector();
	PlayerShip *player = createPlayerShip(rend, playerType);

	blit(player->ally->texture, rend, player->ally->x_axis, player->ally->y_axis, player);
	//Carregando Sons
  	Mix_Music* bgm = Mix_LoadMUS("assets/sounds/bgm_kamikaze_mod.mp3");
  	Mix_Chunk* jetTurbine = Mix_LoadWAV("assets/sounds/jet_loop.wav");
  	Mix_Chunk* bulletSound = Mix_LoadWAV("assets/sounds/ship_bullet.wav");
  	Mix_PlayMusic(bgm, -1);
  	Mix_PlayChannel(-1, jetTurbine, -1);

  	frameTime = 0;
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
			player->doInvencibilityAnimation = false;
		}

		while (SDL_PollEvent(&ev) != 0)
		{
			//Fecha a janela se apertar Esc ou 'X'
			if (ev.type == SDL_QUIT || ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)
			{
				isRunning = false;
				//return 1;
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

			if(pause == false)
			{
				
				switch(ev.type)
				{
					case SDL_KEYDOWN:
						player = doKeyDown(&ev.key, player);
						if(ev.key.keysym.scancode == SDL_SCANCODE_J)
						{
							waveCounter++;
							loadNextWave = true;
							spawnedIndex = 0;
						}
						
						break;

					case SDL_KEYUP:	
						player = doKeyUp(&ev.key, player);
						break;
				}
			}
		}

		if(pause == false)
		{
			player = movePlayer(player, arrayWave, spawnedIndex);
			blit(player->ally->texture, rend, player->ally->x_axis, player->ally->y_axis, player);

			if(spawnedIndex == 0 && arrayWave[spawnedIndex]->spawnType == paralel){spawnedIndex++;}
			//Libera o spawn do próximo membro da wave
			if(spawnedIndex < waveRegister[waveCounter] - 1 && spawnRequest(arrayWave, spawnedIndex, NULL, NULL) == true)
			{
				if(spawnedIndex > 0 && arrayWave[spawnedIndex]->spawnType == paralel && arrayWave[spawnedIndex+1]->spawnType != sequential)
				{
					spawnedIndex++;
				}
				spawnedIndex++;
			}
			
			//Garante que todos os inimigos continuem se movendo
			arrayWave = moveEnemies(arrayWave, player, bulletVectorEnemies, spawnedIndex);

			//Atualiza o BG
			if (a_BG.y == 0)
			{
				a_BG.y = 2160;
			}
			SDL_RenderCopy(rend, bg, &a_BG, &b_BG);
			a_BG.y -= 8;

			//Animacao de Invencibilidade
			if(player->doInvencibilityAnimation == true)
			{
				player->frameTime++;
				if(FPS/player->frameTime == 8)
				{
					player->frameTime = 0;
					playerInvencibilityAnimation(player);
				}
			}
			else if(player->doDmgAnimation == true) //Animacao de dano do player
			{
				player->frameTime++;
				if(FPS/player->frameTime == 10)
				{
					player->frameTime = 0;
					playerDmgAnimation(player);
				}
			}

			//Atualiza o Player
			blit(player->ally->texture, rend, player->ally->x_axis, player->ally->y_axis, player);

			//Atualiza a Wave Inimiga
			for (int j = 0; j < spawnedIndex + 1; j++)
			{
				if (arrayWave[j]->spawned == true)
				{
					if(waveCounter == 19) //Se chegou na wave do boss
					{
						SDL_RenderCopy(rend, bossSheet, &arrayWave[j]->enemy->srcrect, &arrayWave[j]->enemy->dstrect);
					}
					else
					{
						SDL_RenderCopy(rend, enemyShipsSheet, &arrayWave[j]->enemy->srcrect, &arrayWave[j]->enemy->dstrect);
					}
				}
				if (arrayWave[j]->spawned == false &&
					arrayWave[j]->enemy->x_axis + arrayWave[j]->enemy->dstrect.w >= 0 &&
					arrayWave[j]->enemy->y_axis + arrayWave[j]->enemy->dstrect.h>= 0 &&
					arrayWave[j]->enemy->hp > 0 && 
					!(isOffScreen(arrayWave[j]->enemy->x_axis, arrayWave[j]->enemy->y_axis, NULL)))
				{
					arrayWave[j]->spawned = true;
				}
			}

			//Movimentação das balas
			if (player->ally->fire == true)
			{
				if(spawnRequest(NULL, -1, bulletVectorPlayer, player->ally))
				{
					Bullet *bullet = createBullet(player->ally, bulletVectorPlayer, -1, 0, playerType);
				}
				{
					Mix_PlayChannel(2, bulletSound, 0);
				}	
			}
			bulletVectorPlayer = moveBullet(bulletVectorPlayer, arrayWave, player, spawnedIndex);


			for (int i = 0; i < bulletVectorPlayer->firstEmpty; i++)
			{
				if (bulletVectorPlayer->bullets[i] != NULL)
				{
					SDL_RenderCopy(rend, bullet_1, &bulletVectorPlayer->bullets[i]->srcrect, &bulletVectorPlayer->bullets[i]->dstrect);
				}
			}
			for (int i = 0; i < bulletVectorEnemies->firstEmpty; i++)
			{
				if (bulletVectorEnemies->bullets[i] != NULL)
				{
					SDL_RenderCopy(rend, bullet_1, &bulletVectorEnemies->bullets[i]->srcrect, &bulletVectorEnemies->bullets[i]->dstrect);
				}
			}

			//Animacao das naves inimigas
			for (int i = 0; i < waveRegister[waveCounter]; i++)
			{
				if(arrayWave[i] != NULL && arrayWave[i]->doDestroyingAnimation)
				{
					arrayWave[i]->frameTime++;
					if(FPS/arrayWave[i]->frameTime == 30)
					{
						arrayWave[i]->frameTime = 0;
						destructionAnimation(arrayWave[i]->enemy);
						if(arrayWave[i]->enemy->srcrectExplosion.y < 950)
						{
							SDL_RenderCopy(rend, explosions, &arrayWave[i]->enemy->srcrectExplosion, &arrayWave[i]->enemy->dstrect);
						}
						else
						{ //Provavelmente o src das explosoes esta sendo requisitado demais e estao interpolando as animacoes uns dos outros
							arrayWave[i]->doDestroyingAnimation = false;
							arrayWave[i]->enemy->srcrectExplosion.x = 0;
							arrayWave[i]->enemy->srcrectExplosion.y = 0;
						}
					}
				}
				else if(arrayWave[i] != NULL && arrayWave[i]->doDmgAnimation)
				{
					arrayWave[i]->frameTime++;
					if(FPS/arrayWave[i]->frameTime == 15)
					{
						arrayWave[i]->frameTime = 0;
						enemyDmgAnimation(arrayWave[i]);
					}
				}
			}

			if(player->doDmgAnimation == false)
			{
				changeShipColor(player);
			}

			if(player->ally->hp <= 0)
			{
				break;
			} 

			if(waveCounter == 19 && arrayWave[0]->enemy->hp <= 0)
			{

				FILE* hsFile = fopen("high_scores.bin", "ab+");
				int offset = getOffset(hsFile);
				
				if(offset == 10)
				{
					int lowestHighScore = findLowestHighScore();

					if(player->points > lowestHighScore)
					{

						SDL_DestroyTexture(bg);
						SDL_DestroyTexture(bossSheet);
						SDL_DestroyTexture(bullet_1);
						SDL_DestroyTexture(pause_overlay);
						SDL_DestroyTexture(player->ally->texture);
						SDL_DestroyTexture(enemyShipsSheet);
						SDL_DestroyTexture(explosions);
						SDL_DestroyTexture(textureWithScoring);

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

						victoryHighScore(window, rend, player->points);
						isRunning = false;
					}
				}

				else
				{
					SDL_DestroyTexture(bg);
					SDL_DestroyTexture(bossSheet);
					SDL_DestroyTexture(bullet_1);
					SDL_DestroyTexture(pause_overlay);
					SDL_DestroyTexture(player->ally->texture);
					SDL_DestroyTexture(enemyShipsSheet);
					SDL_DestroyTexture(explosions);
					SDL_DestroyTexture(textureWithScoring);

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
					victoryHighScore(window, rend, player->points);
					isRunning = false;
				}
			}

			//Comeca a proxima wave. Obs.:spawnedIndex +1 == Total de Naves inimigas spawnadas
			if (spawnedIndex + 1 == waveRegister[waveCounter] && checkWaveStatus(arrayWave, spawnedIndex + 1) && waveCounter != 19)
			{
				for (int i = 0; i < waveRegister[waveCounter]; i++) //Free na wave passada
				{
					if (arrayWave[i] != NULL)
					{
						free(arrayWave[i]->enemy);
						free(arrayWave[i]);
					}
				}

				//Atualizando valores
				waveCounter++;
				loadNextWave = true;
				spawnedIndex = 0;
			}	
		}


		textureWithScoring = showScoreOnScreen(rend, bg, player->points);
		SDL_RenderPresent(rend);
		SDL_UpdateWindowSurface(window);
	}

	player->points += 20 * (player->ally->hp);

	int retryOrExit;

	if(player->ally->hp <= 0)
	{
		FILE* hsFile = fopen("high_scores.bin", "ab+");
		int offset = getOffset(hsFile);

		//printf("entrei aqui\n");
		if(offset == 10)
		{
			int lowestHighScore = findLowestHighScore();

			if(player->points > lowestHighScore)
			{
				SDL_DestroyTexture(bg);
				SDL_DestroyTexture(bossSheet);
				SDL_DestroyTexture(bullet_1);
				SDL_DestroyTexture(pause_overlay);
				SDL_DestroyTexture(player->ally->texture);
				SDL_DestroyTexture(enemyShipsSheet);
				SDL_DestroyTexture(explosions);
				SDL_DestroyTexture(textureWithScoring);

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

				gameOverHighScore(window, rend, player->points);
			}
		}

		else
		{
			SDL_DestroyTexture(bg);
			SDL_DestroyTexture(bossSheet);
			SDL_DestroyTexture(bullet_1);
			SDL_DestroyTexture(pause_overlay);
			SDL_DestroyTexture(player->ally->texture);
			SDL_DestroyTexture(enemyShipsSheet);
			SDL_DestroyTexture(explosions);
			SDL_DestroyTexture(textureWithScoring);

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
			gameOverHighScore(window, rend, player->points);
		}

		retryOrExit = gameOver(window, rend);
	}

	//Descarregando Memória
	Mix_HaltMusic();

	SDL_DestroyTexture(bg);
	SDL_DestroyTexture(bossSheet);
	SDL_DestroyTexture(bullet_1);
	SDL_DestroyTexture(pause_overlay);
	SDL_DestroyTexture(player->ally->texture);
	SDL_DestroyTexture(enemyShipsSheet);
	SDL_DestroyTexture(explosions);
	SDL_DestroyTexture(textureWithScoring);

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

	if(retryOrExit == 1)
	{	
		int retry = 2;
		return retry;
	}

	else if(retryOrExit == 2)
	{
		int exit = 0;
		return exit;
	}

	return 0;
}

void gameOverHighScore(SDL_Window *window, SDL_Renderer *rend, int scorePlayer)
{
	bool isRunning = true;
	SDL_Rect srcBG = {0,0,1280,720};
	SDL_Rect dstBG = {0,0,1280,720};
	SDL_Event ev;
	bool touch = false;

	SDL_Texture *bg = loadShipImage("assets/menu/gameOverRecordScreen.png", rend);
	SDL_Rect a_BG = {0, 1080, 1920, 1080};
	SDL_Rect b_BG = {0, 0, 1280, 720};

	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);
	Mix_Music* bgm = Mix_LoadMUS("assets/sounds/bgm_title.mp3");
	Mix_Chunk* nav = Mix_LoadWAV("assets/sounds/sfx_navigation.wav");
	Mix_Chunk* select = Mix_LoadWAV("assets/sounds/sfx_selection.wav");
  	Mix_PlayMusic(bgm, -1);

	SDL_RenderCopy(rend, bg, NULL, &srcBG);
	SDL_RenderPresent(rend);
	SDL_UpdateWindowSurface(window);

  	writeThingsOnScreen(rend, bg, scorePlayer);

  	SDL_DestroyTexture(bg);

  	Mix_FreeChunk(nav);
	Mix_FreeChunk(select);
	Mix_FreeMusic(bgm);
	Mix_HaltMusic();
}

void victoryHighScore(SDL_Window *window, SDL_Renderer *rend, int scorePlayer)
{
	bool isRunning = true;
	SDL_Rect srcBG = {0,0,1280,720};
	SDL_Rect dstBG = {0,0,1280,720};
	SDL_Event ev;
	bool touch = false;

	SDL_Texture *bg = loadShipImage("assets/menu/victory_screen.png", rend);
	SDL_Rect a_BG = {0, 1080, 1920, 1080};
	SDL_Rect b_BG = {0, 0, 1280, 720};

	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);
	Mix_Music* bgm = Mix_LoadMUS("assets/sounds/bgm_title.mp3");
	Mix_Chunk* nav = Mix_LoadWAV("assets/sounds/sfx_navigation.wav");
	Mix_Chunk* select = Mix_LoadWAV("assets/sounds/sfx_selection.wav");
  	Mix_PlayMusic(bgm, -1);

	SDL_RenderCopy(rend, bg, NULL, &srcBG);
	SDL_RenderPresent(rend);
	SDL_UpdateWindowSurface(window);

  	writeThingsOnScreen(rend, bg, scorePlayer);

  	SDL_DestroyTexture(bg);

  	Mix_FreeChunk(nav);
	Mix_FreeChunk(select);
	Mix_FreeMusic(bgm);
	Mix_HaltMusic();
}

int gameOver(SDL_Window *window, SDL_Renderer *rend)
{
	bool isRunning = true;
	SDL_Rect srcBG = {0,0,1280,720};
	SDL_Rect dstBG = {0,0,1280,720};
	SDL_Event ev;
	bool touch = false;

	SDL_Texture *bg = loadShipImage("assets/menu/gameOverScreen.png", rend);
	SDL_Rect a_BG = {0, 1080, 1920, 1080};
	SDL_Rect b_BG = {0, 0, 1280, 720};

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
			if(ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_RETURN)
			{
				Mix_PlayChannel(-1, select, 0);

				if(srcBG.y != 720){

					Mix_FreeChunk(nav);
					Mix_FreeChunk(select);
					Mix_FreeMusic(bgm);
					Mix_HaltMusic();
					
					return 1;
				}

				else{

					Mix_FreeChunk(nav);
					Mix_FreeChunk(select);
					Mix_FreeMusic(bgm);
					Mix_HaltMusic();

					return 2;
				}

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

void destructionAnimation(Ship* ship)
{
	ship->srcrectExplosion.x += 100;
	if(ship->srcrectExplosion.x > 900)
	{
		ship->srcrectExplosion.x = 0;
		ship->srcrectExplosion.y += 100;
	}
}

void enemyDmgAnimation(EnemyShip* ship)
{
	if(ship->type == 1)
	{
		ship->enemy->srcrect.x += 410;
		if(ship->enemy->srcrect.x > 850)
		{
			ship->enemy->srcrect.x = 0;
			ship->doDmgAnimation = false;
		}
	}
	else if(ship->type == 2 || ship->type == 6)
	{
		ship->enemy->srcrect.x += 560;
		if(ship->enemy->srcrect.x > 1080)
		{
			ship->enemy->srcrect.x = 0;
			ship->doDmgAnimation = false;
		}
	}
	else if(ship->type == 3)
	{
		ship->enemy->srcrect.x += 490;
		if(ship->enemy->srcrect.x > 990)
		{
			ship->enemy->srcrect.x = 0;
			ship->doDmgAnimation = false;
		}
	}
	else if(ship->type == 4)
	{
		ship->enemy->srcrect.x += 1070;
		if(ship->enemy->srcrect.x > 1080)
		{
			ship->enemy->srcrect.x = 0;
			ship->doDmgAnimation = false;
		}
	}
	else if(ship->type == 5)
	{
		ship->enemy->srcrect.x += 1070;
		if(ship->enemy->srcrect.x > 1080)
		{
			ship->enemy->srcrect.x = 0;
			ship->doDmgAnimation = false;
		}
	}	
}

void playerDmgAnimation(PlayerShip* ship)
{
	ship->ally->srcrect.y += 890;
	
	if(ship->ally->srcrect.y > 1800)
	{

		ship->ally->srcrect.y = 0;
		ship->doDmgAnimation = false;
	}
}

void playerInvencibilityAnimation(PlayerShip* ship)
{
	ship->ally->srcrect.x += 1060;
	if(ship->ally->srcrect.x > 1070)
	{
		ship->ally->srcrect.x = 0;
	}
}

EnemyShip *createEnemyShip(int enemyType, int x, int y, int movement, int upDown, int leftRight, int spawn, int distance, int stop)
{
	EnemyShip *new = (EnemyShip *)malloc(sizeof(EnemyShip));
	new->enemy = (Ship *)malloc(sizeof(Ship));

	SDL_Rect explosion = {0,0,100,100};
	new->enemy->srcrectExplosion = explosion;

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
		SDL_Rect a = {1, 1, 350, 390};//x = 1, 410, 820, y= 1
		SDL_Rect b = {new->enemy->x_axis, new->enemy->y_axis, 50, 50};
		new->enemy->srcrect = a;
		new->enemy->dstrect = b;
		new->spawned = false;
		new->spawnType = spawn;
		new->dist = distance;
		new->stopCriteria = 0;
	}

	else if (enemyType == 2)
	{
		new->enemy->hp = 300;
		new->enemy->speed = 2;
		new->enemy->x_axis = x;
		new->enemy->y_axis = y;
		new->pointsWorth = 50;
		new->movement = movement;
		new->upDown = upDown;
		new->leftRight = leftRight;
		SDL_Rect a = {1, 460, 490, 480};
		SDL_Rect b = {new->enemy->x_axis, new->enemy->y_axis, 90, 90};
		new->enemy->srcrect = a;
		new->enemy->dstrect = b;
		new->spawned = false;
		new->spawnType = spawn;
		new->dist = distance;
		new->stopCriteria = 1;
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
		new->dist = distance;
		new->stopCriteria = 0;
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
		SDL_Rect b = {new->enemy->x_axis, new->enemy->y_axis, 250, 230};
		new->enemy->srcrect = a;
		new->enemy->dstrect = b;
		new->spawned = false;
		new->spawnType = spawn;
		new->dist = distance;
		new->stopCriteria = 1;
	}
	else if(enemyType == 5)
	{
		new->enemy->hp = 1000;
		new->enemy->speed = 1;
		new->enemy->x_axis = x;
		new->enemy->y_axis = y;
		new->pointsWorth = 200;
		new->movement = movement;
		new->upDown = upDown;
		new->leftRight = leftRight;
		SDL_Rect a = {1, 1, 880, 1021};
		SDL_Rect b = {new->enemy->x_axis, new->enemy->y_axis, 500, 600};
		new->enemy->srcrect = a;
		new->enemy->dstrect = b;
		new->spawned = false;
		new->spawnType = spawn;
		new->dist = distance;
		new->stopCriteria = 1;
	}
	else if (enemyType == 7)
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
		new->dist = distance;
		new->stopCriteria = 1;
	}
	else
	{
		new->enemy->hp = 250;
		new->enemy->speed = 3;
		new->enemy->x_axis = x;
		new->enemy->y_axis = y;
		new->pointsWorth = 50;
		new->movement = movement;
		new->upDown = upDown;
		new->leftRight = leftRight;
		SDL_Rect a = {1, 2400, 490, 480};
		SDL_Rect b = {new->enemy->x_axis, new->enemy->y_axis, 90, 90};
		new->enemy->srcrect = a;
		new->enemy->dstrect = b;
		new->spawned = false;
		new->spawnType = spawn;
		new->dist = distance;
		new->stopCriteria = 1;
	}

	new->doDestroyingAnimation = false;
	new->frameTime = 0;
	new->doDmgAnimation = false;
	new->type = enemyType;
	new->enemy->isPlayer = false;
	new->timeRunning = false;
	new->stopValue = stop;
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

//funcionamento player
PlayerShip *createPlayerShip(SDL_Renderer *renderer, int playerType)
{

	PlayerShip *player = (PlayerShip *)malloc(sizeof(PlayerShip));
	player->ally = (Ship *)malloc(sizeof(Ship));

	if(playerType == 1)
	{
		printf("criei a highwind!\n");
		player->ally->hp = 100;
		player->ally->speed = 9;
		player->ally->texture = loadShipImage("assets/objects/player_sheet_highwind.png", renderer);
	}

	else if(playerType == 2)
	{
		printf("criei a solidsnake!\n");
		player->ally->hp = 100;
		player->ally->speed = 6;
		player->ally->texture = loadShipImage("assets/objects/player_sheet_solidsnake.png", renderer);
	}

	else if(playerType == 3)
	{
		printf("criei a braveheart!\n");
		player->ally->hp = 300;
		player->ally->speed = 6;
		player->ally->texture = loadShipImage("assets/objects/player_sheet_braveheart.png", renderer);
	}
	
	//hp, dano do tiro e velocidade

	player->ally->isPlayer = true;
	player->points = 0;
	player->ally->x_axis = 590;
	player->ally->y_axis = 600;
	player->invincible = false;
	
	player->up = 0;
	player->down = 0;
	player->left = 0;
	player->right = 0;
	SDL_Rect a = {0,0, 230, 280};
	SDL_Rect b = {player->ally->x_axis, player->ally->y_axis, 100, 135};
	player->ally->srcrect = a;
	player->ally->dstrect = b;
	player->doInvencibilityAnimation = false;
	player->frameTime = 0;
	
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

	if(player->invincible == false)
	{
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
			player->invincible = true;
			player->ally->hp -= 20;
			player->ally->x_axis = 590;
			player->ally->y_axis = 600;
			player->ally->dstrect.x = player->ally->x_axis;
			player->ally->dstrect.y = player->ally->y_axis;
			time(&(player->invincibilityTimeStart));
			player->doInvencibilityAnimation = true;
			printf("Player is invincible!\nstarted in second %ld\n", player->invincibilityTimeStart);
		}
	}
	return player;
}

//funcionamento do inimigo
EnemyShip **moveEnemies(EnemyShip **arrayWave, PlayerShip *player, BulletVector* bulletVector, int i)
{
	int count = 0;
	for (int j = i; j >= 0; j--)
	{	
		if(shouldEnemyMove(arrayWave[j]))
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
					arrayWave[j]->spawned = false;
				}
			}
		}
	}
	int contador = 0;
	static int contador2 = 0;
	for(int k = 0; k < i + 1; k++)
	{
		if(!shouldEnemyMove(arrayWave[k]))
		{
			if(arrayWave[k]->spawned == true && spawnRequest(NULL, -1, bulletVector, arrayWave[k]->enemy))
			{
				if(arrayWave[k]->type == 1 || arrayWave[k]->type == 2 || arrayWave[k]->type == 6 || arrayWave[k]->type == 3 || arrayWave[k]->type == 7)
				{
					Bullet* enemyBullet = createBullet(arrayWave[k]->enemy, bulletVector, arrayWave[k]->type, 0, -1);
				}
				else if(arrayWave[k]->type == 5)
				{
					for(int j = 0; j < 5; j++)
					{

						contador2++;
						if(contador2 >= 100)
						{
							Bullet* enemyBullet = createBullet(arrayWave[k]->enemy, bulletVector, 9, contador, -1);
						}
						else
						{
							contador += 60;
							Bullet* enemyBullet = createBullet(arrayWave[k]->enemy, bulletVector, arrayWave[k]->type, contador, -1);
						}
					}
					
				}
				
			}
		}
		bulletVector = moveBullet(bulletVector, arrayWave, player, i);
	}
	return arrayWave;
}

//funcionamento das balas
Bullet *createBullet(Ship *source, BulletVector *bulletVector, int bulletType, int dist, int playerType)
{
	Bullet *bullet = (Bullet *)malloc(sizeof(Bullet));

	if(source->isPlayer == true)
	{
		bullet->speed = 5;
		bullet->x_axis = source->x_axis + 27;
		bullet->y_axis = source->y_axis - 45;
		bullet->owner = source;
		SDL_Rect a = {1, 1, 36, 46};
		SDL_Rect b = {bullet->x_axis, bullet->y_axis, 38, 46};
		bullet->srcrect = a;
		bullet->dstrect = b;
		bullet->fireRate = 80;

		if(playerType == 2)
		{
			bullet->damage = 100;
		}	

		else
		{
			bullet->damage = 70;
		}
	}

	else if(bulletType == 5)
	{
		bullet->speed = 9;
		bullet->damage = 10;
		bullet->x_axis = source->x_axis + dist + 100;
		bullet->y_axis = source->y_axis + 500;
		bullet->owner = source;
		SDL_Rect a = {1, 1, 36, 46};
		SDL_Rect b = {bullet->x_axis, bullet->y_axis, 38, 46};
		bullet->srcrect = a;
		bullet->dstrect = b;
		bullet->type = bulletType;	
		bullet->fireRate = 500;
	}
	else if(bulletType == 9)
	{
		bullet->speed = 9;
		bullet->damage = 10;
		bullet->x_axis = 1300;
		bullet->y_axis = 700 + dist;
		bullet->owner = source;
		SDL_Rect a = {1, 1, 36, 46};
		SDL_Rect b = {bullet->x_axis, bullet->y_axis, 38, 46};
		bullet->srcrect = a;
		bullet->dstrect = b;
		bullet->type = bulletType;	
		bullet->fireRate = 500;
	}

	else
    {
        bullet->speed = 1;
        bullet->damage = 10;
        bullet->x_axis = source->x_axis + dist;
        bullet->y_axis = source->y_axis;
        bullet->owner = source;
        SDL_Rect a = {1, 1, 36, 46};
        SDL_Rect b = {bullet->x_axis, bullet->y_axis, 38, 46};
        bullet->srcrect = a;
        bullet->dstrect = b;
        bullet->type = bulletType;
        bullet->fireRate = 500;
    }

	bullet->start_x = bullet->x_axis;
	bullet->start_y = bullet->y_axis;
	
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

BulletVector *moveBullet(BulletVector *bulletVector, EnemyShip** arrayWave, PlayerShip* player, int spawnedIndex)
{
	int cont = 0;
	Bullet* bullet = NULL;

	for (int i = 0; i < bulletVector->firstEmpty; i++)
	{	
		bullet = bulletVector->bullets[i];
		if (bullet->owner->isPlayer)
		{
			bullet->y_axis -= bullet->speed;
			bullet->dstrect.y = bullet->y_axis;

			for(int enemy_index = 0; enemy_index < spawnedIndex + 1; enemy_index++) 
			{
				if ((bulletCollision(arrayWave[enemy_index]->enemy, bullet) && arrayWave[enemy_index]->spawned == true)) 
				{
					if(arrayWave[enemy_index]->enemy->hp <= 0)
					{
						printf("Destrui a nave %d\n", enemy_index);
						arrayWave[enemy_index]->spawned = false;
						player->points += arrayWave[enemy_index]->pointsWorth;
						arrayWave[enemy_index]->doDestroyingAnimation = true;
					}
					arrayWave[enemy_index]->enemy->hp -= bullet->damage;
					free(bullet);
					bulletVector->bullets[i] = NULL;
					rearrangeBulletVector(bulletVector);
					i--;

					cont++;
					if(arrayWave[enemy_index]->doDestroyingAnimation == false)
					{
						arrayWave[enemy_index]->doDmgAnimation = true;
					}
						
					break;
				}
				else if(bullet->y_axis <= -HEIGHT_BULLET)
				{
					free(bullet);
					bulletVector->bullets[i] = NULL;
					rearrangeBulletVector(bulletVector);
					i--;
					break;
				}
			}
		}
		else
		{
			if(bullet->type == 2)
			{

				bullet->x_axis += bullet->speed;
			}
			else if(bullet->type == 6 || bullet->type == 9)
			{
				bullet->x_axis -= bullet->speed;
			}
			else
			{
				bullet->y_axis += bullet->speed;
			}
			bullet->dstrect.x = bullet->x_axis;	
			bullet->dstrect.y = bullet->y_axis;

			if(bulletCollision(player->ally, bullet) && player->invincible == false) 
			{
				player->doDmgAnimation = true;
				player->ally->hp -= bullet->damage;
				free(bullet);
				bulletVector->bullets[i] = NULL;
				rearrangeBulletVector(bulletVector);
				i--;
			}
			else if(bullet->y_axis >= HEIGHT)
			{
				free(bullet);
				bulletVector->bullets[i] = NULL;
				rearrangeBulletVector(bulletVector);
				i--;
				break;
			}
		}

	}

	return bulletVector;
}

BulletVector* rearrangeBulletVector(BulletVector* bulletVector)
{
	for(int i = 0; i < LEN_BULLET; i++)
	{
		if(bulletVector->bullets[i] == NULL)
		{
			for(int j = i; j < LEN_BULLET; j++)
			{
				if(bulletVector->bullets[j] != NULL)
				{
					bulletVector->bullets[i] = bulletVector->bullets[j];
					bulletVector->bullets[j] = NULL;
					break;
				}
			}
		}
	}
	for(int i = 0; i < LEN_BULLET; i++)
	{
		if(bulletVector->bullets[i] == NULL)
		{
			bulletVector->firstEmpty = i;
			break;
		}
	}
	return bulletVector;
}

bool bulletCollision(Ship* ship, Bullet* bullet)
{
	if ((max(bullet->x_axis, ship->x_axis) < min(bullet->x_axis + bullet->dstrect.w, ship->x_axis + ship->dstrect.w)) &&
	(max(bullet->y_axis, ship->y_axis) < min(bullet->y_axis + bullet->dstrect.h, ship->y_axis + ship->dstrect.h)))
	{
		return true;
	}	

	return false;
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

bool spawnRequest(EnemyShip **arrayWave, int spawnedIndex, BulletVector* bulletVector, Ship* ship)
{
	if(arrayWave!= NULL)
	{
		if ((arrayWave[spawnedIndex]->movement == horizontal || arrayWave[spawnedIndex]->movement == diagonal) &&
			arrayWave[spawnedIndex]->leftRight == direita &&
			arrayWave[spawnedIndex]->enemy->x_axis > arrayWave[spawnedIndex]->enemy->dstrect.w + arrayWave[spawnedIndex]->dist)
		{
			
			return true;
		}
		if ((arrayWave[spawnedIndex]->movement == horizontal || arrayWave[spawnedIndex]->movement == diagonal) &&
			arrayWave[spawnedIndex]->leftRight == esquerda &&
			arrayWave[spawnedIndex]->enemy->x_axis < 1280 - arrayWave[spawnedIndex]->enemy->dstrect.w - arrayWave[spawnedIndex]->dist)
		{
			
			return true;
		}
		if (arrayWave[spawnedIndex]->movement == vertical &&
			arrayWave[spawnedIndex]->upDown == cima &&
			arrayWave[spawnedIndex]->enemy->y_axis < 720 - arrayWave[spawnedIndex]->enemy->dstrect.h - arrayWave[spawnedIndex]->dist)
		{
			
			return true;
		}
		if (arrayWave[spawnedIndex]->movement == vertical &&
			arrayWave[spawnedIndex]->upDown == baixo &&
			arrayWave[spawnedIndex]->enemy->y_axis > arrayWave[spawnedIndex]->enemy->dstrect.h + arrayWave[spawnedIndex]->dist)
		{
			
			return true;
		}
	}
	else if(ship->isPlayer == false)
	{
		int lastPosition = bulletVector->firstEmpty - 1;
		for (int i = lastPosition; i >= 0; i--)
		{
			if(bulletVector->bullets[i]->owner == ship)
			{
				
				if(bulletVector->bullets[i]->type == 2)
				{
					if((bulletVector->bullets[i]->x_axis > bulletVector->bullets[i]->start_x + bulletVector->bullets[i]->fireRate))
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				else if(bulletVector->bullets[i]->type == 6)
				{
					if((bulletVector->bullets[i]->x_axis < bulletVector->bullets[i]->start_x - bulletVector->bullets[i]->fireRate))
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				else if((bulletVector->bullets[i]->y_axis > bulletVector->bullets[i]->start_y + bulletVector->bullets[i]->fireRate))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		return true;
	}
	else
	{
		int lastPosition = bulletVector->firstEmpty - 1;
		if(bulletVector->firstEmpty != 0)
		{
			if((bulletVector->bullets[lastPosition]->y_axis < bulletVector->bullets[lastPosition]->start_y - bulletVector->bullets[lastPosition]->fireRate))
			{
				
				return true;
			}
			else
			{
				return false;
			}
		}
		else if(bulletVector->firstEmpty == 0)
		{
			
			return true;
		}
	}

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

bool shouldEnemyMove(EnemyShip* foe)
{
	bool x = false;
	bool y = true;

	
	if((foe->stopCriteria == x && foe->enemy->x_axis == foe->stopValue) || (foe->stopCriteria == y && foe->enemy->y_axis == foe->stopValue))
	{
		if(foe->type == 4 || foe->type == 5)
		{
			return false;
		}

		if(foe->timeRunning == false)
		{
			time(&(foe->timeStop));
			foe->timeRunning = true;
			return false;
		}
		else
		{
			time(&(foe->timeResume));
			if((int) (foe->timeResume - foe->timeStop) >= 5)
			{
				return true;
			}
			else
			{
				return false;
			}
		}	
	}
	return true;
}

EnemyShip** waveLoader(EnemyShip** arrayWave, int* waveRegister, int waveCounter)
{
	int aux1 = 160;
	int aux2 = 300;
	int aux3 = 0;
	int aux4 = 0;
	int stopAux = 0;

	//aux são tipos de stopAux, pois cada nave tem um stopAux especifico,
	//caso tenha um vetor com multiplos tipos de nave

	//stopAux sao os passos que uma nave pode andar (exceto aux3)

	//createEnemyShip, parametro 1: tipo da nave, 
	//				   parametro 2: x da nave, 
	//				   parametro 3: y da nave, 
	// 				   parametro 4: tipo do movimento (vertical ou horizontal), 
	//				   parametro 5: tipo 2 de movimento(diagonal ou nao), 
	//				   parametro 6: tipo 3 de movimento (esquerda ou direita), 
	// 				   parametro 7: sequencial ou paralelo (nao mexer), 
	//				   parametro 8: distancia entre uma nave e outra, 
	//				   parametro 9: valor de parada (stopAux ou n).
	
	for (int i = 0; i < waveRegister[waveCounter]; i++)
	{
		if(waveCounter == 0)
		{
			printf("wave %d\n", waveCounter);
			if(stopAux == 0){stopAux = 192;} 
			arrayWave[i] = createEnemyShip(1, -2, 200, horizontal, -1, direita, sequential, 0, stopAux);
			arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis + arrayWave[i]->enemy->speed*stopAux;
			stopAux -= (int)((arrayWave[i]->enemy->dstrect.w/arrayWave[i]->enemy->speed) + 2);
		}
		else if(waveCounter == 1)
		{
			printf("wave %d\n", waveCounter);
			if(stopAux == 0){stopAux = 192;}
			arrayWave[i] = createEnemyShip(1, 1282, 200, horizontal, -1, esquerda, sequential, 0, 0);
			arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis - arrayWave[i]->enemy->speed*stopAux;
			stopAux -= (int)((arrayWave[i]->enemy->dstrect.w/arrayWave[i]->enemy->speed) + 2);
		}
		else if(waveCounter == 2)
		{
			printf("wave %d\n", waveCounter);
			if(stopAux == 0){stopAux = 350;}
			if(i == 0)
			{
				arrayWave[i] = createEnemyShip(2, 0, 720, vertical, cima, -1, sequential, 0, stopAux);	
				arrayWave[i]->stopValue = arrayWave[i]->enemy->y_axis - arrayWave[i]->enemy->speed*stopAux;
				if(i + 1 < waveRegister[waveCounter])
				{
					i++;
					arrayWave[i] = createEnemyShip(6, 1280-90, 720, vertical, cima, -1, sequential, 0, stopAux);
					arrayWave[i]->stopValue = arrayWave[i-1]->stopValue + arrayWave[i]->enemy->dstrect.h;
				}
			}
			else
			{
				arrayWave[i] = createEnemyShip(2, 0, 720, vertical, cima, -1, sequential, 0, stopAux);	
				arrayWave[i]->stopValue = arrayWave[i-1]->stopValue + arrayWave[i]->enemy->dstrect.h;
				
				if(i + 1 < waveRegister[waveCounter])
				{
					i++;
					arrayWave[i] = createEnemyShip(6, 1280-90, 720, vertical, cima, -1, sequential, 0, stopAux);
					arrayWave[i]->stopValue = arrayWave[i-1]->stopValue + arrayWave[i]->enemy->dstrect.h;
				}
			}
		}
		else if(waveCounter == 3)
		{
			printf("wave %d\n", waveCounter);
			if(i < 6)
			{
				if(stopAux == 0){stopAux = 200;}
				arrayWave[i] = createEnemyShip(6, 1280-90, 720, vertical, cima, -1, paralel, 0, stopAux);
				arrayWave[i]->stopValue = arrayWave[i]->enemy->y_axis - arrayWave[i]->enemy->speed*stopAux;

				if(i+1 < waveRegister[waveCounter])
				{	
					i++;
					arrayWave[i] = createEnemyShip(2, 0, 720, vertical, cima, -1, paralel, 0, stopAux);
					arrayWave[i]->stopValue = arrayWave[i-1]->stopValue;
				}
				stopAux -= 50;
			}
			else
			{
				arrayWave[i] = createEnemyShip(1, -2, 301, horizontal, -1, direita, sequential, 0, 0);
				arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis + arrayWave[i]->enemy->speed*aux1;
				aux1 -= (int)((arrayWave[i]->enemy->dstrect.w/arrayWave[i]->enemy->speed) + 2);			
			}
		}
		else if(waveCounter == 4)
		{
			printf("wave %d\n", waveCounter);
			if(stopAux == 0){stopAux = 216;}
			arrayWave[i] = createEnemyShip(1, 1282, 201, horizontal, -1, esquerda, paralel, 0, stopAux);
			arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis - arrayWave[i]->enemy->speed*stopAux;

			if(i+1 < waveRegister[waveCounter])
			{
				i++;
				arrayWave[i] = createEnemyShip(1, -2, 401, horizontal, -1, direita, paralel, 0, 0);
				arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis + arrayWave[i]->enemy->speed*stopAux;
			}
			stopAux -= (int)((arrayWave[i]->enemy->dstrect.w/arrayWave[i]->enemy->speed) + 2);
		}
		else if(waveCounter == 5)
		{
			printf("wave %d\n", waveCounter);
			arrayWave[i] = createEnemyShip(1, -2, 300, diagonal, cima, direita, sequential, 50, 0);
			if(i+1 < waveRegister[waveCounter])
			{
				arrayWave[i+1] = createEnemyShip(1, 1280, 300, diagonal, cima, esquerda, sequential, 0, 0);
				i++;
			}
		}	
		else if(waveCounter == 6)
		{
			printf("wave %d\n", waveCounter);
			if(stopAux == 0){stopAux = 195;}
			arrayWave[i] = createEnemyShip(3, 1280, 50, diagonal, baixo, esquerda, sequential, 0, 0);
			arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis - arrayWave[i]->enemy->speed*stopAux;
			stopAux -= (int)((arrayWave[i]->enemy->dstrect.w/arrayWave[i]->enemy->speed) + 2);
		}
		else if(waveCounter == 7)
		{
			printf("wave %d\n", waveCounter);
			if(stopAux == 0){stopAux = 195;}
			arrayWave[i] = createEnemyShip(3, -65, 50, diagonal, baixo, direita, sequential, -65, 0);
			arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis + arrayWave[i]->enemy->speed*stopAux;
			stopAux -= (int)((arrayWave[i]->enemy->dstrect.w/arrayWave[i]->enemy->speed) + 2);
		}
		else if(waveCounter == 8)
		{
			printf("wave %d\n", waveCounter);
			if(i < 8)
			{
				if(stopAux == 0){stopAux = 195;}
				arrayWave[i] = createEnemyShip(3, 1280, 50, diagonal, baixo, esquerda, paralel, 0, 0);
				arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis - arrayWave[i]->enemy->speed*stopAux;
				if(i + 1 < waveRegister[waveCounter])
				{
					i++;
					arrayWave[i] = createEnemyShip(3, -65, 50, diagonal, baixo, direita, paralel, -65, 0);
					arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis + arrayWave[i]->enemy->speed*stopAux;
				}
				stopAux -= (int)((arrayWave[i]->enemy->dstrect.w/arrayWave[i]->enemy->speed) + 2);
				aux1 = 215;
			}
			else
			{
				arrayWave[i] = createEnemyShip(1, 1280, 70, horizontal, -1, esquerda, paralel, 0, aux1);
				arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis - arrayWave[i]->enemy->speed*aux1;

				if(i + 1 < waveRegister[waveCounter])
				{
					i++;
					arrayWave[i] = createEnemyShip(1, -2, 500, horizontal, -1, direita, paralel, 0, 0);
					arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis + arrayWave[i]->enemy->speed*aux1;
				}
				aux1 -= (int)((arrayWave[i]->enemy->dstrect.w/arrayWave[i]->enemy->speed) + 2);
			}
		}
		else if(waveCounter == 9)
		{
			printf("wave %d\n", waveCounter);
			if(i < 4)
			{
				if(stopAux == 0){stopAux = 195;}
				arrayWave[i] = createEnemyShip(3, 1280, 50, diagonal, baixo, esquerda, paralel, 0, 0);
				arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis - arrayWave[i]->enemy->speed*stopAux;
				if(i + 1 < waveRegister[waveCounter])
				{
					i++;
					arrayWave[i] = createEnemyShip(3, -65, 50, diagonal, baixo, direita, paralel, -65, 0);
					arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis + arrayWave[i]->enemy->speed*stopAux;
				}
				stopAux -= (int)((arrayWave[i]->enemy->dstrect.w/arrayWave[i]->enemy->speed) + 2);
			}
			else
			{
				arrayWave[i] = createEnemyShip(2, 0, 720, vertical, cima, -1, sequential, 0, aux2);	
				arrayWave[i]->stopValue = arrayWave[i]->enemy->y_axis - (arrayWave[i]->enemy->speed * aux2);
				
				if(i + 1 < waveRegister[waveCounter])
				{
					i++;
					arrayWave[i] = createEnemyShip(6, 1280-90, 720, vertical, cima, -1, sequential, 0, aux2);
					arrayWave[i]->stopValue = arrayWave[i-1]->stopValue + (arrayWave[i]->enemy->dstrect.h);
				}

				aux2-=70;
			}
		}
		else if(waveCounter == 10)
		{
			printf("wave %d\n", waveCounter);
			if(i < 10)
			{
				if(stopAux == 0){stopAux = 215;}
				arrayWave[i] = createEnemyShip(1, 1282, 301, horizontal, -1, esquerda, paralel, 0, stopAux);
				arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis - arrayWave[i]->enemy->speed*stopAux;

				if(i + 1 < waveRegister[waveCounter])
				{
					i++;
					arrayWave[i] = createEnemyShip(1, -2, 601, horizontal, -1, direita, paralel, 0, 0);
					arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis + arrayWave[i]->enemy->speed*stopAux;
				}
				stopAux -= (int)((arrayWave[i]->enemy->dstrect.w/arrayWave[i]->enemy->speed) + 2);
			}
			else if (i < 16)
			{
				if(aux4 == 0){aux4 = 200;}
				arrayWave[i] = createEnemyShip(7, 200+aux3, -64, vertical, baixo, -1, paralel, 0, 0);
				arrayWave[i]->stopValue = arrayWave[i]->enemy->y_axis + arrayWave[i]->enemy->speed*aux4;
				printf("%d\n", arrayWave[i]->stopValue);
				if(i + 1 < waveRegister[waveCounter])
				{
					i++;
					arrayWave[i] = createEnemyShip(7, 300+aux3, -64, vertical, baixo, -1, paralel, 0, 0);
					arrayWave[i]->stopValue = arrayWave[i]->enemy->y_axis + arrayWave[i]->enemy->speed*aux4;
				}
				aux3+= 200;
				aux4 -= (int)((arrayWave[i]->enemy->dstrect.h/arrayWave[i]->enemy->speed) + 2);
			}
			else
			{

				arrayWave[i] = createEnemyShip(2, 0, 720, vertical, cima, -1, sequential, 0, aux2);	
				arrayWave[i]->stopValue = arrayWave[i]->enemy->y_axis - arrayWave[i]->enemy->speed*aux2;
				if(i + 1 < waveRegister[waveCounter])
				{
					i++;
					arrayWave[i] = createEnemyShip(6, 1280-90, 720, vertical, cima, -1, sequential, 0, aux2);
					arrayWave[i]->stopValue = arrayWave[i-1]->stopValue + arrayWave[i]->enemy->dstrect.h;
				}
				aux2 -= 70;
				
			}
		}
		else if(waveCounter == 11)
		{
			printf("wave %d\n", waveCounter);
			arrayWave[i] = createEnemyShip(4, 500, -50, vertical, baixo, -1, sequential, 0, 0);
		}
		else if(waveCounter == 12)
		{
			printf("wave %d\n", waveCounter);
			if(i < 9)
			{
				if(stopAux == 0){stopAux = 215;}
				arrayWave[i] = createEnemyShip(1, 1282, 301, horizontal, -1, esquerda, paralel, 0, stopAux);
				arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis - arrayWave[i]->enemy->speed*stopAux;

				if(i+1 < waveRegister[waveCounter])
				{
					i++;
					arrayWave[i] = createEnemyShip(1, -2, 501, horizontal, -1, direita, paralel, 0, 0);
					arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis + arrayWave[i]->enemy->speed*stopAux;
				}
				stopAux -= (int)((arrayWave[i]->enemy->dstrect.w/arrayWave[i]->enemy->speed) + 2);
			}
			else if (i < 15)
			{
				if(aux4 == 0){aux4 = 200;}
				arrayWave[i] = createEnemyShip(7, 200+aux3, -64, vertical, baixo, -1, paralel, 0, 0);
				arrayWave[i]->stopValue = arrayWave[i]->enemy->y_axis + arrayWave[i]->enemy->speed*aux4;
				printf("%d\n", arrayWave[i]->stopValue);
				if(i+1 < waveRegister[waveCounter])
				{
					i++;
					arrayWave[i] = createEnemyShip(7, 300+aux3, -64, vertical, baixo, -1, paralel, 0, 0);
					arrayWave[i]->stopValue = arrayWave[i]->enemy->y_axis + arrayWave[i]->enemy->speed*aux4;
				}
				aux3+= 200;
				aux4 -= (int)((arrayWave[i]->enemy->dstrect.h/arrayWave[i]->enemy->speed) + 2);
			}
			else if (i < 18)
			{
				arrayWave[i] = createEnemyShip(2, 0, 720, vertical, cima, -1, sequential, 0, aux2);	
				arrayWave[i]->stopValue = arrayWave[i]->enemy->y_axis - arrayWave[i]->enemy->speed*aux2;
				if(i+1 < waveRegister[waveCounter])
				{
					i++;
					arrayWave[i] = createEnemyShip(6, 1280-90, 720, vertical, cima, -1, sequential, 0, aux2);
					arrayWave[i]->stopValue = arrayWave[i-1]->stopValue + arrayWave[i]->enemy->dstrect.h;
				}
				aux2 -= 70;
			}

			else
			{
				arrayWave[i] = createEnemyShip(4, 500, -230, vertical, baixo, -1, sequential, 0, 0);
			}
		}
		else if(waveCounter == 13)
		{
			printf("wave %d\n", waveCounter);
			if(stopAux == 0){stopAux = 260;}
			arrayWave[i] = createEnemyShip(1, 1282, 250, horizontal, -1, esquerda, sequential, 0, 0);
			arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis - arrayWave[i]->enemy->speed*stopAux;
			stopAux -= (int)((arrayWave[i]->enemy->dstrect.w/arrayWave[i]->enemy->speed) + 2);
		}
		else if(waveCounter == 14)
		{
			printf("wave %d\n", waveCounter);
			if(stopAux == 0){stopAux = 224;}
			arrayWave[i] = createEnemyShip(1, 1282, 220, horizontal, -1, esquerda, paralel, 0, stopAux);
			arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis - arrayWave[i]->enemy->speed*stopAux;

			if(i+1 < waveRegister[waveCounter])
			{
				i++;
				arrayWave[i] = createEnemyShip(1, -2, 400, horizontal, -1, direita, paralel, 0, 0);
				arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis + arrayWave[i]->enemy->speed*stopAux;
			}
			stopAux -= (int)((arrayWave[i]->enemy->dstrect.w/arrayWave[i]->enemy->speed) + 2);
		}
		else if(waveCounter == 15)
		{
			printf("wave %d\n", waveCounter);
			arrayWave[i] = createEnemyShip(4, 400, -230, vertical, baixo, -1, paralel, 50, 0);
			i++;
			arrayWave[i] = createEnemyShip(4, 650, -230, vertical, baixo, -1, paralel, 50, 0);
		}
		else if(waveCounter == 16)
		{
			printf("wave %d\n", waveCounter);
			if(stopAux == 0){stopAux = 450;} 
			arrayWave[i] = createEnemyShip(3, -65, 200, horizontal, -1, direita, sequential, 0, 0);;
			arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis + arrayWave[i]->enemy->speed*stopAux;
			stopAux -= (int)((arrayWave[i]->enemy->dstrect.w/arrayWave[i]->enemy->speed) + 2);
			
		}
		else if(waveCounter == 17)
		{
			printf("wave %d\n", waveCounter);
			if(stopAux == 0){stopAux = 520;}
			arrayWave[i] = createEnemyShip(3, 1280+65, 200, horizontal, -1, esquerda, sequential, 0, 0);
			arrayWave[i]->stopValue = arrayWave[i]->enemy->x_axis - arrayWave[i]->enemy->speed*stopAux;
			stopAux -= (int)((arrayWave[i]->enemy->dstrect.w/arrayWave[i]->enemy->speed) + 2);

		}
		else if(waveCounter == 18)
		{
			printf("wave %d\n", waveCounter);
			arrayWave[i] = createEnemyShip(4, 400, -230, vertical, baixo, -1, paralel, 50, 0);
			
			if(i+1 < waveRegister[waveCounter])
			{
				arrayWave[i+1] = createEnemyShip(4, 650, -230, vertical, baixo, -1, paralel, 50, 0);
				i++;
			}
		}
		else if(waveCounter == 19)
		{
			printf("wave %d\n", waveCounter);
			arrayWave[i] = createEnemyShip(5, 400, -600, vertical, baixo, -1, sequential, 0, 0);
			arrayWave[i]->stopValue = -200;
		}
	}
	return arrayWave;
}

bool isInvincible(time_t start)
{
	time_t end;
	time(&end);
	int seconds = (int)(end - start);

	if(seconds == 2){
		printf("difference is %d seconds!\n", seconds);
		return true;
	}

	return false;
}

void writeScore(char* nomePlayer, int scorePlayer)
{
    FILE *hsFile = fopen("high_scores.bin", "ab+");
    char buffer[7];
    int aux = 0;
    //int contador = 0;

    if(hsFile != NULL) 
    {
	   	fseek(hsFile, 0, SEEK_END);
	    int size = ftell(hsFile);
	    
	    if (size == 0) 
	    {
	        printf("file is empty\n");
	        rewind(hsFile);
	        fwrite(&scorePlayer, sizeof(int), 1, hsFile);
	        fwrite(nomePlayer, sizeof(char), sizeof(buffer), hsFile);
	        fclose(hsFile);
	    }

	    else
	    {
	    	int offset = getOffset(hsFile);
	    	rewind(hsFile);
	    	fclose(hsFile);
	    	organizeHighScore(scorePlayer, nomePlayer);
	    }
	}
}

void organizeHighScore(int scorePlayer, char* nomePlayer)
{

	FILE *hsFile = fopen("high_scores.bin", "rb+");
	printf("entrei na organizacao!\n");

	int auxScore;
	char buffer[7];

	int offset = getOffset(hsFile);
	
	int i = 0;
	int j = 0;

	printf("offset: %d\n", offset);

	char **stringOrganizadas = malloc((offset+1) * sizeof(char*));
	int intOrganizados[offset+1];

	for(int i = 0; i <= offset; i++)
	{
		stringOrganizadas[i] = (char*) malloc(7);
	}


	while(!feof(hsFile))
	{
		fread(&intOrganizados[i], sizeof(int), 1, hsFile);
		fread(stringOrganizadas[i], sizeof(char), sizeof(buffer), hsFile);
		printf("to no while, nome atual: %s e pontuacao: %d\n", stringOrganizadas[i], intOrganizados[i]);
		i++;
	}

	intOrganizados[offset] = scorePlayer;
	stringOrganizadas[offset] = nomePlayer;

	rewind(hsFile);
	fclose(hsFile);

	for (i = 0; i <= offset; i++) 
	{
		for (j = i + 1; j <= offset; j++)
		{
	        if (intOrganizados[i] < intOrganizados[j]){

	        	printf("iteracao i = %d, j = %d\ttrocando de lugar os jogadores %s e %s\n", i, j, stringOrganizadas[i], stringOrganizadas[j]);
	            char *tempString = stringOrganizadas[i]; //temp = a
	            stringOrganizadas[i] = stringOrganizadas[j]; //a = c
	            stringOrganizadas[j] = tempString; //c = a
	            printf("trocados os jogadores %s e %s\n", stringOrganizadas[i], stringOrganizadas[j]);

	            int tempInt = intOrganizados[i];
	            intOrganizados[i] = intOrganizados[j];
	            intOrganizados[j] = tempInt;
	        }
		}
	}

	offset++;

	hsFile = fopen("high_scores.bin", "wb");

	if(offset > 10)
	{
		for(i = 0; i < offset - 1; i++)
		{
			fwrite(&intOrganizados[i], sizeof(int), 1, hsFile);
			fwrite(stringOrganizadas[i], sizeof(buffer), 1, hsFile);
			printf("colocando o jogador %s de pontuacao %d\n", stringOrganizadas[i], intOrganizados[i]);
		}
	}

	else
	{
		for(i = 0; i < offset; i++)
		{
			fwrite(&intOrganizados[i], sizeof(int), 1, hsFile);
			fwrite(stringOrganizadas[i], sizeof(buffer), 1, hsFile);
			printf("colocando o jogador %s de pontuacao %d\n", stringOrganizadas[i], intOrganizados[i]);
		}
	}

	fclose(hsFile);
	
}

void readScore(SDL_Renderer* renderer, SDL_Window* window)
{
	FILE *hsFile = fopen("high_scores.bin", "ab+");
	int aux = 0;
	char buffer[7];
	char buffer1[100];
	buffer1[0] = '\0';

	char strScore[100];

	SDL_Texture *bgHighScores = loadShipImage("assets/menu/score_screen.png", renderer);
	SDL_Rect dstBG = {0,0,1280,720};

	SDL_Rect highScoreRect;
	highScoreRect.x = 350;
	highScoreRect.y = 70;

	TTF_Font* font = TTF_OpenFont("assets/fonts/Oswald-Light.ttf", 50);
	SDL_Color color = {255,255,255,255};
	
	SDL_Surface* highScoreSurface;
	SDL_Texture* highScoreTexture;

	SDL_Event event;
	bool quit = false;
	int offset = getOffset(hsFile);
	char strPrintHighScores[offset][1000];
	rewind(hsFile);

	SDL_RenderCopy(renderer, bgHighScores, NULL, &dstBG);

	for(int i = 0; i < offset; i++)
	{
		fread(&aux, sizeof(int), 1, hsFile);
		fread(buffer, sizeof(char), sizeof(buffer), hsFile);
		printf("posicao %d, nome %s e pontuacao %d\n", i + 1, buffer, aux);
		itoa(aux, strScore, 10);
		strcat(buffer1, strScore);
		strcat(buffer1, " --------------------------- ");
		strcat(buffer1, buffer);
		highScoreSurface = TTF_RenderText_Solid(font, buffer1, color);
		highScoreTexture = SDL_CreateTextureFromSurface(renderer, highScoreSurface);
		SDL_QueryTexture(highScoreTexture, NULL, NULL, &highScoreRect.w, &highScoreRect.h);
		SDL_RenderCopy(renderer, highScoreTexture, NULL, &highScoreRect);
		SDL_RenderPresent(renderer);
		highScoreRect.y += 50;
		buffer1[0] = '\0';
	}

	while(!quit){

		while(SDL_PollEvent(&event) != 0){

			switch(event.type)
			{
				case SDL_QUIT:

					quit = true;
					break;

				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_ESCAPE)
					{
						quit = true;

					}
					break;

				default:
					break;
			}
		}

		//SDL_RenderCopy(renderer, bgHighScores, NULL, &dstBG);
		//SDL_RenderCopy(renderer, highScoreTexture, NULL, &highScoreRect);
		SDL_RenderPresent(renderer);
		SDL_UpdateWindowSurface(window);
	}


	
	fclose(hsFile);
}

int getOffset(FILE* arq)
{
    rewind(arq);
    int offset = -1;
    int contador = 0;
    int aux = 0;
    char buffer[7];

    while(!feof(arq))
    {
    	//printf("entrei o while do getoffset\n");
		readData(arq, contador, &aux, buffer, sizeof(buffer));
		contador++;
        offset++;
    }

    rewind(arq);
    return offset;
}

void readData(FILE* arq, int offset, int* scorePlayer, char* nomePlayer, int tamanhoNome)
{


	rewind(arq);
	fseek(arq, offset * (sizeof(int) + tamanhoNome), SEEK_SET);
	fread(scorePlayer, sizeof(int), 1, arq);
	fread(nomePlayer, sizeof(char), sizeof(nomePlayer), arq);
}

int findLowestHighScore(void)
{

	FILE* arq = fopen("high_scores.bin", "rb");

	int offset = getOffset(arq);
	int lowestHighScore = 0;
	char nomePlayer[7];

	fseek(arq, (offset - 1) * (sizeof(int) + sizeof(nomePlayer)), SEEK_SET);
	fread(&lowestHighScore, sizeof(int), 1, arq);
	fread(nomePlayer, sizeof(char), sizeof(nomePlayer), arq);

	fclose(arq);

	return lowestHighScore;	
}

bool writeThingsOnScreen(SDL_Renderer* renderer, SDL_Texture* bg, int intScore)
{
	TTF_Font* font = TTF_OpenFont("assets/fonts/Oswald-Light.ttf", 72);
	SDL_Color color = {255,255,255,255};
	SDL_Surface* textForSurface;
	SDL_Surface* scoreForSurface; 
	SDL_Texture* textWithFont;
	SDL_Texture* textWithScore;
	SDL_Rect textRect;
	SDL_Rect scoreRect;
	SDL_Rect bgRect;
	bgRect.x = 0;
	bgRect.y = 720;
	SDL_Event event;

	textRect.x = 500;
	textRect.y = 380;

	scoreRect.x = 300;
	scoreRect.y = 380;

	int quit = false;
	char strScore[1000];
	bool pressed = false;
	bool canWrite = true;
	int contador = 0;

	itoa(intScore, strScore, 10);

	printf("%d\n", intScore);

	char* resultado = (char*) malloc(sizeof(char)* 100); 
	char string1[100] = "";

	scoreForSurface = TTF_RenderText_Solid(font, strScore, color);
	textWithScore = SDL_CreateTextureFromSurface(renderer, scoreForSurface);
	SDL_QueryTexture(textWithScore, NULL, NULL, &scoreRect.w, &scoreRect.h);
	SDL_RenderCopy(renderer, bg, NULL, NULL);
	SDL_RenderCopy(renderer, textWithScore, NULL, &scoreRect);
	SDL_FreeSurface(scoreForSurface);
	SDL_RenderPresent(renderer);
	
	SDL_StartTextInput();
	
	while(!quit)
	{

		bool renderText = false;
	
		while(SDL_PollEvent(&event) != 0)
		{
			if(contador >= 6 && pressed == false)
			{
				canWrite = false;
			}

			if(event.key.keysym.sym == SDLK_ESCAPE)
			{
				quit = true;
				return 1;
			}

			else if(event.type == SDL_QUIT)
			{
				return 1;
			}

			else if(event.type == SDL_TEXTINPUT || event.type == SDL_KEYDOWN)
			{
				
				if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN)
				{
					pressed = true;
					printf("string sendo passada para o arquivo: %s\n", string1);
					writeScore(string1, intScore);
					return 1;

				}

				else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE && pressed == false && strlen(string1) >= 0)
				{
					
					string1[strlen(string1) -1] = '\0';
					printf("%s\n", string1);
					textForSurface = TTF_RenderText_Solid(font, string1, color);
					scoreForSurface = TTF_RenderText_Solid(font, strScore, color);
					textWithFont = SDL_CreateTextureFromSurface(renderer, textForSurface);
					textWithScore = SDL_CreateTextureFromSurface(renderer, scoreForSurface);
					SDL_QueryTexture(textWithScore, NULL, NULL, &scoreRect.w, &scoreRect.h);
					SDL_QueryTexture(textWithFont, NULL, NULL, &textRect.w, &textRect.h);
					SDL_RenderCopy(renderer, bg, NULL, NULL);
					SDL_RenderCopy(renderer, textWithFont, NULL, &textRect);
					SDL_RenderCopy(renderer, textWithScore, NULL, &scoreRect);
					SDL_FreeSurface(textForSurface);
					SDL_FreeSurface(scoreForSurface);
					SDL_RenderPresent(renderer);
					contador--;
					
					if(contador < 0)
					{
						contador = 0;
					}

					canWrite = true;
					renderText = true;
					//return 1;
				}

				else if(event.type == SDL_TEXTINPUT && pressed == false && canWrite == true){
					
					resultado = event.text.text;
					strcat(string1, resultado);
					printf("%s\n", string1);
					contador++;
					renderText = true;
					//return 1;
				}		
			}
		}

		if(renderText)
		{
			if(string1 != "")
			{
				textForSurface = TTF_RenderText_Solid(font, string1, color);
				scoreForSurface = TTF_RenderText_Solid(font, strScore, color);
				textWithFont = SDL_CreateTextureFromSurface(renderer, textForSurface);
				textWithScore = SDL_CreateTextureFromSurface(renderer, scoreForSurface);
				SDL_QueryTexture(textWithScore, NULL, NULL, &scoreRect.w, &scoreRect.h);
				SDL_QueryTexture(textWithFont, NULL, NULL, &textRect.w, &textRect.h);
				SDL_RenderCopy(renderer, bg, NULL, &bgRect);
				SDL_RenderCopy(renderer, textWithFont, NULL, &textRect);
				SDL_RenderCopy(renderer, textWithScore, NULL, &scoreRect);
				SDL_FreeSurface(textForSurface);
				SDL_FreeSurface(scoreForSurface);
				SDL_RenderPresent(renderer);
			}

			else
			{
				textForSurface = TTF_RenderText_Solid(font, "", color);
				scoreForSurface = TTF_RenderText_Solid(font, strScore, color);
				textWithFont = SDL_CreateTextureFromSurface(renderer, textForSurface);
				textWithScore = SDL_CreateTextureFromSurface(renderer, scoreForSurface);
				SDL_QueryTexture(textWithScore, NULL, NULL, &scoreRect.w, &scoreRect.h);
				SDL_QueryTexture(textWithFont, NULL, NULL, &textRect.w, &textRect.h);
				SDL_RenderCopy(renderer, bg, NULL, &bgRect);
				SDL_RenderCopy(renderer, textWithFont, NULL, &textRect);
				SDL_RenderCopy(renderer, textWithScore, NULL, &scoreRect);
				SDL_FreeSurface(textForSurface);
				SDL_FreeSurface(scoreForSurface);
				SDL_RenderPresent(renderer);
			}
		}
	}

	SDL_StopTextInput();
}

SDL_Texture* showScoreOnScreen(SDL_Renderer* renderer, SDL_Texture* bg, int scorePlayer)
{
	
	char pontuacao[6];
	char score[100] = "score: ";
	
	itoa(scorePlayer, pontuacao, 10);

	strcat(score, pontuacao);

	TTF_Font* font = TTF_OpenFont("assets/fonts/Oswald-Light.ttf", 72);
	SDL_Color color = {255,255,255,255};
	SDL_Surface* surfaceWithScoring = TTF_RenderText_Solid(font, score, color);
	SDL_Texture* textureWithScoring = SDL_CreateTextureFromSurface(renderer, surfaceWithScoring);
	SDL_Rect scoreRect;

	scoreRect.x = scoreRect.y = 0; 

	SDL_QueryTexture(textureWithScoring, NULL, NULL, &scoreRect.w, &scoreRect.h);
	SDL_FreeSurface(surfaceWithScoring);
	SDL_RenderCopy(renderer, textureWithScoring, NULL, &scoreRect);

	return textureWithScoring;
}