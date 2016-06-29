#include "unp.h"

#define PORT 3333
#define BUF_SIZE 64

int main() {

	int serv_sock, client_sock;
	int answ_len, count = 0;

	char buffer[BUF_SIZE];

	struct sockaddr_in sin, client;

	if ( (serv_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){

		perror("SERVER: socket:\t");
		exit(-1);

	}

	memset((char *)&sin, '\0', sizeof(sin));

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = PORT;

	if (bind(serv_sock, (struct sockaddr *)&sin, sizeof(sin)) < 0){

		perror("SERVER: bind:\t");
		exit(-1);

	}

	printf("server started\n");

	listen(serv_sock, 3);

	for(;;) {

		printf("new try!\n");

		client_sock = accept(serv_sock, (struct sockaddr *)&client, &answ_len);

		write(client_sock, "My server is here!\n", sizeof("My server is here!\n"));
		count++;

		answ_len = recv(client_sock, buffer, BUF_SIZE, 0);
		write(1, buffer, answ_len);

		printf("client #%d\n", count);

		shutdown(client_sock, SHUT_RDWR);
		close(client_sock);

	}

	return 0;

}