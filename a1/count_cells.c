#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_image(int num_rows, int num_cols,
                int arr[num_rows][num_cols], FILE *fp);


void print_image(int num_rows, int num_cols, int arr[num_rows][num_cols]);

int min(int a, int b);

int max(int a, int b);

void count_neighbour(int row, int col, int num_rows, int num_cols, int arr[num_rows][num_cols], int currColor);

int count_cells(int num_rows, int num_cols, int arr[num_rows][num_cols]);

// Remember to include the function prototype for count_cells


int main(int argc, char **argv) {

    // Print a message to stderr and exit with an argument of 1 if there are
    // not the right number of parameters, or the second argument is not -p
    if (argc > 3 || argc < 2){
      fprintf(stderr, "Usage: count_cells <imagefile.txt> [-p]");
      return 1;
    } else {
      FILE *file = fopen(argv[1], "r");
      if (file == NULL){
        printf("Invalid File!\n");
        return 1;
      }
      int row;
      int col;
      fscanf(file, "%d", &row);
      fscanf(file, "%d", &col);
      int array[row][col];
      read_image(row, col, array, file);
      fclose(file);
      if (argc == 3){
        if (strcmp(argv[2], "-p") == 0){
          int num_cells = count_cells(row, col, array);
          print_image(row, col, array);
          printf("Number of Cells is %d\n", num_cells);
        } else {
          fprintf(stderr, "Usage: count_cells <imagefile.txt> [-p]");
          return 1;
        }
      } else {
        printf("Number of Cells is %d\n", count_cells(row, col, array));
    }
  }

    return 0;
}
