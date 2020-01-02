#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define SHM_KEY 10282
#define SEM_KEY 11280
#define SEG_SIZE 200

int main(int argc, char *argv[]) {
  // for semaphore
  int semd;
  int v, r;
  // for shared memory
  int shmd;
  char *data;
  // for file
  int fd;
  // for user input
  char input[200];
  printf("trying to get in\n");
  semd = semget(SEM_KEY, 1, 0);
  if (semd == -1){
    printf("%s\n", strerror(errno));
  }
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_op = -1;
  semop(semd, &sb, 1);
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }
  shmd = shmget(SHM_KEY, SEG_SIZE, 0);
  if (shmd == -1){
    printf("%s\n", strerror(errno));
  }
  data = shmat(shmd, 0, 0);
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }
  fd = open("file.txt", O_WRONLY | O_APPEND, 0644);
  if (fd == -1){
    printf("%s\n", strerror(errno));
  }
  printf("Last addition: %s\n", data);
  printf("Your addition: ");
  fgets(input, 200, stdin);
  strcpy(data, input);
  *strchr(data, '\n') = 0;
  write(fd, input, strlen(input));
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }
  shmdt(data);
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }
  close(fd);
  sb.sem_op = 1;
  semop(semd, &sb, 1);
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }
  /* code */
  return 0;
}
