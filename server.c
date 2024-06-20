#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>

void ServerConnection(int);
void ReadLineFromNetwork(int fd, char *buf, int size);

int main(int argc, const char * argv[])
{
	int serverSocket = -1, clientConnection;
	struct sockaddr_in server;
	struct sockaddr_in client;
	unsigned int alen;
	
	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	/*printf("Port = %s\n", argv[1]);*/

	memset(&server, 0 ,sizeof(struct sockaddr_in));

	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[1]));

	server.sin_addr.s_addr = INADDR_ANY;

	if(bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) != 0) {
		fprintf(stderr, "bind() failed -- exiting\n");
		exit(1);
	}
	
	if(listen(serverSocket, 15) < 0) {
		fprintf(stderr, "listen() failed -- exiting\n");
		exit(2);
	}

	while(1) {

		alen = sizeof(client);
		clientConnection = accept(serverSocket, (struct sockaddr*)&client, &alen);
		/*printf("Connection from %x port %d... \n", ntohl(client.sin_addr.s_addr), ntohs(client.sin_port));*/
		ServerConnection(clientConnection);
		close(clientConnection);
	}
	return 0;
}


void ServerConnection(int fd) {
	const char *greeting = "+OK Connection accepted\r\n";
	const char *ok = "+OK";
	const char *error = "-ERR Not Implemented";
	char outBuffer[512];
	char inputBuffer[512], *p;

	int WAITING = 0;
	int SENTRESPONSE = 1;
	int BYE = 2;
	int state = WAITING;

	ssize_t n;
	
	write(fd, greeting, strlen(greeting));
	state = SENTRESPONSE;

	while(state != BYE) {

		ReadLineFromNetwork(fd, inputBuffer, 512);
		if(state = SENTRESPONSE) {
			if(0 == strcmp(inputBuffer, "QUIT")) {
				sprintf(outBuffer, "%s\r\n", ok);
				state = BYE;

			}	
			if(state != BYE) {
				sprintf(outBuffer, "%s\r\n", error);
			}


		}

		write(fd, outBuffer, strlen(outBuffer));
	}
}

void ReadLineFromNetwork(int fd, char *buf, int size)
{
	int kBufSize = 12;
	char l[kBufSize+1];
	ssize_t n;
	int cline = 0;
	char *p = buf;
	int i;
	
	do
	{
		n = read(fd, l, kBufSize);
		
		for(i = 0; i < n; i++)
		{
			*p = l[i];
			if(p != buf && *p == 10 && *(p-1) == 13)
			{
				*(p-1) = 0;
				cline = 1;
				break;
			}
			p++;
		}
		*p = 0;
	} while(cline == 0 && n > 0);
	
}