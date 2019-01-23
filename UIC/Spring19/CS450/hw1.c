// CS 450 Spring 2019
// Zack Labaschin
// Reading and storing files was found at this url:
// https://stackoverflow.com/questions/2029103/correct-way-to-read-a-text-file-into-a-buffer-in-c
// socket setup taken from the following url:
// https://www.geeksforgeeks.org/socket-programming-cc/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 25
#define BUFSIZE 1024

int debug = 0;

void sendAndRead(int fileDescriptor, char * lineToSend) {
  char received[BUFSIZE] = {0};
  printf("%s", lineToSend);
  send(fileDescriptor, lineToSend, strlen(lineToSend), 0);
  read(fileDescriptor, received, strlen(received));
  printf("%s", received);
}

void readAndPrintFD(int fileDescriptor, char * lineToSend) {
  char result[BUFSIZE] = {0};
  read(fileDescriptor, result, BUFSIZE);
  printf("%s\n%s\n", lineToSend, result);
}

void sendReceiveAndPrint(int fileDescriptor, char *lineToSend) {
  send(fileDescriptor, lineToSend, strlen(lineToSend), 0);
  readAndPrintFD(fileDescriptor, lineToSend);
}

int setSocketFd() {
  int socketFd = -1;
  if((socketFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    printf("Rookie mistake - the socket is -1!\n");
    return -1;
  }
  if(debug){ printf("This is the socket: %d\n", socketFd);}

  return socketFd;
}

int connectToServer(int socketFd, char *iP) {
  struct sockaddr_in servadd;
  memset(&servadd, '0', sizeof(servadd));
  if((servadd.sin_addr.s_addr = inet_addr("131.193.32.56")) < 0) {
    perror("Error with inet_addr");
    return -1;
  }
  servadd.sin_family = AF_INET;
  servadd.sin_port   = htons(PORT);

  if(connect(socketFd, (struct sockaddr *)&servadd, sizeof(servadd)) < 0) {
    perror("The error at connect is: ");
    printf("\nRookie mistake - connection did not occur\n");
    return -1;
  }
  return socketFd;
}

char * getTheiP() {
  FILE * mailhost = popen("host -t MX cs.uic.edu", "r");
  char popenText[80];
  char mailClientSpaced[50];
  char mailClient[50];
  char * tok = {0};
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
  char IPSpaced[30] = {0};
  char iP[30] = {0};
  char IpNeededAdd[30] = {0};

  // this part was having problems when setting up the IP storing
  // The tok cuts off the first 1, and when concat a 1 with the rest
  // of the IP, a bunch of garbage gets added, which is taken care of
  // with the intermiate IPNeededAdd
  if(mailIP != NULL) {
    fgets(popenText, 80, mailIP);
    if(debug) {printf("The result of the popen: %s", popenText);}
    tok = strtok(popenText, "0123456789");
    tok = strtok(NULL, "\n");
    strcpy(IPSpaced, tok);
    strcat(IpNeededAdd,"1");
    strcat(IpNeededAdd,IPSpaced);
    strncpy(iP,&IpNeededAdd[0],strlen(IpNeededAdd));
    if(debug) {printf("The result of the IP: %s\n", iP);}
  }
  pclose(mailIP);

  return iP;
}

char * openFileAndLoad(char *argv) {
  FILE * file;
  char * source = NULL;
  file = fopen(argv, "r");
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
    int i = -1;
    for(; i < bufsize; i++)
      printf("%c", source[i]);
    printf("----\n");
  }



  return source;
}

char ** parseTheEmail(char *source) {
  // char **emailArray = (char**) malloc(sizeof(char*) * 7);
  // char * tok = strtok(source, "<");
  // char *from1 = malloc(sizeof(char) * 80);
  // // store the from
  // char from[80] = {0}; // TODO better way of determining size;
  // tok = strtok(NULL, ">");
  // snprintf(from1, 80, "MAIL FROM: <%s>", tok);
  // // strcat(from, "MAIL FROM: <");
  // // strcat(from, tok);
  // // strcat(from, ">");
  // strcpy(emailArray[1], from1);
  // printf("The length of from is: %d\n", (int)strlen(from));
  // if(debug) {printf("this is the debug for the from: %s\n----\n", emailArray[1]);}
  // // store the to
  // tok = strtok(NULL, "\n");
  // printf("%s", tok);
  // char to[80] = {0}; // TODO better way of determining size
  // strcat(to, "RCPT TO: <");
  // strcat(to, tok);
  // strcat(to, ">\n");
  // emailArray[2] = to;
  // if(debug) {printf("this is the debug for the to: %s\n----\n", emailArray[2]);}

  // // store the subject
  // char subject[BUFSIZE] = {0}; // TODO better way of determining size
  // tok = strtok(NULL, "\n");
  // tok = strtok(NULL, "\n");
  // strcat(subject, tok);
  // strcat(subject, "\n\n");
  // tok = strtok(NULL, "\n");
  // strcat(subject, tok);
  // strcat(subject, "\n.\n");
  // //strcpy(emailArray[2], tok);
  // if(debug) {printf("this is the debug for the subject: %s\n----\n", subject);}
  //
  // // // store the body
  // // char body[2000]; // TODO better way of determining size
  // // tok = strtok(NULL, "\n");
  // // strcpy(body, tok);
  // // //strcpy(emailArray[3], tok);
  // if(debug) {printf("this is the debug for the body: %s\n----\n", tok);}
  //
  // emailArray[0] = "HELO server\n";
  // //emailArray[1] = "MAIL FROM: <zlabas2@uic.edu>\n";
  // printf("THis is emailArray[1]: %s and this is from: %s", emailArray[1], from);
  // printf("The length of emailArray[1] is: %d\n", (int)strlen(emailArray[1]));
  // emailArray[2] = "RCPT TO: <zlabas2@uic.edu>\n";
  // emailArray[3] = "DATA\n";
  // emailArray[4] = "FROM: zack <zlabas2@uic.edu>\n";
  // emailArray[5] = "Subject: Tests for CS450 HW1!\n\nGo Bears\n.\n";
  // emailArray[6] = "QUIT\n";

  return NULL;
}

int main(int argc, char *argv[]) {

  if(debug) {printf(" Number of arguments besides executable %d\n", argc - 1);}

  int i = 1;
  for(; i < argc; i++) {
    if(debug) {printf("Now parsing file %s\n", argv[i]);}

    // load the email into a char array
    char **emailArray = {0};
    char * source = openFileAndLoad(argv[i]);
    // parsing the stored email
    //emailArray = parseTheEmail(source);

    char * helloServer = "HELO SERVER\n";
    char * data = "DATA\n";
    char * end = "QUIT\n";

    char from[BUFSIZE];
    char * tok = strtok(source, "<");
    tok = strtok(NULL, ">");
    snprintf(from, BUFSIZE, "MAIL FROM: <%s>\n", tok);
    printf("This is the from: %s\n", from);

    char to[BUFSIZE];
    tok = strtok(NULL, "<");
    tok = strtok(NULL, ">");
    snprintf(to, BUFSIZE, "RCPT TO: <%s>\n", tok);
    printf("This is the to: %s\n", to);

    // store the subject
    char subject[BUFSIZE] = {0}; // TODO better way of determining size
    tok = strtok(NULL, "\n");
    while(tok != NULL) {
      strcat(subject, tok);
      strcat(subject, "\n");
      tok = strtok(NULL, "\n");
    }
    printf("This is the strcat: %s\n", subject);
    strcat(subject, ".\n");
    printf("This is the strcat: %s\n", subject);

    if(debug) {printf("this is the debug for the subject: %s\n----\n", subject);}

    if(debug) {printf("this is the debug for the body: %s\n----\n", tok);}

    // emailArray[0] = "HELO server\n";
    // //emailArray[1] = "MAIL FROM: <zlabas2@uic.edu>\n";
    // printf("THis is emailArray[1]: %s and this is from: %s", emailArray[1], from);
    // printf("The length of emailArray[1] is: %d\n", (int)strlen(emailArray[1]));
    // emailArray[2] = "RCPT TO: <zlabas2@uic.edu>\n";
    // emailArray[3] = "DATA\n";
    // emailArray[4] = "FROM: zack <zlabas2@uic.edu>\n";
    // emailArray[5] = "Subject: Tests for CS450 HW1!\n\nGo Bears\n.\n";
    // emailArray[6] = "QUIT\n";


    // get the IP
    char *iP = getTheiP();
    // set the socket
    int socketFd = setSocketFd();
    // connect to server
    connectToServer(socketFd, iP);

    char received[BUFSIZE];
    read(socketFd, received, 1024);
    printf("%s", received);

    // sending the email
    printf("%s", helloServer);
    send(socketFd, helloServer, strlen(helloServer), 0);
    read(socketFd, received, strlen(received));
    printf("%s", received);

    // sendAndRead(socketFd, helloServer);
    // sendAndRead(socketFd, from);
    // sendAndRead(socketFd, to);
    // sendAndRead(socketFd, data);
    // sendAndRead(socketFd, subject);
    // sendAndRead(socketFd, end);
    printf("%s", from);
    send(socketFd, from, strlen(from), 0);
    read(socketFd, received, strlen(received));
    printf("%s", received);

    printf("%s", to);
    send(socketFd, to, strlen(to), 0);
    read(socketFd, received, strlen(received));
    printf("%s", received);

    printf("%s", data);
    send(socketFd, data, strlen(data), 0);
    read(socketFd, received, strlen(received));
    printf("%s", received);

    printf("%s", subject);
    send(socketFd, subject, strlen(subject), 0);
    read(socketFd, received, strlen(received));
    printf("%s", received);

    printf("%s", end);
    send(socketFd, end, strlen(end), 0);
    read(socketFd, received, strlen(received));
    printf("%s", received);


    // free memory
    free(source);
  }
  return 0;
}
