#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gene.h"

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
  link_masses_dist_eq(v3dist(&m[gene[0]].pos, &m[gene[1]].pos),
                      m+gene[0], m+gene[1], c);
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

void init_gene(gene *g, float xrange, float yrange, float massrange) {
  for(int i = 0; i < NUM_MASSES; i++) {
    g->masses[4*i + 0] = (rand() * massrange) / RAND_MAX;
    g->masses[4*i + 1] = (rand() * xrange) / RAND_MAX;
    g->masses[4*i + 2] = (rand() * yrange) / RAND_MAX;
    g->masses[4*i + 3] = 0;
  }

  for(int i = 0; i < NUM_CONSTRAINTS; i++) {
    g->constraints[2*i + 0] = (rand() * NUM_MASSES) / RAND_MAX;
    g->constraints[2*i + 1] = (rand() * NUM_MASSES) / RAND_MAX;
  }
}

void init_pool(gene *genes, int ngenes, float xrange, float yrange, float massrange) {
  for(int i = 0; i < ngenes; i++) 
    init_gene(&genes[i], xrange, yrange, massrange);
}

void init_model_for_gene(model *m) {
  m->nmasses = NUM_MASSES;
  m->masses = malloc(m->nmasses * sizeof(mass));

  m->nconstraints = NUM_CONSTRAINTS;
  m->constraints = malloc(m->nconstraints * sizeof(constraint));
}

void genetest() {
  gene g1, g2;

  init_gene(&g1, 100, 100, 100);
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
