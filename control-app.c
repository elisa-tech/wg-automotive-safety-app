// SPDX-License-Identifier: GPL-2.0-only
#include <stdio.h>
#include <unistd.h>     //needed for named pipes
#include <fcntl.h>
#include <stdbool.h>    //boolean type
#include <sys/stat.h>
#include <ncurses.h>
#include <string.h>

// Options to choose from
char *fi_choices[] = {
	"corrupt next message sent",
	"drop next message",
	"trigger Safe State",
};
int n_fi_choices = sizeof(fi_choices) / sizeof(char *);
// Corresponding messages to be sent
char *fi_message_strings[] = {
	"corrupt message",
	"skip message",
	"safe state",
};

// print selection window with highlighted selection
void print_menu(WINDOW *W, int selection)
{
	box(W, 0, 0);
	int y = 1;

	for (int i = 0; i < n_fi_choices; i++) {
		if (i == selection) {
			wattron(W, A_REVERSE);
			mvwprintw(W, y, 1, "%s\n", fi_choices[i]);
			wattroff(W, A_REVERSE);
		} else {
			mvwprintw(W, y, 1, "%s\n", fi_choices[i]);
		}
		y++;
	}
	wrefresh(W);
}

// write selected command to the signal-source-contro-pipe
void send_control_message(int selection)
{
	const char *Pipe = "/tmp/signal-source-control-pipe";

	mkfifo(Pipe, 0666);
	int fd = open(Pipe, O_WRONLY | O_NONBLOCK);

	write(fd, fi_message_strings[selection], strlen(fi_message_strings[selection]));
	close(fd);
}

int main(void)
{
	int selection = 0;        // currently highlighted row
	int key_pressed;

	initscr();      // Start curses mode
	cbreak();
	WINDOW *W;

	W = newwin(n_fi_choices+2, 50, 3, 1);
	keypad(W, TRUE);
	printw("Welcome to the signal-source-control panel\n");
	printw("Enter to Send selected control message\n");
	printw("ESC to quit Application\n\n");
	refresh();
	while (1) {
		print_menu(W, selection);        //refresh window
		key_pressed = wgetch(W);
		switch (key_pressed) {
		case KEY_UP:
			selection--;
			if (selection < 0) {
				selection = 0;
			}
			break;
		case KEY_DOWN:
			selection++;
			if (selection >= n_fi_choices-1) {
				selection = n_fi_choices-1;
			}
			break;
		case 10:                // Enter key, send highlighted message
			send_control_message(selection);
			break;
		case 27:                // ESC key, quit application
			endwin();               //End curses mode and return
			return 0;
		}
	}
}
