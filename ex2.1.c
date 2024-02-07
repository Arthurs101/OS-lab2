#include <stdio.h>
#include <time.h>

int main() {
    clock_t start_time, end_time;
    double elapsed_time;

    // Primer clock antes del primer ciclo
    start_time = clock();

    for (int i = 0; i < 1000000; ++i) {}

    // Segundo ciclo for
    for (int i = 0; i < 1000000; ++i) {}

    for (int i = 0; i < 1000000; ++i) {}

    end_time = clock();

    elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Tiempo transcurrido: %f segundos\n", elapsed_time);
    return 0;
}
