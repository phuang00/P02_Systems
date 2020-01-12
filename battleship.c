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

int fire(int key, int row, char column){
    //check coordinates
    column = tolower(column);
    if (!check_coord(row, column)) return 0;
    int coll = column % 97;
    row -= 1;

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
    int target = (*(copy + row * 11 + coll) != '-');
    if (!target) {
        printf("Missed.\n");
        return 0;
    } else {
        printf("Hit!\n");
        strcpy(*(copy + row * 11 + coll), "X");
    }
    data = strcpy(data, copy);
    shmdt(data);
    return 0;
}

int boat_sank(int key, int boat) {
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
  while (1){

  }
  return 0;
}
