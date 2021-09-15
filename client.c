#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void send_file(FILE *fp, int sockfd, int fileSize)
{
  int n;
  char data[4096];

  while (!feof(fp))
  {
    fread(data, 1, sizeof(data), fp);
    write(sockfd, data, sizeof(data));
    bzero(data, sizeof(data));
  }
}

void write_file(int sockfd, int fileSize)
{
  int n;
  FILE *fp;

  char *filename = "recv";

  char buffer[4096];
  bzero(buffer, sizeof(buffer));

  fp = fopen(filename, "w");

  // Reading byte array
  while (fileSize > 0)
  {
    read(sockfd, buffer, 4096);
    fwrite(buffer, 1, sizeof(buffer), fp);
    fileSize -= 4096;
    bzero(buffer, sizeof(buffer));
  }

  fclose(fp);

  return;
}

int main(int argc, char **argv)
{
  char *ip = "127.0.0.1";
  int port = 6666;
  int e;

  int sockfd, cli_sock;
  struct sockaddr_in server_addr, client_addr;
  FILE *fp;

  char *action = argv[1];
  char *fileName = argv[2];
  int fileSize, fileNameSize, maxFileSize, actionID;
  int minFileSize = 10485760; // = 10MB

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");

  // creating a socket for client
  cli_sock = socket(AF_INET, SOCK_STREAM, 0);

  client_addr.sin_family = AF_INET;
  client_addr.sin_port = 5555;
  client_addr.sin_addr.s_addr = inet_addr(ip);

  e = bind(cli_sock, (struct sockaddr *)&client_addr, sizeof(client_addr));
  if (e < 0)
  {
    perror("[-]Error in bind");
    exit(1);
  }
  printf("[+]Binding successfull.\n");

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

  // Getting fileNameSize
  fileNameSize = strlen(argv[2]);

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

  // Push (Uploading file)
  if (strcmp(argv[1], "push") == 0)
  {

    // Sending action as push(1)
    actionID = 1;
    if (send(sockfd, &actionID, sizeof(actionID), 0) == -1)
    {
      perror("[-]Error in sending action as push.");
      exit(1);
    }

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

    if (fileSize > maxFileSize || fileSize < minFileSize)
    {
      printf("[-]File size out of range.\n");
      fileSize = -1;
      send(sockfd, &fileSize, sizeof(fileSize), 0);
      printf("[-]Closing the connection.\n");
      close(sockfd);
      exit(1);
    }

    // Sending file Size
    if (send(sockfd, &fileSize, sizeof(fileSize), 0) == -1)
    {
      perror("[-]Error in sending file size.");
      exit(1);
    }

    send_file(fp, sockfd, fileSize);
    printf("[+]File sent successfully.\n");
  }
  
  //Pulling a file
  else if (strcmp(argv[1], "pull") == 0)
  {
    // Sending action as pull(0)
    actionID = 0;
    if (send(sockfd, &actionID, sizeof(actionID), 0) == -1)
    {
      perror("[-]Error in sending action as pull.");
      exit(1);
    }
//
      // Read file size
      read(sockfd, &fileSize, sizeof(fileSize));
      if (fileSize == -1)
      {
        printf("[-]Maximum file size of %d exceeded.\n[-]File not downloaded.\n", maxFileSize);
        printf("[+]Server connection successfully terminated\n");
        printf("[-]Closing the connection.\n");
        close(sockfd);
        exit(1);
      }

      write_file(sockfd, fileSize);
      rename("recv", fileName);
      printf("[+]Data written in the file successfully.\n");
      printf("[+]Server connection successfully terminated\n");
//    
  }
  else
  {
    // Sending action as error(-1)
    actionID = -1;
    if (send(sockfd, &actionID, sizeof(actionID), 0) == -1)
    {
      perror("[-]Error in sending action as error.");
      exit(1);
    }
    printf("[-]No such argument as \'%s\'.\n", argv[1]);
  }

  printf("[+]Closing the connection.\n");
  close(sockfd);

  return 0;
}
