#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define FIFO_1 "./fifo1to2"
#define FIFO_2 "./fifo2to1"

#define MAX_RBUF 80

int FIFO_FD1, FIFO_FD2;

int main(int argc, char *argv[])
{
    int child, nbytes;
    char rbuf[MAX_RBUF] = "";

    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <1,2>\n", *argv);
        exit(EXIT_FAILURE);
    }

    if (access(FIFO_1, F_OK) == -1) 
    {
        if (mkfifo(FIFO_1, 0666) == -1)
        {
            perror("Error creating FIFO 1");
            exit(EXIT_FAILURE);
        }
    }
    if (access(FIFO_2, F_OK) == -1) 
    {
        if (mkfifo(FIFO_2, 0666) == -1)
        {
            perror("Error creating FIFO 2");
            exit(EXIT_FAILURE);
        }
    }

    if (strcmp(argv[1], "1") == 0)
    {
        printf("Chatter 1 starting...\n");
        FIFO_FD2 = open(FIFO_2, O_RDONLY);
        FIFO_FD1 = open(FIFO_1, O_WRONLY);
        
        if (FIFO_FD1 == -1 || FIFO_FD2 == -1) {
            perror("Error opening FIFOs");
            exit(EXIT_FAILURE);
        }

        child = fork();
        switch (child)
        {
            case -1: 
                perror("Fork error"); 
                exit(EXIT_FAILURE);
            
            case 0: 
                while (1)
                {
                    memset(rbuf, 0, sizeof(rbuf));
                    nbytes = read(FIFO_FD2, rbuf, sizeof(rbuf)-1);
                    if (nbytes > 0) {
                        printf("Chatter2: %s", rbuf);
                        fflush(stdout);
                    }
                    if (strncmp(rbuf, "end chat", 8) == 0) {
                        break;
                    }
                }
                exit(EXIT_SUCCESS);
                break;

            default: 
                while (1)
                {
                    printf("Chatter1: ");
                    fflush(stdout);
                    memset(rbuf, 0, sizeof(rbuf));
                    if (fgets(rbuf, sizeof(rbuf), stdin) == NULL) {
                        break;
                    }
                    write(FIFO_FD1, rbuf, strlen(rbuf));
                    if (strncmp(rbuf, "end chat", 8) == 0) {
                        break;
                    }
                }
                wait(NULL);
                break;
        }
    }
    else if (strcmp(argv[1], "2") == 0)
    {
        printf("Chatter 2 starting...\n");
        FIFO_FD2 = open(FIFO_1, O_RDONLY);
        FIFO_FD1 = open(FIFO_2, O_WRONLY);
        
        if (FIFO_FD1 == -1 || FIFO_FD2 == -1) {
            perror("Error opening FIFOs");
            exit(EXIT_FAILURE);
        }

        child = fork();
        switch (child)
        {
            case -1: 
                perror("Fork error"); 
                exit(EXIT_FAILURE);
            
            case 0: 
                while (1)
                {
                    memset(rbuf, 0, sizeof(rbuf));
                    nbytes = read(FIFO_FD2, rbuf, sizeof(rbuf)-1);
                    if (nbytes > 0) {
                        printf("Chatter1: %s", rbuf);
                        fflush(stdout);
                    }
                    if (strncmp(rbuf, "end chat", 8) == 0) {
                        break;
                    }
                }
                exit(EXIT_SUCCESS);
                break;
            
            default: 
                while (1)
                {
                    printf("Chatter2: ");
                    fflush(stdout);
                    memset(rbuf, 0, sizeof(rbuf));
                    if (fgets(rbuf, sizeof(rbuf), stdin) == NULL) {
                        break;
                    }
                    write(FIFO_FD1, rbuf, strlen(rbuf));
                    if (strncmp(rbuf, "end chat", 8) == 0) {
                        break;
                    }
                }
                wait(NULL);
                break;
        }
    }
    else
    {
        fprintf(stderr, "Invalid argument. Use 1 or 2.\n");
        exit(EXIT_FAILURE);
    }

    if (FIFO_FD1 != -1) close(FIFO_FD1);
    if (FIFO_FD2 != -1) close(FIFO_FD2);

    return 0;
}