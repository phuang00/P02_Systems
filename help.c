#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main() {
    FILE * fd = fopen("helpMenu", "r");
    if (errno != 0){
      printf("%s\n", strerror(errno));
    }
    char buffer[200];
    while (fgets(buffer, 200, fd) != NULL){
      printf("%s", buffer);
    }
    fclose(fd);
    return 0;
}
