#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <libavcodec/avfft.h>

// Returns time in ms since the last call.
static int timer()
{
    static struct timeval tv;
    struct timeval tv2;
    gettimeofday(&tv2, NULL);
    int ms = (tv2.tv_sec - tv.tv_sec) * 1000 + (tv2.tv_usec - tv.tv_usec) / 1000;
    tv = tv2;
    return ms;
}

static float *generate_samples(int num_samples)
{
    float *samples = malloc(num_samples * sizeof(float));
    float f = M_PI;
    for (int i = 0; i < num_samples; ++i) {
        f *= M_PI;
        f -= floorf(f);
        samples[i] = f;
    }
    return samples;
}

static int test(int (*fn)(float *, int, int), float *samples, int num_samples, int nbits)
{
    int min_time = -1;
    for (int i = 0; i < 5; ++i) {
        int ms = fn(samples, num_samples, nbits);
        if (min_time == -1 || ms < min_time) {
            min_time = ms;
        }
    }
    return min_time;
}

static int test_ffmpeg(float *samples, int num_samples, int nbits)
{
    RDFTContext *cx = av_rdft_init(nbits, DFT_R2C);
    int fft_size = 1 << (nbits - 1);

    timer();
    for (int offset = 0; offset < num_samples - fft_size; offset += fft_size) {
        av_rdft_calc(cx, samples + offset);
    }
    int ms = timer();

    av_rdft_end(cx);
    return ms;
}

int main()
{
    int num_samples = 6 * 60 * 44100; // 6 minutes of 44.1k signal

    float *samples = generate_samples(num_samples);

    for (int nbits = 9; nbits <= 13; ++nbits) {
        int ms = test(test_ffmpeg, samples, num_samples, nbits);
        printf("ffmpeg\t%d\t%d ms\n", nbits, ms);
    }

    free(samples);
    return 0;
}
