
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>



void error() {
	char *string = "Fatal error\n";
	write(2, string, strlen(string));
	exit(1);
}

int main (int ac, char **av) {

	if (ac != 2){
		char *string = "Wrong number of arguments\n";
		write(2, string, strlen(string));
		exit(1);
	}

		struct sockaddr_in servaddr, cli; 

	// socket create and verification 
		int sockfd, connfd, len;

	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(8081); 

}

