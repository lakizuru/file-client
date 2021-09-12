#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

void send_file(FILE *fp, int sockfd, int fileSize)
{
  int n;
  char data[4096];

  while(!feof(fp)){
    fread(data, 1, sizeof(data), fp);
    write(sockfd, data, sizeof(data)); 
    bzero(data, sizeof(data));
  }
}

int main(int argc, char **argv)
{
  char *ip = "127.0.0.1";
  int port = 6666;
  int e;

  int sockfd;
  struct sockaddr_in server_addr;
  FILE *fp;
  char *fileName = argv[1];
  int fileSize, fileNameSize, maxFileSize;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (e == -1)
  {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Connected to Server.\n");

  // Reading maximum file size
  read(sockfd, &maxFileSize, sizeof(maxFileSize));

  fp = fopen(fileName, "r");
  if (fp == NULL)
  {
    perror("[-]Error in reading file.");
    exit(1);
  }

  // Getting file size
  fseek(fp, 0, SEEK_END);
  fileSize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  if (fileSize > maxFileSize){
    printf("[-]Maximum file size of %d exceeded.\n", maxFileSize);
    fileSize = -1;
    send(sockfd, &fileSize, sizeof(fileSize), 0);
    printf("[-]Closing the connection.\n");
    close(sockfd);
    exit(1);
  }

  // Getting fileNameSize
  fileNameSize = strlen(argv[1]);
  printf("%d\n", fileNameSize);

  // Sending file Size
  if (send(sockfd, &fileSize, sizeof(fileSize), 0) == -1)
    {
      perror("[-]Error in sending file size.");
      exit(1);
    }

  // Sending fileNameSize
  if (send(sockfd, &fileNameSize, sizeof(fileNameSize), 0) == -1)
    {
      perror("[-]Error in sending file name size.");
      exit(1);
    }

  // Sending file Name
  if (send(sockfd, fileName, fileNameSize, 0) == -1)
    {
      perror("[-]Error in sending file Name.");
      exit(1);
  }

  send_file(fp, sockfd, fileSize);
  printf("[+]File data sent successfully.\n");

  printf("[+]Closing the connection.\n");
  close(sockfd);

  return 0;
}
