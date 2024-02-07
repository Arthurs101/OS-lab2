#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Proceso hijo
        execl("./ipc", "ipc", "2", "A", (char *)NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else { // Proceso padre
        // Ejecutar otra instancia de ipc con identificador "B" en el proceso padre
        execl("./ipc", "ipc", "2", "B", (char *)NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
}
