#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int saveFile(int sockfd, char *filename){
    int n;
    FILE *fp;
    char buffer[1024];
    fp = fopen(filename, "w");
    while (1) {
        n = recv(sockfd, buffer, 1024, 0);
            if (n <= 0){
                break;    
    }
    fprintf(fp, "%s", buffer);
    bzero(buffer, 1024);
    }
      
      fclose(fp);
      return 0;
}


int main(int argc, char *argv[])
{
    int sockfd, portno, e;
    struct sockaddr_in serv_addr;
    char *filename = argv[2];

    if (argc < 3) {
        perror("Nie podano portu lub nazwy pliku\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("Socket error\n");
    }
       
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = portno;
    
    e = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
  
    if(e == -1){
        perror("Connect error\n");
        exit(1);
    }

    if (send(sockfd, filename, strlen(filename), 0) < 0) {
        perror("Błąd send");
        exit(1);
    }


    if(saveFile(sockfd, filename) == 0){
        printf("Przesłanie plików ukończone pomyślnie\n");
        return 0;
    }else{
        exit(1);
    }
    
    
    close(sockfd);

    return 0;
}

