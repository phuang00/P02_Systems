#include "battleship.h"

int game_semd;
struct sembuf sb;

static void sighandler(int signo){
  printf("Exiting game...\n");
  game_semd = semget(GSEM_KEY, 1, 0);
  if (game_semd == -1){
    printf("wrong sem: %s\n", strerror(errno));
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
  int ship_exists = (*(copy + row * 11 + coll) != '-');
  strcpy(*(copy + row * 11 + coll), "X");
  data = strcpy(data, copy);
  shmdt(data);
  if (!ship_exists) {
    return 0;
  } else {
    return 1;
  }
}

int faulty_coord(int row, char column) { //coordinate entered has been hit already
  int been_hit = (*(copy + row * 11 + coll) == 'X')
  if (been_hit) {
    return 1;
  }
  return 0;
}

int boat_sank(int key, int boat) { // returns 1 if boat has been sank, 0 if not
  //access shared memory for board
  int shmd = shmget(key, BOARD_SIZE, 0);
  if (shmd == -1){
    errno = 0;
    return 1;
  }
  char * data = shmat(shmd, 0, 0);
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }

  //check if boat number is on the board
  for (int row = 0; row < 10; i++){
    for (int col = 0, col < 10, j++) {
      if (*(copy + row * 11 + coll) == boat) {
        return 0;
      }
    }
  }
  return 1;
}

int main(int argc, char const *argv[]) {
  signal(SIGINT, sighandler);
  printf("Waiting to play...\n");
  game_semd = semget(GSEM_KEY, 1, 0);
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
  while (1){
    printf("\nNow placing your shots on the opponent's board...\n", );
    printf("Please input a row (int) and a column (char) separated by space:\n");
    fgets(input, 20, stdin);
    *strchr(input, '\n') = 0;
    sscanf(input, "%d %c", &row, &column);



  }
  return 0;
}
