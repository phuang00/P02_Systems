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

int board_filled(int key){ //check if board is filled or not
  //access shared memory for board
  int shmd = shmget(key, BOARD_SIZE, 0);
  if (shmd == -1){
    errno = 0;
    return 0;
  }
  char * data = shmat(shmd, 0, 0);
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }
  int one = 0;
  int two = 0;
  int three = 0;
  int four = 0;
  int five = 0;
  int i;
  for (i = 0; i < strlen(data); i++){
    if (data[i] == '1') one++;
    if (data[i] == '2') two++;
    if (data[i] == '3') three++;
    if (data[i] == '4') four++;
    if (data[i] == '5') five++;
  }
  if (one == 1 && two == 2 && three == 3 & four == 4 && five == 5) return 1;
  return 0;
}
