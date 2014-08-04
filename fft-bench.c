#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

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

int main()
{
    int num_samples = 5 * 60 * 44100; // 5 minutes of 44.1k signal

    timer();
    float *samples = generate_samples(num_samples);
    printf("%'d ms\n", timer());
    free(samples);

    return 0;
}
