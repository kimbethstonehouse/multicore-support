#include <pthread.h>
#include <unistd.h>

typedef signed long int int64_t;

using namespace std;

#define MAXITERATE 10000000
#define NORM_FACT 67108864
#define NORM_BITS 26

int64_t realMin, realMax;
int64_t imagMin, imagMax;
int64_t deltaReal, deltaImag;

int width = 80; // frame is 80x25
int height = 25;

int next_pixel = 0;
int *next_pixel_ptr = &next_pixel;
int last_pixel;

int display[80][25];

static inline int fetch_and_add(int* variable, int value) {
    asm volatile("lock; xaddl %0, %1"
    : "+r" (value), "+m" (*variable) // input + output
    : // No input-only
    : "memory"
    );
    return value;
}

static void *mandelbrot(void *arg) {
    int my_next_pixel = fetch_and_add(next_pixel_ptr, 1);

    while (my_next_pixel <= last_pixel) {
        int x = my_next_pixel % width;
        int y = my_next_pixel / width;

        int64_t real0, imag0, realq, imagq, real, imag;
        int count;

        real0 = realMin + x*deltaReal; // current real value
        imag0 = imagMax - y*deltaImag;

        real = real0;
        imag = imag0;
        for (count = 0; count < MAXITERATE; count++) {
            realq = (real * real) >> NORM_BITS;
            imagq = (imag * imag) >> NORM_BITS;

            if ((realq + imagq) > ((int64_t) 4 * NORM_FACT)) break;

            imag = ((real * imag) >> (NORM_BITS-1)) + imag0;
            real = realq - imagq + real0;
        }

        display[x][y] = count;
        my_next_pixel = fetch_and_add(next_pixel_ptr, 1);
    }

    pthread_exit(NULL);
}

int main(const char *cmdline) {
    cpu_set_t cpu[8]; // Create a singleton set for each core
    for (int i = 0; i < 8; i++) {
        CPU_ZERO(&cpu[i]);
        CPU_SET(i, &cpu[i]);
    }

    int numCores = 1;
    int numThreads = 1;
    pthread_t threads[numThreads];
    pthread_attr_t attr;
    void *status;

    // Initialize and set thread joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    // Set max priority FIFO to give the best chance at execution
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    struct sched_param params;
    params.sched_priority = sched_get_priority_max(SCHED_FIFO);

    realMin = -2 * NORM_FACT;
    realMax = 1 * NORM_FACT;
    imagMin = -1 * NORM_FACT;
    imagMax = 1 * NORM_FACT;

    deltaReal = (realMax - realMin)/(width - 1);
    deltaImag = (imagMax - imagMin)/(height - 1);

    last_pixel = width * height;

    for (unsigned int k = 0; k < numThreads; k++) {
        pthread_create(&threads[k], &attr, mandelbrot, (void *) k);
        pthread_setaffinity_np(threads[k], sizeof(cpu[k%numCores]), &cpu[k%numCores]);
    }

    pthread_attr_destroy(&attr);
    for (unsigned int k = 0; k < numThreads; k++) {
        pthread_join(threads[k], &status);
    }

    pthread_exit(NULL);
    return 0;
}