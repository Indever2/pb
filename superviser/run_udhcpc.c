#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

#define SHM_PREFIX "shm_run_udhcpc."
#define NET_VARIABLES_FILE "net_variables"

int shm_fd;
pid_t udhcpc_pid;

void get_variable(const char *var_name, char *buf)
{
  char read_string[64] = { 0 };
  char *found_string = NULL;

  FILE *net_variables = fopen(NET_VARIABLES_FILE, "r");
  while(!feof(net_variables)) 
  {
    fgets(read_string, 63, net_variables);
    if (strstr(read_string, var_name))
    {
      strtok(read_string, "="); found_string = strtok(NULL, "=");
      if (found_string)
      {
        strncpy(buf, found_string, strlen(found_string) - 1);
        return;
      }
      break;
    }
  }
  fclose(net_variables);
}

void get_rt_table(const char *ifname, char *rt_table)
{
  char if2rt_couples[128] = { 0 };
  get_variable("IF2RT_COUPLES", &if2rt_couples[0]);

  char *couple = strtok(if2rt_couples, " ");
  while (couple)
  {
    if (strstr(couple, ifname))
    {
      strtok(couple, "@");
      strncpy(rt_table, strtok(NULL, "@"), 31);
      return;
    }
    couple = strtok(NULL, " ");
  }
}

char **format_argv(ifname, script, vendor_info, hostname, rt_table)
const char *ifname, *script, *vendor_info, *hostname, *rt_table;
{
  printf(">FUNC: format_argv\n");
  int param_counter = 4; //-t 0 -b -R
  int argv_pos = 0;
  if (ifname && strlen(ifname)) param_counter += 2;
  if (script && strlen(script)) param_counter += 2;
  if (vendor_info && strlen(vendor_info)) param_counter += 2;
  if (hostname && strlen(hostname)) param_counter += 2;
  if (rt_table && strlen(rt_table)) param_counter += 2;

  char **argv = malloc(sizeof(char *) * (param_counter + 1)); //last argv must be NULL
#define fullarg(num, str) argv[num] = malloc(strlen(str)); snprintf(argv[num], strlen(str) + 1, "%s", str)
  fullarg(0, "-t");
  fullarg(1, "0");
  fullarg(2, "-b");
  fullarg(3, "-R");
  argv_pos = 4;

  if (ifname && strlen(ifname)) { fullarg(argv_pos, "-i"); fullarg(argv_pos + 1, ifname); argv_pos += 2; }
  if (script && strlen(script)) { fullarg(argv_pos, "-s"); fullarg(argv_pos + 1, script); argv_pos += 2; }
  if (vendor_info && strlen(vendor_info)) 
  { fullarg(argv_pos, "-V"); fullarg(argv_pos + 1, vendor_info); argv_pos += 2; }
  if (hostname && strlen(hostname)) { fullarg(argv_pos, "-h"); fullarg(argv_pos + 1, hostname); argv_pos += 2; }
  if (rt_table && strlen(rt_table)) { fullarg(argv_pos, "-S"); fullarg(argv_pos + 1, rt_table); argv_pos += 2; }

  argv[argv_pos] = NULL; //last argv must be NULL

  return argv;
}

void free_argv(char **argv)
{
  printf(">FUNC: free_argv()\n");
  int i = 0;
  while (argv[i])
  {
    free(argv[i]);
    i++;
  }
  free(argv);
  return;
}

void run_udhcpc(ifname, script, vendor_info, hostname, rt_table)
const char *ifname, *script, *vendor_info, *hostname, *rt_table;
{
  printf(">FUNC: run_udhcpc\n");
  char **argv_for_exec;
  //pid_t pid = -1;
  int i = 0;

  //pid = fork();
  //if (!pid)
  //{
    printf(">we are CHILD\n");
    argv_for_exec = format_argv(ifname, script, vendor_info, hostname, rt_table);
    while(argv_for_exec[i])
    {
      printf(" --> %s\n", argv_for_exec[i]);
      i++;
    }
    free_argv(argv_for_exec);
    return;
    //мы - ребенок
  //}
  //else 
  //{
   // printf(">we are PARENT\n");
    //мы - родитель
  //}
}

void shm_by_if(const char *ifname, char *shm_name)
{
  snprintf(shm_name, 31, "%s%s", SHM_PREFIX, ifname);
}

int check_shm()
{
  int len;
  char buf[0x0f] = { 0 };

  len = read(shm_fd, buf, 0x0e);
  if (!len) return 0;

  printf("> msg: %s\n", buf);
  if (!strncmp(buf, "stop", 4))
    return 1;
  printf(">empty...\n");
  return 0;
}

void run_daemon(ifname, script, vendor_info, hostname, rt_table)
const char *ifname, *script, *vendor_info, *hostname, *rt_table;
{
  //daemon(0, 0);
  printf("> > run_daemon()\n");
  printf("> > > ifname      = %s\n", ifname);
  printf("> > > script      = %s\n", script);
  printf("> > > vendor_info = %s\n", vendor_info);
  printf("> > > hostname    = %s\n", hostname);
  printf("> > > rt_table    = %s\n", rt_table);
  printf("\n\n");
  run_udhcpc(ifname, script, vendor_info, hostname, rt_table);
  
  printf(" infinite loop!\n\n");
  for (;;)
  {
    if (check_shm(shm_fd))
    {
      //kill udhcpc
      break;
    }
    usleep(500000);
  }
  close(shm_fd);
  printf(">r_u - done...\n");
  return;
}

int main(int argc, char **argv)
{
  char ifname[32] = { 0 };
  char script[64] = { 0 };
  char vendor_info[64] = { 0 };
  char hostname[64] = { 0 };
  char rt_table[32] = { 0 };
  char shm_name[32] = { 0 };

  shm_fd = -1;

  if (argc < 2 || argc > 6)
    goto usage;

  printf(">argc = %d\n", argc);

  /* ifname is set */
  if (argc >= 3)
  {
    strncpy(ifname, argv[2], sizeof(ifname) - 1);
  }
  else
  {
    get_variable("INTERNET_SERVICE_IFACE", &ifname[0]);
  }

  get_rt_table(ifname, &rt_table[0]);
  if (!strlen(rt_table))
    strcpy(rt_table, "def_rt_table");

  shm_by_if(ifname, &shm_name[0]);

  /* script file is set */
  if (argc >= 4)
    strncpy(script, argv[3], sizeof(script) - 1);
  else
    strncpy(script, "/usr/sbin/dhcpc.script", sizeof(script) - 1);

  /* vendor info is set */
  if (argc >= 5)
    strncpy(vendor_info, argv[4], sizeof(vendor_info) - 1);

  /* hostname is set */
  if (argc >= 6)
    strncpy(hostname, argv[5], sizeof(hostname) - 1);

  printf(">shm_name    = %s\n", shm_name);
  printf(">rt_table    = %s\n", rt_table);
  printf(">ifname      = %s\n", ifname);
  printf(">script      = %s\n", script);
  if (strlen(vendor_info))
    printf(">vendor_info = %s\n", vendor_info);
  if (strlen(hostname))
    printf(">hostname    = %s\n", hostname);

  if (!strncmp(argv[1], "start", strlen("start")))
  {
    printf(">Start!\n");
    if (!access(shm_name, F_OK))
    {
      printf(">r_u on %s is already running! Execute r_u stop %s\n", ifname, ifname);
      return 0;
    }
    if (mkfifo(shm_name, 0777))
    {
      printf(">can't create fifo channel\n");
      return -1;
    }
    shm_fd = open(shm_name, O_RDONLY);
    run_daemon(ifname, script, vendor_info, hostname, rt_table);
  }
  else if (!strncmp(argv[1], "stop", strlen("stop")))
  {
    printf(">Stop!\n");
    if (access(shm_name, F_OK))
    {
      printf(">there is no running r_u on %s found\n", ifname);
      return 0;
    }
    shm_fd = open(shm_name, O_WRONLY);
    write(shm_fd, "stop", 0x0e);
    close(shm_fd);
    remove(shm_name);
  }
  else
  {
    goto usage;
  }

  return 0;
usage:
  printf("run_udhcpc usage:\n\trun_udhcpc [stop|start] [interface] [path to script (default /usr/sbin/dhcpc.script)] [VENDOR INFO] [HOSTNAME]\n");
  return -1;
}
