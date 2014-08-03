CC := gcc48
CFLAGS := -O2
LDFLAGS :=

all: fft-bench

clean:
	rm -f fft-bench

fft-bench: fft-bench.c
	$(CC) $(CFLAGS) $(LDFLAGS) -std=c99 -o fft-bench fft-bench.c

run: fft-bench
	./fft-bench

.PHONY: clean run
