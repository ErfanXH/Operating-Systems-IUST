#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>

int mypipe[2];

sem_t sem;

int main()
{
	sem_init(&sem, 0, 0);

	pipe(mypipe);

	int size = 0;
	FILE *file = fopen("Numbers.txt", "r");

	double num;
	while (fscanf(file, "%lf", &num) == 1)
	{
		size++;
	}

	fclose(file);

	double numbers[size]; // = malloc(sizeof(double) * size);

	file = fopen("Numbers.txt", "r");

	int index = 0;
	while (fscanf(file, "%lf", &numbers[index]) == 1)
	{
		index++;
	}

	fclose(file);

	pid_t pid;

	pid = fork();
	if (pid == 0)
	{
		// Avergae
		double sum = 0;
		for (int i = 0; i < size; i++)
		{
			sum += numbers[i];
		}
		double avergae = sum / size;
		write(mypipe[1], &avergae, sizeof(double));
		sem_post(&sem);
	}

	pid = fork();
	if (pid == 0)
	{
		// Min
		sem_wait(&sem);
		double Min = numbers[0];
		for (int i = 1; i < size; i++)
		{
			if (numbers[i] < Min)
			{
				Min = numbers[i];
			}
		}
		write(mypipe[1], &Min, sizeof(double));
		sem_post(&sem);
	}

	if (pid > 0)
	{
		double Avergae;
		read(mypipe[0], &Avergae, sizeof(double));
		printf("Average = %lf\n", Avergae);
		double Min;
		read(mypipe[0], &Min, sizeof(double));
		printf("Min = %lf\n", Min);
	}

	sem_destroy(&sem);

	return 0;
}