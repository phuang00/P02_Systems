#include "battleship.h"

int game_semd;
char player;
int skey;
int you;
int them;
struct sembuf sb;

static void sighandler(int signo){
  printf("Exiting game...\n");
  game_semd = semget(skey, 1, 0);
  if (game_semd == -1){
    errno = 0;
  }
  sb.sem_op = 1;
  semop(game_semd, &sb, 1);
  if (errno != 0){
    printf("this: %s\n", strerror(errno));
  }
  exit(1);
}

int fire(int key, int row, char column){ //returns 1 is successfully hit, 0 if not
  //access shared memory for board
  int shmd = shmget(key, BOARD_SIZE, 0);
  if (shmd == -1){
    printf("%s\n", strerror(errno));
  }
  char * data = shmat(shmd, 0, 0);
  char copy[BOARD_SIZE]; //copy of data
  strcpy(copy, data);
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }

  //target ship
  int ship_exists = (*(copy + row * 11 + column) != '-');
  strcpy((copy + row * 11 + column), "X");
  data = strcpy(data, copy);
  shmdt(data);
  if (!ship_exists) {
    return 0;
  }
  return 1;
}

int faulty_coord(int row, char column, int key) { //1 if coordinate entered has been hit already, 0 if not
  int shmd = shmget(key, BOARD_SIZE, 0);
  if (shmd == -1){
    printf("%s\n", strerror(errno));
  }
  char * data = shmat(shmd, 0, 0);
  char copy[BOARD_SIZE]; //copy of data
  strcpy(copy, data);
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }

  int been_hit = (*(copy + row * 11 + column) == 'X');
  if (been_hit) {
    return 1;
  }
  return 0;
}

int boat_sank(int key, int boat) { // returns 1 if boat has been sank, 0 if not
  //access shared memory for board
  int shmd = shmget(key, BOARD_SIZE, 0);
  if (shmd == -1){
    printf("%s\n", strerror(errno));
  }
  char * data = shmat(shmd, 0, 0);
  char copy[BOARD_SIZE]; //copy of data
  strcpy(copy, data);
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }

  //check if boat number is on the board
  int row, col;
  for (row = 0; row < 10; row++){
    for (col = 0; col < 10; col++) {
      if (*(copy + row * 11 + col) == boat) {
        return 0;
      }
    }
  }
  return 1;
}

int win(int key) { //1 is player won, 0 if not
  int opponent;
  if (key == BOARD1_KEY) {
    opponent = BOARD2_KEY;
  } else {
    opponent = BOARD1_KEY;
  }
  //access shared memory for board
  int shmd = shmget(opponent, BOARD_SIZE, 0);
  if (shmd == -1){
    printf("%s\n", strerror(errno));
  }
  char * data = shmat(shmd, 0, 0);
  char copy[BOARD_SIZE]; //copy of data
  strcpy(copy, data);
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }

  //check if any boat is left
  int boat, row, col;
  for (boat = 0; boat < 5; boat++){
    for (row = 0; row < 10; row++){
      for (col = 0; col < 10; col++) {
        if (*(copy + row * 11 + col) == boat) {
          return 0;
        }
      }
    }
  }
  return 1;
}

void display_boards(){
  int them_id;
  char *data;
  printf("Your Board:\n");
  display_board(you);
  printf("Opponent's Board:\n");
  them_id = shmget(them, BOARD_SIZE, 0);
  if (them_id == -1){
    printf("bad: %s\n", strerror(errno));
  }
  data = shmat(them_id, 0, 0);
  if (errno != 0){
    printf("very bad: %s\n", strerror(errno));
  }
  else{
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
        char temp = *(data + i * 11 + j);
        if (temp == 'X' || temp == '\n'){
          printf(" %c", temp);
        } else{
          printf(" -");
        }
      }
    }
    printf("%s\n", data);

  }
}

int main(int argc, char const *argv[]) {
  signal(SIGINT, sighandler);
  if (argc > 1){
    printf("Waiting to play...\n");
    if (!strcmp(argv[1], "1")){
      skey = G1SEM_KEY;
      you = BOARD1_KEY;
      them = BOARD2_KEY;
    }
    else if (!strcmp(argv[1], "2")){
      skey = G2SEM_KEY;
      you = BOARD2_KEY;
      them = BOARD1_KEY;
    }
    else {
      printf("Please run ./play with either 1 or 2\n");
      return 0;
    }
    game_semd = semget(skey, 1, 0);
    if (game_semd == -1){
      printf("%s\n", strerror(errno));
    }
    sb.sem_num = 0;
    sb.sem_op = -1;
    semop(game_semd, &sb, 1);
    if (errno != 0){
      printf("%s\n", strerror(errno));
    }
    printf("Entered game successfully\n");
    int row;
    char input[20];
    char column;
    display_boards();
    while (1){
      printf("\nNow placing your shots on the opponent's board...\n");
      printf("Please input a row (int) and a column (char) separated by space:\n");
      fgets(input, 20, stdin);
      *strchr(input, '\n') = 0;
      sscanf(input, "%d %c", &row, &column);
      while (!check_coord(row, column) && faulty_coord(row, column, BOARD1_KEY)) {
        printf("The values you inputted were not valid. Please try again:\n");
        fgets(input, 20, stdin);
        *strchr(input, '\n') = 0;
        sscanf(input, "%d %c", &row, &column);
      }
      fire(them, row, column);
      display_boards();
    }
  }
  return 0;
}
