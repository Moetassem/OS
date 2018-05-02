#include<stdio.h> 
#include<unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {

//create file, write capability
FILE* file_ptr = fopen("redirect_out.txt", "w");

//print to screen (stdout)
printf("First : Print to stdout\n");

//get handle/slot number of the new file
int fileHandleNumber = open("redirect_out.txt", O_WRONLY);

//copy stdout to the next available slot
int defOutCopy = dup(1);

//make slot 1 available
close (1);

//copy file handle to the default stdout handle (1)
dup2(fileHandleNumber, 1);

//print now which will be in the file redirect_out.txt
printf ("Second : Print to redirect_out.txt\n");

//bring back to initial state by closing slots 
//and using the copies we did to bring them back
//to the initial slots 
close(1);
close (fileHandleNumber);
dup2(defOutCopy, 1);
close(defOutCopy);

//therefore bring back to screen print (stdout)
printf ("Third : Print to stdout\n");

return 0; 
}