// CS 450 Spring 2019
// Zack Labaschin
// Reading and storing files was found at this url:
// https://stackoverflow.com/questions/2029103/correct-way-to-read-a-text-file-into-a-buffer-in-c
// socket setup taken from the following url:
// https://www.geeksforgeeks.org/socket-programming-cc/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 25

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
  char popenText[80];
  char mailClientSpaced[50];
  char mailClient[50];

  if(mailhost != NULL) {
    fgets(popenText, 80, mailhost);
    if(debug) {printf("The result of the popen: %s", popenText);}
    tok = strtok(popenText, "0123456789");
    tok = strtok(NULL, "\n");
    strcpy(mailClientSpaced, tok);
    strncpy(mailClient, &mailClientSpaced[1], 49);
    if(debug) {printf("The result of the mailClient: %s\n", mailClient);}
  }
  pclose(mailhost);

  char hostMailClinetCommand[1024] = {0};
  strcat(hostMailClinetCommand, "host ");
  strcat(hostMailClinetCommand, mailClient);
  if(debug) {printf("This is the concat result of hostMailClinetCommand: %s\n", hostMailClinetCommand);}

  FILE *mailIP = popen(hostMailClinetCommand, "r");
  char IPSpaced[30];
  char IP[30];

  if(mailIP != NULL) {
    fgets(popenText, 80, mailIP);
    if(debug) {printf("The result of the popen: %s", popenText);}
    tok = strtok(popenText, "0123456789");
    tok = strtok(NULL, "\n");
    strcpy(IPSpaced, tok);
    strncpy(IP, &IPSpaced, 29);
    if(debug) {printf("The result of the IP: %s\n", IP);}
  }
  pclose(mailIP);

  // set the socket
  int socketfd = -1;
  if((socketfd = socket(AF_INET, SOCK_STREAM, 0) == -1)) {
    printf("Rookie mistake - the socket is -1!\n");
    return -1;
  }

  struct sockaddr_in servadd;
  memset(&servadd, '0', sizeof(servadd));
  servadd.sin_family = AF_INET;
  servadd.sin_port   = htons(PORT);

// converting addresses from text to binary
  if(inet_pton(AF_INET, IP, &servadd.sin_addr) <= 0) {
    printf("Rookie mistake - address invalid\n");
    return -1;
  }

  if(connect(socketfd, (struct sockaddr *)&servadd, sizeof(servadd)) < 0) {
    printf("\nRookie mistake - connection did not occur\n");
    return -1;
  }

  send(socketfd, "helo server!", strlen("helo server!"), 0);
  char receivedText[1024] = {0};
  int retval = read(socketfd, receivedText, 1024);
  printf("%s\n", receivedText);

  free(source);
  return 0;
}
