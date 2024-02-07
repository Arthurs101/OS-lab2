#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

int main(){
    pid_t p = fork();
    if (p < 0 ){
        perror("fork failed");
        exit(1);
    }else if (p == 0 ){
        for (int i = 0 ; i <= 400000000; i++){
            printf("number %d",i);
        }
    }else{ //parent 
        while(true){    }        
    }
}
