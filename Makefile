CC ?= gcc48
CFLAGS ?= -g -O2 -std=c99 $(shell pkg-config --cflags libavcodec fftw3f)
LDFLAGS ?= -lm $(shell pkg-config --libs libavcodec fftw3f)

all: fft-bench

clean:
	rm -f fft-bench

fft-bench: fft-bench.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o fft-bench fft-bench.c

run: fft-bench
	./fft-bench

valgrind: fft-bench
	valgrind --leak-check=full ./fft-bench

.PHONY: clean run
