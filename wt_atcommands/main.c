/* * * * * * * * * * * * * * * * * * * * * * * * * *
 * This utility sends AT command to usb-modem      *
 * by the /dev/ttyUSB30 and returns a result string* 
 * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "resources.h"
#include "wrappers.h"

#define TRUE  1
#define FALSE 0

/**
 * [usage description]
 * This function prints how-to use this program;
 */
void usage() {
  printf("usage: atput <port num (1 or 2)> <AT command> (<arg> <arg value>):\n");
  exit(-1);
}

/**
 * [is_string_empty description]
 * This function checks whether input string is empty
 * @str         input string 
 * @return      TRUE/FALSE in dependence of string is empty
 */
int is_string_empty(char *str) {
  int i = 0;
  for(i = 2; i < strlen(str); i++) {
    if ((str[i] != ' ') && (str[i] != '\t') && (str[i] != '\n')) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * [is_string_important description]
 * This function checks whether input string is not intresting to output
 * @str         input string
 * @return      TRUE/FALSE in dependence of string
 */
int is_string_important(char *str) {
  if (strcmp(str, "--  OK\n") == 0)
    return FALSE;
  if (strcmp(str, "--  alarm\n") == 0) 
    return FALSE;
  if (strcmp(str, "--   -- got it\n") == 0) 
    return FALSE;
  return TRUE;
}

int main(int argc, char const *argv[]) {
  /**
   * @i               :cycles counter  
   * @fd              :fifo file descriptor
   * @return_val      :return value of Read() func
   * @position        :position in fabricating string
   * @buf_size        :max size of fabricating string
   * @string_count    :count of readed strings
   * @needed          :string's maintenance in memory
   * @buffer          :char to read from fifo file
   * @modem_name      :path to target modem
   * @chat_ex_start   :first component to fabricating string
   * @chat_ex_body    :AT command
   * @chat_ex_fin     :path to modem
   * @final_chat_req  :request to chat
   * @res_buffer      :one string of chat answer
   * @result          :final output array
   */

  int i = 0;
  int fd = 0;
  int return_val = 0, position = 0;
  int buf_size = 1024;
  int string_count = 0;

  size_t needed = 0;

  char buffer = '\0';
  char modem_name[14];
  char chat_ex_start[] = "chat -v -t 1 -L /tmp/atfile ''";
  char *chat_ex_body = NULL;
  char *chat_ex_fin = NULL;
  char *final_chat_req = NULL;
  char res_buffer[255];
  char **result = malloc(sizeof(char *));

  memset(&modem_name, 0, sizeof(modem_name));
  memset(&res_buffer, 0, sizeof(res_buffer));

  if ((argc % 2 == 0) || (argc < 3)) {
    usage();
  } else if (argc == 5) {
      if(strcmp(argv[3], "-s") == 0)
        buf_size = strtoint(argv[4]);
  }

  /* selecting modem port */
  switch (strtoint(argv[1])) {
    case 1:
      strcpy(modem_name, MODEM_NAME_1);
      break;
    case 2:
      strcpy(modem_name, MODEM_NAME_2);
      break;
    default:
      usage();
      return 0;
  }

  /* calculating needed string size */
  needed = snprintf(NULL, 0, " '%s' ", argv[2]);
  /* allocating memory for the string */
  chat_ex_body = malloc(needed++);
  memset(chat_ex_body, 0, sizeof(chat_ex_body));
  /* fabricating the string */
  snprintf(chat_ex_body, needed, " '%s' ", argv[2]);

  needed = 0;

  /* no comments, all the same */
  needed = snprintf(NULL, 0, " <%s >%s", modem_name, modem_name);
  chat_ex_fin = malloc(needed++);
  memset(chat_ex_fin, 0, sizeof(chat_ex_fin));
  snprintf(chat_ex_fin, needed, " <%s >%s", modem_name, modem_name);

  needed = 0;

  needed = snprintf(NULL, 0, "%s%s'OK'%s", chat_ex_start, 
          chat_ex_body, chat_ex_fin);
  final_chat_req = malloc(needed += 2);
  memset(final_chat_req, 0, sizeof(final_chat_req));
  /* final request */
  snprintf(final_chat_req, needed, "%s%s'OK'%s", chat_ex_start, 
          chat_ex_body, chat_ex_fin);

  /* cleaning up */
  memset(chat_ex_body, 0, sizeof(chat_ex_body));
  memset(chat_ex_fin, 0, sizeof(chat_ex_fin));
  free(chat_ex_body); 
  free(chat_ex_fin);

  /* invoking chat utility with our request */
  if ((system(final_chat_req)) < 0) {
    perror("system ");
    exit(-1);
  }

  free(final_chat_req);

  /* opening chat's result file */
  if ((fd = open(PATH_TO_FIFO, O_CREAT, S_IRWXO)) < 0) {
    perror("open ");
    exit(-1);
  }

  printf("reading the answer...\n");

  /**
   * This is preambule to all strings.
   */
  position = 4;
  res_buffer[position - 4] = '-';
  res_buffer[position - 3] = '-';
  res_buffer[position - 2] = ' ';
  res_buffer[position - 1] = ' ';

  while (((return_val = Read(fd, &buffer, 1)) > 0) && (position < buf_size)) {

    /* reading symbols */
    res_buffer[position] = buffer;
    position++;

    /*if \n symbol found */
    if (buffer == '\n') {
      res_buffer[position] = '\0';

      result[string_count] = malloc(sizeof(char) * strlen(res_buffer));
      memset(result[string_count], 0, sizeof(result[string_count]));
      memmove(result[string_count], res_buffer, strlen(res_buffer));
      result[string_count][strlen(res_buffer)] = '\0';
      string_count++;

      result = realloc(result, sizeof(char *) * (string_count + 1));
      memset(res_buffer, 0, sizeof(res_buffer));

      position = 4;
      res_buffer[position - 4] = '-';
      res_buffer[position - 3] = '-';
      res_buffer[position - 2] = ' ';
      res_buffer[position - 1] = ' ';
    }
  }

  if (string_count > 4) {
    printf("modem answer:\n\n");
    for (i = 0; i < string_count - 1; i++){
      /* output condition */
      if ((is_string_empty(result[i]) == FALSE) 
      && (i > 2) && (is_string_important(result[i]) == TRUE))
        printf("%s", result[i]);
    }
    printf("\n");
  }
  else 
    printf("Error. We have no answer from %s.\nCheck your command or change port.\n", modem_name);

  close(fd);

  /* removing chat result file */
  if ((unlink(PATH_TO_FIFO)) < 0) {
    perror("unlink ");
  }

  free(result);  
  return 0;
}