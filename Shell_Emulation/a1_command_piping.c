#include <stdio.h>
#include <unistd.h>
int main()
{
    int handle[2];
    if (pipe(handle)){
        return 0;
    }

    if (fork() == 0)
    {
        //inside child : execute ls using execvp

        //close stdout
        close(1);

        //copy write end of pipe to 1
        dup2(handle[1], 1);

        char *cmd = "ls";
        char *argv[2];
        argv[0] = "ls";
        argv[1] = NULL;
        execvp(cmd, argv);
    }
    
    else
    {
        //inside parent : read from pipe
        
        char writtenIntoPipe[128];

        //read end of the pipe
        read(handle[0], writtenIntoPipe, 128);
        
        //print into slot 1 
        //comment the following line to make sure printing is from the parent (testing way)
        printf("%s", writtenIntoPipe);
    }
    return 0;
}