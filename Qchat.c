#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <mqueue.h>

#define MAX_TEXT 80

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <1 or 2>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    mqd_t mq_send_fd, mq_recv_fd;
    struct mq_attr attr = {0, 10, MAX_TEXT, 0};
    char send_name[20], recv_name[20];
    char msg[MAX_TEXT];

    if (strcmp(argv[1], "1") == 0) {
        printf("User 1 starting...\n");
        strcpy(send_name, "/chat1to2");
        strcpy(recv_name, "/chat2to1");
    } else if (strcmp(argv[1], "2") == 0) {
        printf("User 2 starting...\n");
        strcpy(send_name, "/chat2to1");
        strcpy(recv_name, "/chat1to2");
    } else {
        fprintf(stderr, "Invalid argument.\n");
        exit(EXIT_FAILURE);
    }

    mq_send_fd = mq_open(send_name, O_WRONLY | O_CREAT, 0666, &attr);
    mq_recv_fd = mq_open(recv_name, O_RDONLY | O_CREAT, 0666, &attr);
    if (mq_send_fd == (mqd_t)-1 || mq_recv_fd == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    pid_t child = fork();
    if (child == 0) {
        //2nd user
        while (1) {
            memset(msg, 0, sizeof(msg));
            if (mq_receive(mq_recv_fd, msg, MAX_TEXT, NULL) == -1) {
                perror("mq_receive");
                break;
            }
            printf("Received: %s", msg);
            fflush(stdout);
            if (strncmp(msg, "end chat", 8) == 0)
                break;
        }
        exit(EXIT_SUCCESS);
    } else {
        //1st user
        while (1) {
            //printf("You: ");
            fflush(stdout);
            if (fgets(msg, sizeof(msg), stdin) == NULL)
                break;
            if (mq_send(mq_send_fd, msg, strlen(msg) + 1, 0) == -1) {
                perror("mq_send");
                break;
            }
            if (strncmp(msg, "end chat", 8) == 0)
                break;
        }
        wait(NULL);
    }

    mq_close(mq_send_fd);
    mq_close(mq_recv_fd);
    mq_unlink(send_name);
    mq_unlink(recv_name);

    return 0;
}
