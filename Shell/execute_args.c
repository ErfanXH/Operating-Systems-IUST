#include <unistd.h>
#include "shell.h"

int execute_args(char **args)
{
	pid_t pid;
	int status;
	char *builtin_func_list[] = {
		// you should add something here
		"exit",
		"cd"};
	int (*builtin_func[])(char **) = {
		// you should add something here
		&own_exit,
		&own_cd};

	long unsigned int i = 0;
	for (; i < sizeof(builtin_func_list) / sizeof(char *); i++)
	{
		/* if there is a match execute the builtin command */
		if (strcmp(args[0], builtin_func_list[i]) == 0)
		{
			return ((*builtin_func[i])(args));
		}
	}

	// TODO
	pid = fork();

	if (pid == 0) // child
	{
		execvp(args[0], args);
		exit(0);
	}

	else // parent
	{
		wait(NULL);
	}

	return (-1); // this status code means everything is ok
}