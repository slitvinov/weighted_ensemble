.POSIX:
.SUFFIXES:
.SUFFIXES: .c

M = bin main merge
CFLAGS = -O3
GSL_FLAGS = `pkg-config --libs --cflags gsl`
all: $M
.c:; $(CC) $< -o $@ $(CFLAGS) $(GSL_FLAGS) $(LDFLAGS)
clean:; @rm $M
