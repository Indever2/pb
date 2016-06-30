#include "unp.h"
#include <time.h>

#define PORT 3333
#define BUF_SIZE 64

int main() {

	int serv_sock, client_sock;
	int answ_len, count = 0;
	pid_t pid;
	char buffer[BUF_SIZE], time_buf[BUF_SIZE];

	struct sockaddr_in sin, client;

	if ( (serv_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

		perror("SERVER: socket:\t");
		exit(-1);

	}

	int enable = 1;
	if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    	perror("setsockopt(SO_REUSEADDR) failed:\t");

	bzero(&sin, sizeof(sin));

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = PORT;

	if (bind(serv_sock, (struct sockaddr *)&sin, sizeof(sin)) < 0) {

		perror("SERVER: bind:\t");
		exit(-1);

	}

	printf("server started\n");

	listen(serv_sock, 3);

	for(;;) {

		client_sock = accept(serv_sock, (struct sockaddr *)NULL, NULL);

		write(client_sock, "request accepted", sizeof("request accepted"));
		count++;

		printf("connection #%d open\n", count);

		answ_len = recv(client_sock, buffer, BUF_SIZE, 0);
		printf("REQUEST FROM CLIENT:\t%s\n", buffer);

		if (strcmp(buffer, "need_time") == 0) {

			pid = fork();

			if (pid == -1) {

				perror("SERVER: fork error:\t");

			} else if (!pid) {

				printf("CHILD SERVER PROCESS:\tstart\n");
				close(serv_sock);

				time_t ticks = time(NULL);
				snprintf(time_buf, sizeof(time_buf), "%.24s\er\en", ctime(&ticks));
				write(client_sock, time_buf, sizeof(time_buf)); //отправляем тек. время клиенту.

				close(client_sock);
				printf("CHILD SERVER PROCESS:\tend\n");

				exit(0);

			}

		}

		sleep(2);
		write(client_sock, "HALTU", sizeof("HALTU")); //отправляем сигнал остановки клиенту.

		printf("connection #%d closed\n", count);

		shutdown(client_sock, SHUT_RDWR);
		close(client_sock);

	}

	return 0;

}