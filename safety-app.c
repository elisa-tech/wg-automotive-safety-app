#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>

unsigned int Message_counter=0;

//E2E check on read message, 
bool do_E2Echeck(unsigned char Message[6])
{
    // 1 byte yes or no
    // 4 byte / 1 Integer message counter
    unsigned char help[4];
    help[0]=Message[1];
    help[1]=Message[2];
    help[2]=Message[3];
    help[3]=Message[4];
    //unsigned int count=(unsigned int) *help;
    //this casting drives me crazy... 
    unsigned int count=help[3]+help[2]*256+help[1]*256*256+help[0]*256*256*256;

    // 1 byte CRC, parity bit
    unsigned char parity =(unsigned char) Message[5];
    
    //checksum check
    unsigned char computed_parity=(Message[0]+Message[1]+Message[2]+Message[3]+Message[4])%256;
    if (computed_parity != parity)
    {
        printf("Parity miss");
        return false;
    } 

    //Message counter too low
    if (count < Message_counter)
    {
        printf("Message Counter too low, expected %i, received %i \n",Message_counter,count);
        return false;
    }

    //Message counter too large, i.e. messages got lost inbetween
    if (count >= Message_counter+2)
    {
        printf("Message Counter too big, expected %i, received %i \n",Message_counter,count);
        Message_counter=count;
        return false;
    }

    Message_counter+=1;
    return true; 
        
}

int main()
{
const char* Pipe="./Pipe";
unsigned char Message[6];
printf("here I am, listening to a pipe\n");

mkfifo(Pipe,0666);
int fd=open(Pipe, O_RDONLY);
while (1) {
    if (read(fd,Message,6) > 0)
    {
        printf("Received Message:%i %i %i %i %i %i \n",Message[0],Message[1],Message[2],Message[3],Message[4],Message[5]);
        if (do_E2Echeck(Message))
        {
            printf("%s\n",Message);
        }
        else
        {
            printf("What a mess!, SAFESTATE\n");
        }
    }
    int sleeptime=100000;
    usleep(sleeptime);
    printf("here I am, having waited for %f seconds \n",sleeptime / 1000000.0);
}
//close file again
close(fd);
return 0;
}
