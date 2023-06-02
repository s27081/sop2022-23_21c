#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int usr1Signal = 0;

void signalHandler(int sig){
    printf("Wykryto probe uruchomienia drugiej instancji programu\n");
}

void handleSigInt(int sig){
    if(remove("/tmp/locki") == 0){
        printf("Usunieto plik tymczasowy\n");
    }else{
        printf("Blad przy usuwaniu pliku tymczasowego\n");
    }

    if(remove("lock_file") == 0){
        printf("Usunieto plik blokady\n");
    }else{
        printf("Blad przy usuwaniu pliku blokady\n");
    }
    exit(1);

}

int main(){

    FILE *fd, *lock_file;

    int masterPID = getpid();
    int filePID;

    char buffer[16];


    signal(SIGINT, handleSigInt);
    signal(SIGUSR1, signalHandler);

    lock_file = fopen("lock_file","r");
    if(lock_file){
        printf("Inna instancja programu juz jest uruchomiona\n");
        fclose(lock_file);
        return 0;
    }

    lock_file = fopen("lock_file","w");

        if(!lock_file){
            printf("Blad przy tworzeniu pliku\n");
            fclose(lock_file);
            return 1;
        }
    
    if(access("/tmp/locki", F_OK) == 0){

        printf("Plik instnieje\n");

        fd = fopen("/tmp/locki","r");

        while(fgets(buffer, 16, fd) != NULL){
            filePID=atoi(buffer);
        }
        
        kill(filePID, SIGUSR1);
        
    }else{

        printf("Plik nie istnieje\n");


        fd = fopen("/tmp/locki", "w");
        fprintf(fd,"%d",masterPID);
        printf("Utworzono plik tymczasowy\n");
        fclose(fd);
        
        
        while(1){
            sleep(1);           
        }
 
    }

    fclose(lock_file);

    return 0;
}
