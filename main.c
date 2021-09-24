#include <stdio.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
static const char *me = "main";
static const gsl_rng_type * T;
static gsl_rng * r;
enum {BUDGET = 160000000};
enum {BMAX = 100, NMAX = 1000};
enum {NP = BMAX * NMAX};
static double w[NP];
static double x[NP];
static int bins[BMAX][NMAX];
static int nb[BMAX];
static unsigned long id[NP];
static unsigned long pid[NP];

static void
usg(void)
{
    fprintf(stderr,
	    "%s -n int\n",
	    me);
    exit(1);
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

int
main(int argc, char **argv)
{
  char *end;
  double dt;
  double F;
  double sdt;
  double u;
  double w0;
  double w1;
  double wm;
  double *P;
  int i0;
  int i1;
  int j;
  int k;
  int keep[NP];
  int l;
  int m;
  int Nflag;
  int Bflag;
  int nnew;
  int o;
  long double cumflux;
  long n;
  unsigned long gid;
  unsigned long i;
  unsigned long N;
  unsigned long step;
  unsigned long b;

  Nflag = 0;
  Bflag = 0;
  while (*++argv != NULL && argv[0][0] == '-')
    switch (argv[0][1]) {
    case 'h':
      usg();
      break;
    case 'n':
      argv++;
      if (argv[0] == NULL) {
	fprintf(stderr, "%s: -n needs an argument\n", me);
	return 1;
      }
      n = strtol(argv[0], &end, 10);
      if (errno != 0 || *end != '\0') {
	fprintf(stderr, "%s: -n argument is not an integer '%s'\n",
		me, argv[0]);
	return 1;
      }
      Nflag = 1;
      break;
    case 'b':
      argv++;
      if (argv[0] == NULL) {
	fprintf(stderr, "%s: -b needs an argument\n", me);
	return 1;
      }
      b = strtol(argv[0], &end, 10);
      if (errno != 0 || *end != '\0') {
	fprintf(stderr, "%s: -b argument is not an integer '%s'\n",
		me, argv[0]);
	return 1;
      }
      Bflag = 1;
      break;
    default:
      fprintf(stderr, "%s: unknown option '%s'\n", me, argv[0]);
      return 1;
    }

  if (Nflag == 0) {
    fprintf(stderr, "%s: -n is not set\n", me);
    return 1;
  }
  if (Bflag == 0) {
    fprintf(stderr, "%s: -b is not set\n", me);
    return 1;
  }

  if ((P = malloc(b * sizeof *P)) == NULL) {
    fprintf(stderr, "%s: malloca failed\n", me);
    return 1;
  }

  gsl_rng_free(r);
  gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc (T);
  gid = 0;

  F = -15.76;
  dt = 0.0001;
  sdt = sqrt(dt);
  N = n * b;
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
    for (i = 0; i < b; i++)
      nb[i] = 0;
    for (i = 0; i < N; i++) {
      k = x[i] * b;
      bins[k][nb[k]] = i;
      if (nb[k] == NMAX) {
	fprintf(stderr, "%s: nb[k]=%d = NMAX=%d\n", me, nb[k], NMAX);
	exit(2);
      }
      nb[k]++;
    }

    /* prob */
    for (i = 0; i < b; i++)
      P[i] = 0;
    for (i = 0; i < b; i++)
      for (k = 0; k < nb[i]; k++)
	P[i] += w[bins[i][k]];

    /* split */
    for (i = 0; i < b; i++)
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
	      fprintf(stderr, "%s: N=%ld = NP=%d\n", me, N, NP);
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
    for (i = 0; i < b; i++)
      qsort(bins[i], nb[i], sizeof *bins[i], comp);

    /* merge */
    for (i = 0; i < N; i++)
      keep[i] = 1;
    for (i = 0; i < b; i++) {
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
      printf("%.16e %.16Le %.16Le\n", step * dt, cumflux, step ? cumflux / step / dt : 0);
    fflush(stdout);
  }

  free(P);
}
