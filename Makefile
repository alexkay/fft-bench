CC = gcc48
CFLAGS = -g -O2 -std=c99 $(shell pkg-config --cflags libavcodec fftw3f)
LDFLAGS = $(shell pkg-config --libs libavcodec fftw3f)
LDLIBS = /usr/local/lib/libdjbfft.a

all: fft-bench

clean:
	rm -f fft-bench

fft-bench: fft-bench.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o fft-bench fft-bench.c $(LDLIBS)

run: fft-bench
	./fft-bench

valgrind: fft-bench
	valgrind --leak-check=full ./fft-bench

.PHONY: clean run
