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

static char *output = NULL;
void sig_handler(int signo)
{
  gene *genes = NULL;
  int ngenes = 0;

  if (signo == SIGINT) {
    printf("received SIGINT\n");
    if(output) {
      printf("Writing to %s\n", output);
      get_latest_genes(&genes, &ngenes);
      FILE *f = fopen(output, "w");
      write_genes(genes, ngenes, f);
      fclose(f);
    }
    else
      printf("No output filename to write\n");
  }
  exit(0);
}

int main(int argc, char **argv) {
  if (signal(SIGINT, sig_handler) == SIG_ERR)
    printf("\ncan't catch SIGINT\n");

  char *input = NULL;
  bool test = false;

  for(int i = 1; i < argc; i++) 
    switch(argv[i][1]) {
      case 'i':
        input = argv[++i];
        break;
      case 'o':
        output = argv[++i];
        break;
      case 't':
        test = true;
        break;
      case 'h':
      default:
        printf("-i <filename> : specify the input filename\n"
              "-o <filename> : specify the output filename\n"
              "-t : self test\n");
        break;
    }

  if(test) {
    v3test();
    masstest();
    genetest();
  }
  else {
    srand(time(NULL));

    init_drawing();
    start_GA(input);
    cleanup_drawing();
  }

  return 0;
}
