// CS 450 Spring 2019
// Zack Labaschin
// Reading and storing files was found at this url:
// https://stackoverflow.com/questions/2029103/correct-way-to-read-a-text-file-into-a-buffer-in-c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int debug = 1;

int main(int argc, char *argv[]) {

  if(debug) {
    printf("You have entered %d\n", argc);
    for (int i = 1; i < argc; ++i)
        printf("%s\n", argv[i]);
    printf("\n");
  }

  FILE * file;
  char * source = NULL;
  file = fopen(argv[1], "r");
  long bufsize;

  if(file != NULL) {
    // go to the end of the file
    if(fseek(file, 0L, SEEK_END) == 0) {
      // size of the file
      bufsize = ftell(file);
      if(bufsize == -1) {
        printf("Rookie mistake - error in buffsize\n");
        exit(1);
      }

      // allocate our buffer size
      source = (char *) malloc(sizeof(char) * (bufsize + 1));

      // back to start of file
      if(fseek(file, 0L, SEEK_SET) != 0) {
        printf("Rookie mistake - error in getting back to the front of the file\n");
      }

      // read and store file
      size_t length = fread(source, sizeof(char), bufsize, file);
      if(ferror(file) != 0) {
        fputs("Error reading file", stderr);
      }
      else {
        source[length++] = '\0';
      }
    }
  }
  fclose(file);

  if(debug) {
    for(int i = -1; i < bufsize; i++)
      printf("%c", source[i]);
  }

  free(source);
  return 0;
}
