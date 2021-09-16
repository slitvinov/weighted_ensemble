#include <stdio.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

static const gsl_rng_type * T;
static gsl_rng * r;

int main() {
  double dt;
  double F;
  double sdt;
  double x;
  int i;

  gsl_rng_free(r);
  gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc (T);


  x = 0;
  F = -15.26;
  dt = 0.0001;
  sdt = sqrt(dt);

  for (x = 0; x < 1;) {
    x  +=  F*dt + sqrt(2)*gsl_ran_gaussian(r, sdt);
    printf("%.2e\n", x);
    if (x < 0)
      x = 0;
  }
}
