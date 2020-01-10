#include "battleship.h"
#include <ctype.h>

// union semun {
//   int              val;    /* Value for SETVAL */
//   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
//   unsigned short  *array;  /* Array for GETALL, SETALL */
//   struct seminfo  *__buf;  /* Buffer for IPC_INFO
//                               (Linux-specific) */
// };

void create_sem(int key){
  int semd;
  semd = semget(key, 1, IPC_CREAT | 0644);
  if (semd != -1){
    union semun us;
    us.val = 1;
    semctl(semd, 0, SETVAL, us);
  }
  else {
    printf("%s\n", strerror(errno));
  }
}

void create_board(int key){
  //printf("errno wass %d\n", errno);
  int shmd;
  char *data;
  shmd = shmget(key, BOARD_SIZE, IPC_CREAT | 0644);
  if (shmd == -1){
    printf("%s\n", strerror(errno));
    //printf("life sucks\n");
  }
  data = shmat(shmd, 0, 0);
  if (errno != 0){
    printf("so you? %s\n", strerror(errno));
    //printf("i agree\n");
  }
  else {
    int i;
    for (i = 0; i < 10; i++){
      strcpy(data + (11 * i), "----------\n");
    }
    //printf("%s\n", data);
    shmdt(data);
    if (errno != 0){
      printf("screw life: %s\n", strerror(errno));
    }
  }
  printf("shared memory created\n\n");
}

int check_board(int key){ //check if board is filled or not
  //access shared memory for board
  int shmd = shmget(key, BOARD_SIZE, 0);
  if (shmd == -1){
    //printf("we good\n");
    errno = 0;
    return 1;
  }
  char * data = shmat(shmd, 0, 0);
  if (errno != 0){
    //printf("ahhh\n");
    printf("%s\n", strerror(errno));
  }
  int one, two, three, four, five = 0;
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

int check_coord(int row, char column){ //check if coordinates are out of bounds
  column = tolower(column);
  if (row < 1 || row > 11) return 0;
  if (column < 97 || column > 123) return 0;
  return 1;
}

int place_boat(int boat, int row, char column, char orient, int key){
  //check coordinates
  column = tolower(column);
  //printf("is it you?\n");
  if (!check_coord(row, column)) return 0;
  //printf("so it's not you?\n");
  int coll = column % 97;
  row -= 1;
  //check orientation
  orient = tolower(orient);
  //printf("well well\n");
  if (orient != 'l' && orient != 'r' && orient != 'u' && orient != 'd') return 0;
  //access shared memory for board
  int shmd = shmget(key, BOARD_SIZE, 0);
  //printf("you?\n");
  if (shmd == -1){
    //printf("so it's you?\n");
    printf("%s\n", strerror(errno));
  }
  char * data = shmat(shmd, 0, 0);
  //printf("%s\n", data);
  //printf("hmm is you?\n");
  char copy[BOARD_SIZE]; //copy of data
  strcpy(copy, data);
  //printf("so you?\n");
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }
  //add coordinates
  int i;
  for (i = 0; i < boat; i++){
    if (i != 0){
      if (orient == 'l') coll -= 1;
      else if (orient == 'r') coll += 1;
      else if (orient == 'u') row -= 1;
      else row += 1;
      if (!check_coord(row, column)) return 0;
    }
    int empty = (*(copy + row * 11 + coll) == '-');
    if (!empty) return 0;
    *(copy + row * 11 + coll) = boat + '0'; //place boat down
  }
  data = strcpy(data, copy);
  shmdt(data);
  return 1;
}

int main(int argc, char const *argv[]) {
  create_sem(SEM1_KEY);
  create_sem(SEM2_KEY);
  create_sem(GSEM_KEY);
  printf("semaphore created\n");
  if (check_board(BOARD1_KEY)){
    create_board(BOARD1_KEY);
  }
  if (check_board(BOARD2_KEY)){
    create_board(BOARD2_KEY);
  }

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
  char input[20];
  char column, orient;
  int i;
  for (i = 1; i <= 5; i++){
    printf("\nNow placing Boat %d...\n", i);
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
  printf("Boats have been placed!\n");
  return 0;
}
