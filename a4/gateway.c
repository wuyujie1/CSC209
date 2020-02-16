#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <time.h>
#include "socket.h"
#include "message.h"
#include "controller.h"
#include <errno.h>

#define MAXFD(x,y) ((x) >= (y)) ? (x) : (y)

int main(int argc, char *argv[]){
	int port;
	struct cignal cig;
	// A buffer to store a serialized message
	char *cig_serialized = malloc(sizeof(char)*CIGLEN);
	// An array to registered sensor devices
	int device_record[MAXDEV] = {0};

	if(argc == 2){
		port = strtol(argv[1], NULL, 0);
	} else{
		fprintf(stderr, "Usage: %s port\n", argv[0]);
		exit(1);
	}

	int gatewayfd = set_up_server_socket(port);
	printf("\nThe Cignal Gateway is now started on port: %d\n\n", port);
	int peerfd;

	/* TODO: Implement the body of the server.
	 *
	 * Use select so that the server process never blocks on any call except
	 * select. If no sensors connect and/or send messsages in a timespan of
	 * 5 seconds then select will return and print the message "Waiting for
	 * Sensors update..." and go back to waiting for an event.
	 *
	 * The server will handle connections from devices, will read a message from
	 * a sensor, process the message (using process_message), write back
	 * a response message to the sensor client, and close the connection.
	 * After reading a message, your program must print the "RAW MESSAGE"
	 * message below, which shows the serialized message received from the *
	 * client.
	 *
	 *  Print statements you must use:
     * 	printf("Waiting for Sensors update...\n");
	 * 	printf("RAW MESSAGE: %s\n", YOUR_VARIABLE);
	 */

	// TODO implement select loop

	// Suppress unused variable warning.  The next 5 ilnes can be removed
	// after the variables are used.

	//initialize set of fds for select
	struct timeval timeout;
	int max_fd = gatewayfd;
	fd_set read_fds;
	FD_ZERO(&read_fds);
	FD_SET(gatewayfd, &read_fds);

	while(1) {
		timeout.tv_sec = 5;
    timeout.tv_usec = 0;

		fd_set listen_fds = read_fds;
		int nready = select(max_fd + 1, &listen_fds, NULL, NULL, &timeout);
		if (nready == 0){
			printf("Waiting for Sensors update...\n");
			continue;
		}
		if (nready == -1){
			perror("select");
			exit(1);
		}

		if (FD_ISSET(gatewayfd, &listen_fds)){
			peerfd = accept_connection(gatewayfd);
			if (peerfd == -1){
				perror("accept");
				exit(1);
			} else{
				if (peerfd > max_fd){
					max_fd = peerfd;
				}
				FD_SET(peerfd, &read_fds);
			}
		}

		if (FD_ISSET(peerfd, &listen_fds) != 0){
			int red = read(peerfd, cig_serialized, CIGLEN);
			if (red == -1){
				perror("read");
				exit(1);
			} else {
				printf("RAW MESSAGE: %s\n", cig_serialized);
				unpack_cignal(cig_serialized, &cig);
				if (process_message(&cig, device_record) == -1){
					perror("process_message");
					exit(1);
				}
				cig_serialized = serialize_cignal(cig);
				if (write(peerfd, cig_serialized, CIGLEN) == -1){
					perror("write");
					exit(1);
				}
				if (close(peerfd) == -1){
					perror("close");
					exit(1);
				}
				FD_CLR(peerfd, &read_fds);
			}
		}
	}
	return 0;
}
