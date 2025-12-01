#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/wait.h>

// We need to use System V semaphores instead of POSIX semaphores
int sem_id;
struct sembuf sem_b;

void f1(pid_t pid);
void f2(pid_t pid);

static int semaphore_p(int sem_num)
{
struct sembuf sem_b;

sem_b.sem_num = 0;
sem_b.sem_op = -1; /* P() */
sem_b.sem_flg = SEM_UNDO;
if (semop(sem_id, &sem_b, 1) == -1)
{
fprintf(stderr, "semaphore_p failed\n");
return(0);
}
return(1);
}

static int semaphore_v(int sem_num)
{
struct sembuf sem_b;

sem_b.sem_num = 0;
sem_b.sem_op = 1; /* V() */
sem_b.sem_flg = SEM_UNDO;
if (semop(sem_id, &sem_b, 1) == -1)
{
fprintf(stderr, "semaphore_v failed\n");
return(0);
}
return(1);
}



void f1(pid_t pid) {
    semaphore_p(0);
    //sleep(2);
    if(pid) f2(pid);
    semaphore_v(0);
}

void f2(pid_t pid) {
    semaphore_p(1);
    //sleep(2);
    if(!pid) f1(pid);
    semaphore_v(1);
}

int main() {

     // Create a semaphore set with 2 semaphores
    sem_id = semget(IPC_PRIVATE, 2, 0666 | IPC_CREAT);
    if (sem_id == -1) {
        perror("semget failed");
        exit(1);
    }
    
    // Initialize both semaphores to 1
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } arg;

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // CHILD calls f2
        printf("Child calling f2\n");
        f2(pid);
        printf("Child done!\n");
    } else {
        // PARENT calls f1
        printf("Parent calling f1\n");
        f1(pid);
        printf("Parent done!\n");
        wait(NULL);
    }

    return 0;
}