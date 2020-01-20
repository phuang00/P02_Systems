#include "battleship.h"

int game_semd;
int game2_semd;
char player;
int skey;
int s2key;
int turn;
struct sembuf sb;

static void sighandler(int signo){
  printf("Exiting game...\n");
  game_semd = semget(skey, 1, 0);
  if (game_semd == -1){
    errno = 0;
  }
  int last_game_pid = semctl(game_semd, 0, GETPID, 0);
  int my_pid = getpid();
  if (last_game_pid == my_pid){
    sb.sem_op = 1;
    semop(game_semd, &sb, 1);
    if (errno != 0){
      printf("%s\n", strerror(errno));
    }
  }
  turn = semget(TURN_KEY, 1, 0);
  int last_turn_pid = semctl(turn, 0, GETPID, 0);
  if (last_turn_pid == my_pid){
    sb.sem_op = 1;
    semop(turn, &sb, 1);
    if (errno != 0){
      printf("%s\n", strerror(errno));
    }
  }
  exit(1);
}

int fire(int key, int row, char column){ //returns 1 is successfully hit, 0 if not
  //handling coordinates
  column = tolower(column);
  column = column % 97; // column 0 - 9

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
  int ship_exists;
  char target[1];
  if (strncmp(&copy[row * 11 + column], "-", 1) != 0){
    ship_exists = 1;
  } else {
    ship_exists = 0;
  }
  if (ship_exists){
    strncpy(&copy[row * 11 + column], "X", 1); //cannot do this
  }
  else{
    strncpy(&copy[row * 11 + column], "O", 1); //cannot do this
  }
  data = strcpy(data, copy);
  shmdt(data);
  return ship_exists;
}

int dup_coord(int row, char column, int key) { //1 if coordinate entered has been hit already, 0 if not
  //handling coordinates
  column = tolower(column);
  column = column % 97; // column 0 - 9

  printf("_%d\n", key);
  int shmd = shmget(key, BOARD_SIZE, 0);
  if (shmd == -1){
    printf("%s\n", strerror(errno));
  }
  char * data = shmat(shmd, 0, 0);
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }
  int been_hit = (*(data + row * 11 + column) == 'X') || (*(data + row * 11 + column) == 'O');
  shmdt(data);
  return been_hit;
}

int boat_sank(int key, int boat) { // returns 1 if boat has been sank, 0 if not
  //access shared memory for board
  int shmd = shmget(key, BOARD_SIZE, 0);
  if (shmd == -1){
    printf("%s\n", strerror(errno));
  }
  char * data = shmat(shmd, 0, 0);
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }

  //check if boat number is on the board
  int row, col;
  for (row = 0; row < 10; row++){
    for (col = 0; col < 10; col++) {
      if (*(data + row * 11 + col) == boat) {
        return 0;
      }
    }
  }
  shmdt(data);
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
  shmdt(data);

  //check if any boat is left
  for (int i = 0; i < BOARD_SIZE; i++) {
    if (copy[i] == '1') return 0;
    if (copy[i] == '2') return 0;
    if (copy[i] == '3') return 0;
    if (copy[i] == '4') return 0;
    if (copy[i] == '5') return 0;
  }
  return 1;
}

void display_boards(int you, int them){
  int them_id;
  char *data;
  printf("Your Board:\n");
  display_board(you);
  //printf("hello\n");
  printf("Opponent's Board:\n");
  them_id = shmget(them, BOARD_SIZE, 0);
  if (them_id == -1){
    printf("%s\n", strerror(errno));
  }
  data = shmat(them_id, 0, 0);
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }
  else{
    printf("    A B C D E F G H I J\n");
    int i;
    for (i = 0; i < 10; i++) {
      printf("  %d", i);
      int j;
      for (j = 0; j < 11; j++) {
        char temp = *(data + i * 11 + j);
        if (temp == 'X' || temp == '\n' || temp == 'O'){
          printf(" %c", temp);
        } else{
          printf(" -");
        }
      }
    }
  }
  shmdt(data);
}

int main(int argc, char const *argv[]) {
  int you;
  int them;
  signal(SIGINT, sighandler);
  if (argc > 1){
    printf("Waiting to play...\n");
    if (!strcmp(argv[1], "1")){
      skey = G1SEM_KEY;
      s2key = G2SEM_KEY;
      you = BOARD1_KEY;
      them = BOARD2_KEY;
    }
    else if (!strcmp(argv[1], "2")){
      skey = G2SEM_KEY;
      s2key = G1SEM_KEY;
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
    game2_semd = semget(s2key, 1, 0);
    if (game2_semd == -1){
      printf("%s\n", strerror(errno));
    }
    int val;
    printf("Entered game successfully!\n");
    if (board_filled(you)){
      int row;
      char input[20];
      char column;
      while (!win(you) && !win(them)){
        val = semctl(game2_semd, 0, GETVAL, 0);
        printf("Waiting for other player to join the game...\n");
        while (val != 0){ //player is not logged on
          val = semctl(game2_semd, 0, GETVAL, 0);
        }
        printf("Waiting for your turn...\n");
        int turn = semget(TURN_KEY, 1, 0);
        if (turn == -1) {
          printf("%s\n", strerror(errno));
        }
        sb.sem_num = 0;
        sb.sem_op = -1;
        semop(turn, &sb, 1);
        if (errno != 0){
          printf("%s\n", strerror(errno));
        }
        display_boards(you, them);
        printf("\nNow placing your shots on the opponent's board...\n");
        printf("Please input a column (char) and a row (int):\n");
        fgets(input, 20, stdin);
        *strchr(input, '\n') = 0;
        sscanf(input, "%c %d", &column, &row);
        while (!check_coord(row, column) || dup_coord(row, column, them)){
          printf("The values you inputted were not valid. Please try again:\n");
          fgets(input, 20, stdin);
          *strchr(input, '\n') = 0;
          sscanf(input, "%c %d", &column, &row);
        }
        fire(them, row, column);
        display_boards(you, them);
        sb.sem_op = 1;
        semop(turn, &sb, 1);
        if (errno != 0){
          printf("%s\n", strerror(errno));
        }
      }
      if (win(you)){
        printf("You won!\n");
      }
      else {
        printf("You lost!\n");
      }
    }
    else {
      printf("Board is incomplete!\n");
    }
    sb.sem_op = 1;
    semop(game_semd, &sb, 1);
    if (errno != 0){
      printf("%s\n", strerror(errno));
    }
  }
  else {
    printf("Please run ./play with either 1 or 2\n");
  }
  return 0;
}
