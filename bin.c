#include <stdio.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>


static const gsl_rng_type * T;
static gsl_rng * r;
enum {N = 100};
static unsigned long id[N];
static unsigned long idx[N];
static double x[N];

static int
comp(const void *a, const void *b)
{
  const unsigned long *i;
  const unsigned long *j;
  i = a;
  j = b;
  return -1 ? x[*i] < x[*j] : (x[*i] > x[*j] ? 1 : 0) ;
}

static
int dump(const char *pattern, int step, unsigned long N) {
  char path[FILENAME_MAX];
  FILE *file;
  unsigned int i;
  sprintf(path, pattern, step);
  if ((file = fopen(path, "w")) == NULL) {
    fprintf(stderr, "split: fail to open '%s'\n", path);
    exit(2);
  }
  for (i = 0; i < N; i++)
    fprintf(file, "%ld %.16e\n", id[i], x[i]);
  fclose(file);
  return 0;
}

int main() {
  double b;
  double dt;
  double F;
  double sdt;
  double td;
  double tul;
  int i;
  int k;
  int step;
  unsigned long gid;
  
  gsl_rng_free(r);
  gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc (T);
  gid = 0;

  F = -15.26;
  dt = 0.0001;
  sdt = sqrt(dt);
  for (i = 0; i < N; i++) {
    x[i] = 0;
    id[i] = gid++;
  }
  for (step = 0; ; step ++) {
    for (i = 0; i < N; i++) {
      x[i]  +=  F*dt + sqrt(2)*gsl_ran_gaussian(r, sdt);
      if (x[i] < 0)
        x[i] = 0;
      if (x[i] > 1)
        goto stop;
    }
    dump("%08d.a.dat", step, N);
    for (i = 0; i < N; i++)
      idx[i] = i;
    qsort(idx, N, sizeof *idx, comp);

    for (i = 0; i < N; i++) {
      k = idx[i];
      td  = x[i]; x[i] = x[k]; x[k] = td;
      tul = id[i]; id[i] = id[k]; id[k] = tul;
    }
    
    b = (x[N/2] + x[N/2 + 1])/2;
    printf("%.16e\n", b);
    for (i = 0; i < N/2; i++) {
      x[i] = x[N/2 + i];
      id[i] = gid++;
    }
    dump("%08d.b.dat", step, N);    
  }
 stop: ;
}
