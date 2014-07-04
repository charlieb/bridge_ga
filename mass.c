#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "vec3.h"


typedef struct mass {
  float m; /* as in f=ma */
  v3 pos;
  v3 prev_pos;
  v3 acc;
  char fixed;
} mass;

void mass_at(float m, float x, float y, float z, mass *mp) {
  memset(mp, 0, sizeof(mass));
  mp->m = m;
  mp->pos.x = x;
  mp->pos.y = y;
  mp->pos.z = z;
}

void print_mass(mass *m) {
  printf("Mass:\n"
         "\tpos = %f, %f, %f\n"
         "\tprev_pos = %f, %f, %f\n"
         "\tacc = %f, %f, %f\n"
         "\tfixed = %i\n",
         m->pos.x, m->pos.y, m->pos.z,
         m->prev_pos.x, m->prev_pos.y, m->prev_pos.z,
         m->acc.x, m->acc.y, m->acc.z,
         m->fixed);
}

typedef enum { DIST_EQ, DIST_GT, DIST_LT, XGT, YGT, ZGT, XLT, YLT, ZLT, XEQ, YEQ, ZEQ } constraint_type;
typedef struct constraint {
  constraint_type type;
  mass **masses;
  int nmasses;
  float val;
} constraint;

void print_constraint(constraint *c) {
  printf("Constraint:\n"
         "\ttype = %i\n"
         "\tnmasses = %i\n"
         "\tval = %f\n"
         "\tmasses = \n",
         c->type,
         c->nmasses,
         c->val);
  for(int i = 0; i < c->nmasses; i++)
    print_mass(c->masses[i]);
}
    
typedef struct model {
  mass *masses;
  int nmasses;

  constraint *constraints;
  int nconstraints;
} model;

void print_model(model *m) {
  printf("Model:\n"
         "\tnmasses = %i\n"
         "\tnconstraints = %i\n"
         "\tconstraints = \n",
         m->nmasses,
         m->nconstraints);
  for(int i = 0; i < m->nconstraints; i++)
    print_constraint(&m->constraints[i]);
}


void dist_eq(constraint *c) {
  v3 delta, *p1 = &c->masses[0]->pos, *p2 = &c->masses[1]->pos;
  float d;

  v3sub(p2, p1, &delta);
  d = c->val - v3mag(&delta);
  
  v3norm(&delta, &delta);
  v3mul(&delta, d/2, &delta);

  v3sub(p1, &delta, p1);
  v3add(p2, &delta, p2);
}

void run_constraint(constraint *c) {
  switch(c->type) {
    case DIST_EQ:
      dist_eq(c);
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
      printf("Tried to run constraint with unsupported type\n");
      break;
  }
}

void link_masses_dist_eq(float dist, mass *m1, mass *m2, constraint *c) {
  c->type = DIST_EQ;
  c->nmasses = 2;
  c->masses = malloc(c->nmasses*sizeof(mass*));
  c->masses[0] = m1;
  c->masses[1] = m2;
  c->val = dist;
}

void make_grid(int x, int y, float spacing, model *model) {
  mass *m;
  constraint *c;

  int ccount = 0;

  model->nmasses = x*y;
  model->masses = malloc(model->nmasses * sizeof(mass));

  model->nconstraints = (x-1)*(y-1)*3 + y-1 + x-1;
  model->constraints = malloc(model->nconstraints * sizeof(constraint));


  for(int i = 0; i < x; i++)
    for(int j = 0; j < y; j++) {
      m = &model->masses[i+j*x];
      mass_at(1.0, i * spacing, j * spacing, 0, m);

      if(i < x - 1) {
        c = &model->constraints[ccount++];
        link_masses_dist_eq(spacing,           m, &model->masses[(i+1)+ j   *x], c);
      }

      if(i < x - 1 && j < y - 1) {
        c = &model->constraints[ccount++];
        link_masses_dist_eq(spacing * sqrt(2), m, &model->masses[(i+1)+(j+1)*x], c);
      }

      if(j < y - 1) {
        c = &model->constraints[ccount++];
        link_masses_dist_eq(spacing,           m, &model->masses[i    +(j+1)*x], c);
      }
    }
}

void masstest() {
  mass m1, m2;
  constraint c;
  v3 diff;

  c.type = DIST_EQ;
  c.nmasses = 2;
  c.masses = malloc(c.nmasses * sizeof(mass*));
  c.masses[0] = &m1;
  c.masses[1] = &m2;
  c.val = 10;

  m1.pos.x = m1.pos.y = m1.pos.z = 0;
  m2.pos.x = m2.pos.y = m2.pos.z = 0;

  m2.pos.x = 1;
  m2.pos.y = 2;
  m2.pos.z = 3;

  run_constraint(&c);
  v3sub(&m1.pos, &m2.pos, &diff);
  
  if(flt_eq(c.val, v3mag(&diff)))
    printf("PASS DIST_EQ\n");
  else
    printf("FAIL DIST_EQ: dist = %f, expected %f\n", v3mag(&diff), c.val);

  model m;
  memset(&m, 0, sizeof(model));
  make_grid(10,5,1,&m);
  for(int i = 0; i < m.nconstraints; i++) {
    v3sub(&m.constraints[i].masses[0]->pos, 
          &m.constraints[i].masses[1]->pos,
          &diff);
    if(flt_eq(m.constraints[i].val, v3mag(&diff)))
      printf("PASS grid check\n");
    else
      printf("FAIL grid check: dist = %f, expected %f\n", v3mag(&diff), m.constraints[i].val);
  }
}
