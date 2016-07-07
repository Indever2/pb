/* * * * * * * * * * * * * * * * * * * * * * *
*     Клиент для подключения к серверу;       *
*     Отправляет серверу сообщения,           *
*     считываемые из файла.                   *   
* * * * * * * * * * * * * * * * * * * * * * */

#include "unp.h"

#define SERVER_HOST "localhost"

int main() {

	int sock = 0;
	int answ_len = 0;
	int enable = 1;

	char buffer[BUF_SIZE()], inp_buffer[BUF_SIZE()]; //константу BUF_SIZE() искать в unp.h (строка 143)

	FILE *inp = fopen("input.txt", "r+");

	struct hostent *h;
	struct sockaddr_in server, client;

	if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

		perror("CLIENT: socket: \t");
		exit(-1);

	}

	memset(&client, 0, sizeof(client));

	client.sin_family = AF_INET;
	client.sin_addr.s_addr = INADDR_ANY;
	client.sin_port = CLIENT_PORT();

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    	perror("setsockopt(SO_REUSEADDR) failed:\t");

	if (bind(sock, (struct sockaddr *)&client, sizeof(client)) < 0) {

		perror("CLIENT: bind:\t");
		exit(-1);

	}

    memset(&server, 0, sizeof(server));

	h = gethostbyname(SERVER_HOST);
	server.sin_family = AF_INET;
	memcpy((char *)&server.sin_addr, h->h_addr, h->h_length);
	server.sin_port = SERVER_PORT();

	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {

		perror("CLIENT: connect:\t");
		exit(-1);

	}

    while (!feof(inp)) {

        fgets(inp_buffer, BUF_SIZE(), inp);
        send(sock, inp_buffer, sizeof(inp_buffer), 0);
        printf("OUTPUT:\tMESSAGE \"%s\" SENT\n", inp_buffer);
        sleep(1);

    }
 
	for(;;) {

		answ_len = recv(sock, buffer, BUF_SIZE(), 0);
		printf("%s\n", buffer);

		if (strcmp(buffer, "HALTU") == 0)
			break;

	}

	shutdown(sock, SHUT_RDWR);
	close(sock);

	return 0;

}
