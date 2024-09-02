#include "Shell.h"

int main()
{
	sem_t *empty = sem_open("empty", O_RDONLY, 0666, BUFFER_SIZE);
	sem_t *full = sem_open("full", O_RDONLY, 0666, 0);
	sem_t *mutex = sem_open("mutex", O_RDONLY, 0666, 1);

	int shm_fd_buffer = shm_open("shared_memory_buffer", O_RDWR, 0666);
	int *buffer = (int *)mmap(0, BUFFER_SIZE * sizeof(int), PROT_READ, MAP_SHARED, shm_fd_buffer, 0);

	int shm_fd_index = shm_open("shared_memory_index", O_RDWR, 0666);
	int *index = (int *)mmap(0, sizeof(int), PROT_READ, MAP_SHARED, shm_fd_index, 0);

	bool flag_delete = false, flag_add = false;

	pid_t consumers_pids[NUMBER_OF_CONSUMERS] = {0};
	int count_consumers = 0;

	while (true)
	{

		sleep(1);
		if (flag_delete || flag_add)
		{
			sleep(1);
			flag_delete = false;
			flag_add = false;
			continue;
		}
		sem_wait(mutex);

		int value_full, value_empty;
		sem_getvalue(full, &value_full);
		// printf("full: %d\n", value_full);
		sem_getvalue(empty, &value_empty);
		// printf("empty: %d\n", value_empty);

		if (*index <= LOWER_BOUND)
		{ // Consumer too fast! -> kill one consumer
			if (count_consumers == 0)
			{
				sem_post(mutex);
				continue;
			}

			if (count_consumers > 0)
			{
				printf("Consumer %d Deleted!\n", consumers_pids[count_consumers - 1]);
				consumers_pids[count_consumers] = 0;
				kill(consumers_pids[count_consumers - 1], SIGKILL);
				count_consumers--;
				flag_delete = true;
			}
		}
		else if (*index >= HIGHER_BOUND)
		{ // Consumer too slow! -> create one consumer
			if (count_consumers == NUMBER_OF_CONSUMERS)
			{
				sem_post(mutex);
				continue;
			}
			pid_t pid = fork();
			if (pid == 0)
			{
				execlp("./consumer", "./consumer", NULL);
			}
			else
			{
				consumers_pids[count_consumers] = pid;
				count_consumers++;
				printf("Consumer %d Created!\n", consumers_pids[count_consumers - 1]);
				flag_add = true;
			}
		}

		sem_post(mutex);
	}

	close(shm_fd_buffer);
	close(shm_fd_index);
	sem_close(full);
	sem_close(empty);
	sem_close(mutex);

	return 0;
}