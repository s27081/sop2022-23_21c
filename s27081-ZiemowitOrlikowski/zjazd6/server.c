#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include<signal.h>

int sendFile(FILE *fp, int sockfd){
  char buffer[1024] = {0};
  size_t bytesRead;
 
 while ((bytesRead = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        if (send(sockfd, buffer, bytesRead, 0) == -1) {
            perror("Sending file error\n");
            return 1;
        }
    }

  return 0;
}

int logFile(int sockfd, struct sockaddr_in client_addr){
    FILE *fp;
    fp=fopen("log","a");

    fprintf(fp,"Client IP address: %s\n",inet_ntoa(client_addr.sin_addr));
    fprintf(fp, "Client port: %d\n", (int) ntohs(client_addr.sin_port));
    fprintf(fp,"|=========================|\n");

    fclose(fp);

    return 0;
}

void handleSigInt(int sig){
    printf(" Zamknieto polaczenie serwera, sygnal %d\n", sig);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen, e;
    struct sockaddr_in serv_addr, cli_addr;
    FILE *fp;
    char buffer[64];
    struct stat st;

    
    if (argc < 2) {
        perror("Nie podano portu\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(signal(SIGINT, handleSigInt)){
        close(sockfd);
    };


    if (sockfd < 0) {
        perror("Socket error\n");
    }

    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = portno;
    
    e = bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr));

    if(e == -1){
        perror("Binding error\n");
    }
    
    if(listen(sockfd,5) == 0){
        printf("Nasłuchiwanie...\n");
    }else{
        perror("Listen error\n");
    };
    
    clilen = sizeof(cli_addr);

    while((newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen)) >= 0){
        
        if (newsockfd < 0) {
             perror("Accept error\n");
        }

        read(newsockfd, buffer, 64);

        fp = fopen(buffer, "r");
        stat(buffer, &st);

        if (fp == NULL) {
            printf("Wielkość pliku: -1 bajty/ów\n");
            perror("Reading file error\n");
            exit(1);
        }else{
            printf("Wielkość pliku: %ld bajty/ów\n",st.st_size);
        }

        if(sendFile(fp, newsockfd) == 0){
            printf("Przesłanie plików ukończone pomyślnie\n");
        }else{
            exit(1);
        }

        if(logFile(sockfd,cli_addr) == 0){
            printf("Zapis logow udany\n");
        }else{
            printf("Nie udalo sie zapisac logow\n");
        }


        fclose(fp);
        close(newsockfd);
    }
    return 0; 
}
