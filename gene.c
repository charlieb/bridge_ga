#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "gene.h"

static const float xrange = 400;
static const float yrange = 400;
static const float massrange = 10;

void mass_to_gene(mass *m, float *gene) {
  gene[0] = m->m;
  gene[1] = m->pos.x;
  gene[2] = m->pos.y;
  gene[3] = m->pos.z;
}

void gene_to_mass(float *gene, mass *m) {
  m->m = gene[0];
  m->pos.x = gene[1];
  m->pos.y = gene[2];
  m->pos.z = gene[3];
}

void constraint_to_gene(constraint *c, mass *m, int *gene) {
  gene[0] = c->masses[0] - m;
  gene[1] = c->masses[1] - m;
}

void gene_to_constraint(int *gene, constraint *c, mass *m) {
  const float max_dist = 15;

  float d = v3dist(&m[gene[0]].pos, &m[gene[1]].pos);
  if(d < max_dist) link_masses_dist_eq(d, m+gene[0], m+gene[1], c);
}

void model_to_gene(model *m, gene *g) {
  for(int i = 0; i < NUM_MASSES; i++) 
    mass_to_gene(&m->masses[i], &g->masses[i*4]);

  for(int i = 0; i < NUM_CONSTRAINTS; i++) 
    constraint_to_gene(&m->constraints[i], m->masses, &g->constraints[i*2]);
}

void gene_to_model(gene *g, model *m) {
  for(int i = 0; i < NUM_MASSES; i++) 
    gene_to_mass(&g->masses[i*4], &m->masses[i]);

  for(int i = 0; i < NUM_CONSTRAINTS; i++) 
    gene_to_constraint(&g->constraints[i*2], &m->constraints[i], m->masses);
}

void crossover(gene *g1, gene *g2, gene *res) {
  int mass_point = (rand() * NUM_MASSES) / RAND_MAX;
  int cons_point = (rand() * NUM_CONSTRAINTS) / RAND_MAX;
  gene tmp;

  memcpy(tmp.masses, g1->masses, mass_point * 4 * sizeof(float));
  memcpy(tmp.masses + mass_point, g2->masses, (NUM_MASSES - mass_point) * 4 * sizeof(float));

  memcpy(tmp.constraints, g1->constraints, cons_point * 2 * sizeof(float));
  memcpy(tmp.constraints + cons_point, g2->constraints, (NUM_CONSTRAINTS - cons_point) * 2 * sizeof(float));

  memcpy(res, &tmp, sizeof(gene));
}

void init_gene(gene *g) {
  for(int i = 0; i < NUM_MASSES; i++) {
    g->masses[4*i + 0] = (rand() * massrange) / RAND_MAX;
    g->masses[4*i + 1] = (rand() * xrange) / RAND_MAX;
    g->masses[4*i + 2] = (rand() * yrange) / RAND_MAX;
    g->masses[4*i + 3] = 0;
  }

  for(int i = 0; i < NUM_CONSTRAINTS; i++) {
    g->constraints[2*i + 0] = ((long int)rand() * (long int)NUM_MASSES) / RAND_MAX;
    g->constraints[2*i + 1] = ((long int)rand() * (long int)NUM_MASSES) / RAND_MAX;
  }
}

void init_model_for_gene(model *m) {
  m->nmasses = NUM_MASSES;
  m->masses = malloc(m->nmasses * sizeof(mass));

  m->nconstraints = NUM_CONSTRAINTS;
  m->constraints = malloc(m->nconstraints * sizeof(constraint));

}

void randomize_gene_pool(gene *genes, int ngenes) {
  for(int i = 0; i < ngenes; i++) 
    init_gene(&genes[i]);
}

void init_gene_pool(gene **genes, int ngenes) {
  *genes = malloc(ngenes * sizeof(gene));

  randomize_gene_pool(*genes, ngenes);
}

void run_until_settled(model *m) {
  const float max_dist = 1.000001;
  bool settled = false;
  while(!settled) {
    step_model(m, 1);
    settled = true;
    for(int i = 0; i < m->nmasses; i++)
      if(v3dist(&m->masses[i].pos, &m->masses[i].prev_pos) > max_dist) {
        settled = false;
        break;
      }
  }
}

float fitness(gene *g) {
  static model m = {0,};
  if(m.masses == 0) 
    init_model_for_gene(&m);

  // Fix positions of first two genes in x and y
  g->masses[1] = 100;
  g->masses[2] = 0;

  g->masses[5] = 300;
  g->masses[6] = 0;

  gene_to_model(g, &m);
  fix_unconstrained(&m);
  
  m.masses[0].fixed = true;
  m.masses[1].fixed = true;

  run_until_settled(&m);

  float max_y = 0;

  for(int i = 0; i < m.nmasses; i++)
    if(m.masses[i].pos.y > max_y)
      max_y = m.masses[i].pos.y;

  /*
  printf("0.5 * %f * |%f, %f| = %f\n", max_y, m.masses[0].pos.x, m.masses[1].pos.x,
    0.5 * max_y * fabs(m.masses[0].pos.x - m.masses[1].pos.x));
    */

 return 0.5 * max_y * fabs(m.masses[0].pos.x - m.masses[1].pos.x);
}

int roulette_select(float *scores, int nscores) {
  float selector = ((float)rand()) / RAND_MAX;
  float total = 0;
  for(int i = 0; i < nscores; i++) {
    total += scores[i];
    if(total > selector)
      return i;
  }
  return nscores -1;
}

void mutate(gene *g, float rate) {
  for(int i = 0; i < NUM_MASSES; i++) {
    if(rand() < RAND_MAX * rate) {
      //printf("Mutate! mass: %f -> ", g->masses[4*i + 0]);
      g->masses[4*i + 0] = (rand() * massrange) / RAND_MAX;
      //printf("%f\n", g->masses[4*i + 0]);
    }
    if(rand() < RAND_MAX * rate) {
      //printf("Mutate! x: %f -> ", g->masses[4*i + 1]);
      g->masses[4*i + 1] = (rand() * xrange) / RAND_MAX;
      //printf("%f\n", g->masses[4*i + 1]);
    }
    if(rand() < RAND_MAX * rate) {
      //printf("Mutate! y: %f -> ", g->masses[4*i + 2]);
      g->masses[4*i + 2] = (rand() * yrange) / RAND_MAX;
      //printf("%f\n", g->masses[4*i + 2]);
    }
  }

  for(int i = 0; i < NUM_CONSTRAINTS; i++) {
    if(rand() < RAND_MAX * rate) {
      //printf("Mutate! constraint: %i -> ", g->constraints[2*i + 0]);
      g->constraints[2*i + 0] = ((long int)rand() * (long int)NUM_MASSES) / RAND_MAX;
      //printf("%i\n", g->constraints[2*i + 0]);
    }
    if(rand() < RAND_MAX * rate) {
      //printf("Mutate! constraint: %i -> ", g->constraints[2*i + 1]);
      g->constraints[2*i + 1] = ((long int)rand() * (long int)NUM_MASSES) / RAND_MAX;
      //printf("%i\n", g->constraints[2*i + 1]);
    }
  }
}

void next_generation(gene *genes, int ngenes, gene *next_gen) {
  float scores[ngenes];
  float total_score = 0;
  float hi_score = 0;
  float lo_score = 0;

  for(int i = 0; i < ngenes; i++) {
    scores[i] = fitness(&genes[i]);
    total_score += scores[i];
  }

  for(int i = 0; i < ngenes; i++) {
    if(scores[i] > hi_score) hi_score = scores[i];
    if(scores[i] < lo_score) lo_score = scores[i];
  }
    
  printf("Best: %f, Worst %f\n", hi_score, lo_score); fflush(NULL);

  // normalize scores
  for(int i = 0; i < ngenes; i++)
    scores[i] /= total_score;

  int g1,g2;

  for(int i = 0; i < ngenes; i++) {
    g1 = roulette_select(scores, ngenes);
    g2 = roulette_select(scores, ngenes);
    crossover(&genes[g1], &genes[g2], &next_gen[i]);
  }
  
  hi_score = lo_score = 0;
  for(int i = 0; i < ngenes; i++) {
    if(scores[i] > hi_score) hi_score = scores[i];
    if(scores[i] < lo_score) lo_score = scores[i];
  }
    
  printf("Norm Best: %f, Worst %f\n", hi_score, lo_score); fflush(NULL);

  for(int i = 0; i < ngenes; i++)
    mutate(&genes[i], 0.001 * (1 - (hi_score - lo_score)));
}

void start_GA() {
  int ngenes = 200;
  gene *pool1, *pool2;
  gene *this_gen, *next_gen, *swap;

  init_gene_pool(&pool1, ngenes);
  init_gene_pool(&pool2, ngenes);
  
  this_gen = pool1;
  next_gen = pool2;

  while(true) {
    printf("Generation\n"); fflush(NULL);
    next_generation(this_gen, ngenes, next_gen);
    swap = this_gen;
    this_gen = next_gen;
    next_gen = swap;
  }
}

void genetest() {
  gene g1, g2;

  init_gene(&g1);
  memcpy(&g2, &g1, sizeof(gene));

  model m;
  memset(&m, 0, sizeof(model));
  init_model_for_gene(&m);

  gene_to_model(&g1, &m);
  memset(&g1, 0, sizeof(gene));
  model_to_gene(&m, &g1);
  if(memcmp(&g1, &g2, sizeof(gene)))
    printf("FAIL gene_to_model\n");
  else
    printf("PASS gene_to_model\n");

}
