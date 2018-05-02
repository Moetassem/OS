//Name: Moetassem Abdelazim
//McGill ID: 260685819

#include <stdio.h>  //for printf and scanf
#include <stdlib.h> //for malloc

#define LOW 0
#define HIGH 199
#define START 53

//compare function for qsort
//you might have to sort the request array
//use the qsort function
// an argument to qsort function is a function that compares 2 quantities
//use this there.
int cmpfunc(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

//function to swap 2 integers
void swap(int *a, int *b)
{
    if (*a != *b)
    {
        *a = (*a ^ *b);
        *b = (*a ^ *b);
        *a = (*a ^ *b);
        return;
    }
}

//Prints the sequence and the performance metric
void printSeqNPerformance(int *request, int numRequest)
{
    int i, last, acc = 0;
    last = START;
    printf("\n");
    printf("%d", START);
    for (i = 0; i < numRequest; i++)
    {
        printf(" -> %d", request[i]);
        acc += abs(last - request[i]);
        last = request[i];
    }
    printf("\nPerformance : %d\n", acc);
    return;
}

//access the disk location in FCFS
void accessFCFS(int *request, int numRequest)
{
    //simplest part of assignment
    printf("\n----------------\n");
    printf("FCFS :");
    printSeqNPerformance(request, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in SSTF
void accessSSTF(int *request, int numRequest)
{
    //write your logic here
    int *updatedRequest = malloc(sizeof(int)*numRequest);

    //sorting so that we loop until we find the closest two points
    
    updatedRequest[0] = START;

    for(int i=0; i< numRequest; i++){
        updatedRequest[i+1] = request[i];
    }
    
    for (int j = 0; j < numRequest; j++)
    {
        int minDistance = HIGH;
        int minDistanceIndex = 0;
        for (int k=0; k<numRequest - j; k++){
          if (abs(request[j] - request[k]) < minDistance)
          {
              minDistance = abs(request[j] - request[k]);
              minDistanceIndex = k;
          }
        }
        swap(&updatedRequest[j],&updatedRequest[minDistanceIndex]);
        //updatedRequest[j] = request[i];
    }
    
    for(int z=0; z < numRequest; z++){
        request[z] = updatedRequest[z+1];
    }
    
    // //initialize minDistance to the distance between start and the first point in the list
    // //this way, avoided one iteration in the for loop and an if statement in the for loop
    // int minDistance = (START - request[0]);
    // //starting from the second
    // int i = 1;
    // while (i < numRequest)
    // {
    //     if (abs(START - request[i]) < minDistance)
    //     {
    //         minDistance = abs(START - request[i]);
    //         i++;
    //     }
    //     else
    //     {
    //         i--;
    //         break;
    //     }
    // }

    // int currentPoint = START;
    // for (int j = 0; j < numRequest; j++)
    // {
    //     if (abs(currentPoint - request[i]) <= abs(currentPoint - request[i + 1]))
    //     {
    //         updatedRequest[j] = request[i];
    //         currentPoint = request[i];
    //         // if (i >= 1)
    //         // {
    //         //     //request[i]= request[i-1];
    //         //     i++;
    //         // }
    //         for (int k = 0; k < i - 1; k++)
    //         {
    //             request[k + 1] = request[k];
    //         }
    //         //i--;
    //     }
    //     /**** error here*/
    //     else
    //     {
    //         currentPoint = request[i + 1];
    //         updatedRequest[j] = request[i + 1];

    //         for (int k = i + 1; k < numRequest - 1; k++)
    //         {
    //             request[k] = request[k + 1];
    //         }
    //     }
    // }

    printf("\n----------------\n");
    printf("SSTF :");
    printSeqNPerformance(request, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in SCAN
void accessSCAN(int *request, int numRequest)
{

    //write your logic here
    int updatedRequestNum = numRequest+1;
    int *midRequest = malloc((numRequest+1) * sizeof(int));
    int *updatedRequest = malloc((updatedRequestNum) * sizeof(int));

    //COPYING TO NEW ARRAY WITH Size + 1 (with START POSITION) THEN SORTING THE ARRAY   
    midRequest[0] = START;

    for(int i=0; i< numRequest; i++){
        midRequest[i+1] = request[i];
    }
    
    qsort(midRequest, numRequest+1, 1 * sizeof(int), cmpfunc);

    //finding the index of the start position in the sorted array
    int j=0; 
    while(j<numRequest+1){
        if(midRequest[j] == START){
            break;
        }
        else{
            j++;
        }
    }

    int k = j;

    int z=0;
    if (START < 100){
        while(j>0){
            updatedRequest[z] = midRequest[j-1];
            j--;
            z++;
        }
        if(z!=numRequest){
            updatedRequest[z] = LOW;
            z++;
        }
        else{
            updatedRequestNum = numRequest;
        }
        while(k<numRequest+1){
            updatedRequest[z] = midRequest[k+1];
            k++;
            z++;
        }
    }
    else{
        while(j<numRequest){
            updatedRequest[z] = midRequest[j+1];
            j++;
            z++;
        }
        if(z!=numRequest){
            updatedRequest[z] = HIGH;
            z++;
        }
        else{
            updatedRequestNum = numRequest;
        }
        while(k>0){
            updatedRequest[z] = midRequest[k-1];
            k--;
            z++;
        }
    }
    
    printf("\n----------------\n");
    printf("SCAN :");
    printSeqNPerformance(updatedRequest, updatedRequestNum);
    printf("----------------\n");
    return;
}

//access the disk location in CSCAN
void accessCSCAN(int *request, int numRequest)
{
    //write your logic here
    int updatedRequestNum = numRequest+2;
    int *midRequest = malloc((numRequest+1) * sizeof(int));
    int *updatedRequest = malloc((updatedRequestNum) * sizeof(int));

    //COPYING TO NEW ARRAY WITH Size + 1 (with START POSITION) THEN SORTING THE ARRAY   
    midRequest[0] = START;

    for(int i=0; i< numRequest; i++){
        midRequest[i+1] = request[i];
    }
    
    qsort(midRequest, numRequest+1, 1 * sizeof(int), cmpfunc);

    //finding the index of the start position in the sorted array
    int j=0; 
    while(j<numRequest+1){
        if(midRequest[j] == START){
            break;
        }
        else{
            j++;
        }
    }

    int k = numRequest;
    int startIndex = j;
    int index = 0;

    int z=0;
    if (START < 100){
        while(j>0){
            updatedRequest[z] = midRequest[j-1];
            j--;
            z++;
        }
        if(z!=numRequest){
            updatedRequest[z] = LOW;
            z++;
            updatedRequest[z] = HIGH;
            z++;
        }
        else{
            updatedRequestNum = numRequest;
        }
        while(k>startIndex){
            updatedRequest[z] = midRequest[k];
            k--;
            z++;
        }
    }
    else{
        while(j<numRequest){
            updatedRequest[z] = midRequest[j+1];
            j++;
            z++;
        }
        if(z!=numRequest){
            updatedRequest[z] = HIGH;
            z++;
            updatedRequest[z] = LOW;
            z++;
        }
        else{
            updatedRequestNum = numRequest;
        }
        while(index < startIndex){
            updatedRequest[z] = midRequest[index];
            index++;
            z++;
        }
    }

    printf("\n----------------\n");
    printf("CSCAN :");
    printSeqNPerformance(updatedRequest, updatedRequestNum);
    printf("----------------\n");
    return;
}

//access the disk location in LOOK
void accessLOOK(int *request, int numRequest)
{
    //write your logic here
    int *midRequest = malloc((numRequest+1) * sizeof(int));
    int *updatedRequest = malloc((numRequest) * sizeof(int));

    //COPYING TO NEW ARRAY WITH Size + 1 (with START POSITION) THEN SORTING THE ARRAY   
    midRequest[0] = START;

    for(int i=0; i< numRequest; i++){
        midRequest[i+1] = request[i];
    }
    
    qsort(midRequest, numRequest+1, 1 * sizeof(int), cmpfunc);

    //finding the index of the start position in the sorted array
    int j=0; 
    while(j<numRequest+1){
        if(midRequest[j] == START){
            break;
        }
        else{
            j++;
        }
    }

    int k = j;

    int z=0;
    if (START < 100){
        while(j>0){
            updatedRequest[z] = midRequest[j-1];
            j--;
            z++;
        }

        while(k<numRequest+1){
            updatedRequest[z] = midRequest[k+1];
            k++;
            z++;
        }
    }
    else{
        while(j<numRequest){
            updatedRequest[z] = midRequest[j+1];
            j++;
            z++;
        }
        while(k>0){
            updatedRequest[z] = midRequest[k-1];
            k--;
            z++;
        }
    }

    printf("\n----------------\n");
    printf("LOOK :");
    printSeqNPerformance(updatedRequest, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in CLOOK
void accessCLOOK(int *request, int numRequest)
{
    //write your logic here
    //write your logic here
    int *midRequest = malloc((numRequest+1) * sizeof(int));
    int *updatedRequest = malloc((numRequest) * sizeof(int));

    //COPYING TO NEW ARRAY WITH Size + 1 (with START POSITION) THEN SORTING THE ARRAY   
    midRequest[0] = START;

    for(int i=0; i< numRequest; i++){
        midRequest[i+1] = request[i];
    }
    
    qsort(midRequest, numRequest+1, 1 * sizeof(int), cmpfunc);

    //finding the index of the start position in the sorted array
    int j=0; 
    while(j<numRequest+1){
        if(midRequest[j] == START){
            break;
        }
        else{
            j++;
        }
    }

    int k = numRequest;
    int startIndex = j;
    int index = 0;

    int z=0;
    if (START < 100){
        while(j>0){
            updatedRequest[z] = midRequest[j-1];
            j--;
            z++;
        }

        while(k>startIndex){
            updatedRequest[z] = midRequest[k];
            k--;
            z++;
        }
    }
    else{
        while(j<numRequest){
            updatedRequest[z] = midRequest[j+1];
            j++;
            z++;
        }
        while(index < startIndex){
            updatedRequest[z] = midRequest[index];
            index++;
            z++;
        }
    }

    printf("\n----------------\n");
    printf("CLOOK :");
    printSeqNPerformance(updatedRequest, numRequest);
    printf("----------------\n");
    return;
}

int main()
{
    int *request, numRequest, i, ans;

    //allocate memory to store requests
    printf("Enter the number of disk access requests : ");
    scanf("%d", &numRequest);
    request = malloc(numRequest * sizeof(int));

    printf("Enter the requests ranging between %d and %d\n", LOW, HIGH);
    for (i = 0; i < numRequest; i++)
    {
        scanf("%d", &request[i]);
    }

    printf("\nSelect the policy : \n");
    printf("----------------\n");
    printf("1\t FCFS\n");
    printf("2\t SSTF\n");
    printf("3\t SCAN\n");
    printf("4\t CSCAN\n");
    printf("5\t LOOK\n");
    printf("6\t CLOOK\n");
    printf("----------------\n");
    scanf("%d", &ans);

    switch (ans)
    {
    //access the disk location in FCFS
    case 1:
        accessFCFS(request, numRequest);
        break;

    //access the disk location in SSTF
    case 2:
        accessSSTF(request, numRequest);
        break;

        //access the disk location in SCAN
    case 3:
        accessSCAN(request, numRequest);
        break;

        //access the disk location in CSCAN
    case 4:
        accessCSCAN(request, numRequest);
        break;

    //access the disk location in LOOK
    case 5:
        accessLOOK(request, numRequest);
        break;

    //access the disk location in CLOOK
    case 6:
        accessCLOOK(request, numRequest);
        break;

    default:
        break;
    }
    return 0;
}