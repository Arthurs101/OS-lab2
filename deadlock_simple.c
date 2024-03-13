#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t printing_resource;
pthread_mutex_t web_resource;

void *process1(void *arg) {
    while (1) {
        // Intenta adquirir el recurso de impresión
        pthread_mutex_lock(&printing_resource);
        printf("Proceso 1 adquirió el recurso de impresión\n");
        sleep(1);
        
        // Intenta adquirir el recurso web
        pthread_mutex_lock(&web_resource);
        printf("Proceso 1 adquirió el recurso web\n");
 
        // Liberar recursos  (plot twist no sucederá)
        pthread_mutex_unlock(&web_resource);
        pthread_mutex_unlock(&printing_resource);
    }
    return NULL;
}

void *process2(void *arg) {
    while (1) {
        // Intenta adquirir el recurso web
        pthread_mutex_lock(&web_resource);
        printf("Proceso 2 adquirió el recurso web\n");
        sleep(1);
        
        // Intenta adquirir el recurso de impresión
        pthread_mutex_lock(&printing_resource);
        printf("Proceso 2 adquirió el recurso de impresión\n");

        // Liberar recursos
        pthread_mutex_unlock(&printing_resource);
        pthread_mutex_unlock(&web_resource);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;

    // Inicializar mutexes
    pthread_mutex_init(&printing_resource, NULL);
    pthread_mutex_init(&web_resource, NULL);

    // Crear procesos
    pthread_create(&tid1, NULL, process1, NULL);
    pthread_create(&tid2, NULL, process2, NULL);

    // Esperar a que los procesos terminen (nunca sucederá)
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
	
    printf("tasks completed");
    return 0;
}

