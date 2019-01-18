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
    printf("Arguments besides executable %d\n", argc - 1);
    for (int i = 1; i < argc; ++i)
        printf("%s\n", argv[i]);
    printf("----\n");
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
    printf("This is the debug for the entire email file\n");
    for(int i = -1; i < bufsize; i++)
      printf("%c", source[i]);
    printf("----\n");
  }

  // parsing the stored email
  char * tok = strtok(source, "<");

  // store the from
  char from[80]; // TODO better way of determining size
  tok = strtok(NULL, ">");
  strcpy(from,tok);
  if(debug) {printf("this is the debug for the from: %s\n----\n", from);}

  // store the to
  char to[80]; // TODO better way of determining size
  tok = strtok(NULL, "<");
  tok = strtok(NULL, ">");
  strcpy(to, tok);
  if(debug) {printf("this is the debug for the to: %s\n----\n", to);}

  // store the subject
  char subject[500]; // TODO better way of determining size
  tok = strtok(NULL, ":");
  tok = strtok(NULL, "\n");
  strcpy(subject, tok);
  if(debug) {printf("this is the debug for the subject: %s\n----\n", subject);}

  // store the body
  char body[2000]; // TODO better way of determining size
  tok = strtok(NULL, "\n");
  strcpy(body, tok);

  if(debug) {printf("this is the debug for the body: %s\n----\n", body);}

  FILE * mailhost = popen("host -t MX cs.uic.edu", "r");
  char * mailHostAddress[80];

  if(mailhost != NULL) {

  }
  pclose(mailhost);
  free(source);
  return 0;
}
