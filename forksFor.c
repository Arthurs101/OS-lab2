#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    for(int i = 0 ; i < 4 ; i++)
    {
        pid_t p = fork();
        if(p<0){
        perror("fork fail");
        exit(1);
        }
        else if (p == 0)
            printf("Child pid = %d\n", getpid());
        else
            printf("Parent pid = %d\n", getpid());
        }
        
    return 0;
}