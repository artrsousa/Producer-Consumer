#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>
#include "semun.h"

#define QUEUE_SIZE 10

static int set_semvalue(int pSem_id, int pSem_num);
static void del_semvalue(int pSem_id);
static int sem_wait(int pSem_id, int pSem_num);
static int sem_signal(int pSem_id, int pSem_num);

/* The sem_num = 0 is the sem_mutex 
   The sem_num = 1 is the sem_empty
   The sem_num = 2 is the sem_full. */


/* The function set_semvalue initializes the semaphore using the SETVAL command in a
 semctl call. We need to do this before we can use the semaphore. */

static int set_semvalue (int pSem_id, int pSem_num) {
	union semun sem_union;
	int sem_id    = pSem_id;
	int sem_num   = pSem_num;

	if (sem_num == 1)      /* sem_empty conditional */
		sem_union.val = QUEUE_SIZE;
    if (sem_num == 0)      /* sem_mutex conditional */
		sem_union.val = 1;
	if (sem_num == 2)
		sem_union.val = 0; /* sem_full conditional */
   
    if (semctl(sem_id, sem_num, SETVAL, sem_union) == -1) 
	    return 0;

    return 1;
}

/* The del_semvalue function has almost the same form, except the call to semctl uses
 the command IPC_RMID to remove the semaphore's ID. */

static void del_semvalue (int pSem_id) {
	union semun sem_union;
	int sem_id    = pSem_id;
    
    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore\n");
}

/* sem_wait changes the semaphore by -1 (waiting). */

static int sem_wait (int pSem_id, int pSem_num) {
    struct sembuf sem_b;
  	int sem_id    = pSem_id;
	int sem_num   = pSem_num;
    
    sem_b.sem_num = sem_num;
    sem_b.sem_op  = -1; /* P() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_p failed\n");
        return(0);
    }

    return(1);
}

/* sem_signal is similar except for setting the sem_op part of the sembuf structure to 1,
 so that the semaphore becomes available. */

static int sem_signal (int pSem_id, int pSem_num) {
    struct sembuf sem_b;
    int sem_id    = pSem_id;
	int sem_num   = pSem_num;
    
    sem_b.sem_num = sem_num;
    sem_b.sem_op  = 1; /* V() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_v failed\n");
        return(0);
    }

    return(1);
}

#endif /* SEMAPHORE_H */
