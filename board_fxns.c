#include "battleship.h"

int check_coord(int row, char column){ //check if coordinates are out of bounds
  column = tolower(column);
  if (row < 1 || row > 11) return 0;
  if (column < 97 || column > 123) return 0;
  return 1;
}

void display_board(int key){
    int shmd;
    char *data;
    shmd = shmget(key, BOARD_SIZE, 0);
    if (shmd == -1){
      printf("%s\n", strerror(errno));
    }
    data = shmat(shmd, 0, 0);
    if (errno != 0){
      printf("%s\n", strerror(errno));
    } else{
      printf("    A B C D E F G H I J\n");
      int i;
      for (i = 0; i < 10; i++) {
        if (i == 9) {
          printf(" %d", i + 1);
        } else {
          printf("  %d", i + 1);
        }
        int j;
        for (j = 0; j < 11; j++) {
          printf(" %c", *(data + i * 11 + j));
        }
      }
      //printf("%s\n",data);
    }
    shmdt(data);
}
