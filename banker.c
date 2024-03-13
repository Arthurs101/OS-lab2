//el siguiente algoritmo es adaptación de lo hallado en Geeks For Geeks
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_PROCESSES 5
#define NUM_RESOURCES 3
#define bool int
#define true  1
#define false 0
int alloc[NUM_PROCESSES][NUM_RESOURCES] = {
        {1, 0, 0},
        {0, 2, 0},
        {0, 0, 3},
        {3, 0, 1},
        {0, 2, 0}
};

int max[NUM_PROCESSES][NUM_RESOURCES] = {
    {7, 5, 3},
    {3, 2, 2},
    {9, 3, 2},
    {2, 2, 2},
    {4, 3, 3}
};

int avail[NUM_RESOURCES] = {20, 10, 12};
int finish[NUM_PROCESSES] = {0};
int safe_sequence[NUM_PROCESSES];
int work[NUM_RESOURCES];
int need[NUM_PROCESSES][NUM_RESOURCES];

//devolver los recursos , es decir liberarlo
void free_alloc(int p_id){
    avail[0] += alloc[p_id][0];
    avail[1] += alloc[p_id][1];
    avail[2] += alloc[p_id][2];
    
    need[p_id][0] = 0;
    need[p_id][1] = 0;
    need[p_id][2] = 0;

    //liberar los recursos
    alloc[p_id][0] = 0;
    alloc[p_id][1] = 0;
    alloc[p_id][2] = 0;
}

bool findSafeSecuence(){
    int count = 0; // Count of finished processes
    while (count < NUM_PROCESSES) {
        int found = 0; // Flag to indicate if a safe process was found in this iteration
        for (int i = 0; i < NUM_PROCESSES; ++i) {
            if (!finish[i]) {
                int j;
                for (j = 0; j < NUM_RESOURCES; ++j) {
                    if (need[i][j] > work[j]) {
                        break; // Unsafe, skip this process
                    }
                }
                if (j == NUM_RESOURCES) { // Found a safe process
                    found = 1;
                    safe_sequence[count++] = i;
                    for (j = 0; j < NUM_RESOURCES; ++j) {
                        work[j] += alloc[i][j];
                    }
                    finish[i] = 1; // Mark process as finished
                }
            }
        }
        if (!found) {
            printf("OS:\tPossible Deadlock detected or no safe sequence found\n");
            return false;
            break;
        }
    }
    // Print the safe sequence
    printf("OS:\tSafe sequence found for giving resources: ");
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        if(i < NUM_PROCESSES -1){printf("P%d->", safe_sequence[i]);}
        else{printf("P%d->", safe_sequence[i]);}
    }
    printf("\n");
    return true;
}

bool requestResources(int p_id, int requests[NUM_RESOURCES]){
    printf("\033[1;37m");
    for (int i=0;i < NUM_RESOURCES; i++ ){ //verificar que no solicite más de lo que necesita
        if(requests[i] > need[p_id][i]){
            printf("OS:\tProcess P%d requested more than needed \n",p_id);
            return false;
        }
        if(requests[i] > avail[i]){
            printf("OS:\tProcess P%d requested more than is available \n",p_id);
            return false;
        }
        if(requests[i] > max[p_id][i]){
            printf("OS:\tProcess P%d requested more than it has allowed \n",p_id);
            return false;
        }
    }
    // give the resources
    printf("OS:\tGiving resources to P%d \n",p_id);
    for (int res = 0; res < NUM_RESOURCES ; res++){
        need[p_id][res] -= requests[res];
        avail[res] -= requests[res];
        alloc[p_id][res] += requests[res];
    }
    //check for deadlock
    if (findSafeSecuence()){
        return true; //there is no problem giving the resources
    }else{
        //hault and roll back
        printf("OS:\tgiving resources to P%d has been halted, deadlock adverted\n",p_id);
        printf("\033[0m");
        for (int res = 0; res < NUM_RESOURCES ; res++){
            need[p_id][res] += requests[res];
            avail[res] += requests[res];
            alloc[p_id][res] -= requests[res];
        }
        return false;
    }
}

void calculateNeeds(){
    // Calculating the need matrix
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        for (int j = 0; j < NUM_RESOURCES; ++j) {
            need[i][j] = max[i][j] - alloc[i][j];
        }
    }
}

void calculateWork(){
    // Initialize work to available
    for (int i = 0; i < NUM_RESOURCES; ++i) {
        work[i] = avail[i];
    }
}

int main() {
    calculateNeeds();
    calculateWork();
    int clocks = 0;
    printf("initializing simulations \n");
    srand(time(NULL));
    int random_process;
    while (clocks <10)
    {
        random_process= rand() % NUM_PROCESSES;
        int random_action = rand()%4;
        switch (random_action)
        {
        case 0:
            printf("P%d:\tfinished,resources freed\n",random_process);
            free_alloc(random_process);
            break;
        
        default:
            printf("P%d:\tasking for resources \n",random_process);
            int request[NUM_RESOURCES];
            for (int res= 0; res < NUM_RESOURCES; res++){
                request[res] = rand()%2;
            }
            if (requestResources(random_process,request)){
                printf("P%d:\tresources got\n",random_process);
            }else{
                printf("P%d:\tresources denied by OS\n",random_process);
            }
            break;
        } 
        clocks++;
    }  
    return 0;
}
