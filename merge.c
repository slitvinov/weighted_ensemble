#include <stdio.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
static const gsl_rng_type * T;
static gsl_rng * r;
enum {BUDGET = 10000};
enum {NB = 10, NMAX = 10000};
enum {NP = NB * NMAX};
enum {n = 5}; /* target in each bean */
static double w[NP];
static double x[NP];
static int bins[NB][NMAX];
static int nb[NB];
static unsigned long id[NP];
static unsigned long pid[NP];

static
int dump(const char *pattern, int step, unsigned long N) {
  char path[FILENAME_MAX];
  FILE *file;
  unsigned long kk;
  unsigned int i;
  sprintf(path, pattern, step);
  if ((file = fopen(path, "w")) == NULL) {
    fprintf(stderr, "merge: fail to open '%s'\n", path);
    exit(2);
  }
  for (i = 0; i < N; i++)
    fprintf(file, "%ld %ld %.16e %.16e\n", id[i], pid[i], x[i], w[i]);
  fclose(file);
  return 0;
}

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
  double dt;
  double F;
  double P[NB];
  double sdt;
  double u;
  double w0;
  double w1;
  double wm;
  unsigned long i;
  int i0;
  int i1;
  int j;
  int k;
  int keep[NP];
  int l;
  int m;
  unsigned long N;
  int o;
  int step;
  unsigned long gid;

  gsl_rng_free(r);
  gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc (T);
  gid = 0;

  F = -15.76;
  dt = 0.0001;
  sdt = sqrt(dt);
  N = n;
  for (i = 0; i < N; i++) {
    x[i] = 0;
    w[i] = 1.0/N;
    id[i] = gid++;
    pid[i] = id[i];
  }
  for (j = step = 0; j < BUDGET; step ++) {
    /* move */
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
	fprintf(stderr, "merge: nb[k]=%d = NMAX=%d\n", nb[k], NMAX);
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
	      fprintf(stderr, "merge: N=%ld = NP=%d\n", N, NP);
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
    if (step % 1 == 0)
      dump("%08d.a.dat", step, N);

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
	id[k] = id[i];
	pid[k] = pid[i];
	k++;
      }
    N = k;
    if (step % 1 == 0)
      dump("%08d.b.dat", step, N);

  }
  for (i = 0; i < NB; i++)
    printf("%.16e %d\n", P[i], nb[i]);
}
