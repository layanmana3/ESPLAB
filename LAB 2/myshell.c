#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "LineParser.h"
#include <signal.h>

int dubguermode = 0;
void execute(cmdLine *pCmdLine)
{
    pid_t childPid;
    childPid = fork();
    if (childPid < 0)
    {
        perror("error while forking");
        exit(EXIT_FAILURE);
    }
    if (childPid == 0) //child process
    {
        if (dubguermode == 1)
        {
            fprintf(stderr, "pid: %d\n executing command:%s\n", getpid(),
                    pCmdLine->arguments[0]);
        }
        if (pCmdLine->inputRedirect != NULL) //task 2 input
        {
            int input_file_descriptor = open(pCmdLine->inputRedirect, O_RDONLY);
            if (input_file_descriptor == -1)
            {
                fprintf(stderr, "error opening file for output redirection");
                _exit(EXIT_FAILURE);
            }
            int input_redirect_status = dup2(input_file_descriptor, 0);
            if (input_redirect_status == -1)
            {
                fprintf(stderr, "error in duplicate");
                _exit(EXIT_FAILURE);
            }
        }
        if (pCmdLine->outputRedirect != NULL)
        {
            int output_file_descriptor = open(pCmdLine->outputRedirect, O_RDONLY | O_WRONLY | O_CREAT, 0777);
            if (output_file_descriptor == -1)
            {
                fprintf(stderr, "error opening file for output redirection");
                _exit(EXIT_FAILURE);
            }
            int output_redirect_status = dup2(output_file_descriptor, 1);
            if (output_redirect_status == -1)
            {
                fprintf(stderr, "error in duplicate ");
                _exit(EXIT_FAILURE);
            }
        }
        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1)
        {
            perror("error while executing");
            freeCmdLines(pCmdLine);
            exit(EXIT_FAILURE);
        }
    }
    else //parent process
    {
        int status;
        if (pCmdLine->blocking)
        {
            if (waitpid(childPid, &status, 0) == -1)
            {
                perror("error in waitpad");
                exit(EXIT_FAILURE);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1 && strncmp("-d", argv[1], 2) == 0) //debugger
    {
        dubguermode = 1;
    }

    while (1)
    {
        int noExc = 0;
        char path[PATH_MAX];
        if (getcwd(path, sizeof(path)) != NULL)
        {
            printf("  %s $ ", path);
        }
        else
        {
            perror("can't get this path");
        }
        char userInput[2048];
        if (fgets(userInput, sizeof(userInput), stdin) == NULL)
        {
            fprintf(stdout, "\n");
            break; 
        }

        //--------------parsing-------------------------
        cmdLine *parsedLine;
        parsedLine = parseCmdLines(userInput); 
        if (strcmp(parsedLine->arguments[0], "quit") == 0)
        {
            exit(EXIT_SUCCESS);
            break;
        }
        if (strcmp(parsedLine->arguments[0], "cd") == 0) 
        {
            if (chdir(parsedLine->arguments[1]) == -1)
                fprintf(stderr, "there is an error while changing working directory-cd shell command");
            noExc = 1;
        }
        if (strcmp(parsedLine->arguments[0], "wakeup") == 0) 
        {
            int proId;
            sscanf(parsedLine->arguments[1], "%d", &proId);
            kill(proId, SIGCONT);
            noExc = 1;
        }
        else if (strcmp(parsedLine->arguments[0], "nuke") == 0)
        {
            int proId;
            sscanf(parsedLine->arguments[1], "%d", &proId);
            kill(proId, SIGTERM);//replacing the SIGSTP, SIGTERM terminating it gracefully.
            noExc = 1;
        }
        if (noExc == 0)
        {
            execute(parsedLine);
        }
        freeCmdLines(parsedLine);
    }
    exit(EXIT_SUCCESS);

    return 1;
}