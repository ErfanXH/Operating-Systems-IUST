#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "constants.h"
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>

int main()
{
	// int M1[M][N] = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 0, 1, 2}};
	// int M2[N][K] = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 0}, {1, 2, 3, 4, 5}, {6, 7, 8, 9, 0}};
	// int M3[M][K] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
	srand(time(NULL));

	int M, N, K;

	printf("Enter M N K:\n");
	scanf("%d %d %d", &M, &N, &K);

	int M1[M][N];
	int M2[N][K];
	int M3[M][K];

	printf("M1:\n");
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N; j++)
		{
			M1[i][j] = rand() % 10;
			printf("%d ", M1[i][j]);
		}
		printf("\n");
	}

	printf("M2:\n");
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < K; j++)
		{
			M2[i][j] = rand() % 10;
			printf("%d ", M2[i][j]);
		}
		printf("\n");
	}

	for (int i = 0; i < M; i++)
		for (int j = 0; j < K; j++)
			M3[i][j] = 0;

	// int size = SIZE * sizeof(int);
	int size = M * K * sizeof(int);

	int shm_fd = shm_open("SharedMemory", O_CREAT | O_RDWR, 0666);

	ftruncate(shm_fd, size);

	int *ptr = (int *)mmap(0, size, PROT_WRITE, MAP_SHARED, shm_fd, 0);

	pid_t pid;
	for (int i = 0; i < M; i++)
	{
		pid = fork();
		if (pid == 0)
		{
			printf("Calculating Row: %d\n", i);
			for (int j = 0; j < K; j++)
				for (int k = 0; k < N; k++)
					M3[i][j] += M1[i][k] * M2[k][j];

			for (int j = 0; j < K; j++)
				ptr[i * K + j] = M3[i][j];

			exit(0);
		}
	}

	for (int i = 0; i < M; i++)
		wait(NULL);

	for (int i = 0; i < M; i++)
		for (int j = 0; j < K; j++)
			M3[i][j] = ptr[i * K + j];

	printf("M3:\n");
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < K; j++)
			printf("%d ", M3[i][j]);
		printf("\n");
	}

	munmap(ptr, size);
	close(shm_fd);

	return 0;
}