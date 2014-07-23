#include "mass.h"
#include "gene.h"

#ifdef NOX

int start_draw(model *m) {
  v3test();
  masstest();
  genetest();

  //while(1)
   // step_model(m, 1);

  return 0;
}

int init_drawing() { return 0; }
void draw_until_settled(model *m) { run_until_settled(m); }
void cleanup_drawing() { return; }
void draw_for_n(model *m, int n) { return; }

#else

#include <stdbool.h>
#include <SDL2_gfxPrimitives.h>

const int SCREEN_WIDTH = 640; 
const int SCREEN_HEIGHT = 480;
static SDL_Window *win = NULL;
static SDL_Renderer *ren = NULL;

void draw_mass(SDL_Renderer *ren, mass *m) {
  /* 0xAABBGGRR */
  if(!m->dead)
    circleColor(ren, m->pos.x, m->pos.y, 2, 0xFFFFFFFF);
  else
    circleColor(ren, m->pos.x, m->pos.y, 2, 0xFF000055);
}
void draw_constraint(SDL_Renderer *ren, constraint *c) {
  bool mass_ok = c->nmasses == 2 && !c->masses[0]->dead && !c->masses[1]->dead;
  switch(c->type) {
    case NONE:
      aalineColor(ren, c->masses[0]->pos.x, c->masses[0]->pos.y,
                       c->masses[1]->pos.x, c->masses[1]->pos.y,
                  mass_ok ? 0x44005500 : 0x44000055);
      break;
    case DIST_EQ:
      aalineColor(ren, c->masses[0]->pos.x, c->masses[0]->pos.y,
                       c->masses[1]->pos.x, c->masses[1]->pos.y,
                  mass_ok ? 0xFFFFFFFF : 0x44444444);
      break;
    case DIST_GT:
    case DIST_LT:
    case XGT:
    case YGT:
    case ZGT:
    case XLT:
    case YLT:
    case ZLT:
    case XEQ:
    case YEQ:
    case ZEQ:
    default:
      //printf("Tried to draw constraint with unsupported type\n");
      break;
  }
}


void draw_model(SDL_Renderer *ren, model *m) {
  for(int i = 0; i < m->nconstraints; i++)
    draw_constraint(ren, &m->constraints[i]);

  for(int i = 0; i < m->nmasses; i++)
    draw_mass(ren, &m->masses[i]);
}

void draw_for_n(model *m, int n) {
  v3 center = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0 };
  translate_model(m, &center);

  for(int i = 0; i < n; i++) {
    step_model(m, 1);

    //Render the scene
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    draw_model(ren, m);
    SDL_RenderPresent(ren);

  }
}

void draw_until_settled(model *m) {
  const float max_dist = 0.001;
  bool settled = false;
  int its = 0;
  const int max_its = 100;

  v3 center = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0 };
  translate_model(m, &center);

  while(!settled && its < max_its) {
    step_model(m, 1);

    //Render the scene
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    draw_model(ren, m);
    SDL_RenderPresent(ren);

    settled = true;
    for(int i = 0; i < m->nmasses; i++)
      if(v3dist(&m->masses[i].pos, &m->masses[i].prev_pos) > max_dist) {
        settled = false;
        break;
      }
    its++;
  }
  
  center.x = -center.x;
  center.y = -center.y;
  translate_model(m, &center);
}

int mainloop(SDL_Renderer *ren, model *m) {
  SDL_Event e;
  bool quit = 0;
  while(!quit) {
    while(SDL_PollEvent(&e)) {
      if(e.type == SDL_QUIT)
        quit = true;
      if(e.type == SDL_KEYDOWN)
        quit = true;
      if(e.type == SDL_MOUSEBUTTONDOWN)
        quit = true;
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

int init_drawing() {
  if(win != NULL || ren != NULL) {
    printf("Already inited - doing nothing");
    return 0;
  }

  if(SDL_Init(SDL_INIT_EVERYTHING) == -1) {
    printf("%s", SDL_GetError());
    return 1;
  }
    
  win = SDL_CreateWindow("GA", 0,0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  void draw_model(SDL_Renderer *ren, model *m);
  if(win == NULL) {
    printf("%s", SDL_GetError());
    return 1;
  }

  ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(ren == NULL) {
    printf("%s", SDL_GetError());
    return 1;
  }
  return 0;
}

void cleanup_drawing() {
  if(ren == NULL)
    SDL_DestroyRenderer(ren);
  if(win == NULL)
    SDL_DestroyWindow(win);
  ren = NULL;
  win = NULL;

  SDL_Quit();
}

#endif
