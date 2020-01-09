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

struct ships{
    char* s0_coordinates;
    char* s1_coordinates;
    char* s2_coordinates;
    char* s3_coordinates;
    char* s4_coordinates;
};

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
}

int place_boat(int boat, char row, int c, char orient, int key){
  row = tolower(row);
  orient = tolower(orient);
  if (c < 1 || c > 11) return 0;
  if (row < 97 || row > 123) return 0;
  if (orient != 'l' && orient != 'r' && orient != 'u' && orient != 'd') return 0;
  int r = row % 97;
  c -= 1;
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
  int empty = (*(data + c * 11 + r) == '-');
  if (!empty) return 0;
  *(data + c * 11 + r) = 'O'; //place boat down
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

  int column;
  char row, orient;
  char input[20];
  int i;
  for (i = 0; i < 5; i++){
    printf("Now placing Boat %d\n", i + 1);
    printf("Please input a row (char), a column (int), and an orientation (l, r, u, d) separated by spaces:\n");
    fgets(input, 20, stdin);
    *strchr(input, '\n') = 0;
    sscanf(input, "%c %d %c", &row, &column, &orient);
    while (!place_boat(i + 1, row, column, orient, BOARD1_KEY)){
      printf("The values you inputted were not valid. Please try again:\n");
      fgets(input, 20, stdin);
      *strchr(input, '\n') = 0;
      sscanf(input, "%c %d %c", &row, &column, &orient);
    }
    display_board(BOARD1_KEY);
  }
  return 0;
}
