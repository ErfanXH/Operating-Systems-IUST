#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define BUFFER_SIZE 5

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
	*index = 0;

	munmap(buffer, BUFFER_SIZE * sizeof(int));
	munmap(index, sizeof(int));
	sem_close(full);
	sem_close(empty);
	sem_close(mutex);
	sem_unlink("empty");
	sem_unlink("full");
	sem_unlink("mutex");
	close(shm_fd_buffer);
	close(shm_fd_index);

	return 0;
}