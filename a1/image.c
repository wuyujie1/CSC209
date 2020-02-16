#include <stdio.h>

/* Reads the image from the open file fp into the two-dimensional array arr
 * num_rows and num_cols specify the dimensions of arr
 */
void read_image(int num_rows, int num_cols,
                int arr[num_rows][num_cols], FILE *fp) {
    for (int row = 0; row < num_rows; row++){
      for (int col = 0; col < num_cols; col++){
        fscanf(fp, "%d", &arr[row][col]);}
      }
}

/* Print to standard output the contents of the array arr */
void print_image(int num_rows, int num_cols, int arr[num_rows][num_cols]) {
  for (int row = 0; row < num_rows; row++){
    for (int col = 0; col < num_cols + 1; col++){
      if (col == num_cols){
        printf("\n");
      } else{
        printf("%d ", arr[row][col]);
      }
    }
  }
}

int min(int a, int b){
  return a < b? a:b;
}

int max(int a, int b){
  return a > b? a:b;
}
/* TODO: Write the count_cells function */

void count_neighbour(int row, int col, int num_rows, int num_cols, int arr[num_rows][num_cols], int currColor){
  if (arr[row][max(0, col - 1)] == currColor){    //check left
    arr[row][max(0, col - 1)] = 0;    //whiten
    count_neighbour(row, max(0, col - 1), num_rows, num_cols, arr, currColor); //recursive check
  }

  if (arr[row][min(num_cols - 1, col + 1)] == currColor){   //check right
    arr[row][min(num_cols - 1, col + 1)] = 0;
    count_neighbour(row, min(num_cols - 1, col + 1), num_rows, num_cols, arr, currColor);
  }

  if (arr[max(0, row - 1)][col] == currColor){    //check top
    arr[max(0, row - 1)][col] = 0;
    count_neighbour(max(0, row - 1), col, num_rows, num_cols, arr, currColor);
  }

  if (arr[min(num_rows - 1, row + 1)][col] == currColor){   //check bot
    arr[min(num_rows - 1, row + 1)][col] = 0;
    count_neighbour(min(num_rows - 1, row + 1), col, num_rows, num_cols, arr, currColor);
  }
}

int count_cells(int num_rows, int num_cols, int arr[num_rows][num_cols]){
  int num_cells = 0;
  for (int i = 0; i < num_rows; i++){
    for (int j = 0; j < num_cols; j++){
      if (arr[i][j] != 0){      //cell found
        int currC = arr[i][j];
        arr[i][j] = 0;      //whiten the cell first
        num_cells+= 1;
        count_neighbour(i, j, num_rows, num_cols, arr, currC);  //eliminate all adjacent non-white pixel
      }
    }
  }
  return num_cells;
}
