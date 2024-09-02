#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

int count_query(char *file, char *query);

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        // Search <File> <Query1> <Query2> ...
        printf("Not Enough Data...");
        return -1;
    }

    int queries_count = argc - 2; // number of queries -> number of process

    // int *array_count = malloc(sizeof(int) * queries_count); // store result of ith process in ith index of array

    char *file = argv[1]; // file

    int sum = 0; // sum of counts

    for (int i = 2; i < queries_count + 2; i++)
    {
        char *query = argv[i];

        pid_t pid = vfork(); // creating child process

        if (pid == 0) // in child process
        {
            int count_i = count_query(file, query);
            printf("<%s>: <%d>\n", argv[i], count_i);
            sum += count_i;
            exit(0);
        }
    }

    // for (int j = 0; j < queries_count; j++)
    //     exit(getpid());

    for (int j = 0; j < queries_count; j++)
    {
        wait(NULL);
    }

    printf("Total Occurances: %d\n", sum);

    return 0;
}

int count_query(char *file, char *query)
{
    FILE *text;
    text = fopen(file, "r");

    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), text))
    {
        int len_line = strlen(line);
        int len_query = strlen(query);

        for (int i = 0; i < len_line;)
        {
            int j = 0;
            int count_char = 0;
            while ((line[i] == query[j]))
            {
                count_char++;
                i++;
                j++;
                if (i == len_line)
                    break;
            }
            if (count_char == len_query)
            {
                count++;
                count_char = 0;
            }
            else
                i++;
        }
    }

    fclose(text);
    return count;
}