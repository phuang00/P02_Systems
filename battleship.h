#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define BOARD1_KEY 10282
#define BOARD2_KEY 12345
#define SEM1_KEY 10050
#define SEM2_KEY 11220
#define GSEM_KEY 11228
#define BOARD_SIZE 111
