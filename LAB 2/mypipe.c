#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int fd[2]; //fd[0]->read, fd[1]->write
    int pid;

    char message[] = "hello";
    char buffer[50];

    if (pipe(fd) == -1)
    {
        perror("error while creating a pipe");
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if (pid == -1)
    {
        perror("error while creating a fork");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) //parent
    {
        close(fd[1]);
        read(fd[0], buffer, sizeof(buffer));
        close(fd[0]);
        //to check if we got the message
        //printf("%s\n",buffer);
        exit(EXIT_SUCCESS);
    }
    else //child
    {
        close(fd[0]);
        write(fd[1], message, strlen(message));
        close(fd[1]);
        exit(EXIT_SUCCESS);
    }

    return 0;
}