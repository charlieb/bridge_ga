#include "mass.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>



/* MASS *************************/

void mass_at(float m, float x, float y, float z, mass *mp) {
  memset(mp, 0, sizeof(mass));
  mp->m = m;
  mp->pos.x = x;
  mp->pos.y = y;
  mp->pos.z = z;
  mp->prev_pos.x = x;
  mp->prev_pos.y = y;
  mp->prev_pos.z = z;
  mp->fixed = 0;
}

void print_mass(mass *m) {
  printf("Mass:\n"
         "\tmass = %f\n"
         "\tpos = %f, %f, %f\n"
         "\tprev_pos = %f, %f, %f\n"
         "\tacc = %f, %f, %f\n"
         "\tfixed = %i\n"
         "\tdead = %i\n",
         m->m,
         m->pos.x, m->pos.y, m->pos.z,
         m->prev_pos.x, m->prev_pos.y, m->prev_pos.z,
         m->acc.x, m->acc.y, m->acc.z,
         m->fixed,
         m->dead);
}

void step_mass(mass *m, float dt) {
  v3 pos;
  float f = 0.25; /* damping */
  memcpy(&pos, &m->pos, sizeof(v3));

  if(!m->fixed && !m->dead) {
    /* pn+1 = pn * (2-f) + pn-1 * (1-f) + acc^2 */
    v3mul(&m->pos, 2 - f, &m->pos);
    v3mul(&m->prev_pos, 1 - f, &m->prev_pos);
    v3sub(&m->pos, &m->prev_pos, &m->pos);

    v3mul(&m->acc, dt*dt, &m->acc);
    v3add(&m->pos, &m->acc, &m->pos);
  }
    
  memcpy(&m->prev_pos, &pos, sizeof(v3));
  memset(&m->acc, 0, sizeof(v3));
}

void gravity(mass *m) { 
  v3 g;
  g.x = g.z = 0;
  g.y = 0.98;
  v3add(&m->acc, &g, &m->acc);
}

/* CONSTRAINT ******************/

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
    
void dist_eq(constraint *c) {
  v3 delta;
  if(c->nmasses != 2) return; 
  if(c->masses[0]->dead || c->masses[1]->dead) return;

  mass *m1 = c->masses[0], *m2 = c->masses[1];
  float d;

  v3sub(&m2->pos, &m1->pos, &delta);
  d = c->val - v3mag(&delta);
  
  v3norm(&delta, &delta);
  if(m1->fixed || m2->fixed)
    v3mul(&delta, d, &delta);
  else
    v3mul(&delta, d/2, &delta);

  if(!m1->fixed)
    v3sub(&m1->pos, &delta, &m1->pos);
  if(!m2->fixed)
    v3add(&m2->pos, &delta, &m2->pos);
}

void run_constraint(constraint *c) {
  switch(c->type) {
    case NONE:
      break;
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
      //printf("Tried to run constraint with unsupported type\n");
      break;
  }
}

/* MODEL ************/

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

void translate_model(model *m, v3 *t) {
  for(int i = 0; i < m->nmasses; i++) {
    v3add(&m->masses[i].pos, t, &m->masses[i].pos);
    v3add(&m->masses[i].prev_pos, t, &m->masses[i].prev_pos);
  }
}

void step_model(model *m, float dt) {
  const int nrelaxes = 20;

  for(int i = 0; i < m->nmasses; i++) {
    gravity(&m->masses[i]);
    step_mass(&m->masses[i], dt);
  }

  for(int i = 0; i < nrelaxes; i++)
    for(int j = 0; j < m->nconstraints; j++)
      run_constraint(&m->constraints[j]);
}

void link_masses(mass *m1, mass *m2, constraint *c) {
  if(c->nmasses != 2) {
    c->nmasses = 2;
    if(c->masses == NULL)
      c->masses = malloc(c->nmasses*sizeof(mass*));
    else
      c->masses = realloc(c->masses, c->nmasses*sizeof(mass*));
  }
  c->masses[0] = m1;
  c->masses[1] = m2;
}

void link_masses_none(mass *m1, mass *m2, constraint *c) {
  link_masses(m1, m2, c);
  c->type = NONE;
}

void link_masses_dist_eq(float dist, mass *m1, mass *m2, constraint *c) {
  link_masses(m1, m2, c);
  c->type = DIST_EQ;
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
  //model->nconstraints = ccount;
}

/* MODEL PROCESSING *************/

void kill_unconstrained(model *m) {
  for(int i = 0; i < m->nmasses; i++)
    m->masses[i].dead = true;

  //int max_path = 0;

  mass *path[200];
  int path_end = -1;
  mass* found = 0;

  path[++path_end] = &m->masses[0];
  path[++path_end] = &m->masses[1];
  m->masses[0].dead = false;
  m->masses[1].dead = false;

  while(path_end >= 0) {
    for(int c = 0; c < m->nconstraints; c++) {
      constraint *cons = &m->constraints[c];
      if(cons->nmasses != 2 || cons->type != DIST_EQ) continue;

      if(cons->masses[0] == path[path_end]) {
        if(cons->masses[1]->dead) {
          found = cons->masses[1];
          break;
        }
      }
      if(cons->masses[1] == path[path_end]) {
        if(cons->masses[0]->dead) {
          found = cons->masses[0];
          break;
        }
      }
    }
    if(found) {
      path[++path_end] = found;
      found->dead = false;
      found = 0;
      //if(max_path < path_end) max_path = path_end;
    }
    else 
      path_end--;
  }
  //printf("Max path: %i\n", max_path);
}

/* TEST **********************/

void masstest() {
  mass m1 = {0,}, m2 = {0,};
  constraint c = {0,};
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
  bool grid_pass = true;
  for(int i = 0; i < m.nconstraints; i++) {
    v3sub(&m.constraints[i].masses[0]->pos, 
          &m.constraints[i].masses[1]->pos,
          &diff);
    if(!flt_eq(m.constraints[i].val, v3mag(&diff))) grid_pass = false;
  }
  if(grid_pass)
    printf("PASS grid check");
  else
    printf("FAIL grid check");

  model mo;
  memset(&m, 0, sizeof(model));
  make_grid(3,3,1, &mo);
  kill_unconstrained(&mo);

}
