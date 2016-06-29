#include "unp.h"

#define SERVER_PORT 3333
#define CLIENT_PORT 3334
#define SERVER_HOST "localhost"
#define BUF_SIZE 64

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

	memset((char *)&client, '\0', sizeof(client));

	client.sin_family = AF_INET;
	client.sin_addr.s_addr = INADDR_ANY;
	client.sin_port = CLIENT_PORT;

	if (bind(sock, (struct sockaddr *)&client, sizeof(client)) < 0) {

		perror("CLIENT: bind:\t");
		exit(-1);

	}

	memset((char *)&client, '\0', sizeof(server));

	h = gethostbyname(SERVER_HOST);
	server.sin_family = AF_INET;
	memcpy((char *)&server.sin_addr, h->h_addr, h->h_length);
	server.sin_port = SERVER_PORT;

	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {

		perror("CLIENT: connect:\t");
		exit(-1);

	}

	answ_len = recv(sock, buffer, BUF_SIZE, 0);
	write(1, buffer, answ_len);
	send(sock, "Ivan :)\n", sizeof("Ivan :)\n"), 0);

	shutdown(sock, SHUT_RDWR);
	close(sock);

	return 0;

}