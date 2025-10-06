#include <pthread.h>
#include <stdio.h>
#include <stdint.h>

#define THREADS 5   //Easily changeable for single vs 5 threads comparasion
#define RANGE 10000 //50 000 numbers, 10 000 per threads

void* compute_prime(void* arg)
{
    int id = (int)(intptr_t)arg;
    int start = id * RANGE + 1;
    int end = (id + 1) * RANGE;

    int candidate = start;
    while (candidate <= end){  
        int factor, is_prime = 1;
        for (factor = 2; factor < candidate; ++factor) {
            if (candidate % factor == 0) {
                is_prime = 0;
                break;
            }
        }
        if (is_prime && candidate > 1) { //output
            printf("%d ", candidate); 
        }
            ++candidate;
        }
        return NULL;
}

int main(void) {
    pthread_t thread[THREADS];

    puts("Ranging from 1-50000: concurrently ");

    for (int i = 0; i < 5; ++i) {
        pthread_create(&thread[i], NULL, compute_prime, (void*)(intptr_t)i);
        
    }
    for (int i = 0; i < 5; ++i) { 
        pthread_join(thread[i], NULL);
    }

    return 0;
}
