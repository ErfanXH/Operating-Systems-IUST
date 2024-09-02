#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
	int cnt = 0;
	for (int i = 0; i < argc; i++)
	{
		if (argv[i][0] == '/')
			cnt++;

		// printf("argv[%d]: %s\n", i, argv[i]);
	}
	cnt++;

	char *commands[cnt];
	for (int i = 0; i < cnt; i++)
		commands[i] = (char *)malloc(100 * sizeof(char));

	int enumerator = 0;

	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '/')
		{
			if (i != 1)
				enumerator++;

			continue;
		}

		if (i != 1 && argv[i - 1][0] != '/')
		{
			int len = strlen(commands[enumerator]);
			commands[enumerator][len] = ' ';
			commands[enumerator][len + 1] = '\0';
		}

		strcat(commands[enumerator], argv[i]);
	}

	for (int i = 0; i <= enumerator; i++)
	{
		printf("commands[%d]: %s\n", i, commands[i]);
	}

	// commands array contains all commands now!
	// number of commands = enumerator + 1

	int my_pipe[2];
	int pipe_in = 0;

	for (int i = 0; i <= enumerator; i++)
	{
		pipe(my_pipe);

		pid_t pid = fork();

		if (pid == 0)
		{

			dup2(pipe_in, 0);

			if (i != enumerator)
			{
				dup2(my_pipe[1], 1);
			}

			close(my_pipe[0]);
			close(my_pipe[1]);

			// printf("commands[%d]: %s\n", i, commands[i]);

			char *commands_splitted[10];
			int len_commands = 0;
			commands_splitted[0] = strtok(commands[i], ",");
			for (int j = 1; j < 10; j++)
			{
				commands_splitted[j] = strtok(NULL, ",");
				if (commands_splitted[j] == NULL)
					break;
				len_commands = j;
			}

			// for (int z = 0; z <= len_commands; z++)
			// printf("commands_splitted[%d]: %s\n", z, commands_splitted[z]);

			for (int j = 0; j <= len_commands; j++)
			{
				char *splitted[10];
				int len_splitted = 0;
				splitted[0] = strtok(commands_splitted[j], " ");
				for (int k = 1; k < 10; k++)
				{
					splitted[k] = strtok(NULL, " ");
					if (splitted[k] == NULL)
						break;
					len_splitted = k;
				}

				splitted[len_splitted + 1] = NULL; // '\0'

				// for (int k = 0; k <= len_splitted; k++)
				// printf("splitted[%d]: %s\n", k, splitted[k]);

				execvp(splitted[0], splitted);
			}
		}
		else
		{
			close(my_pipe[1]);

			if (i != enumerator)
				pipe_in = my_pipe[0];

			else
				close(my_pipe[0]);
		}
	}

	for (int i = 0; i <= enumerator; i++)
		wait(NULL);

	return 0;
}
