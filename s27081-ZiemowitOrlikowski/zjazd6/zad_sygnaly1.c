#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

void signalHandler(int sig){
    if(sig == SIGUSR1){
        printf("Otrzymano sygnal: %d\n",sig);
        exit(1);
    }
}


int main(){
    int masterPID = getpid();

    printf("Moj PID to: %d\n", masterPID);

    while(1){
        signal (SIGUSR1, signalHandler);
    }

    return 0;
}
