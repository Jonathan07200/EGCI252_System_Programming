#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

/* Prints Xs to stderr. */
void *print_Xs(void *unused)
{
int n;
for (n = 100; n > 0; n--)
{
fputc ('X', stderr); usleep(1);
}
return NULL;
}


/* Prints Ys to stderr. */
void* print_Ys(void* unused)
{
int n;
for (n = 100; n > 0; n--)
{
fputc ('Y', stderr); usleep(1);
}
return NULL;
}

/* The main program. */
int main()
{
int n;
pthread_t thread_id;
/* Create a new thread. The new thread will run the print_xs function. */
pthread_create (&thread_id, NULL, &print_Xs, NULL);
/* Create a new thread. The new thread will run the print_ys function. */
pthread_create (&thread_id, NULL, &print_Ys, NULL);
/* Print Zs to stderr. */
for (n = 100; n > 0; n--)
{
fputc('Z', stderr); usleep(1);
}
return 0;
}