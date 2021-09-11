#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

void send_file(FILE *fp, int sockfd, int fileSize)
{
  int n;
  char data[fileSize];

  while(!feof(fp)){
    fread(data, 1, sizeof(data), fp);
    if(n = write(sockfd, data, sizeof(data)) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }
/*
    else if (n != sizeof(data) && !feof(fp)){
      // retrying until whole dataset is sent
      while(n = write(sockfd, data, sizeof(data)) != sizeof(data)){}
    }
*/  
    bzero(data, sizeof(data));
  }

/*
  while (fgets(data, SIZE, fp) != NULL)
  {
    if (send(sockfd, data, sizeof(data), 0) == -1)
    {
      perror("[-]Error in sending file.");
      exit(1);
    }
    bzero(data, SIZE);
  }
  */
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
  int fileSize;

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

  // Sending file Size
  if (send(sockfd, &fileSize, sizeof(fileSize), 0) == -1)
    {
      perror("[-]Error in sending file size.");
      exit(1);
    }

  /* Sending file Name
  if (send(sockfd, &fileName, sizeof(fileName), 0) == -1)
    {
      perror("[-]Error in sending file Name.");
      exit(1);
    }
*/

  send_file(fp, sockfd, fileSize);
  printf("[+]File data sent successfully.\n");

  printf("[+]Closing the connection.\n");
  close(sockfd);

  return 0;
}
