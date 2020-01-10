#include "battleship.h"
#include <ctype.h>

// union semun {
//   int              val;    /* Value for SETVAL */
//   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
//   unsigned short  *array;  /* Array for GETALL, SETALL */
//   struct seminfo  *__buf;  /* Buffer for IPC_INFO
//                               (Linux-specific) */
// };

void create_board(int key){
  int shmd;
  char *data;
  shmd = shmget(key, BOARD_SIZE, IPC_CREAT | 0644);
  if (shmd == -1){
    printf("%s\n", strerror(errno));
  }
  data = shmat(shmd, 0, 0);
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }
  else {
    int i;
    for (i = 0; i < 10; i++){
      strcpy(data + (11 * i), "----------\n");
    }
    //printf("%s\n", data);
    shmdt(data);
    if (errno != 0){
      printf("%s\n", strerror(errno));
    }
  }
}

void display_board(int key){
    int shmd;
    char *data;
    shmd = shmget(key, BOARD_SIZE, IPC_CREAT | 0640);
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

int check_coord(int row, char column){ //check if coordinates are out of bounds
  column = tolower(column);
  if (row < 1 || row > 11) return 0;
  if (column < 97 || column > 123) return 0;
  return 1;
}

int place_boat(int boat, int row, char column, char orient, int key){
  //check coordinates
  column = tolower(column);
  if (!check_coord(row, column)) return 0;
  int coll = column % 97;
  row -= 1;
  //check orientation
  orient = tolower(orient);
  if (orient != 'l' && orient != 'r' && orient != 'u' && orient != 'd') return 0;
  //access shared memory for board
  int shmd = shmget(key, BOARD_SIZE, 0);
  if (shmd == -1){
    printf("%s\n", strerror(errno));
  }
  char * data = shmat(shmd, 0, 0);
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }
  //add coordinates
  int empty = (*(data + row * 11 + coll) == '-');
  if (!empty) return 0;
  *(data + row * 11 + coll) = 'O'; //place boat down
  shmdt(data);
  return 1;
}

int main(int argc, char const *argv[]) {
  int semd;
  semd = semget(SEM_KEY, 1, IPC_CREAT | 0644);
  if (semd != -1){
    union semun us;
    us.val = 1;
    semctl(semd, 0, SETVAL, us);
  }
  else {
    printf("%s\n", strerror(errno));
  }
  printf("semaphore created\n");
  create_board(BOARD1_KEY);
  create_board(BOARD2_KEY);
  printf("shared memory created\n\n");

  //printing initial board for player 1
  printf("PLAYER 1\n\n");
  printf("My Board\n");
  display_board(BOARD1_KEY);
  printf("Opponent Board\n");
  display_board(BOARD2_KEY);
  printf("\n");

  //printing initial board for player 2
  printf("PLAYER 2\n\n");
  printf("My Board\n");
  display_board(BOARD2_KEY);
  printf("Opponent Board\n");
  display_board(BOARD1_KEY);
  printf("\n");

  int row;
  char column, orient;
  char input[20];
  int i;
  for (i = 1; i <= 5; i++){
    printf("Now placing Boat %d\n", i);
    printf("Please input a row (int), a column (char), and an orientation (l, r, u, d) separated by spaces:\n");
    fgets(input, 20, stdin);
    *strchr(input, '\n') = 0;
    sscanf(input, "%d %c %c", &row, &column, &orient);
    while (!place_boat(i, row, column, orient, BOARD1_KEY)){
      printf("The values you inputted were not valid. Please try again:\n");
      fgets(input, 20, stdin);
      *strchr(input, '\n') = 0;
      sscanf(input, "%d %c %c", &row, &column, &orient);
    }
    display_board(BOARD1_KEY);
  }
  printf("FINISHED PLACING BOATS\n");
  return 0;
}
