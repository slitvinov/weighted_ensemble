#include <stdio.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
static const gsl_rng_type * T;
static gsl_rng * r;
enum {BUDGET = 16000000000};
enum {NB = 122, NMAX = 10000};
enum {NP = NB * NMAX};
enum {n = 100}; /* target in each bean */
static long double w[NP];
static long double x[NP];
static int bins[NB][NMAX];
static int nb[NB];
static unsigned long id[NP];
static unsigned long pid[NP];

static int
comp(const void *a, const void *b)
{
  const int *i;
  const int *j;
  i = a;
  j = b;
  return -1 ? w[*i] > w[*j] : (w[*i] < w[*j] ? 1 : 0) ;
}

int main() {
  long double cumflux;
  long double dt;
  long double F;
  long double P[NB];
  long double sdt;
  long double u;
  long double w0;
  long double w1;
  long double wm;
  int i0;
  int i1;
  int j;
  int k;
  int keep[NP];
  int l;
  int m;
  int nnew;
  int o;
  unsigned long step;
  unsigned long gid;
  unsigned long i;
  unsigned long N;

  gsl_rng_free(r);
  gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc (T);
  gid = 0;

  F = -15.76;
  dt = 0.000005;
  sdt = sqrt(dt);
  N = n;
  for (i = 0; i < N; i++) {
    x[i] = 0;
    w[i] = 1.0/N;
    id[i] = gid++;
    pid[i] = id[i];
  }
  cumflux = 0;
  for (j = step = 0; j < BUDGET; step ++) {
    for (i = 0; i < N; i++) {
      double xn;
      xn = x[i] + F*dt + sqrt(2)*gsl_ran_gaussian(r, sdt);
      j++;
      if (xn > 1) {
	x[i] = 0;
	cumflux += w[i];
      } else if (xn > 0) {
	x[i] = xn;
      }
    }

    /* sort into bins */
    for (i = 0; i < NB; i++)
      nb[i] = 0;
    for (i = 0; i < N; i++) {
      k = x[i] * NB;
      bins[k][nb[k]] = i;
      if (nb[k] == NMAX) {
	fprintf(stderr, "main: nb[k]=%d = NMAX=%d\n", nb[k], NMAX);
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
	  nnew += 1;
	  wm = w[l]/m;
	  for (o = 0; o < m - 1; o++) {
	    if (N == NP) {
	      fprintf(stderr, "main: N=%ld = NP=%d\n", N, NP);
	      exit(2);
	    }
	    x[N] = x[l];
	    w[N] = wm;
	    id[N] = gid++;
	    pid[N] = id[i];
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
    /* delete */
    for (i = k = 0; i < N; i++)
      if (keep[i]) {
	x[k] = x[i];
	w[k] = w[i];
	id[k] = id[i];
	pid[k] = pid[i];
	k++;
      }
    N = k;

    if (step % 1000 == 0)
      printf("%.16Le %.16Le\n", step * dt, cumflux);
    fflush(stdout);
  }
  //for (i = 0; i < NB; i++)
  //  fprintf(stderr, "%.16e %d\n", P[i], nb[i]);
}
