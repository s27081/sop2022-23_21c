#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <signal.h>

pid_t pid;

void httpResponse(int socket, const char *content){
    const char *respTemplate = "HTTP/1.1 200 OK\r\n"
                                "Content-Length: %lu\r\n"
                                "Content-Type: text/html\r\n\r\n"
                                "%s";

    char response[1024];

    snprintf(response, sizeof(response), respTemplate, strlen(content), content);

    send(socket, response, strlen(response), 0);
}

int logFile(int sockfd, struct sockaddr_in client_addr, int pid){
    FILE *fp;
    fp=fopen("log","a");

    fprintf(fp,"Client IP address: %s\n",inet_ntoa(client_addr.sin_addr));
    fprintf(fp, "Client port: %d\n", (int) ntohs(client_addr.sin_port));
    fprintf(fp,"PID number: %d\n",pid);
    fprintf(fp,"|=========================|\n");

    fclose(fp);

    return 0;
}

void handleSigInt(int sig){
    if(pid == 0){
    printf(" Zamknieto polaczenie serwera, sygnal %d\n", sig);
    kill(pid, SIGKILL);
    }
    exit(1);
}

int main(int argc, char *argv[])
{
    const int masterPID = getpid();
    int sockfd, newsockfd, clilen, e, i;
    struct sockaddr_in serv_addr, cli_addr;

    const char blackList[20][12] = {"10.54.21.42","23.243.43.21"};

    int blackListSize = sizeof(blackList)/sizeof(blackList[0]);

    FILE *fp;
    
    const char *content = "<html><head><title>Test page</title></head><body><h1>Hello HTTP server!</h1></body></html>";
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(signal(SIGINT, handleSigInt)){
        close(sockfd);
    };

    if (sockfd < 0) {
        perror("Socket error\n");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(8080);
    
    e = bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr));

    if(e == -1){
        perror("Binding error\n");
    }

    while(1){
        if(masterPID != getppid()){
            pid = fork();
            if(listen(sockfd,5) == 0){
                printf("Nasłuchiwanie...\n");
            }else{
                perror("Listen error\n");
            };

            clilen = sizeof(cli_addr);
            
            while((newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen)) >= 0){
               
                char clientIP[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(cli_addr.sin_addr), clientIP, INET_ADDRSTRLEN);
            
                for(i=0; i<blackListSize;i++){
                    if(strcmp(blackList[i], clientIP) == 0){
                        printf("Zabroniony adres IP: %s\n", clientIP);
                        close(newsockfd);
                        break;
                    }
                }

                if (newsockfd < 0) {
                     perror("Accept error\n");
                }

                fp=fopen("log","a");

                httpResponse(newsockfd, content);

                fprintf(fp,"Wyslano strone\n");

                 if(logFile(sockfd,cli_addr,getpid()) == 0){
                    printf("Zapis logow udany\n");
                }else{
                    printf("Nie udalo sie zapisac logow\n");
                }

                close(newsockfd);
                fclose(fp);
            
            }
        
    }
    }
    return 0; 
}
