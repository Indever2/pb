#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h> /* put_user */

#define DEVICE_NAME "test"

/* Поддерживаемые нашим устройством операции */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

/* Переменные оформлены как глобальные дабы избежать конфликта имен */
static int major_number;
static int is_device_open = 0;
static char text[5] = "test\n";
static char *text_ptr = text;

static struct file_operations fops = 
{
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};

void SayRed(char *buff) {
  pr_alert("%s\n", buff);
}

void SayWhite(char *buff) {
  pr_info("%s\n", buff);
}

/* Обертки для pr_info и pr_alert */
EXPORT_SYMBOL(SayRed);
EXPORT_SYMBOL(SayWhite);

static int __init test_init(void) {
  SayRed("Test module is loaded.");
  major_number = register_chrdev(0, DEVICE_NAME, &fops);

  if (major_number < 0) {
    pr_alert("chrdev_register if failed with %d\n", major_number);
    return major_number;
  }
  SayWhite("Test module device registered succesfull!");
  pr_info("Please, create device file with mknod /dev/test c %d 0\n", major_number);
  return 0;
}

static void __exit test_exit(void) {
  unregister_chrdev(major_number, DEVICE_NAME);
  pr_alert("Test module is unloaded.\n");
}

static int device_open( struct inode *inode, struct file *file ) {
  text_ptr = text;
  if (is_device_open)
    return -EBUSY;
  is_device_open++;
  return 0;
}

static int device_release(struct inode *inode, struct file *file) {
  is_device_open--;
  return 0;
}

static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t *off) {
  SayWhite("Sorry, this operation isn't suppotted.");
  return -EINVAL;
}

static ssize_t device_read( struct file *filp, char *buffer, size_t length, loff_t * offset) {
  int byte_read = 0;

  if ( *text_ptr == 0 )
    return 0;

  while ( length && *text_ptr ) {
    put_user( *( text_ptr++ ), buffer++ );
    length--;
    byte_read++;
  }

  return byte_read;
}

module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL" );
MODULE_AUTHOR(" Krivcov Ivan vankrivcov9@gmail.com");
MODULE_DESCRIPTION(" My nice module");
MODULE_SUPPORTED_DEVICE(DEVICE_NAME);