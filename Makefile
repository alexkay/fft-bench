CC ?= gcc48
CFLAGS ?= -O2 -std=c99 $(shell pkg-config --cflags libavcodec)
LDFLAGS ?= -lm $(shell pkg-config --libs libavcodec)

all: fft-bench

clean:
	rm -f fft-bench

fft-bench: fft-bench.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o fft-bench fft-bench.c

run: fft-bench
	./fft-bench

.PHONY: clean run
