#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>   // --> man htons

typedef struct s_clients {
	int  id;
	char msg[1024];
} t_clients;

t_clients   client[120000];


struct sockaddr_in  servaddr;
socklen_t           len;


fd_set readFd;
fd_set writeFd;
fd_set active;


int maxFd  = 0;
int nextFd = 0;

char bufferRead[120000];
char bufferWrite[120000];




void    ft_error(char *str) {
	if (str)
		write(2, str, strlen(str));
	else
		write(2, "Fatal error", strlen("Fatal error"));
	write(2, "\n", 1);
	exit(1);
}

void    send_all(int sender) {
	for(int i = 0; i <= maxFd; i++)
		if(FD_ISSET(i, &writeFd) && i != sender)
			send(i, bufferWrite, strlen(bufferWrite), 0);
}

int main(int ac, char **av) {


	// ======================================================================
	// ERROR CHECK
	// ======================================================================
	if (ac != 2)
		ft_error("Wrong number of arguments");

	/* ************************************************************************** */
	// INIT SOCKET
	/* ************************************************************************** */
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)  ft_error(NULL);
	maxFd = sockfd;


	/* ====================================================================== */
	// INIT STRUCTS
	/* ====================================================================== */
	bzero(&client, sizeof(client));
	bzero(&servaddr, sizeof(servaddr));


	//This macro clears (removes all file descriptors from) set.  It should be
	// employed as the first step in initializing a file descriptor set.
	FD_ZERO(&active);

	// This macro adds the file descriptor fd to set.  Adding a file descriptor
	// that is already present in the set is a no-op, and does not  produce  an
	// error.
	FD_SET(sockfd, &active);

	/* ************************************************************************** */
	// SERVER DEFINITION
	/* ========================================================================== */
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(av[1]));

	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) < 0)  ft_error(NULL);
	if (listen(sockfd, 10) < 0) ft_error(NULL);
	/* ************************************************************************** */

	while(1) {
		readFd = writeFd = active;

		int newFd = maxFd + 1;

		/* ================================================================== */
		//  Checking if newFd is valid on read/write
		/* ================================================================== */
		if (select(newFd, &readFd, &writeFd, NULL, NULL) < 0)
			continue;

		/* ================================================================== */
		// Looping thru all fds
		/* ================================================================== */
		for(int currentFd = 0; currentFd < newFd; currentFd++) {

			/* ============================================================== */
			// FD_ISSET()  macro  can  be
            // used  to  test  if a file descriptor is still present in a set.
			// FD_ISSET() returns nonzero if the file descriptor fd is present in set,
			// and zero if it is not.
			//
			//
			// This monitor connections, by listening to events on input socket
			/* ============================================================== */
			if (FD_ISSET(currentFd, &readFd) && currentFd == sockfd) {
					int acceptFd = accept(currentFd, (struct sockaddr *)&servaddr, &len);
					if (acceptFd < 0)  continue; // Reset if error

					maxFd = (acceptFd > maxFd) ? acceptFd : maxFd;
					client[acceptFd].id = nextFd++; // This global var inits at 0 and just increments infinitely

					FD_SET(acceptFd, &active);

					sprintf(bufferWrite, "server: client %d just arrived\n", client[acceptFd].id);
					send_all(acceptFd);

					break;
			}

			/* ============================================================== */
			// Checking if fd is arleady registered
			// and is NOT sender
			/* ============================================================== */
			if (FD_ISSET(currentFd, &readFd) && currentFd != sockfd) {

				int res = recv(currentFd, bufferRead, 65536, 0);

				/* ========================================================== */
				/* ----------------- Monitor disconnection ------------------ */
				/* ========================================================== */
				if (res <= 0) {
					sprintf(bufferWrite, "server: client %d just left\n", client[currentFd].id);
					send_all(currentFd);
					FD_CLR(currentFd, &active);
					close(currentFd);
					break;
				}

				/* ========================================================== */
				/* ---------------------- Send message ---------------------- */
				/* ========================================================== */
				else {
					for (int i = 0, j = strlen(client[currentFd].msg); i < res; i++, j++) {
						client[currentFd].msg[j] = bufferRead[i];
						if (client[currentFd].msg[j] == '\n') {
							client[currentFd].msg[j] = '\0';
							sprintf(bufferWrite, "client %d: %s\n", client[currentFd].id, client[currentFd].msg);
							send_all(currentFd);
							bzero(&client[currentFd].msg, strlen(client[currentFd].msg));
						}
					}
					break;
				}
			}
		}
	}
}
