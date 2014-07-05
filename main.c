#include <stdio.h> 

#include <SDL.h> 

#include "mass.h"
#include "vec3.h"

const int SCREEN_WIDTH = 640; 
const int SCREEN_HEIGHT = 480;

typedef char bool;
#define TRUE 1
#define FALSE 0

int mainloop(SDL_Renderer *ren, model *m) {
  SDL_Event e;
  bool quit = 0;
  while(!quit) {
    while(SDL_PollEvent(&e)) {
      if(e.type == SDL_QUIT)
        quit = TRUE;
      if(e.type == SDL_KEYDOWN)
        quit = TRUE;
      if(e.type == SDL_MOUSEBUTTONDOWN)
        quit = TRUE;
    }
    //Render the scene
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    step_model(m, 1);
    draw_model(ren, m);

    SDL_RenderPresent(ren);
  }
  return 0;
}

int main() {
  /*
  v3test();
  masstest();
  */

  if(SDL_Init(SDL_INIT_EVERYTHING) == -1) {
    printf("%s", SDL_GetError());
    return 1;
  }

  SDL_Window *win = SDL_CreateWindow("GA", 0,0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  void draw_model(SDL_Renderer *ren, model *m);
  if(win == NULL) {
    printf("%s", SDL_GetError());
    return 1;
  }

  SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(ren == NULL) {
    printf("%s", SDL_GetError());
    return 1;
  }

  model m;
  memset(&m, 0, sizeof(model));
  make_grid(20,20,10,&m);
  v3 t = {0};
  t.x = t.y = 100;
  translate_model(&m, &t);

  m.masses[0].fixed = TRUE;
  m.masses[19].fixed = TRUE;

  mainloop(ren, &m);

  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
