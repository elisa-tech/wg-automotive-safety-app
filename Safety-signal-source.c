// SPDX-License-Identifier: GPL-2.0-only
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
// Needed for random numbers
#include <stdlib.h>
//needed for mkfifo function
#include <sys/types.h>
#include <sys/stat.h>
int main(void)
{
	// creating the communication pipe
	const char *Pipe = "/tmp/safety-signal-source_to_safety-app";

	mkfifo(Pipe, 0666);
	int fd = open(Pipe, O_WRONLY);

	// Message counter and Buffer for Message to be sent
	unsigned int MC = 0;
	unsigned char Message[6];

	// creating the control pipe
	const char *controlpipe = "/tmp/signal-source-control-pipe";

	mkfifo(controlpipe, 0666);
	int fd_controlpipe = open(controlpipe, O_RDONLY | O_NONBLOCK);

	char Controlmessage[1];
	// 10 for no command given, enter character coming from empty pipes
	// "1" for break checksum "2" for skip message
	Controlmessage[0] = 10;

	while (1) {
		Message[0] = 1;
		Message[1] = (MC >> 24) & 0xFF;
		Message[2] = (MC >> 16) & 0xFF;
		Message[3] = (MC >> 8) & 0xFF;
		Message[4] = MC & 0xFF;
		Message[5] = (Message[0]+Message[1]+Message[2]+Message[3]+Message[4])%256;

		read(fd_controlpipe, Controlmessage, 1);
		printf("Command code: %i\n", Controlmessage[0]);

		// wrong checksum test "1" corresponds to ASCII 49
		if (Controlmessage[0] == 49) {
			Controlmessage[0] == 10;
			Message[5] = random();
		}

		printf("Sending Message %i\n", MC);
		printf("%i %i %i %i %i\n", Message[0], Message[1], Message[2], Message[3], Message[4]);
		fflush(NULL);

		// skip message test "2" corresponds to ASCII 50
		if (Controlmessage[0] == 50) {
			Controlmessage[0] == 10;
		} else {
			write(fd, Message, 6);
		}

		MC += 1;
		int sleeptime = 1000000;

		usleep(sleeptime);
	}
	close(fd);

return 0;
}
