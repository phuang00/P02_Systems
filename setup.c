#include "battleship.h"

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

void display_initBoard(int key){
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

int process_coord(char * input){ //return boolean if coordinate is valid
  if (strlen(input) >= 2){
    char letter = input[0];
    char number = input[1];
    printf("letter: %c\n", letter);
    printf("number: %c\n", number);
    return 1;
  }
  //int boat, char * coord, int orientation
  return 0;
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
  display_initBoard(BOARD1_KEY);
  printf("Opponent Board\n");
  display_initBoard(BOARD2_KEY);
  printf("\n");

  //printing initial board for player 2
  printf("PLAYER 2\n\n");
  printf("My Board\n");
  display_initBoard(BOARD2_KEY);
  printf("Opponent Board\n");
  display_initBoard(BOARD1_KEY);
  printf("\n");

  int row;
  char column, orient;
  char input[20];
  fgets(input, 20, stdin);
  *strchr(input, '\n') = 0;
  printf("INPUT: %s\n", input);
  //sscanf(input, "%s %c", coord, &orient);
  //printf("COORD: %s\nORIENT: %c\n", coord, orient);
  /*
  fgets
  - ask for ship number
  - ask for coordinate
  - ask for orientation (0-3 for right, down, left, up)
  */

  /* code */
  return 0;
}
