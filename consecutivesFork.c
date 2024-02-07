#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    pid_t f1 = fork();

    if (f1  > 0 ){
        printf("fork 1 pid%d \n",getpid());
    }else{
        printf("fork 1 failed \n");
    }
    
    pid_t f2 = fork();

    if (f2  > 0 ){
        printf("fork 2 pid%d \n",getpid());
    }else{
        printf("fork 2 failed \n");
    }

    pid_t f3 = fork();

    if (f3  > 0 ){
        printf("fork 3 pid%d \n",getpid());
    }else{
        printf("fork 3 failed \n");
    }

    pid_t f4 = fork();
    if (f4  > 0 ){
        printf("fork 4 pid%d \n",getpid());
    }else{
        printf("fork 4 failed \n");
    }
    
    return 0;
}