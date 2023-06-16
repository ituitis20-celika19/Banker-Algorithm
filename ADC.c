//150190112 - Arda Deniz Çelik
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    int* resources;
    int** allocations;
    int** requests;
} SystemState;

void initializeSystemState(SystemState* system, int numProcesses, int numResources) {
    // Allocate memory for resources, allocations, and requests
    system->resources = (int*)malloc(numResources * sizeof(int));
    system->allocations = (int**)malloc(numProcesses * sizeof(int*));
    system->requests = (int**)malloc(numProcesses * sizeof(int*));

    for (int i = 0; i < numProcesses; i++) {
        // Allocate memory for each process's allocations and requests
        system->allocations[i] = (int*)malloc(numResources * sizeof(int));
        system->requests[i] = (int*)malloc(numResources * sizeof(int));
    }
}

void freeSystemState(SystemState* system, int numProcesses) {
    // Free allocated memory for resources, allocations, and requests
    free(system->resources);

    for (int i = 0; i < numProcesses; i++) {
        // Free memory for each process's allocations and requests
        free(system->allocations[i]);
        free(system->requests[i]);
    }

    free(system->allocations);
    free(system->requests);
}

void readResourceFile(FILE* file, int* resources, int numResources) {
    // Read resource values from the file
    for (int i = 0; i < numResources; i++) {
        fscanf(file, "%d", &resources[i]);
    }
}

void readAllocationFile(FILE* file, int** allocations, int numProcesses, int numResources) {
    // Read allocation matrix from the file
    for (int i = 0; i < numProcesses; i++) {
        for (int j = 0; j < numResources; j++) {
            fscanf(file, "%d", &allocations[i][j]);
        }
    }
}

void readRequestFile(FILE* file, int** requests, int numProcesses, int numResources) {
    // Read request matrix from the file
    for (int i = 0; i < numProcesses; i++) {
        for (int j = 0; j < numResources; j++) {
            fscanf(file, "%d", &requests[i][j]);
        }
    }
}

void printProcessInfo(int processId, int numResources, int* allocations, int* requests) {
    // Print process information (allocated resources and resource requests)
    printf("Information for Process P%d:\n", processId + 1);
    printf("Allocated Resources: ");
    for (int j = 0; j < numResources; j++) {
        printf("R%d:%d ", j, allocations[j]);
    }
    printf("\nResource Request: ");
    for (int j = 0; j < numResources; j++) {
        printf("R%d:%d ", j, requests[j]);
    }
    printf("\n\n");
}

void printRunningOrder(int* processOrder, int count) {
    // Print the running order of the processes
    printf("Running Order for Processes: ");
    for (int i = 0; i < count; i++) {
        printf("P%d ", processOrder[i] + 1);
    }
    printf("\n");
}

void printDeadlockCause(bool* isExecutionFinished, int numProcesses) {
    // Print the processes that are causing the deadlock
    printf("There is a deadlock. ");
    for (int i = 0; i < numProcesses; i++) {
        if (!isExecutionFinished[i]) {
            printf("P%d ", i + 1);
        }
    }
    printf("is/are the cause of deadlock.\n");
}

void bankerAlgorithm(SystemState* system, int numProcesses, int numResources) {
    // Allocate memory for execution status and process order
    bool* isExecutionFinished = (bool*)malloc(numProcesses * sizeof(bool));
    int* processOrder = (int*)malloc(numProcesses * sizeof(int));

    // Initialize execution status and update available resources
    for (int i = 0; i < numProcesses; i++) {
        isExecutionFinished[i] = false;

        for (int j = 0; j < numResources; j++) {
            system->resources[j] -= system->allocations[i][j];
        }
    }

    int count = 0;

    // Execute the banker's algorithm
    while (count < numProcesses) {
        bool found = false;

        // Find a process that can be executed
        for (int i = 0; i < numProcesses; i++) {
            if (!isExecutionFinished[i]) {
                bool canExecute = true;

                // Check if the process can be executed based on available resources
                for (int j = 0; j < numResources; j++) {
                    if (system->requests[i][j] > system->resources[j]) {
                        canExecute = false;
                        break;
                    }
                }

                if (canExecute) {
                    // Update available resources and mark the process as executed
                    for (int j = 0; j < numResources; j++) {
                        system->resources[j] += system->allocations[i][j];
                    }

                    isExecutionFinished[i] = true;
                    processOrder[count++] = i;
                    found = true;
                }
            }
        }

        // If no process can be executed, print the deadlock cause and return
        if (!found) {
            printRunningOrder(processOrder, count);
            printDeadlockCause(isExecutionFinished, numProcesses);

            free(isExecutionFinished);
            free(processOrder);
            return;
        }
    }

    // Print the running order of all processes
    printRunningOrder(processOrder, numProcesses);

    free(isExecutionFinished);
    free(processOrder);
}

int main() {
    FILE* resourceFile = fopen("resources.txt", "r");
    FILE* allocationFile = fopen("allocations.txt", "r");
    FILE* requestFile = fopen("requests.txt", "r");

    int numProcesses = 0;
    int numResources = 0;

    char ch;
    int check = 0;

    // Count the number of processes and resources
    while ((ch = fgetc(requestFile)) != EOF) {
        if (ch == '\n') {
            numProcesses++;
            check = 1;
        } else if (ch == ' ') {
            if (check == 0) {
                numResources++;
            }
        }
    }

    numResources++;
    numProcesses++;

    SystemState system;
    initializeSystemState(&system, numProcesses, numResources);

    rewind(requestFile); // Reset file pointer to the beginning

    // Read resource values, allocation matrix, and request matrix
    readResourceFile(resourceFile, system.resources, numResources);
    readAllocationFile(allocationFile, system.allocations, numProcesses, numResources);
    readRequestFile(requestFile, system.requests, numProcesses, numResources);

    fclose(resourceFile);
    fclose(allocationFile);

    rewind(requestFile); // Reset file pointer to the beginning

    // Read request matrix again (as mentioned in the requirements)
    readRequestFile(requestFile, system.requests, numProcesses, numResources);

    fclose(requestFile);

    // Print process information (allocated resources and resource requests)
    for (int i = 0; i < numProcesses; i++) {
        printProcessInfo(i, numResources, system.allocations[i], system.requests[i]);
    }

    // Execute the banker's algorithm
    bankerAlgorithm(&system, numProcesses, numResources);

    // Free allocated memory
    freeSystemState(&system, numProcesses);

    return 0;
}
