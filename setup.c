#include "battleship.h"
int main_semd;
struct sembuf sb;
int key;

// union semun {
//   int              val;    /* Value for SETVAL */
//   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
//   unsigned short  *array;  /* Array for GETALL, SETALL */
//   struct seminfo  *__buf;  /* Buffer for IPC_INFO
//                               (Linux-specific) */
// };

static void sighandler(int signo){
  printf("Exiting game...\n");
  main_semd = semget(key, 1, 0);
  if (main_semd == -1){
    printf("wrong sem: %s\n", strerror(errno));
  }
  sb.sem_op = 1;
  semop(main_semd, &sb, 1);
  if (errno != 0){
    printf("this: %s\n", strerror(errno));
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

int board_filled(int key){ //check if board is filled or not
  //access shared memory for board
  int shmd = shmget(key, BOARD_SIZE, 0);
  if (shmd == -1){
    errno = 0;
    return 0;
  }
  char * data = shmat(shmd, 0, 0);
  if (errno != 0){
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
    printf("Please input a column (char), a row (int), and an orientation (l, r, u, d) separated by spaces:\n");
    fgets(input, 20, stdin);
    *strchr(input, '\n') = 0;
    sscanf(input, "%c %d %c", &column, &row, &orient);
    while (!place_boat(i, row, column, orient, key)){
      printf("The values you inputted were not valid. Please try again:\n");
      fgets(input, 20, stdin);
      *strchr(input, '\n') = 0;
      sscanf(input, "%c %d %c", &column, &row, &orient);
    }
    display_board(key);
  }
  printf("Boats have been placed!\n");
}

int main(int argc, char const *argv[]) {
  signal(SIGINT, sighandler);
  //create semaphores
  create_sem(SEM1_KEY);
  create_sem(SEM2_KEY);
  create_sem(G1SEM_KEY);
  create_sem(G2SEM_KEY);
  if (argc > 1){
    //player 1
    if (!strcmp(argv[1], "1")){
      //check semaphore availability
      printf("Waiting to access board 1...\n");
      key = SEM1_KEY;
      main_semd = semget(key, 1, 0);
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
      if (board_filled(key)){
        printf("This board has already been filled out.\n");
      }
      else {
        create_board(key);
        boat_input(key);
      }
      //done with semaphore
      printf("done\n");
      sb.sem_op = 1;
      semop(main_semd, &sb, 1);
      if (errno != 0){
        printf("%s\n", strerror(errno));
      }
    }

    //player 2
    else if (!strcmp(argv[1], "2")){
      //check semaphore availability
      key = SEM2_KEY;
      main_semd = semget(key, 1, 0);
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
      if (board_filled(key)){
        printf("This board has already been filled out.\n");
      }
      else {
        create_board(key);
        boat_input(key);
      }
      //done with semaphore
      sb.sem_op = 1;
      semop(main_semd, &sb, 1);
      if (errno != 0){
        printf("%s\n", strerror(errno));
      }
    }
    else {
      printf("Please run setup.c with either 1 or 2\n");
    }
  }
  else {
    printf("Please run setup.c with either 1 or 2\n");
  }
  return 0;
}
