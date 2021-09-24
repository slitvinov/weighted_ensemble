#include <stdio.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
static const gsl_rng_type * T;
static gsl_rng * r;
enum {BUDGET = 16000000000};
enum {n = 100000};
static double w[n];
static double x[n];

int main() {
  long double cumflux;
  double dt;
  double F;
  double sdt;
  int j;
  unsigned long step;
  unsigned long i;
  unsigned long N;

  gsl_rng_free(r);
  gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc (T);
  F = -15.76;
  dt = 0.0001;
  sdt = sqrt(dt);
  N = n;
  for (i = 0; i < N; i++) {
    x[i] = 0;
    w[i] = 1.0/N;
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
    if (step % 100 == 0)
      printf("%.16e %.16Le\n", step * dt, cumflux);
    fflush(stdout);
  }
}
