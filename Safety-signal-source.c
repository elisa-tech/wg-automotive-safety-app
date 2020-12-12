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
#include <string.h>
// needed for strcomp
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

	char readbuffer[20];

	bool fi_skip_message = false;
	bool fi_corrupt_E2E = false;
	bool trigger_safe_state = false;

	while (1) {
		//invalidate buffer to prevent reuse of last command if nothing is read
		readbuffer[0] = '\0';
		//read from control pipe and set switches accordingly
		read(fd_controlpipe, readbuffer, 20);

		// evaluate trigger by keyword
		fi_skip_message = !strncmp(readbuffer, "skip message", 12);
		fi_corrupt_E2E = !strncmp(readbuffer, "corrupt message", 15);
		trigger_safe_state = !strncmp(readbuffer, "safe state", 10);

		// evaluate trigger by character
		switch (readbuffer[0]) {
		case 49:        // "1" for break checksum
			fi_corrupt_E2E = true;
			break;
		case 50:        // "2" for skip message
			fi_skip_message = true;
			break;
		case 51:        // "3" for send safety bool = 0
			trigger_safe_state = true;
			break;
		}

		printf("Trigger status:\n  corrupt E2E: %i\n  skip message: %i\n  trigger safe state: %i\n", fi_corrupt_E2E, fi_skip_message, trigger_safe_state);
		// Compose message
		if (trigger_safe_state) {
			Message[0] = 0;
			// Trigger corruption in QT app
			system("cansend can0 021#0000000002000000");
		} else {
			Message[0] = 1;
		}
		//encode Integer to bytes
		Message[1] = (MC >> 24) & 0xFF;
		Message[2] = (MC >> 16) & 0xFF;
		Message[3] = (MC >> 8) & 0xFF;
		Message[4] = MC & 0xFF;
		Message[5] = (Message[0]+Message[1]+Message[2]+Message[3]+Message[4])%256;

		// corrupt CRC if fault injection E2E is triggered
		if (fi_corrupt_E2E) {
			Message[5] = random();
		}

		printf("Sending Message Number %i\n", MC);
		printf("%i %i %i %i %i %i\n\n", Message[0], Message[1], Message[2], Message[3], Message[4], Message[5]);
		fflush(NULL);

		// send message, unless fault injection skip message is triggered
		if (!fi_skip_message) {
			write(fd, Message, 6);
		}
		MC += 1;
		int sleeptime = 1000000;

		usleep(sleeptime);
	}
	close(fd);

	return 0;
}
