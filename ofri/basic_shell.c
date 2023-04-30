#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_COMMAND_LENGTH 1024

// define a signal handler function that does nothing
void sigint_handler(int signum) {
    // do nothing
}

int main()
{
    char command[MAX_COMMAND_LENGTH];
    char* argv[MAX_COMMAND_LENGTH / 2 + 1]; // max number of arguments is half of the command length
    int argc, status;

    // register signal handler for SIGINT (Ctrl-C)
    signal(SIGINT, sigint_handler);

    while (1)
    {
        printf("stshell: ");
        fflush(stdout);

        // read input command
        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL)
        {
            // handle read error
            fprintf(stderr, "Error reading command\n");
            continue;
        }

        // replace newline character with null terminator
        command[strcspn(command, "\n")] = '\0';

        // check if command is empty
        if (strlen(command) == 0)
        {
            continue;
        }

        // parse command
        argc = 0;
        char* token = strtok(command, " ");
        while (token != NULL)
        {
            argv[argc] = token;
            argc++;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL;

        // execute command
        if (strcmp(argv[0], "exit") == 0)
        {
            // exit shell
            exit(0);
        }
        else
        {
            // fork child process
            pid_t pid = fork();

            if (pid == -1)
            {
                // handle fork error
                fprintf(stderr, "Error forking child process\n");
                continue;
            }
            else if (pid == 0)
            {
                // execute command in child process
                execvp(argv[0], argv);
                fprintf(stderr, "Error executing command\n");
                exit(1);
            }
            else
            {
                // wait for child process to finish
                waitpid(pid, &status, 0);
            }
        }
    }

    return 0;
}
