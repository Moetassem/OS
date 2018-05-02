/*
 ----------------- COMP 310/ECSE 427 Winter 2018 -----------------
 Dimitri Gallos
 Assignment 2 skeleton
 
 -----------------------------------------------------------------
 I declare that the awesomeness below is a genuine piece of work
 and falls under the McGill code of conduct, to the best of my knowledge.
 -----------------------------------------------------------------
 */

//Please enter your name and McGill ID below
//Name: Moetassem Abdelazim
//McGill ID: 260685819

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>

int BUFFER_SIZE = 100; //size of queue
sem_t full;
sem_t empty;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// A structure to represent a queue
struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int *array;
};

// function to create a queue of given capacity.
// It initializes size of queue as 0
struct Queue *createQueue(unsigned capacity)
{
    struct Queue *queue = (struct Queue *)malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1; // This is important, see the enqueue
    queue->array = (int *)malloc(queue->capacity * sizeof(int));
    return queue;
}

// Queue is full when size becomes equal to the capacity
int isFull(struct Queue *queue)
{
    return ((queue->size) >= queue->capacity);
}

// Queue is empty when size is 0
int isEmpty(struct Queue *queue)
{
    return (queue->size == 0);
}

// Function to add an item to the queue.
// It changes rear and size
void enqueue(struct Queue *queue, int item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue.
// It changes front and size
int dequeue(struct Queue *queue)
{
    if (isEmpty(queue))
        return 0;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int front(struct Queue *queue)
{
    if (isEmpty(queue))
        return 0;
    return queue->array[queue->front];
}

// Function to get rear of queue
int rear(struct Queue *queue)
{
    if (isEmpty(queue))
        return 0;
    return queue->array[queue->rear];
}

void print(struct Queue *queue)
{
    if (queue->size == 0)
    {
        return;
    }

    for (int i = queue->front; i < queue->front + queue->size; i++)
    {

        printf(" Element at position %d is %d \n ", i % (queue->capacity), queue->array[i % (queue->capacity)]);
    }
}

struct Queue *queue;

// creats Id for passenger and uses chars to combine plane id with passenger Id
int createPassengerId(int planeId)
{
    int passengerId;
    //max of 1000 passengers
    passengerId = (rear(queue) + 1) % 1000;
    //putting passenger id as chars in passenger
    char passenger[3];
    sprintf(passenger, "%03d", passengerId);
    //same for plane id in plane
    char plane[3];
    sprintf(plane, "%03d", planeId);

    //putting together
    char finalId[7];
    sprintf(finalId, "1%s%s", plane, passenger);
    //and then bringing back to int
    int compiledId = atoi(finalId);
    return compiledId;
}

/*Producer Function: Simulates an Airplane arriving and dumping 5-10 passengers to the taxi platform */
void *FnAirplane(void *cl_id)
{
    int *airplaneNumber = (int *)cl_id;
    //comment the line below (and line under sleep(1)) and change the condition of while loop to 1 if you want to run forever
    int x=10;
    while (x>0)
    {
        printf("Airplane %d arrives\n", *airplaneNumber);
        //generate a random number between 5 and 10 to represent the people taking taxis from a certain plane
        int randomPassengersfromPlane = (rand() % 5 + 5);
        //go over loop for each passenger taking a taxi
        for (int i = 0; i < randomPassengersfromPlane; i++)
        {
            //if queue is full then the rest of passengers on this plane will go to the bus 
            //and therefore we'll move on to the next plane in the next hour
            if (isFull(queue))
            {
                printf("Platform is full: Rest of passengers of plane %d take the bus\n", *airplaneNumber);
                break;
            }
            
            //decrementing empty semaphore
            sem_wait(&empty);

            //critical section
            pthread_mutex_lock(&mutex); //locking to enter section
            int passengerId = createPassengerId(*airplaneNumber);
            enqueue(queue, passengerId); //adding passenger to queue
            printf("Passenger %d of airplane %d arrives to platform\n", passengerId, *airplaneNumber);
            pthread_mutex_unlock(&mutex); //unlocking at the end of section
            //end of critical section

            //incrementing full semaphore
            sem_post(&full);
        }

        //sleep for 1 hr (1sec)
        sleep(1);
        x--;
    }
}

/* Consumer Function: simulates a taxi that takes n time to take a passenger home and come back to the airport */
void *FnTaxi(void *pr_id)
{
    int *taxiNumber = (int *)pr_id;
    
    while (1)
    {

        printf("Taxi driver %d arrives\n", *taxiNumber);
        //if queue is empty then taxi will wait for passengers to come using the while loop in the if statement
        if (isEmpty(queue))
        {
            printf("Taxi driver %d waits for passengers to enter the platform\n", *taxiNumber);
            while (isEmpty(queue));
        }

        //decrementing full semaphore
        sem_wait(&full);

        //getting passenger from top of queue cuz FIFO in lane for taxis 
        //before entering the critical section where passenger is dequeued 
        int passengerId = front(queue);

        //critical section
        pthread_mutex_lock(&mutex); //locking to enter
        dequeue(queue); 
        pthread_mutex_unlock(&mutex); //unlocking
        //done critical section
        
        
        //incrementing empty semaphore
        sem_post(&empty);
        
        printf("Taxi %d picked up client %d from the platform\n", *taxiNumber, passengerId);

        //sleeping from 10 to 30 min (0.166666 to 0.5 sec)
        int randomSleepTime = ((rand() % 20) + 10);
        usleep((randomSleepTime/60) * 1000000);
    }
}

int main(int argc, char *argv[])
{

    int num_airplanes;
    int num_taxis;

    num_airplanes = atoi(argv[1]);
    num_taxis = atoi(argv[2]);

    printf("You entered: %d airplanes per hour\n", num_airplanes);
    printf("You entered: %d taxis\n", num_taxis);

    //initialize queue
    queue = createQueue(BUFFER_SIZE);

    //declare arrays of threads and initialize semaphore(s)
    pthread_t *taxiThreads = malloc(sizeof(pthread_t) * num_taxis);
    pthread_t *airplaneThreads = malloc(sizeof(pthread_t) * num_airplanes);
    pthread_mutex_init(&mutex, NULL);

    //note: program would exit/return 0 if semaphores are not initialized properly

    //producer semaphore initially =0
    //increment when adding people to buffer and decrement when taking out from buffer
    if (sem_init(&full, 0, 0) < 0)
        return 0;

    //consumer semaphore initially = buffer size = 100
    //opposite to producer semaphore
    if ((sem_init(&empty, 0, BUFFER_SIZE) < 0))
        return 0;

    //create arrays of integer pointers to ids for taxi / airplane threads
    int *taxi_ids[num_taxis];
    int *airplane_ids[num_airplanes];

    //create threads for airplanes
    for (int i = 0; i < num_airplanes; i++)
    {
        airplane_ids[i] = malloc(sizeof(int));
        *airplane_ids[i] = i;

        //error message if creation of thread is not successful
        if (pthread_create(&airplaneThreads[i], NULL, FnAirplane, airplane_ids[i]) != 0)
        {
            printf("Error : can not create airplane thread %i\n", i);
        }
        else
        {
            printf("creating airplane thread %d\n", i);
        }
    }
    //create threads for taxis
    for (int j = 0; j < num_taxis; j++)
    {
        taxi_ids[j] = malloc(sizeof(int));
        *taxi_ids[j] = j;

        //error message if creation of thread is not successful
        if (pthread_create(&taxiThreads[j], NULL, FnTaxi, taxi_ids[j]) != 0)
        {
            printf("Error : can not create taxi thread %i\n", j);
        }
        else
        {
            printf("creating taxi thread %d\n", j);
        }
    }
    //join airplane threads at the end
    for (int i = 0; i < num_airplanes; i++)
    {
        pthread_join(airplaneThreads[i], NULL);
    }
    //join taxi threads at the end
    for (int j = 0; j < num_taxis; j++)
    {
        pthread_join(taxiThreads[j], NULL);
    }
    pthread_exit(NULL);
}