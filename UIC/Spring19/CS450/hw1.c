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

int checkForValidEmail(char * address) {
  char * valid = strchr(address, '@');
  if(valid == NULL) {
    return 0;
  }
  return 1;
}
char * tokenizer(char * toParse) {
  char * fromCopy[BUFSIZE] = {0};
  strcpy(fromCopy, toParse);
  char * tok = strtok(fromCopy, "@");
  if(tok != NULL) {
    tok = strtok(NULL, ">");
    if(debug){printf("This is the parsed from: %s\n", tok);}
    return tok;
  }
  return NULL;
}

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

int connectToServer(int socketFd, char * iP) {
  struct sockaddr_in servadd;
  memset(&servadd, '0', sizeof(servadd));
  if(debug) {printf("This is connectToServer ip: %s\n\n", iP);}
  if((servadd.sin_addr.s_addr = inet_addr(iP)) < 0) {
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

char * getTheiP(char * domain) {
  char hostString[1024] = {0};
  strcat(hostString, "host -t MX ");
  strcat(hostString, domain);
  FILE * mailhost = popen(hostString, "r");
  char popenText[80];
  char mailClientSpaced[50];
  char mailClient[50];
  char * tok = {0};
  if(mailhost != NULL) {
    fgets(popenText, 80, mailhost);
    if(debug) {printf("The result of the popen: %s\n", popenText);}
    tok = strtok(popenText, "0123456789"); // find the first number
    tok = strtok(NULL, " ");               // advance to the space
    if(strcmp("uic.edu", domain) == 0 || strcmp("gmail.com", domain) == 0) {
      tok = strtok(NULL, "\n");              // this final tok will capture addr
    }
    if(debug) {printf("This is the host -t tok: %s\n", tok);}
    strcpy(mailClientSpaced, tok);
    strncpy(mailClient, &mailClientSpaced[0], 49);
    if(debug) {printf("The result of the mailClient: %s\n", mailClient);}
  }
  pclose(mailhost);

  char hostMailClinetCommand[1024] = {0};
  strcat(hostMailClinetCommand, "host ");
  strcat(hostMailClinetCommand, mailClient);
  if(debug) {printf("This is the concat result of hostMailClinetCommand: %s\n", hostMailClinetCommand);}

  FILE *mailIP = popen(hostMailClinetCommand, "r");
  char IPSpaced[30] = {0};
  char iPv4[30] = {0};
  char IpNeededAdd[30] = {0};

  // this part was having problems when setting up the IP storing
  // The tok cuts off the first 1, and when concat a 1 with the rest
  // of the IP, a bunch of garbage gets added, which is taken care of
  // with the intermiate IPNeededAdd
  if(mailIP != NULL) {
    fgets(popenText, 80, mailIP);
    if(debug) {printf("The result of the popen: %s\n", popenText);}
    tok = strtok(popenText, "0123456789");
    if(strcmp("uic.edu", domain) == 0) {
      tok = strtok(NULL, "0123456789");
      tok = strtok(NULL, "\n");
    }
    else if(strcmp("yahoo.com", domain) == 0) { // real jenk
      tok = strtok(NULL, "0123456789");
      tok = strtok(NULL, " ");
      tok = strtok(NULL, " ");
      tok = strtok(NULL, " ");
    }
    else {
      tok = strtok(NULL, " ");
    }
    if(debug){printf("This is the tok: %s\n", tok);}
    strcpy(IPSpaced, tok);
    if(debug){printf("THis is the IPSpaced: %s\n", IPSpaced);}
    strcat(IpNeededAdd,"1");
    strcat(IpNeededAdd,IPSpaced);
    strncpy(iPv4,&IpNeededAdd[0],strlen(IpNeededAdd));
    if(debug) {printf("The result of the IP: %s\n", iPv4);}
  }
  pclose(mailIP);

  return iPv4;
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

int main(int argc, char *argv[]) {

  if(debug) {printf(" Number of arguments besides executable %d\n", argc - 1);}

  int i = 1;
  for(; i < argc; i++) {
    if(debug) {printf("Now parsing file %s\n", argv[i]);}

    // load the email into a char array
    char * source = openFileAndLoad(argv[i]);
    // parsing the stored email
    //emailArray = parseTheEmail(source);

    char * helloServer = "HELO SERVER\r\n";
    char * data = "DATA\r\n";
    char * end = "QUIT\r\n";

    char from[BUFSIZE];
    char * tok = strtok(source, "<");
    tok = strtok(NULL, ">");
    snprintf(from, BUFSIZE, "MAIL FROM: <%s>\r\n", tok);
    if(debug) {printf("This is the from: %s\n", from);}

    // check email is valid format
    char * validAddress[BUFSIZE] = {0};
    strcpy(validAddress, from);
    int badAddress = checkForValidEmail(validAddress);
    if(!badAddress) {
      printf("The sending address is not valid!\n");
      continue;
    }

    char to[BUFSIZE];
    tok = strtok(NULL, "<");
    tok = strtok(NULL, ">");
    snprintf(to, BUFSIZE, "RCPT TO: <%s>\r\n", tok);
    if(debug) {printf("This is the to: %s\n", to);}

    // store the subject
    char subject[BUFSIZE] = {0}; // TODO better way of determining size
    tok = strtok(NULL, "\r\n");
    while(tok != NULL) {
      strcat(subject, tok);
      strcat(subject, "\r\n");
      tok = strtok(NULL, "\r\n");
    }

    strcat(subject, ".\r\n");

    if(debug) {printf("this is the debug for the subject: %s\n----\n", subject);}

    // get the IP

    char * domain = tokenizer(from);
    if(domain == NULL) {
      printf("The domain is NULL - going to next file!\n");
      continue;
    }
    if(debug){printf("\nThis is the domain: %s\n", domain);}
    char iP[BUFSIZE] = {0};
    strcpy(iP, getTheiP(domain));
    if(debug){printf("This is the return to main value for iP: %s\n", iP);}
    // set the socket
    int socketFd = setSocketFd();
    // connect to server
    connectToServer(socketFd, iP);

    char received[BUFSIZE];
    read(socketFd, received, 1024);
    printf("Initial read from server: %s\n", received);

    // sending the email
    printf("%s", helloServer);
    send(socketFd, helloServer, strlen(helloServer), 0);
    read(socketFd, received, strlen(received));
    printf("%s\n", received);

    printf("\n%s", from);
    send(socketFd, from, strlen(from), 0);
    read(socketFd, received, strlen(received));
    printf("%s\n", received);

    printf("\n%s", to);
    send(socketFd, to, strlen(to), 0);
    read(socketFd, received, strlen(received));
    printf("%s\n", received);

    printf("\n%s", data);
    send(socketFd, data, strlen(data), 0);
    read(socketFd, received, strlen(received));
    printf("%s\n", received);
    if(strstr(received, "550") != NULL) {
      printf("No such user, skipping to next file!\n");
      continue;
    }

    printf("\n%s", subject);
    send(socketFd, subject, strlen(subject), 0);
    read(socketFd, received, strlen(received));
    printf("%s\n", received);

    printf("\n%s", end);
    send(socketFd, end, strlen(end), 0);
    read(socketFd, received, strlen(received));
    printf("%s\n", received);


    // free memory
    free(source);
  }
  return 0;
}
