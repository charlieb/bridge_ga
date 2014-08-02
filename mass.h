#ifndef MASS
#define MASS

#include "vec3.h"
#include <stdbool.h>

/* MASS *************************/

typedef struct mass {
  float m; /* as in f=ma */
  v3 pos;
  v3 prev_pos;
  v3 acc;
  bool fixed;
  bool dead;
} mass;

void print_mass(mass *m);
void step_mass(mass *m, float dt);
void gravity(mass *m);

/* CONSTRAINT ******************/

typedef enum { NONE, DIST_EQ, DIST_GT, DIST_LT, XGT, YGT, ZGT, XLT, YLT, ZLT, XEQ, YEQ, ZEQ } constraint_type;
typedef struct constraint {
  constraint_type type;
  mass **masses;
  int nmasses;
  float val;
} constraint;

void print_constraint(constraint *c);
void run_constraint(constraint *c);

/* MODEL ************/

typedef struct model {
  mass *masses;
  int nmasses;

  constraint *constraints;
  int nconstraints;
} model;

void print_model(model *m);
void translate_model(model *m, v3 *t);
void perturb_model(model *m);
void step_model(model *m, float dt);
void link_masses(mass *m1, mass *m2, constraint *c);
void link_masses_none(mass *m1, mass *m2, constraint *c);
void link_masses_dist_eq(float dist, mass *m1, mass *m2, constraint *c);
void make_grid(int x, int y, float spacing, model *model);
void kill_unconstrained(model *m);
void masstest();

#endif
