#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define SHM_SIZE 1024
#define LOCK_FILE "lock_file"

#define true  1
#define false 0

// Variable global para indicar que el hijo debe desvincularse
int should_detach = false;

static void handler(int signum) {
        // Establecer la variable global para indicar que el hijo debe desvincularse
        if(signum == SIGUSR2){
            should_detach = true;       
        }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <n> <x>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    char x = argv[2][0];

    // Crear o adherirse a un espacio de memoria compartida
    key_t key = ftok(".", 'A');
    int shmid;
    int lock_fd = open(LOCK_FILE, O_CREAT | O_RDWR, 0666);

    if (lock_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (flock(lock_fd, LOCK_EX) == -1) {
        perror("flock");
        exit(EXIT_FAILURE);
    }

    // Comprobar si el espacio de memoria compartida ya existe
    shmid = shmget(key, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    if (shmid == -1) {
        // Si falla, intentar obtener el espacio existente
        shmid = shmget(key, SHM_SIZE, 0666);
        if (shmid == -1) {
            perror("shmget");
            exit(EXIT_FAILURE);
        }

        printf("%c: espacio de memoria compartida hallado, conexión establecida\n", x);
    } else {
        printf("%c: espacio de memoria compartida creado\n", x);
    }

    if (flock(lock_fd, LOCK_UN) == -1) {
        perror("flock");
        exit(EXIT_FAILURE);
    }

    close(lock_fd);

    // Adjuntar la memoria compartida a un puntero
    char *shm_ptr = shmat(shmid, NULL, 0);
    if (shm_ptr == (char *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Configurar la acción para manejar la señal SIGUSR1 en ambos procesos
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGTERM,&sa,NULL);

    // Si ya se creó la memoria compartida, esperar el file descriptor de la otra instancia
    if (shm_ptr[0] != 0) {
        int other_shmid = atoi(shm_ptr);
        shmid = other_shmid;
    } else {
        // Si es la primera instancia, almacenar el file descriptor en la memoria compartida
        snprintf(shm_ptr, SHM_SIZE, "%d", shmid);
    }

    // Fork para crear un nuevo proceso
    pid_t pid = fork();
    printf("%c: contenido de la memoria antes de escribir %s\n", x, shm_ptr);
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Proceso hijo       
        sigset_t new_mask;
        sigemptyset(&new_mask);
        char iterations[SHM_SIZE]="";
        // Bucle para esperar la señal SIGTERM y desvincularse
        while (should_detach != true) {
            sigsuspend(&new_mask); // esperar alguna señal
            if(should_detach == true){
                printf("%c detaching from shared space\n",x);
                break;
            }
            strcat(iterations, &x);  
            usleep(1000);
        }
        printf("%c i am finished writing\n",x);
        strcat(shm_ptr, iterations);
        printf("%c : i wrote %s \n",x,iterations);
        // Obtener el valor de x enviado por el padre y concatenarlo en la memoria compartida

        if (shmdt(shm_ptr) == -1) {
            perror("shmdt en el proceso hijo\n");
            exit(EXIT_FAILURE);
        }      
        exit(EXIT_SUCCESS);
    } else { // Proceso padre
        // Lógica del ciclo for en el proceso padre
        for (int i = 0; i < SHM_SIZE; ++i) {
            if (i % n == 0) {
                // Enviar la señal SIGUSR1 al hijo para indicar que puede procederW
                kill(pid, SIGUSR1);
                usleep(10000); // Dar tiempo al hijo para realizar la concatenación
            }
        }
        printf("%c: proceso padre ha finalizado \n",x);
        // Enviar SIGTERM al hijo para indicar que finalice
        should_detach = true;
        kill(pid, SIGUSR2);

        // Esperar a que el hijo termine
        waitpid(pid, NULL, 0);
        printf("%c: contenido de la memoria después de escribir %s \n", x, shm_ptr);
        // Si el espacio de memoria compartida ya existe, desvincularse de él
        if (shmid != -1) {
            // Desvincularse del espacio de memoria compartida
            if (shmctl(shmid, IPC_RMID, NULL) == -1) {
                perror("shmctl");
                exit(EXIT_FAILURE);
            }
        }
    }
    return 0;
}
