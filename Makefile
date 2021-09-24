.POSIX:
.SUFFIXES:
.SUFFIXES: .c

M = baseline bd bin bin2 main merge several split
CFLAGS = -O3
GSL_FLAGS = `pkg-config --libs --cflags gsl`
all: $M
.c:; $(CC) $< -o $@ $(CFLAGS) $(GSL_FLAGS) $(LDFLAGS)
clean:; @rm $M
