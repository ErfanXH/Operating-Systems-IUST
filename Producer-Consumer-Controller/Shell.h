#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>

#define BUFFER_SIZE 100
#define LOWER_BOUND 14
#define HIGHER_BOUND 16
#define NUMBER_OF_CONSUMERS 5
#define MIN_CONSUMERS 3