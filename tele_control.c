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

union semun {
  int              val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO
                              (Linux-specific) */
};

void get_story() {
  FILE * fp = fopen("file.txt", "r");
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }
  printf("The story so far:\n");
  char line[200];
  while(fgets(line, 200, fp)) {
    printf("%s", line);
  }
  fclose(fp);
}

int main(int argc, char *argv[]) {
  // for semaphore
  int semd;
  int v, r;
  // for shared memory
  int shmd;
  char *data;
  // for file
  int fd;
  if (argc > 1 && !strcmp(argv[1], "-c")) {
    semd = semget(SEM_KEY, 1, IPC_CREAT | 0644);
    if (semd != -1){
      union semun us;
      us.val = 1;
      r = semctl(semd, 0, SETVAL, us);
    }
    else {
      printf("%s\n", strerror(errno));
    }
    printf("semaphore created\n");
    shmd = shmget(SHM_KEY, SEG_SIZE, IPC_CREAT | 0644);
    if (shmd == -1){
      printf("%s\n", strerror(errno));
    }
    data = shmat(shmd, 0, 0);
    if (*data){
      strcpy(data, "");
      shmdt(data);
    }
    printf("shared memory created\n");
    fd = open("file.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd != -1){
      printf("file created\n");
      close(fd);
    }
  }
  if (argc > 1 && !strcmp(argv[1], "-v")) {
    get_story();
  }
  if (argc > 1 && !strcmp(argv[1], "-r")) {
    printf("trying to get in\n");
    semd = semget(SEM_KEY, 1, 0);
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    semop(semd, &sb, 1);
    get_story();
    shmd = shmget(SHM_KEY, SEG_SIZE, IPC_CREAT | 0644);
    shmctl(shmd, IPC_RMID, 0);
    printf("shared memory removed\n");
    remove("file.txt");
    printf("file removed\n");
    semctl(semd, IPC_RMID, 0);
    printf("semaphore removed\n");
  }
  /* code */
  return 0;
}
