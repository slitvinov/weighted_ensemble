/*

c99 split.c -O3 `pkg-config --libs --cflags gsl` -o split

*/

#include <stdio.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>


static const gsl_rng_type * T;
static gsl_rng * r;
enum {BUDGET = 10000000};
enum {NB = 10, NMAX = 10000};
enum {NP = NB * NMAX};
static int bins[NB][NMAX];
static int nb[NB];
static const int n = 10; /* target in each bean */

int main() {
  double dt;
  double F;
  double sdt;
  double w[NP];
  double x[NP];
  double P[NB];
  double wm;
  int i;
  int j;
  int k;
  int N;
  int l;
  int m;
  int o;

  gsl_rng_free(r);
  gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc (T);

  F = -15.26;
  dt = 0.0001;
  sdt = sqrt(dt);
  N = 10;
  for (i = 0; i < N; i++) {
    x[i] = 0;
    w[i] = 1;
  }
  for (j = 0; j < BUDGET; ) {
    for (i = 0; i < N; i++) {
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
    for (i = 0; i < N; i++) {
      k = x[i] * NB;
      bins[k][nb[k]] = i;
      if (nb[k] == NMAX) {
	fprintf(stderr, "split: nb[k]=%d = NMAX=%d\n", nb[k], NMAX);
	exit(2);
      }
      nb[k]++;
    }

    /* prob */
    for (i = 0; i < NB; i++)
      P[i] = 0;
    for (i = 0; i < NB; i++)
      for (k = 0; k < nb[i]; k++)
	P[i] += w[bins[i][k]];

    /* split */
    for (i = 0; i < NB; i++)
      for (k = 0; k < nb[i]; k++) {
	l = bins[i][k];
	if (w[l] > 2 * P[i] / n) {
	  m = n * w[l] / P[i];
	  if (n * w[l] > 2 * P[i] * m)
	    m += 1;
	  wm = w[l]/m;
	  for (o = 0; o < m - 1; o++) {
	    if (N == NP) {
	      fprintf(stderr, "split: N=%d = NP=%d\n", N, NP);
	      exit(2);
	    }
	    x[N] = x[l];
	    w[N] = wm;
	    N++;
	  }
	  w[l] = wm;
	}
      }

    for (i = 0; i < NB; i++)
      printf("%d ", nb[i]);
    printf("\n");
    
    for (i = 0; i < NB; i++)
      printf("%g ", P[i]);
    printf("\n");
  }
}
