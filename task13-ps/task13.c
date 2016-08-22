#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>

/* Функция для вывода всех имеющихся процессов */
void show_proc_list(void) {
  struct task_struct *task;
  for_each_process(task)
  {
    pr_info("%s [%d]\n", task -> comm, task -> pid);
  }
}

static int __init start(void) {
  pr_alert("Module is loaded successfull.\n");
  show_proc_list(); 
  return 0; 
}

static void __exit stop(void) {
  pr_alert("Module unloaded\n");
}

module_init(start);
module_exit(stop);
MODULE_LICENSE("GPL");