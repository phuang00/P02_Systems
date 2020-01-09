#include <stdio.h>

int main() {
    FILE * fd;
    char c;
    fd = fopen("helpMenu", "r");
    if (fd == NULL) {
        printf("File was unable to open.\n");
    }
    while((c=fgetc(fd)) != EOF) {
      printf("%c",c);
   }
   fclose(fd);
}
