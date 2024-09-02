#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

struct search_parameter
{
    char *file;
    char *query;
    pthread_t thread;
};

typedef struct search_parameter Search_Parameter;

void *search(void *args);

int is_valid(DIR *file);

int main(int argc, char *argv[])
{
    // search <start_dir> <search_query>
    if (argc != 3)
    {
        printf("Incorrect Number of Data!\n");
        return -1;
    }

    char directory[strlen(argv[1])]; // = argv[1];
    char query[strlen(argv[2])];     // = argv[2];

    strcpy(directory, argv[1]);
    strcpy(query, argv[2]);

    Search_Parameter par;
    // par.file = directory;
    // par.query = query;

    par.file = malloc(strlen(directory) + 1);
    par.query = malloc(strlen(query) + 1);

    strcpy(par.file, directory);
    strcpy(par.query, query);

    // strcpy(par.file, argv[1]);
    // strcpy(par.query, argv[2]);

    Search_Parameter *par_ptr = &par;

    // pthread_create(&thread, NULL, search, par_ptr);

    search((void *)par_ptr);

    return 0;
}

void *search(void *args)
{
    Search_Parameter *par = (Search_Parameter *)args;
    char *file = par->file;
    char *query = par->query;
    pthread_t thread = par->thread;

    DIR *dir = opendir(file);

    if (is_valid(dir) == -1)
    {
        printf("Invalid Directory!\n");
        pthread_exit(NULL);
    }
    else
    {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            char *directory = entry->d_name;

            if (strcmp(directory, ".") == 0 || strcmp(directory, "..") == 0) // invalid path
                continue;

            char *file_next = (char *)malloc((1 + strlen(file) + strlen(directory)) * sizeof(char));
            file_next[0] = '\0';
            strcat(file_next, file);
            strcat(file_next, directory);

            // List of threads
            pthread_t threads[1000]; // Max 1000 threads

            // List of parametrs(structs)
            Search_Parameter *params = (Search_Parameter *)malloc(1000 * sizeof(Search_Parameter));

            // 1 File -> 1 Parameter -> 1 Thread

            int count = 0;
            if (entry->d_type == DT_DIR)
            {
                strcat(file_next, "/");

                params[count].file = malloc(strlen(file_next) + 1);
                strcpy(params[count].file, file_next);

                params[count].query = malloc(strlen(query) + 1);
                strcpy(params[count].query, query);

                params[count].thread = threads[count];
                pthread_create(&threads[count], NULL, search, &params[count]);

                count += 1;
            }
            else
            {
                if (strcmp(directory, query) == 0)
                    printf("[+] Found: \n%s\n", file_next);
            }

            for (int i = 0; i < count; i++)
                pthread_join(threads[i], NULL);

            // free(file_next);
            free(params);
        }
    }

    // closedir(dir);
    // free(args);
    //  pthread_exit(NULL);
}

int is_valid(DIR *file)
{
    if (file == NULL)
        return -1;

    return 1;
}