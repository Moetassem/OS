//Name: Moetassem Abdelazim
//McGill ID: 260685819

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

//Global Variable Declarations
int num_processes;
int num_resources;
int *availableResources;           //AVAIL in tutorial slides
int *processMaxRequestPerResource; //Max in tutorial slides
int *HOLD;
int *NEED;

//mutex declaration
pthread_mutex_t MUTEX = PTHREAD_MUTEX_INITIALIZER;

/*
Simulates resource requests by processes 
*/
void request_simulator(int pr_id, int *request_vector)
{
    printf("requesting resources for process %i \n", pr_id);
    for (int i = 0; i < num_resources; i++)
    {
        int amountOfResNeedToTerminate = NEED[pr_id, i];
        int randomResRequestOfType = rand() % (amountOfResNeedToTerminate + 1);
        request_vector[i] = randomResRequestOfType;
    }
}
/*
Implementation of isSafe() as described in the slides
*/
int isSafe()
{
    int isSafe = 0;
    int *Work;
    bool *Finish;

    Work = malloc(sizeof(int) * num_resources);
    Finish = malloc(sizeof(bool) * num_processes);

    for (int i = 0; i < num_resources; i++)
    {
        Work[i] = availableResources[i];
    }

    for (int j = 0; j < num_processes; j++)
    {
        Finish[j] = false;
    }

    for (int k = 0; k < num_processes; k++)
    {
        bool needsFoundAndProcessNotDone = true;
        if (Finish[k] == true)
        {
            needsFoundAndProcessNotDone = false;
        }
        for (int z = 0; z < num_resources; z++)
        {
            if (NEED[k, z] > Work[z])
            {
                needsFoundAndProcessNotDone = false;
            }
        }
        if (needsFoundAndProcessNotDone)
        {
            for (int f = 0; f < num_resources; f++)
            {
                Work[f] = Work[f] + HOLD[k, f];
            }
            Finish[k] = true;
        }
    }

    for (int m = 0; m < num_processes; m++)
    {
        if (Finish[m] == false)
        {
            return 0;
        }
    }
    isSafe = 1;

    return isSafe;
}
/*
Implementation of Bankers Algorithm as described in the slides
returns 1 if safe allocation 0 if not safe
*/
int bankers_algorithm(int pr_id, int *request_vector)
{

    printf("process %i request vector:", pr_id);
    for (int i = 0; i < num_resources; i++)
    {
        printf("%i", request_vector[i]);
    }
    printf("\n");

    pthread_mutex_lock(&MUTEX);
    for (int i = 0; i < num_resources; i++)
    {
        if (request_vector[i] > NEED[pr_id, i])
        {
            pthread_mutex_unlock(&MUTEX);
            printf("Error --> Impossible!? \n");
            return -1;
        }

        else if (request_vector[i] > availableResources[i])
        {
            pthread_mutex_unlock(&MUTEX);
            printf("Process %i mus wait. \n", pr_id);
            return 0;
        }

        else
        {
            availableResources[i] = availableResources[i] - request_vector[i];
            HOLD[pr_id, i] = HOLD[pr_id, i] + request_vector[i];
            NEED[pr_id, i] = NEED[pr_id, i] - request_vector[i];
        }
    }

    int safeState = 0;
    safeState = isSafe();
    printf("Checking if allocation is safe. \n");

    if (safeState == 1)
    {
        printf("System is safe: allocating! \n");
        pthread_mutex_unlock(&MUTEX);
        return 0;
    }
    else
    {
        printf("Allocation is not safe, cancelling! \n");
        for (int k = 0; k < num_resources; k++)
        {
            availableResources[k] = availableResources[k] + request_vector[k];
            HOLD[pr_id, k] = HOLD[pr_id, k] - request_vector[k];
            NEED[pr_id, k] = NEED[pr_id, k] + request_vector[k];
        }

        pthread_mutex_unlock(&MUTEX);
        return 0;
    }
}

/*
Simulates processes running on the system.
*/
void *process_simulator(void *pr_id)
{

    int *processID = (int *)pr_id;
    while (1)
    {
        int *processResourceRequests = malloc(sizeof(int) * num_resources);
        request_simulator(*processID, processResourceRequests);
        bankers_algorithm(*processID, processResourceRequests);

        bool completed = true;
        for (int i = 0; i < num_resources; i++)
        {
            if (NEED[processID, i] > 0)
            {
                completed = false;
            }
        }

        if (completed)
        {
            for (int j = 0; j < num_resources; j++)
            {
                availableResources[j] = availableResources[j] + processResourceRequests[j];
                HOLD[processID, j] = 0;
                NEED[processID, j] = 0;
            }
            printf("Process %i completed!\n", *processID);
            break;
        }
        else
        {
            printf("Process %d didn't complete, will try again after 3 seconds\n", *processID);
            sleep(3);
        }
    }
}

/*
Simulates a fault occuring on the system.

*/
void *fault_simulator(void *pr_id)
{
    while (true)
    {
        int resourceToReduce = rand() % num_resources;
        bool probability = rand() % 11;
        if (probability > 5)
        {
            pthread_mutex_lock(&MUTEX);
            availableResources[resourceToReduce] = availableResources[resourceToReduce] - 1;
            printf("Reducing from resource %i \n", resourceToReduce);
            pthread_mutex_unlock(&MUTEX);
        }
        sleep(10);
    }
}

/*
Checks for deadlock
*/
void *deadlock_checker()
{
    while(true){
        //if for each process, need[i][j] > hold[i][j] + avail[j] DEADLOCK WILL OCCUR
        pthread_mutex_lock(&MUTEX);   
        int* Holding = malloc(sizeof(int) * num_resources);
        for(int i=0; i<num_processes; i++){
            for(int j=0; j<num_resources; j++){
                Holding[j] = Holding[j] + HOLD[i,j]; 
            }
        }
        for(int k=0; k<num_processes; k++){
            for(int m=0; m<num_resources; m++){
                pthread_mutex_unlock(&MUTEX);   
                if (NEED[k,m] > HOLD[k,m] + availableResources[k]){
                    printf("Deadlock occured, exiting...\n");
                    exit(0);
                }
            }
        }
        sleep(10);
    }
}

int main()
{
    //Initialize all inputs to banker's algorithm

    printf("Enter number of processes: \n");
    scanf("%d", &num_processes);
    printf("Enter number of resources: \n");
    scanf("%d", &num_resources);

    availableResources = malloc(sizeof(int) * num_resources);

    printf("Enter Available Resources: \n");
    for (int i = 0; i < num_resources; i++)
    {
        scanf("%d", &availableResources[i]);
    }

    processMaxRequestPerResource = malloc(sizeof(int) * num_processes * num_resources);
    NEED = malloc(sizeof(int) * num_processes * num_resources);

    printf("Enter Maximum Resources Each Process Can Claim: \n");
    for (int j = 0; j < num_processes; j++)
    {
        for (int k = 0; k < num_resources; k++)
        {
            scanf("%d", &processMaxRequestPerResource[j, k]);
            NEED[j, k] = processMaxRequestPerResource[j, k];
        }
    }HOLD_ALL[j]

    HOLD = malloc(sizeof(int) * num_processes * num_resources);

    for (int j = 0; j < num_processes; j++)
    {
        for (int k = 0; k < num_resources; k++)
        {
            HOLD[j, k] = 0;
        }
    }

    //attempt to parse

    //const char space[1]= " ";
    // char *nextAvailResOfType = strtok(allAvailableResToParse,space);

    // for (int i=0; (i<num_resources) && (nextAvailResOfType != NULL); i++){
    //     availableResources[i] = atoi(nextAvailResOfType);
    //     //printf("[^\n]", nextAvailResOfType);
    //     nextAvailResOfType = strtok(NULL, space);
    // }

    pthread_mutex_init(&MUTEX, NULL);

    //create threads simulating processes (process_simulator)
    pthread_t *processThreads = malloc(sizeof(pthread_t) * num_processes);

    //create arrays of integer pointers to ids for process threads
    int *process_ids[num_processes];

    for (int i = 0; i < num_processes; i++)
    {
        process_ids[i] = malloc(sizeof(int));
        *process_ids[i] = i;

        //error message if creation of thread is not successful
        if (pthread_create(&processThreads[i], NULL, process_simulator, process_ids[i]) != 0)
        {
            printf("Error : can not create process %i\n", i);
        }
        else
        {
            printf("creating process %d\n", i);
        }
    }

    // create a thread that takes away resources from the available pool (fault_simulator)
    pthread_t fault_thread;

    if (pthread_create(&fault_thread, NULL, fault_simulator, NULL))
    {
        printf("Error with pthread create for faulty thread\n");
    }

    //create a thread to check for deadlock (deadlock_checker)
    pthread_t deadlock_thread;

    if (pthread_create(&deadlock_thread, NULL, deadlock_checker, NULL))
    {
        printf("Error with pthread create for deadlock checker thread\n");
    }

    for (int i = 0; i < num_processes; i++)
    {
        pthread_join(processThreads[i], NULL);
        pthread_mutex_init(&MUTEX, NULL);
    }

    pthread_join(fault_thread, NULL);
    pthread_join(deadlock_thread, NULL);

    pthread_exit(NULL);
    return 0;
}