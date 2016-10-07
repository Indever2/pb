#ifndef __WRAPPERS_H
#define __WRAPPERS_H
/* Функции обертки */

FILE *Fopen(const char * path, const char *mode) {
  FILE *file = fopen(path, mode);

  if (file != NULL)
    return file;
  else {
    perror("FOPEN ERROR");
    exit(-1); 
  }
}

int Close(int fd) {
  fd = close(fd);

  if (fd != 0) {
    fprintf(stderr, "Can't close file descriptor!\n");
    exit(-1);
  } else {
    return 0;
  }
}

ssize_t Write(int fd, const void *buf, size_t count) {
  ssize_t bytes = write(fd, buf, count);

  if (bytes < 0) {
    printf("Write command error.\n");
    return bytes;
  } else {
    return bytes;
  }
}

ssize_t Read(int fd, void *buf, size_t count) {
  ssize_t bytes = read(fd, buf, count);

  if (bytes < 0) {
    perror("Read function error");
    return bytes;
  } else {
    return bytes;
  }
}

#endif /* wrappers.h */
