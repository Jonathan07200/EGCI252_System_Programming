#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
void* thread_function (void* thread_arg)
{
/* Do work here... */
while(1) printf(">");
}
int main (void)
{
pthread_attr_t attr;
pthread_t thread;
pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
pthread_create(&thread, &attr, &thread_function, NULL);
pthread_attr_destroy(&attr);
/* Do work here... */
/* No need to join the second thread. */
/* However the main thread needs to make sure that it will nish after the other threads */
pthread_exit(NULL);
}