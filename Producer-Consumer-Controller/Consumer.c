#include "Shell.h"

int main()
{
	sem_t *empty = sem_open("empty", O_RDONLY, 0666, BUFFER_SIZE);
	sem_t *full = sem_open("full", O_RDONLY, 0666, 0);
	sem_t *mutex = sem_open("mutex", O_RDONLY, 0666, 1);

	int shm_fd_buffer = shm_open("shared_memory_buffer", O_RDWR, 0666);
	ftruncate(shm_fd_buffer, BUFFER_SIZE * sizeof(int));
	int *buffer = (int *)mmap(0, BUFFER_SIZE * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd_buffer, 0);

	int shm_fd_index = shm_open("shared_memory_index", O_RDWR, 0666);
	ftruncate(shm_fd_index, sizeof(int));
	int *index = (int *)mmap(0, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd_index, 0);

	srand(time(NULL));

	while (true)
	{
		int value;
		sem_getvalue(full, &value);
		// printf("full: %d\n", value);
		sem_getvalue(empty, &value);
		// printf("empty: %d\n", value);

		sem_wait(full);
		sleep(1);
		sem_wait(mutex);

		if (*index == 0)
		{
			sem_post(mutex);
			sem_post(full);
			continue;
		}

		(*index)--;
		int taken_number = buffer[*index];
		printf("Consumer %d Took Number %d from Buffer at %d\n", getpid(), taken_number, *index);
		printf("New Buffer:\n");
		for (int i = 0; i < *index; i++)
			printf("%d ", buffer[i]);
		printf("\n");

		int sleep_time = rand() % 2 + 1;
		sleep(sleep_time);

		sem_post(mutex);
		sem_post(empty);
	}

	close(shm_fd_buffer);
	close(shm_fd_index);
	sem_close(full);
	sem_close(empty);
	sem_close(mutex);

	return 0;
}