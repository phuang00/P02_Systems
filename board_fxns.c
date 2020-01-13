#include "battleship.h"

int check_coord(int row, char column){ //check if coordinates are out of bounds
  column = tolower(column);
  if (row < 1 || row > 11) return 0;
  if (column < 97 || column > 123) return 0;
  return 1;
}
