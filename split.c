/*

c99 split.c -O3 `pkg-config --libs --cflags gsl` -o split

*/

#include <stdio.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>


static const gsl_rng_type * T;
static gsl_rng * r;
enum {BUDGET = 100000};
enum {NB = 10, NMAX = 1000};
enum {NP = NB * NMAX};
static int bins[NB][NMAX];
static int nb[NB];

int main() {
  double dt;
  double F;
  double sdt;
  double w[NP];
  double x[NP];
  double P[NB];
  int i;
  int j;
  int k;
  int n;

  gsl_rng_free(r);
  gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc (T);

  F = -15.26;
  dt = 0.0001;
  sdt = sqrt(dt);
  n = 10;
  for (i = 0; i < n; i++) {
    x[i] = 0;
    w[i] = 1;
  }
  for (j = 0; j < BUDGET; ) {
    for (i = 0; i < n; i++) {
      x[i]  +=  F*dt + sqrt(2)*gsl_ran_gaussian(r, sdt);
      j ++;
      if (x[i] < 0)
	x[i] = 0;
      else if (x[i] > 1)
	x[i] = 0;
    }

    /* sort into bins */
    for (i = 0; i < NB; i++)
      nb[i] = 0;
    for (i = 0; i < n; i++) {
      k = x[i] * NB;
      bins[k][nb[k]++] = i;
    }

    /* prob */
    for (i = 0; i < NB; i++)
      P[i] = 0;
    for (i = 0; i < NB; i++)
      for (k = 0; k < nb[i]; k++)
	P[i] += w[bins[i][k]];

    for (i = 0; i < NB; i++)
      printf("%d ", nb[i]);
    printf("\n");

    for (i = 0; i < NB; i++)
      printf("%g ", P[i]);
    printf("\n");
  }
}
