#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include <time.h>

#define NUMBER_OF_RESOURCES 5
#define NUMBER_OF_THREADS 10

typedef struct
{
    int resources[NUMBER_OF_RESOURCES];
    sem_t availableResources;
    pthread_mutex_t poolMutex;
} ResourceManager;

ResourceManager resource_manager;

typedef struct
{
    int thread_id;
    int number_tasks;
} ThreadTask;

void *Func(void *arg);

int main(int argc, char *argv[])
{
    // seed for random
    srand(time(0));

    // initialize resources
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        resource_manager.resources[i] = -1;

    // initialize semaphore
    sem_init(&resource_manager.availableResources, 0, NUMBER_OF_RESOURCES);

    // initialize mutex
    pthread_mutex_init(&resource_manager.poolMutex, NULL);

    // create threads
    pthread_t threads[NUMBER_OF_THREADS];
    ThreadTask *thread_tasks = malloc(sizeof(ThreadTask) * NUMBER_OF_THREADS);
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        thread_tasks[i].thread_id = i;
        thread_tasks[i].number_tasks = (rand() % 5) + 1;
        pthread_create(&threads[i], NULL, Func, (void *)&thread_tasks[i]);
    }

    // join threads
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_join(threads[i], NULL);

    // destroy semaphore
    sem_destroy(&resource_manager.availableResources);

    // destroy mutex
    pthread_mutex_destroy(&resource_manager.poolMutex);

    return 0;
}

void *Func(void *arg)
{
    ThreadTask *thread_task = (ThreadTask *)arg;
    int thread_id = thread_task->thread_id;
    int number_tasks = thread_task->number_tasks;

    while (number_tasks > 0)
    {
        // semaphore :-> common between all threads
        sem_wait(&resource_manager.availableResources);

        // lock mutex to make sure the free resource is not taken by another thread at the same time
        pthread_mutex_lock(&resource_manager.poolMutex);

        // get free resource
        int resource_id = rand() % NUMBER_OF_RESOURCES;
        while (resource_manager.resources[resource_id] != -1)
            resource_id = rand() % NUMBER_OF_RESOURCES;
        resource_manager.resources[resource_id] = thread_id;

        pthread_mutex_unlock(&resource_manager.poolMutex);

        // operation of task : 1 to 3 seconds sleep
        printf("Thread <%d> is performing work with resource <%d>\n", thread_id, resource_id);
        sleep(rand() % 3 + 1);

        // lock mutex to make sure other threads are not trying to release the same resource or can take the same resource at the same time
        pthread_mutex_lock(&resource_manager.poolMutex);

        // release taken resource
        resource_manager.resources[resource_id] = -1;

        pthread_mutex_unlock(&resource_manager.poolMutex);

        // printf("Thread <%d> released resource <%d>\n", thread_id, resource_id);

        // decrement number of tasks left
        number_tasks--;

        sem_post(&resource_manager.availableResources);
    }

    return NULL;
}