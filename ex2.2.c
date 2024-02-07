#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2, pid3;
    clock_t start_time, end_time;

    // Clock antes del primer fork
    start_time = clock();

    // Primer fork
    if ((pid1 = fork()) == 0) { // Proceso hijo
        if ((pid2 = fork()) == 0) { // Proceso nieto
            if ((pid3 = fork()) == 0) { //bisnieto
                for (int i = 0; i < 1000000; ++i) {} 
            } else {
                // nieto espera al bisnieto
                wait(NULL);
            }
        } else {
            //proceso hijo esperando nieto
            wait(NULL);
        }
    } else {
        //esperar padre espera hijo
        wait(NULL);
        end_time = clock();
        double end_time_d = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
        printf("Tiempo transcurrido: %f segundos\n", end_time_d);
    }

    return 0;
}
