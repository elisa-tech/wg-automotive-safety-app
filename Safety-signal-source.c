#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
// Needed for random numbers
#include <stdlib.h>
//needed for mkfifo function
#include <sys/types.h>
#include <sys/stat.h>
int main()
{
    const char* Pipe="./Pipe";
    unsigned int MC=0;
    unsigned char Message[6];
    
    const bool test_checksum=false;
    const bool test_skip_message=false;

    mkfifo(Pipe,0666);
    int fd=open(Pipe, O_WRONLY);
    while (1){
        Message[0]=1;
        Message[1] = (MC >> 24) & 0xFF;
        Message[2] = (MC >> 16) & 0xFF;
        Message[3] = (MC >> 8) & 0xFF;
        Message[4] = MC & 0xFF;
        Message[5] = (Message[0]+Message[1]+Message[2]+Message[3]+Message[4])%256;
        
        // wrong checksum test
        if (test_checksum & (MC%10 ==0))
        {
            Message[5]=random();
        }

        printf("Sending Message %i\n",MC);
        printf("%i %i %i %i %i \n",Message[0],Message[1],Message[2],Message[3],Message[4]);

        // skip message test
        if (test_skip_message & (MC%10 ==0))
        {}
        else
        {
            write(fd,Message,6);
        }
        
        MC+=1;
        int sleeptime=100000;
        usleep(sleeptime);
    }
    close(fd);


return 0;
}