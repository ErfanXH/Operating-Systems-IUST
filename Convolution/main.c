// importing necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/sysinfo.h>

int main()
{
  // getting original matrix as input
  int rows;
  int cols;
  printf("Enter number of rows of original array\n");
  scanf("%d", &rows);
  printf("Enter number of columns of original array\n");
  scanf("%d", &cols);
  int **mat = (int **)malloc(rows * sizeof(int *));
  for (int i = 0; i < rows; i++)
    mat[i] = (int *)malloc(cols * sizeof(int));
  printf("Enter the matrix elements\n");
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      scanf("%d", &mat[i][j]);
    }
  }

  // getting window as input
  int w_rows;
  int w_cols;
  printf("Enter number of rows of window array\n");
  scanf("%d", &w_rows);
  printf("Enter number of columns of window array\n");
  scanf("%d", &w_cols);
  int **window = (int **)malloc(w_rows * sizeof(int *));
  for (int i = 0; i < w_rows; i++)
    window[i] = (int *)malloc(w_cols * sizeof(int));
  printf("Enter the window matrix elements\n");
  for (int i = 0; i < w_rows; i++)
  {
    for (int j = 0; j < w_cols; j++)
    {
      scanf("%d", &window[i][j]);
    }
  }

  // calculating the time, taken by the operation.
  clock_t start_t, end_t;
  double total_t;
  start_t = clock();

  // here begins your code

  int num_of_processes = get_nprocs_conf();                     // number of processes, usually equals number of cores, which is in most cases 8.
  int result_rows = rows - w_rows + 1;                          // The number of rows in result matrix based on number of rows in original matrix and the window. note that we are using convolution with mode='valid'.
  int result_cols = cols - w_cols + 1;                          // The number of columns in result matrix based on number of columns in original matrix and the window. note that we are using convolution with mode='valid'.
  int row_per_proc = result_rows / num_of_processes;            // the number of rows in result matrix that every process take care of. Use variables defined above.
  int **result = (int **)malloc((result_rows) * sizeof(int *)); // allocating memory for result matrix.
  for (int i = 0; i < (result_rows); i++)
    result[i] = (int *)malloc((result_cols) * sizeof(int)); // allocating memory for each row of result matrix. each row has number of elements equal to number of columns in result matrix.
  int current_processor = 0;
  int rows_done = 0;
  while (rows_done < result_rows) // condition on when to finish, which means when the result matrix is ready.
  {
    if (row_per_proc == 0)
      row_per_proc = 1;

    int row_start = current_processor * row_per_proc; // the row index in result matrix that this process should start from.

    int row_end = row_start + row_per_proc; // the row index in result matrix that this process should end at.

    rows_done = row_end;

    pid_t pid = vfork();

    if (pid == 0)
    {
      //  Convolution
      for (int i = row_start; i < row_end; i++)
      {
        if (i >= result_rows)
          break;
        for (int j = 0; j < result_cols; j++)
        {
          int sum = 0;
          for (int k = 0; k < w_rows; k++)
          {
            for (int l = 0; l < w_cols; l++)
            {
              sum += mat[i + k][j + l] * window[k][l];
            }
          }
          result[i][j] = sum;
        }
      }

      current_processor++;

      exit(0);
    }
  }
  // end section

  end_t = clock();
  total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
  printf("Total time taken: %f\n", total_t);
  for (int i = 0; i < rows - w_rows + 1; i++)
  {
    for (int j = 0; j < cols - w_cols + 1; j++)
    {
      printf("%d ", result[i][j]);
    }
    printf("\n");
  }

  for (int i = 0; i < rows; i++)
    free(mat[i]);
  for (int i = 0; i < w_rows; i++)
    free(window[i]);
  for (int i = 0; i < (rows - w_rows + 1); i++)
    free(result[i]);
  free(mat);
  free(window);
  free(result);
}
