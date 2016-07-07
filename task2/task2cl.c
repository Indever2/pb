#include "unp.h"

#define SERVER_PORT 3333
#define CLIENT_PORT 3334
#define BUF_SIZE 64

#define SERVER_HOST "localhost"
#define TIME_REQ "need_time"

int main() {

	int sock;
	int answ_len;

	char buffer[BUF_SIZE];

	struct hostent *h;
	struct sockaddr_in server, client;

	if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

		perror("CLIENT: socket: \t");
		exit(-1);

	}

	bzero(&client, sizeof(client));

	client.sin_family = AF_INET;
	client.sin_addr.s_addr = INADDR_ANY;
	client.sin_port = CLIENT_PORT;

	int enable = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    	perror("setsockopt(SO_REUSEADDR) failed:\t");

	if (bind(sock, (struct sockaddr *)&client, sizeof(client)) < 0) {

		perror("CLIENT: bind:\t");
		exit(-1);

	}

	bzero(&server, sizeof(server));

	h = gethostbyname(SERVER_HOST);
	server.sin_family = AF_INET;
	memcpy((char *)&server.sin_addr, h->h_addr, h->h_length);
	server.sin_port = SERVER_PORT;

	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {

		perror("CLIENT: connect:\t");
		exit(-1);

	}

	answ_len = recv(sock, buffer, BUF_SIZE, 0);
	printf("%s\n", buffer);

	send(sock, TIME_REQ, sizeof(TIME_REQ), 0);
 
	for(;;) {

		answ_len = recv(sock, buffer, BUF_SIZE, 0);
		printf("%s\n", buffer);

		if (strcmp(buffer, "HALTU") == 0)
			break;

	}

	shutdown(sock, SHUT_RDWR);
	close(sock);

	return 0;

}
