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

PlayerShip *createPlayerShip(SDL_Renderer *renderer)
{
  PlayerShip *player = (PlayerShip *)malloc(sizeof(PlayerShip));
  player->ally = (Ship *)malloc(sizeof(Ship));
  player->ally->x_axis = 590;
  player->ally->y_axis = 600;
  player->ally->speed = 4;
  player->ally->texture = loadShipImage("assets/objects/player.png", renderer);

  return player;
}

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
  SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  if (!tex)
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
  SDL_Quit();
  return 0;
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
  }

  return player;
}

PlayerShip *movePlayer(PlayerShip *player)
{

  if (player->up && player->ally->y_axis >= 0)
  {
    printf("Movendo-se para cima\n");
    player->ally->y_axis -= player->ally->speed;
  }

  if (player->down && player->ally->y_axis <= 720 - 46)
  {
    printf("Movendo-se para baixo\n");
    player->ally->y_axis += player->ally->speed;
  }

  if (player->left && player->ally->x_axis >= 0)
  {
    printf("Movendo-se para esquerda\n");
    player->ally->x_axis -= player->ally->speed;
  }

  if (player->right && player->ally->x_axis <= 1280 - 48)
  {
    printf("Movendo-se para direita\n");
    player->ally->x_axis += player->ally->speed;
  }

  return player;
}

int gameLoop(SDL_Window *window, SDL_Renderer *rend, SDL_Texture *bg)
{

  bool isRunning = true;
  SDL_Event ev;
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

  int i = 0;

  PlayerShip *player = createPlayerShip(rend);

  blit(player->ally->texture, rend, player->ally->x_axis, player->ally->y_axis);

  while (isRunning)
  {
    while (SDL_PollEvent(&ev) != 0)
    {

      //Fecha a janela se apertar Esc ou 'X'
      if (ev.type == SDL_QUIT || ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)
      {
        isRunning = false;
      }

      if (ev.type == SDL_KEYDOWN)
      {
        printf("posição ANTES do movimento\nplayer.x: %d\nplayer.y: %d\n", player->ally->x_axis, player->ally->y_axis);
        player = movePlayer(doKeyDown(&ev.key, player));
        printf("posição DEPOIS do movimento\nplayer.x: %d\nplayer.y: %d\n", player->ally->x_axis, player->ally->y_axis);
        blit(player->ally->texture, rend, player->ally->x_axis, player->ally->y_axis);
      }

      if (ev.type == SDL_KEYUP)
      {
        printf("posição ANTES do movimento\nplayer.x: %d\nplayer.y: %d\n", player->ally->x_axis, player->ally->y_axis);
        player = movePlayer(doKeyUp(&ev.key, player));
        printf("posição DEPOIS do movimento\nplayer.x: %d\nplayer.y: %d\n", player->ally->x_axis, player->ally->y_axis);
      }
    }

    //Garante que todos os inimigos continuem se movendo
    for (int j = i; j >= 0; j--)
    {
      arrayWave[j]->enemy->dstrect.x += arrayWave[j]->enemy->speed;
      arrayWave[j]->enemy->dstrect.y--;
      arrayWave[j]->enemy->x_axis = arrayWave[j]->enemy->dstrect.x;
      arrayWave[j]->enemy->y_axis = arrayWave[j]->enemy->dstrect.y;
    }

    if (i < 6 && arrayWave[i]->enemy->dstrect.x == 50)
    {
      i++;
    }

    //Atualiza o BG
    SDL_RenderCopy(rend, bg, NULL, NULL);
    blit(player->ally->texture, rend, player->ally->x_axis, player->ally->y_axis);

    for (int j = 0; j <= i; j++)
    {
      SDL_RenderCopy(rend, arrayWave[0]->enemy->texture, &arrayWave[j]->enemy->srcrect, &arrayWave[j]->enemy->dstrect);
    }

    SDL_RenderPresent(rend);
    SDL_UpdateWindowSurface(window);
  }

  SDL_DestroyTexture(arrayWave[0]->enemy->texture);
  SDL_DestroyTexture(bg);
  SDL_DestroyTexture(player->ally->texture);
  return 0;
}

EnemyShip *createEnemyShip(int enemyType)
{

  EnemyShip *new = (EnemyShip *)malloc(sizeof(EnemyShip));
  new->enemy = (Ship *)malloc(sizeof(Ship));
  new->enemy->bulletType = singleShot;

  if (enemyType == 1)
  {
    new->enemy->speed = 2;
    new->enemy->x_axis = -2;
    new->enemy->y_axis = 301;
    new->type = 0;
    SDL_Rect a = {1, 1, 219, 243};
    SDL_Rect b = {new->enemy->x_axis, new->enemy->y_axis, 50, 50};
    new->enemy->srcrect = a;
    new->enemy->dstrect = b;
  }

  return new;
}
