#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#include <unistd.h>

// semaphore declarations
sem_t sem;
sem_t sem1;
sem_t sem2;

void *p1(void *arg)
{
	sem_wait(&sem1);
	printf("F\n");
	sem_post(&sem2);
	sem_wait(&sem1);
	usleep(1000);
	sem_wait(&sem);
	printf("E\n");
	sem_post(&sem);
	sem_wait(&sem);
	printf("G\n");
	sem_post(&sem);
}

void *p2(void *arg)
{
	printf("A\n");
	sem_post(&sem1);
	sem_wait(&sem2);
	printf("C\n");
	sem_post(&sem1);
	usleep(1000);
	sem_wait(&sem);
	printf("B\n");
	sem_post(&sem);
}

int main()
{
	// semaphore initializations
	sem_init(&sem, 0, 1);
	sem_init(&sem1, 0, 0);
	sem_init(&sem2, 0, 0);

	// thread creations
	pthread_t t1, t2;
	pthread_create(&t1, NULL, p1, NULL);
	pthread_create(&t2, NULL, p2, NULL);

	// thread joining
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	// semaphore destruction
	sem_destroy(&sem);
	sem_destroy(&sem1);
	sem_destroy(&sem2);

	return 0;
}