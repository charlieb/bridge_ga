#ifndef DRAW
#define DRAW

#include "mass.h"

int start_draw(model *m);

int init_drawing();
void draw_until_settled(model *m);
void cleanup_drawing();

void draw_for_n(model *m, int n);

#ifndef NOX

#include <SDL.h> 

void draw_mass(SDL_Renderer *ren, mass *m);
void draw_model(SDL_Renderer *ren, model *m);


#endif

#endif
