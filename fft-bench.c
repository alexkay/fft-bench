#include <stdio.h>
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

int main()
{
    timer();
    printf("%d ms\n", timer());
    return 0;
}
