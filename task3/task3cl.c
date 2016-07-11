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
	int enable = 1; //Для установки параметров сокета

	char buffer[BUF_SIZE()], inp_buffer[BUF_SIZE()]; //константу BUF_SIZE() искать в unp.h (строка 143)

	FILE *inp = fopen("input.txt", "r+");

	struct hostent *h; //Хост
	struct sockaddr_in server, client;

	sock = Socket(AF_INET, SOCK_STREAM, 0);

	memset(&client, 0, sizeof(client));

	client.sin_family = AF_INET;
	client.sin_addr.s_addr = INADDR_ANY;
	client.sin_port = CLIENT_PORT();

	Setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	Bind(sock, (struct sockaddr *)&client, sizeof(client));

    memset(&server, 0, sizeof(server));

	h = gethostbyname(SERVER_HOST);
	server.sin_family = AF_INET;
	memcpy((char *)&server.sin_addr, h->h_addr, h->h_length);
	server.sin_port = SERVER_PORT();

	Connect(sock, (struct sockaddr *)&server, sizeof(server));

    /*читаем из файла и отправляем считанные сообщения серверу*/
    while (!feof(inp)) {

        fgets(inp_buffer, BUF_SIZE(), inp);
        send(sock, inp_buffer, sizeof(inp_buffer), 0);
        printf("OUTPUT:\tMESSAGE \"%s\" SENT\n", inp_buffer);
        sleep(1);

    }
    
    /*Принимаем сообщения от сервера*/
	for(;;) {

		answ_len = recv(sock, buffer, BUF_SIZE(), 0);
		printf("%s\n", buffer);

		if (strcmp(buffer, "HALTU") == 0) //Ключевое слово HALTU означает конец передачи
			break;

	}

	shutdown(sock, SHUT_RDWR);
	close(sock);

	return 0;

}
