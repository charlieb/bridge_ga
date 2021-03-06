#include <stdio.h>

#include "mass.h"

#define NUM_MASSES 100
#define NUM_CONSTRAINTS 200
typedef struct gene {
  float masses[NUM_MASSES*4];
  int constraints[NUM_CONSTRAINTS*2];
} gene;

void mass_to_gene(mass *m, float *gene);
void gene_to_mass(float *gene, mass *m);
void constraint_to_gene(constraint *c, mass *m, int *gene);
void gene_to_constraint(int *gene, constraint *c, mass *m);
void model_to_gene(model *m, gene *g);
void gene_to_model(gene *g, model *m);
void run_until_settled(model *m);
void crossover(gene *g1, gene *g2, gene *res);
void init_gene(gene *g);
void init_pool(gene **genes, int ngenes);
void get_latest_genes(gene **genes, int *ngenes);
void write_genes(gene *genes, int ngenes, FILE *f);
void read_genes(gene *genes, int ngenes, FILE *f);
void start_GA(char *filename);
void genetest();
