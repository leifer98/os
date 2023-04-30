#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_CMD_LEN 1024
#define MAX_ARGS 64
#define MAX_PIPE 2

volatile sig_atomic_t stop = 0;

void handle_sigint(int sig) {
    stop = 1;
}

void run_cmd(char **args, int n_pipes, int *pipes) {
    pid_t pid;
    int fd_in = STDIN_FILENO, fd_out = STDOUT_FILENO, i = 0;
    int is_last_pipe = (n_pipes == 0);

    if (!is_last_pipe) {
        fd_out = pipes[i+1];
    }

    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) { // Child process
        if (n_pipes > 0 && i < n_pipes) { // Has incoming pipe
            close(STDIN_FILENO);
            dup2(fd_in, STDIN_FILENO);
            close(pipes[i]);
        }

        if (!is_last_pipe) { // Has outgoing pipe
            close(STDOUT_FILENO);
            dup2(fd_out, STDOUT_FILENO);
            close(pipes[i+1]);
        }

        if (execvp(args[0], args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }
    else { // Parent process
        if (n_pipes > 0 && i < n_pipes) {
            close(fd_in);
        }

        if (!is_last_pipe) {
            close(fd_out);
        }

        int status;
        while (wait(&status) != pid) {} // Wait for child to terminate
    }
}

int main(int argc, char **argv) {
    char cmd[MAX_CMD_LEN];
    char *args[MAX_ARGS];
    int n_pipes = 0;
    int pipes[MAX_PIPE * 2]; // Max 2 pipes
    int num_args, i, j;

    signal(SIGINT, handle_sigint);

    while (1) {
        printf("stshell> ");
        fflush(stdout);

        if (fgets(cmd, MAX_CMD_LEN, stdin) == NULL) {
            break;
        }

        num_args = 0;
        args[num_args] = strtok(cmd, " \t\n");

        while (args[num_args] != NULL) {
            num_args++;

            if (num_args >= MAX_ARGS) {
                break;
            }

            args[num_args] = strtok(NULL, " \t\n");
        }

        if (num_args == 0) {
            continue;
        }

        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        n_pipes = 0;
        pipes[n_pipes] = -1;

        for (i = 0; i < num_args; i++) {
            if (strcmp(args[i], "|") == 0) {
                if (n_pipes < MAX_PIPE) {
                    pipe(pipes + n_pipes * 2);
                    n_pipes++;
                    pipes[n_pipes * 2 - 1] = -1;
                }
                else {
                    printf("Error: too many pipes\n");
                    break;
                }
            }
            else if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0) {
                char *filename = args[i+1];
                int fd_out;

                if (filename == NULL) {
                printf("Error: missing filename after %s\n", args[i]);
                break;
            }

            if (strcmp(args[i], ">") == 0) {
                fd_out = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            }
            else { // ">>"
                fd_out = open(filename, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            }

            if (fd_out == -1) {
                perror("open");
                break;
            }

            args[i] = NULL;
            args[i+1] = NULL;

            pid_t pid = fork();

            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0) { // Child process
                if (dup2(fd_out, STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }

                run_cmd(args, n_pipes, pipes);

                if (close(fd_out) == -1) {
                    perror("close");
                    exit(EXIT_FAILURE);
                }

                exit(EXIT_SUCCESS);
            }
            else { // Parent process
                if (wait(NULL) == -1) {
                    perror("wait");
                    exit(EXIT_FAILURE);
                }

                if (close(fd_out) == -1) {
                    perror("close");
                    exit(EXIT_FAILURE);
                }

                break;
            }
        }
    }

    if (i == num_args) { // No error occurred during parsing
        int start = 0;
        for (j = 0; j <= n_pipes; j++) {
            int end = (j == n_pipes) ? num_args : start + (pipes[j * 2 + 1] == -1 ? 1 : 0);

            args[end] = NULL;

            run_cmd(args + start, j, pipes);

            if (stop) {
                stop = 0;
                break;
            }

            start = end + 1;
        }

        for (j = 0; j < n_pipes * 2; j++) {
            if (pipes[j] != -1) {
                if (close(pipes[j]) == -1) {
                    perror("close");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}

return 0;
}