#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>

#define bool int
#define true 1
#define false 0

//variable global    
char sudoku[9][9];
bool all_col_valid =  true;
bool all_row_valid = true;

char** get_subset(int sub_id){
    // 1 2 3
    // 4 5 6
    // 7 8 9
    if (0 >= sub_id || sub_id > 9){
        perror("error: subset invalid, must be 1 to 9 \n\t1 2 3\n\t4 5 6\n\t7 8 9\n");
        return NULL;
    }
    int start_row =(sub_id -1)/3*3;
    int start_col =(sub_id -1)%3*3;
    // Crea un arreglo dinámico para almacenar el subconjunto
    char** subset = malloc(3 * sizeof(char));
    for (int i = 0; i < 3; ++i) {
        subset[i] = malloc(3 * sizeof(char));
        if (subset[i] == NULL) {
            printf("Error: No se pudo asignar memoria para el subconjunto.\n");
            // Libera la memoria asignada anteriormente
            for (int j = 0; j < i; ++j) {
                free(subset[j]);
            }
            free(subset);
            return NULL;
        }
    }

    // Copia los elementos del subconjunto a la matriz dinámica
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            subset[i][j] = sudoku[start_row + i][start_col + j];
        }
    }
    return subset;
}
bool isRowValid(int row_id){
    if (row_id <= 0 || row_id > 9 ){
        perror("error, row id must be 1 to 9");
        return false;
    }
    const char* row = sudoku[row_id-1];
    bool numbers[9] = {false}; // 9 posiciones correspondientes a números del 1 al 9
    for (int i = 0; i < 9 ; i++){
        char digit = row[i];
        int number_index = digit - '1';
        if(!numbers[number_index]){ // si no aparece en la fila
            numbers[number_index] = true; 
        }else{ //si ya existía , es decir ya está en la fila con anterioridad , no es válido
            return false;
        }
    }
    return true;
}
bool isColumnValid(int col_id){
    if (col_id <= 0 || col_id > 9 ){
        perror("error, row id must be 1 to 9");
        return false;
    }
    
    col_id = col_id -1;
    char column[9];
    bool numbers[9] = {false}; // 9 posiciones correspondientes a números del 1 al 9
    //obtener los valores correspondientes a la columna 
    for(int i = 0; i < 9 ; i++){
        column[i] = sudoku[i][col_id];
    }
    //verificar valores
    for (int i = 0; i < 9 ; i++){
        char digit = column[i];
        int number_index = digit - '1';
        if(!numbers[number_index]){ // si no aparece en la fila
            numbers[number_index] = true; 
        }else{ //si ya existía , es decir ya está en la fila con anterioridad , no es válido
            return false;
        }
    }
    return true;
}
bool isSubsetValid(char** subset) {
    bool numbers[9] = {false}; // 9 posiciones correspondientes a números del 1 al 9
    for (int row = 0; row < 3 ; row++){
        for(int col = 0; col < 3 ; col++){
            char digit = subset[row][col];
            int number_index = digit - '1';
            if(!numbers[number_index]){ // si no aparece en la fila
                numbers[number_index] = true; 
            }else{ //si ya existía , es decir ya está en la fila con anterioridad , no es válido
                return false;
            }
        }
    }
    return true;
}


void *columnRevision(void *arg) {
    //revisar las columnas
    for (int i = 1; i <= 9 ; i++){
        printf("Thread ID: %ld checking columns, current %d\n", syscall(SYS_gettid),i);
        bool r = isColumnValid(i);
        if (!r){
            all_col_valid = false;
        }
    }
    pthread_exit(NULL);
}

void *rowRevision(void *arg) {
    //revisar las columnas
    for (int i = 1; i <= 9 ; i++){
        printf("Thread ID: %ld checking rows, current %d\n", syscall(SYS_gettid),i);
        bool r = isRowValid(i);
        if (!r){
            all_row_valid = false;
        }
    }
    pthread_exit(NULL);
}


int main(){
    int sudoku_file = open("sudoku",O_RDONLY);
    
    
    // revisar que no haya error
    if(sudoku_file == -1){
        perror("error: could not open sudoku");
        exit(EXIT_FAILURE); //terminar el programa
    }

    //mapear y copirar el resultado

    struct stat st; //buffer para recuperar el resultado
    if (fstat(sudoku_file, &st) == -1){
        perror("error: getting attributes failed (fstat)");
    }
    
    char *addr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, sudoku_file, 0);
    
    if (addr == MAP_FAILED) {
        perror("mmap failed");
        close(sudoku_file); //cerrar el archivo
        exit(EXIT_FAILURE);
    }

    //copiar a sudoku
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            sudoku[i][j] = addr[i * 9 + j];
        }
    }

    // Mostrar el contenido del arreglo sudoku
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            printf("%c ", sudoku[i][j]);
        }
        printf("\n");
    }

    
    munmap(addr, st.st_size);
    close(sudoku_file);

    
    // Obtener el número de proceso padre
    pid_t parent_pid = getpid();

    // Crear un proceso hijo
    pid_t pid = fork();

    if (pid == -1) {
        // Error al crear el proceso hijo
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Código para el proceso hijo

        // Convertir el número del proceso padre a texto
        char parent_pid_str[16];
        snprintf(parent_pid_str, sizeof(parent_pid_str), "%d", parent_pid);

        // Ejecutar el comando ps –p <#proc> -lLf
        execlp("ps", "ps", "-p", parent_pid_str, "-lLf", NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        // Código para el proceso padre

        // Crear un hilo para realizar la revisión de columnas
        pthread_t column_thread;
        if (pthread_create(&column_thread, NULL, columnRevision, NULL) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }

        // Esperar a que el hilo termine
        if (pthread_join(column_thread, NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }

        // Espere a que concluya el hijo que está ejecutando ps
        wait(NULL);

        // Realizar la revisión de filas
        pthread_t row_thread;
        if (pthread_create(&row_thread, NULL, rowRevision, NULL) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }

        // Esperar a que el hilo termine
        if (pthread_join(row_thread, NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }



        // Ejecutar un nuevo fork() y ejecutar el comando ps en el proceso hijo
        pid_t pid2 = fork();

        if (pid2 == -1) {
            // Error al crear el segundo proceso hijo
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid2 == 0) {
            // Código para el segundo proceso hijo
            char parent_pid_str[16];
            snprintf(parent_pid_str, sizeof(parent_pid_str), "%d", parent_pid);
            execlp("ps", "ps", "-p", parent_pid_str, "-lLf", NULL);
            perror("execlp");
            exit(EXIT_FAILURE);
        } else {
            if (all_col_valid   &&  all_row_valid){
                printf("solution is valid\n");
            }else{
                printf("invalid solution\n");
            }
            wait(NULL);
        }
    }

    return 0;
}


