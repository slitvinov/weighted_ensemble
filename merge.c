/*

c99 merge.c -O3 `pkg-config --libs --cflags gsl` -o merge
GSL_RNG_SEED=1 ./merge > 1

*/

#include <stdio.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>


static const gsl_rng_type * T;
static gsl_rng * r;
enum {BUDGET = 100000000};
enum {NB = 10, NMAX = 10000};
enum {NP = NB * NMAX};
static int bins[NB][NMAX];
static int nb[NB];
static const int n = 100; /* target in each bean */
static double w[NP];

static int comp(const void *a, const void *b) {
  const int *i;
  const int *j;
  i = a;
  j = b;
  return -1 ? w[*i] > w[*j] : (w[*i] < w[*j] ? 1 : 0) ;
}


int main() {
  double dt;
  double F;
  double P[NB];
  double sdt;
  double u;
  double w0;
  double w1;
  double wm;
  double x[NP];
  int i;
  int i0;
  int i1;
  int j;
  int k;
  int keep[NP];
  int l;
  int m;
  int N;
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
    w[i] = 1.0/N;
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

    /* sort bins */
    for (i = 0; i < NB; i++)
      qsort(bins[i], nb[i], sizeof *bins[i], comp);

    /* merge */
    for (i = 0; i < N; i++)
      keep[i] = 1;
    for (i = 0; i < NB; i++) {
      for (k = 0; k + 1 < nb[i]; k += 2) {
	i0 = bins[i][k];
	i1 = bins[i][k + 1];
	w0 = w[i0];
	w1 = w[i1];
	wm = w0 + w1;
	if (2*n*wm < P[i]) {
	  u = gsl_rng_uniform(r);
	  if (u * wm < w0) {
	    w[i0] = wm;
	    keep[i1] = 0;
	  } else {
	    w[i1] = wm;
	    keep[i0] = 0;
	  }
	}
      }
    }
    for (i = k = 0; i < N; i++)
      if (keep[i]) {
	x[k] = x[i];
	w[k] = w[i];
	k++;
      }
    N = k;

    /* for (i = 0; i < NB; i++)
      printf("%d ", nb[i]);
    printf("\n");
    */
  }
  for (i = 0; i < NB; i++)
    printf("%.16e %d\n", P[i], nb[i]);
}
