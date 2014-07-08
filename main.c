#include <stdio.h> 
#include <string.h> 


#include "mass.h"
#include "vec3.h"


int main() {
  /*
  v3test();
  masstest();
  */


  model m;
  memset(&m, 0, sizeof(model));
  make_grid(20,20,10,&m);
  v3 t = {0};
  t.x = t.y = 100;
  translate_model(&m, &t);

  m.masses[0].fixed = TRUE;
  m.masses[19].fixed = TRUE;


  return 0;
}
