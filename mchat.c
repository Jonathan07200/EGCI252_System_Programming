
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>  

#define MAX_TEXT 80
#define SHARED_FILE "chat_log"  //shared file chat log 
#define SHARED_SIZE 256  

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <1 or 2>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // memory mapping variables
    int fd;
    char *shared_mem;
    int user_id = atoi(argv[1]);
    char msg[MAX_TEXT];

    // map creation
    fd = open(SHARED_FILE, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, SHARED_SIZE) == -1) {
        perror("ftruncate");
        close(fd);
        exit(EXIT_FAILURE);
    }

    //mapping of the shared memory
    shared_mem = mmap(NULL, SHARED_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // init 
    memset(shared_mem, 0, SHARED_SIZE);
    shared_mem[0] = '0'; //0 = no message, 1 = message

    printf("User %d connected to chat: \n", user_id);

    pid_t child = fork();
    if (child == 0) {
        // child processing, or receiver
        char last_sender = '0';
        while (1) {
            //0 = no message, 1 = message
            if (shared_mem[0] == '1' && shared_mem[1] != user_id + '0') {
                printf("Incomming_message: %s", shared_mem + 2);
                fflush(stdout);
               
                // chat end detection
                if (strncmp(shared_mem + 2, "end chat", 8) == 0) {
                    shared_mem[0] = '1';
                    shared_mem[1] = user_id + '0';
                    strcpy(shared_mem + 2, "end chat");
                    break;
                }
                shared_mem[0] = '0';
            }
            usleep(100000);  //delay 
        }
        exit(EXIT_SUCCESS);
    } else {
        // parent or sender processing
        while (1) {
            //input getting from user
            if (fgets(msg, sizeof(msg), stdin) == NULL)
                break;

            //write message to the shared memory
            while (shared_mem[0] == '1') {
                usleep(100000);
            }
           
            shared_mem[0] = '1';  // Set new message flag
            shared_mem[1] = user_id + '0';  // Set sender ID
            strncpy(shared_mem + 2, msg, MAX_TEXT - 2);

            //chat end detection
            if (strncmp(msg, "end chat", 8) == 0) {
                break;
            }
        }
        wait(NULL);
    }

    munmap(shared_mem, SHARED_SIZE);
    close(fd);
    remove(SHARED_FILE);  // cleanup

    return 0;
}