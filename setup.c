#include "battleship.h"
int main_semd;
struct sembuf sb;
int shmkey;
int semkey;

// union semun {
//   int              val;    /* Value for SETVAL */
//   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
//   unsigned short  *array;  /* Array for GETALL, SETALL */
//   struct seminfo  *__buf;  /* Buffer for IPC_INFO
//                               (Linux-specific) */
// };

static void sighandler(int signo){
  printf("Exiting game...\n");
  main_semd = semget(semkey, 1, 0);
  if (main_semd == -1){
    printf("%s\n", strerror(errno));
  }
  int last_pid = semctl(main_semd, 0, GETPID, 0); //get pid of last process to perform op on semaphore
  int my_pid = getpid();
  if (last_pid == my_pid){ //if this process has control of semaphore
    sb.sem_op = 1;
    semop(main_semd, &sb, 1);
    if (errno != 0){
      printf("%s\n", strerror(errno));
    }
  }
  exit(1);
}

void create_sem(int key){
  int semd;
  semd = semget(key, 1, IPC_CREAT | IPC_EXCL | 0644);
  if (semd != -1){
    union semun us;
    us.val = 1;
    semctl(semd, 0, SETVAL, us);
  } else {
    errno = 0;
  }
}

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
    shmdt(data);
    if (errno != 0){
      printf("%s\n", strerror(errno));
    }
  }
  //printf("shared memory created\n\n");
}

int place_boat(int boat, int row, char column, char orient, int key){
  //check coordinates
  column = tolower(column);
  if (!check_coord(row, column)) return 0;
  int coll = column % 97;
  //check orientation
  orient = tolower(orient);
  if (orient != 'l' && orient != 'r' && orient != 'u' && orient != 'd') return 0;
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

void boat_input(key){
  int row;
  char input[20];
  char column, orient;
  int i;
  display_board(key);
  for (i = 1; i <= 5; i++){
    printf("\nNow placing Boat %d...\n", i);
    row = 0; //reset values
    column = 0;
    orient = 0;
    printf("Please input a column (char), a row (int), and an orientation (l, r, u, d):\n");
    fgets(input, 20, stdin);
    *strchr(input, '\n') = 0;
    sscanf(input, "%c %d %c", &column, &row, &orient);
    while (!place_boat(i, row, column, orient, key)){
      row = 0; //reset values
      column = 0;
      orient = 0;
      printf("The values you inputted were not valid. Please try again:\n");
      fgets(input, 20, stdin);
      *strchr(input, '\n') = 0;
      sscanf(input, "%c %d %c", &column, &row, &orient);
    }
    display_board(key);
  }
  printf("Boats have been placed!\n");
}

void remove_shm(int key) {
  int shmd = shmget(key, 1, 0);
  if (shmd == -1){
    printf("%s\n", strerror(errno));
  }
  shmctl(shmd, IPC_RMID, 0);
  if (errno != 0){
      printf("%s\n", strerror(errno));
  }
}

void remove_sem(int semd) {
  semctl(semd, IPC_RMID, 0);
  if (errno != 0){
      printf("%s\n", strerror(errno));
  }
}

int main(int argc, char const *argv[]) {
  signal(SIGINT, sighandler);
  //create semaphores
  create_sem(SEM1_KEY);
  create_sem(SEM2_KEY);
  create_sem(G1SEM_KEY);
  create_sem(G2SEM_KEY);
  create_sem(TURN_KEY);
  if (argc > 1){
    //player 1
    if (!strcmp(argv[1], "1")){
      //check semaphore availability
      printf("Waiting to access board 1...\n");
      shmkey = BOARD1_KEY;
      semkey = SEM1_KEY;
      main_semd = semget(semkey, 1, 0);
      if (main_semd == -1){
        printf("%s\n", strerror(errno));
      }
      sb.sem_num = 0;
      sb.sem_op = -1;
      semop(main_semd, &sb, 1);
      if (errno != 0){
        printf("%s\n", strerror(errno));
      }
      printf("Access granted!\n");
      if (board_filled(shmkey)){
        printf("This board has already been filled out.\n");
      }
      else {
        create_board(shmkey);
        boat_input(shmkey);
      }
      //done with semaphore
      //printf("done\n");
      sb.sem_op = 1;
      semop(main_semd, &sb, 1);
      if (errno != 0){
        printf("%s\n", strerror(errno));
      }
    }

    //player 2
    else if (!strcmp(argv[1], "2")){
      //check semaphore availability
      shmkey = BOARD2_KEY;
      semkey = SEM2_KEY;
      main_semd = semget(semkey, 1, 0);
      if (main_semd == -1){
        printf("%s\n", strerror(errno));
      }
      printf("Waiting to access board 2...\n");
      sb.sem_num = 0;
      sb.sem_op = -1;
      semop(main_semd, &sb, 1);
      if (errno != 0){
        printf("%s\n", strerror(errno));
      }
      printf("Access granted!\n");
      if (board_filled(shmkey)){
        printf("This board has already been filled out.\n");
      }
      else {
        create_board(shmkey);
        boat_input(shmkey);
      }
      //done with semaphore
      sb.sem_op = 1;
      semop(main_semd, &sb, 1);
      if (errno != 0){
        printf("%s\n", strerror(errno));
      }
    }
    else if (!strcmp(argv[1], "-r")){
      printf("Are you sure you want to reset the game? (y/n): ");
      char input[20];
      fgets(input, 20, stdin);
      input[1] = 0;
      if (!strcmp(input, "y")){
        printf("Waiting for access to semaphores...\n");
        //wait for semaphore to remove game
        int semd1 = semget(SEM1_KEY, 1, 0);
        if (semd1 == -1){
          printf("%s\n", strerror(errno));
        }
        int semd2 = semget(SEM2_KEY, 1, 0);
        if (semd2 == -1){
          printf("%s\n", strerror(errno));
        }
        int semd3 = semget(G1SEM_KEY, 1, 0);
        if (semd3 == -1){
          printf("%s\n", strerror(errno));
        }
        int semd4 = semget(G2SEM_KEY, 1, 0);
        if (semd4 == -1){
          printf("%s\n", strerror(errno));
        }
        int semd5 = semget(TURN_KEY, 1, 0);
        if (semd5 == -1){
          printf("%s\n", strerror(errno));
        }
        sb.sem_num = 0;
        sb.sem_op = -1;
        semop(semd1, &sb, 1);
        if (errno != 0){
          printf("%s\n", strerror(errno));
        }
        sb.sem_num = 0;
        sb.sem_op = -1;
        semop(semd2, &sb, 1);
        if (errno != 0){
          printf("%s\n", strerror(errno));
        }
        sb.sem_num = 0;
        sb.sem_op = -1;
        semop(semd3, &sb, 1);
        if (errno != 0){
          printf("%s\n", strerror(errno));
        }
        sb.sem_num = 0;
        sb.sem_op = -1;
        semop(semd4, &sb, 1);
        if (errno != 0){
          printf("%s\n", strerror(errno));
        }
        sb.sem_num = 0;
        sb.sem_op = -1;
        semop(semd5, &sb, 1);
        if (errno != 0){
          printf("%s\n", strerror(errno));
        }
        remove_shm(BOARD1_KEY);
        remove_shm(BOARD2_KEY);
        remove_sem(semd1);
        remove_sem(semd2);
        remove_sem(semd3);
        remove_sem(semd4);
        remove_sem(semd5);
        printf("semaphores and shared memory removed\n");
      }
    }
    else {
      printf("Please run setup.c with either 1, 2 or -r\n");
    }
  }
  else {
    printf("Please run setup.c with either 1, 2, or -r\n");
  }
  return 0;
}
