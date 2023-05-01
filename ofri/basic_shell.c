#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_COMMAND_LENGTH 1024

// define a signal handler function that does nothing
void sigint_handler(int signum)
{
	// do nothing
}

void copyStringArray(char **src, char **dest, int start, int end)
{
	int i;
	for (i = start; i <= end; i++)
	{
		dest[i - start] = src[i];
	}
	dest[end - start + 1] = NULL;
}

void executeCommand(char **argv, int argc)
{
	int i = 0;
	while (argv[i] != NULL && strcmp(argv[i], ">") != 0 && strcmp(argv[i], ">>") != 0 && strcmp(argv[i], "|") != 0 && i < argc)
		i++;
	char *dest[MAX_COMMAND_LENGTH / 2 + 1];

	int in_fd = STDIN_FILENO, out_fd = STDOUT_FILENO;
	if (i < argc)
	{
		copyStringArray(argv, dest, i + 1, argc);
		printf("dest list:\n");
		for (int j = 0; j < i; j++)
		{
			printf("%s\n", dest[j]);
		}
		if (strcmp(argv[i], ">") == 0)
		{
			out_fd = open(dest[0], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		}
		else if (strcmp(argv[i], ">>") == 0)
		{
			out_fd = open(dest[0], O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		}
	}
	copyStringArray(argv, dest, 0, i - 1);
	printf("%d \n", i);
	printf("dest list:\n");
	for (int j = 0; j < i; j++)
	{
		printf("%s\n", dest[j]);
	}

	int status;
	// fork child process
	pid_t pid = fork();

	if (pid == 0) // Child process
	{
		if (in_fd != STDIN_FILENO)
		{
			dup2(in_fd, STDIN_FILENO); // Duplicate input file descriptor
			close(in_fd);
		}
		if (out_fd != STDOUT_FILENO)
		{
			dup2(out_fd, STDOUT_FILENO);
			close(out_fd);
		}
		if (execvp(dest[0], dest) == -1)
		{
			printf("An error with execvp: %d\n", errno);
			exit(-1);
		}
	}
	else if (pid < 0)
	{
		printf("An error with fork%d\n", errno);
		exit(-1);
	}
	else // Parent process
	{
		waitpid(pid, &status, 0);
	}
}

int main()
{
	char command[MAX_COMMAND_LENGTH];
	char *argv[MAX_COMMAND_LENGTH / 2 + 1]; // max number of arguments is half of the command length
	int argc;

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
		char *token = strtok(command, " ");
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
		executeCommand(argv, argc);
	}

	return 0;
}
