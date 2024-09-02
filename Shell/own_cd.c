#include "shell.h"

/**
 * own_cd - changes the working dir of the current shell executon env
 * @args: target directory
 *
 * Return: -1 one success, 0 otherwise.
 */
int own_cd(char **args)
{
	// TODO
	if (chdir(args[1]) != 0) // success and directory changed
	{
		printf("Faild to change directory!\n");
		return 0;
	}

	return -1; // failed
			   // return (-1);
}
