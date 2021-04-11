/* SPDX-License-Identifier: MIT */

#include <iostream>
#include <stdio.h>

int array[1024] = {1}; // int 4 bytes, so each cache has 1/4 of array which is 1kb
int sum;
int salt;

static inline int fetch_and_add(int* variable, int value) {
    asm volatile("lock; xaddl %0, %1"
    : "+r" (value), "+m" (*variable) // input + output
    : // No input-only
    : "memory"
    );
    return value;
}

static void *thread_proc(void *arg)
{
    unsigned int thread_num = (unsigned int)(unsigned long)arg;
    int hash = 0;

    while (hash < sum+400000) { // Each thread iterates 100,000 times
        hash = salt;
    for (int i = thread_num*256; i < (thread_num+1)*256; i++) {
        hash += array[i];
    };

        // Each thread increments the salt, so each thread will
        // iterate a minimum amount of times before meeting the condition
        // since all we want is the thread to keep iterating
        fetch_and_add(&salt, 1);
//        yield(HTHREAD_SELF);
    }

    printf("Suitable hash found 1%u!\n", hash);
    pthread_exit(NULL);
}

int main(const char *cmdline)
{
    printf("Running Cache Test!\n");
    salt = 0;
    sum = 524800;

    pthread_attr_t attr;
    void *status;

    // Initialize and set thread joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Initialise array with values 1 to 1024, sum is 524,800
	for (unsigned int i = 0; i < 1024; i++) {
	    array[i] = 1;
    }

    pthread_t threads[4];
    printf("Using %d threads...\n", 4);
    for (unsigned int i = 0; i < 4; i++) {
        pthread_create(&threads[i], &attr, thread_proc, (void *) i);
    }

    for (unsigned int i = 0; i < 4; i++) {
        printf("Waiting for thread %d...\n", i);
        pthread_join(threads[i], &status);
    }

    printf("Cache Test Complete\n");
    pthread_exit(NULL);
    return 0;
}
