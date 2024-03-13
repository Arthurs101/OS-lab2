#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5

pthread_mutex_t forks[NUM_PHILOSOPHERS];
pthread_t philosophers[NUM_PHILOSOPHERS];

void *philosopher(void *arg) {
    int id = *((int *) arg);
    int right = id;
    int left = (id + 1) % NUM_PHILOSOPHERS;

    while (1) {
        // Pensar
        printf("Philosopher %d is thinking\n", id);
        sleep(rand() % 5 + 1);

        // Intentar tomar los tenedores
        printf("Philosopher %d is hungry\n", id);
        pthread_mutex_lock(&forks[right]);
        printf("Philosopher %d took fork %d\n", id, right);
        pthread_mutex_lock(&forks[left]);
        printf("Philosopher %d took fork %d\n", id, left);

        // Comer
        printf("Philosopher %d is eating\n", id);
        sleep(rand() % 5 + 1);

        // Liberar los tenedores
        pthread_mutex_unlock(&forks[right]);
        printf("Philosopher %d released fork %d\n", id, right);
        pthread_mutex_unlock(&forks[left]);
        printf("Philosopher %d released fork %d\n", id, left);
    }
}

int main() {
    // Inicializar los mutexes de los tenedores
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    // Crear los hilos para los filósofos
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&philosophers[i], NULL, philosopher, id);
    }

    // Esperar a que los filósofos terminen (esto nunca sucede)
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    // Liberar los mutexes de los tenedores
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&forks[i]);
    }

    return 0;
}
