#include <stdio.h> 
#include <string.h> 
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "gene.h"
#include "draw.h"
#include "mass.h"
#include "vec3.h"


void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("received SIGINT\n");
  exit(1);
}

int main() {
  if (signal(SIGINT, sig_handler) == SIG_ERR)
    printf("\ncan't catch SIGINT\n");

  srand(time(NULL));

  model m;
  memset(&m, 0, sizeof(model));
  make_grid(20,20,10,&m);
  v3 t = {0};
  t.x = t.y = 100;
  translate_model(&m, &t);

  m.masses[0].fixed = true;
  m.masses[19].fixed = true;

  //start_draw(&m);
  init_drawing();
  start_GA();
  cleanup_drawing();

  return 0;
}
