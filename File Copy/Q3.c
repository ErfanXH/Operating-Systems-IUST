#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

pthread_mutex_t mutex;

bool Finished = false;

void *FromSourceToBuffer(void *arg); // Producer

void *FromBufferToDestination(void *arg); // Consumer

typedef struct
{
    FILE *source;
    FILE *dest;
    int buffer_index;
    int buffer_index_producer;
    int buffer_index_consumer;
    char buffer[BUFFER_SIZE];
} Data;

int main(int argc, char *argv[])
{
    FILE *source = fopen(argv[1], "rb");
    FILE *dest = fopen(argv[2], "wb");

    // set struct Data
    Data data;
    data.source = source;
    data.dest = dest;
    data.buffer_index = 0;
    data.buffer_index_producer = 0;
    data.buffer_index_consumer = 0;

    // initialize mutex
    pthread_mutex_init(&mutex, NULL);

    // create threads
    pthread_t reader, writer;
    pthread_create(&reader, NULL, FromSourceToBuffer, (void *)&data);
    pthread_create(&writer, NULL, FromBufferToDestination, (void *)&data);

    // join threads
    pthread_join(reader, NULL);
    pthread_join(writer, NULL);

    // destroy mutex
    pthread_mutex_destroy(&mutex);

    // close files
    fclose(source);
    fclose(dest);

    return 0;
}

void *FromSourceToBuffer(void *arg)
{
    Data *data = (Data *)arg;
    FILE *source = data->source;
    FILE *dest = data->dest;
    char *buffer = data->buffer;
    // int buffer_index = data->buffer_index;

    while (!feof(data->source))
    {
        // while (data->buffer_index == BUFFER_SIZE - 1) // buffer is full
        //     ;

        pthread_mutex_lock(&mutex);
        // printf("@@Producer@@\n");

        if (data->buffer_index != 0) // buffer is not empty
        {
            pthread_mutex_unlock(&mutex);
            continue;
        }

        data->buffer_index = fread(data->buffer, 1, BUFFER_SIZE, data->source);

        // printf("buffer_index: %d\n", data->buffer_index);
        // printf("buffer[0] = %c\n", data->buffer[0]);
        // printf("buffer[321] = %c\n", data->buffer[321]);
        // printf("buffer: %s\n", data->buffer);

        pthread_mutex_unlock(&mutex);
    }

    Finished = true;
}

void *FromBufferToDestination(void *arg)
{
    Data *data = (Data *)arg;
    FILE *source = data->source;
    FILE *dest = data->dest;
    char *buffer = data->buffer;
    // int buffer_index = data->buffer_index;

    while (true)
    {
        if (Finished && data->buffer_index == 0)
            break;

        pthread_mutex_lock(&mutex);
        // printf("--Consumer--\n");

        if (data->buffer_index == 0) // buffer is empty
        {
            pthread_mutex_unlock(&mutex);
            continue;
        }

        fwrite(data->buffer, 1, data->buffer_index, data->dest);
        data->buffer_index = 0;

        pthread_mutex_unlock(&mutex);

        if (feof(data->source))
            break;
    }
}