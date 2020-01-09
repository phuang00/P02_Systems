#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>

#define BOARD1_KEY 10282
#define SHIPS1_KEY 10036
#define SHIPS2_KEY 10004
#define BOARD2_KEY 12345
#define SEM_KEY 10050
#define BOARD_SIZE 111
