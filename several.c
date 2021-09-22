/*

c99 several.c -O3 `pkg-config --libs --cflags gsl` -o several

*/

#include <stdio.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>


static const gsl_rng_type * T;
static gsl_rng * r;
enum {N = 10};
enum {BUDGET = 100000};

int main() {
  double dt;
  double F;
  double sdt;
  double x[N];
  int i;
  int j;

  gsl_rng_free(r);
  gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc (T);

  F = -15.26;
  dt = 0.0001;
  sdt = sqrt(dt);
  for (i = 0; i < N; i++)
    x[i] = 0;
  for (j = 0; j < BUDGET; ) {
    for (i = 0; i < N; i++) {
      x[i]  +=  F*dt + sqrt(2)*gsl_ran_gaussian(r, sdt);
      j ++;
      if (x[i] < 0)
	x[i] = 0;
      else if (x[i] > 1)
	x[i] = 0;
    }
    for (i = 0; i < N; i++)
      printf("%.16e ", x[i]);
    printf("\n");
  }
}
