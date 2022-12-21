// SPDX-License-Identifier: GPL-2.0-only
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/watchdog.h>
//needed for system
#include <stdlib.h>

// As a global variable, Message_counter is initialized to 0
unsigned int Message_counter;

//E2E check on read message
bool do_E2Echeck(unsigned char Message[6])
{
	// 1 byte yes or no
	// 4 byte / 1 Integer message counter
	unsigned char help[4];

	help[0] = Message[1];
	help[1] = Message[2];
	help[2] = Message[3];
	help[3] = Message[4];
	//unsigned int count=(unsigned int) *help;
	//this casting drives me crazy...
	unsigned int count = help[3]+help[2]*256+help[1]*256*256+help[0]*256*256*256;

	// 1 byte CRC, parity bit
	unsigned char parity = (unsigned char) Message[5];

	//checksum check
	unsigned char computed_parity = (Message[0]+Message[1]+Message[2]+Message[3]+Message[4])%256;

	if (computed_parity != parity) {
		printf("Parity miss");
		return false;
	}

	//Message counter too low
	if (count < Message_counter) {
		printf("Message Counter too low, expected %i, received %i\n", Message_counter, count);
		return false;
	}

	//Message counter too large, i.e. messages got lost inbetween
	if (count >= Message_counter+2) {
		printf("Message Counter too big, expected %i, received %i\n", Message_counter, count);
		Message_counter = count;
		return false;
	}

	Message_counter += 1;
	return true;
}

int main(void)
{
	int wdt, ret, timeout = 10;
	bool write_wdt = false;

	// creating the communication pipe
	const char *Pipe = "/tmp/safety-signal-source_to_safety-app";

	printf("here I am, listening to a pipe\n");
	mkfifo(Pipe, 0666);
	int fd = open(Pipe, O_RDONLY);

	//buffer for the message to be read
	unsigned char Message[6];

	wdt = open("/dev/watchdog0", O_WRONLY);
	if (wdt >= 0)
		write_wdt = true;
	ret = ioctl(wdt, WDIOC_SETTIMEOUT, &timeout);
	if (ret)
		perror("ioctl");

	while (1) {
		int i = 0;
		do {
			ret = read(fd, &(Message[i]), 6 - i);
			if (ret < 0)
				break;
			i += ret;
		} while(i < 6);
		if (i == 6) {
			printf("Received Message:%i %i %i %i %i %i\n", Message[0], Message[1], Message[2], Message[3], Message[4], Message[5]);
			if (do_E2Echeck(Message)) {
				printf("%s\n", Message);
			} else {
				printf("What a mess!, SAFESTATE\n");
				// could just kill the qt app to get the safe state black screen
				// killall afbd-cluster-gauges
				// does the trick from the shell, lets try to do that from here yep, works
				// system("killall afbd-cluster-gauges");
				// Command QT app to do safe state display
				system("cansend can0 021#0000000000000080");
				write_wdt = false;
			}
			if (Message[0] == 0) {
				// Signal source triggered safe state
				printf("SAFESTATE (as commanded by signal source)\n");
				system("cansend can0 021#0000000000000080");
				write_wdt = false;
			}
			if (write_wdt)
				write(wdt, "1", 1);
		}
		int sleeptime = 1000000;

		usleep(sleeptime);
		printf("here I am, having waited for %f seconds\n", sleeptime / 1000000.0);
		fflush(NULL);
	}
	//close file again
	close(fd);
	close(wdt);
	return 0;
}
