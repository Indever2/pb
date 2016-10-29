#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

FILE *Fopen(const char * path, const char *mode) {
  FILE *file = fopen(path, mode);

  if (file != NULL)
    return file;
  else {
    perror("FOPEN ERROR");
    exit(-1); 
  }
}

int fail_check () {
  FILE *modem_answer = Fopen("/tmp/modem_answer", "r");
  char buf[256] = {0};

  while (!feof(modem_answer)) {
    fscanf(modem_answer, "%s", buf);
    if (strncmp(buf, "Failed", 6) == 0) {
      return 0;
    }
  }
  return 1;
}

void find_modem(char *first_modem, char *second_modem) {
  char path[256] = {0};
  char chat_req[256] = {0};

  system("ls /dev/ | grep ttyUSB > /tmp/modem_list");
  FILE *modems = Fopen("/tmp/modem_list", "r");
  while (!feof(modems)) {
    fgets(path, 255, modems);
    snprintf(chat_req, 255, "chat -v -t 1 -L /tmp/modem_answer '' 'ATI' 'OK' </dev/%s >/dev/%s", path, path);
    system(chat_req);

    if (fail_check()) {
     	if (!strlen(first_modem)) {
     		strcpy(first_modem, path);
     	} else if (!strlen(second_modem)) {
     		strcpy(second_modem, path);
     	}
    }
  	unlink("/tmp/modem_answer");
  	memset(path, 0, sizeof(path));
  	memset(chat_req, 0, sizeof(chat_req));
  }
}

int main() {
	char first_modem[256] = { 0 };
	char second_modem[256] = { 0 };
	
	find_modem(first_modem, second_modem);
	find_modem(first_modem, second_modem);

	printf("%s || %s\n", first_modem, second_modem);
	return 0;
}