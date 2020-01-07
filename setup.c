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
  printf("shared memory created\n");

  /* code */
  return 0;
}
