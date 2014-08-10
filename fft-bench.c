#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <fftw3.h>
#include <libavcodec/avfft.h>
#include <djbfft/fftr4.h>

// Returns time in ms since the last call.
static int timer()
{
    static struct timeval tv;
    struct timeval tv2;
    gettimeofday(&tv2, NULL);
    int ms = (tv2.tv_sec - tv.tv_sec) * 1000 + (tv2.tv_usec - tv.tv_usec + 500) / 1000;
    tv = tv2;
    return ms;
}

static void generate_samples(float *samples, int num_samples)
{
    float f = M_PI;
    for (int i = 0; i < num_samples; ++i) {
        f *= M_PI;
        f -= floorf(f);
        samples[i] = f;
    }
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
    generate_samples(samples, num_samples);
    RDFTContext *cx = av_rdft_init(nbits, DFT_R2C);
    int fft_size = 1 << nbits;

    timer();
    for (int offset = 0; offset < num_samples - fft_size; offset += fft_size) {
        av_rdft_calc(cx, samples + offset);
    }
    int ms = timer();

    av_rdft_end(cx);
    return ms;
}

static int test_fftw_in(float *samples, int num_samples, int nbits)
{
    generate_samples(samples, num_samples);
    int fft_size = 1 << nbits;
    fftwf_plan p = fftwf_plan_dft_r2c_1d(fft_size, samples, (fftwf_complex*)samples, FFTW_ESTIMATE);

    timer();
    for (int offset = 0; offset < num_samples - fft_size; offset += fft_size) {
        fftwf_execute_dft_r2c(p, samples + offset, (fftwf_complex *)(samples + offset));
    }
    int ms = timer();

    fftwf_destroy_plan(p);
    return ms;
}

static int test_fftw_out(float *samples, int num_samples, int nbits)
{
    generate_samples(samples, num_samples);
    int fft_size = 1 << nbits;
    fftwf_complex *output = fftwf_alloc_complex(fft_size / 2 + 1);
    fftwf_plan p = fftwf_plan_dft_r2c_1d(fft_size, samples, output, FFTW_ESTIMATE);

    timer();
    for (int offset = 0; offset < num_samples - fft_size; offset += fft_size) {
        fftwf_execute_dft_r2c(p, samples + offset, output);
    }
    int ms = timer();

    fftwf_free(output);
    fftwf_destroy_plan(p);
    return ms;
}

static int test_djbfft(float *samples, int num_samples, int nbits)
{
    generate_samples(samples, num_samples);
    int fft_size = 1 << nbits;

    void (*fn)(real4 *) = NULL;
    switch(fft_size) {
    case 512: fn = fftr4_512; break;
    case 1024: fn = fftr4_1024; break;
    case 2048: fn = fftr4_2048; break;
    case 4096: fn = fftr4_4096; break;
    case 8192: fn = fftr4_8192; break;
    }

    timer();
    for (int offset = 0; offset < num_samples - fft_size; offset += fft_size) {
        fn(samples + offset);
    }
    int ms = timer();

    return ms;
}

int main()
{
    int num_samples = 10 * 60 * 44100; // 10 minutes of 44.1 kHz signal
    int alignment = 16; // for SIMD optimization
    void *samples = malloc(num_samples * sizeof(float) + alignment);
    float *aligned = (float *)((size_t)((char *)samples + alignment - 1) & ~(size_t)(alignment - 1));

    for (int nbits = 9; nbits <= 13; ++nbits) {
        printf("lavc/i\t%d\t%d\n", nbits, test(test_ffmpeg, aligned, num_samples, nbits));
        printf("fftw/i\t%d\t%d\n", nbits, test(test_fftw_in, aligned, num_samples, nbits));
        printf("fftw/o\t%d\t%d\n", nbits, test(test_fftw_out, aligned, num_samples, nbits));
        printf("djbf/i\t%d\t%d\n", nbits, test(test_djbfft, aligned, num_samples, nbits));
    }

    free(samples);
    return 0;
}
