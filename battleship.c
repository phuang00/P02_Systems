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
