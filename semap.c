
/* shm.sem.example.c SJ */
/* an example of SysV shmem and semaphore usage */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

/* size of array */
#define N 10
/* number of processes */
#define P 3
/* how many semaphores */
#define SEMNUM 2

#define DELAY 10000000

void manipulate(int *array, int sem_set_id);

int main(int argc, char **argv)
{
  pid_t child;
  int i,rc;

  int sem_set_id;
  union semun {		/* for some reason, this union is not ready */
	  int             val;
  } sem_val;

  struct shmid_ds smdesc; /* shared memory descriptor */
  int smid;	/* shamred memory block id */

  int *array; /* we'll make this point to the shared memory block */

  /* allocate shared memory segments for array */
  smid = shmget(IPC_PRIVATE, N*sizeof(int), IPC_CREAT | IPC_EXCL | 0600);
  if (smid == -1) {
	  perror("shmget: ");
	  exit(1);
  }

  /* attach shared memory for local use */
  array = (int*)shmat(smid, NULL, 0);
  if (!array) {
	  perror("shmat: ");
	  exit(1);
  }

  /* init array */
  for (i = 0; i < N; i++)
	  array[i] = 0;

  /* create semaphore set */

  sem_set_id = semget(IPC_PRIVATE, SEMNUM, IPC_CREAT | IPC_EXCL | 0600);
  if (sem_set_id == -1) {
	  perror("main: semget");
	  exit(1);
  }

  /* initialize both semaphores to 1 */
  for (i = 0; i < SEMNUM; i++) {
	  sem_val.val = 1;
	  rc = semctl(sem_set_id, i, SETVAL, sem_val);
	  if (rc == -1) {
		  perror("main: semctl");
		  exit(1);
	  }
  }

  /* create child processes */
 
  for (i = 0; i < P; i++) {
	  child = fork();
	  if (child == 0) {
			  /* child process */
			  manipulate(array, sem_set_id);
			  exit(0);
		  } else if (child == -1) {
			  perror("fork: ");
			  exit(1);
		  }
			  
  }

  for (i = 0; i < P; i++) {
	  int child_status;
	  wait(&child_status);
  }

  /* print array contents */

  for (i = 0; i < N; i++) {
	  printf("%d ", array[i]);
  }
  printf("\n");

  /* free semaphore set */
  semctl(sem_set_id, 0, IPC_RMID, sem_val);

  /* detach and destroy shared memory */
  shmdt((void*)array);
  shmctl(smid, IPC_RMID, &smdesc);


  return 0;
}


void manipulate(int *array, int sem_set_id)
{
	int i, j, max, sum, tmp, loc, delay;
	struct sembuf sem_op;

	srand(getpid());

	for (i = 0; i < 10; i++) {

		delay = rand()%DELAY;
		for (j = 0; j<delay; j++)
			;

		/* read rest of array */
		max = sum = array[1];
		for (j = 2; j < N; j++) {
			/* read just once to avoid corruption */
			tmp = array[j];
			sum += tmp;
			if (tmp > max)
				max = tmp;
		}

		/* some random position */
		loc = rand()%(N-1)+1;

		/* get exclusive write access to the rest of array (sem1) */
		/* wait on the semaphore 1*/
		sem_op.sem_num = 1;
		sem_op.sem_op = -1;
		sem_op.sem_flg = 0;
		semop(sem_set_id, &sem_op, 1);

		/* now modify array */
		/* others may read this concurrently */

		array[loc] = max + 1;

		/* release semaphore */
		sem_op.sem_num = 1;
		sem_op.sem_op = 1;
		sem_op.sem_flg = 0;
		semop(sem_set_id, &sem_op, 1);

		/* now we go for index 0 */
		/* we need exclusive access to both read and write */

		/* wait on the semaphore 0 */
		sem_op.sem_num = 0;
		sem_op.sem_op = -1;
		sem_op.sem_flg = 0;
		semop(sem_set_id, &sem_op, 1);

		/* now modify index */
		/* no else reads or writes this concurrently */

		if (array[0] < sum)
			array[0] = sum;

		/* release semaphore */
		sem_op.sem_num = 0;
		sem_op.sem_op = 1;
		sem_op.sem_flg = 0;
		semop(sem_set_id, &sem_op, 1);
	}

} /* manipulate() */


