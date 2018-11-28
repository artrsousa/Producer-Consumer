#include "shmem.h"
#include "semaphore.h"
#include <time.h>

#define SKEY  1      /* semaphore key         */
#define SSIZE 3      /* semaphore array size. */
#define SHKEY 2      /* shared memory key     */
#define CSIZE 26		
#define TRUE 1
#define FALSE 0

int char_vet[CSIZE];

/* sem_num = 0 is the sem_mutex 
 * sem_num = 1 is the sem_empty
 * sem_num = 2 is the sem_full. */

void init_vet();

int main (void) {
	static int sem_id;    /* semaphore id           */
    static int shm_id;    /* shared memory block id */
    void* shared_memory = (void*)0; /* */
    
	srand((unsigned int)getpid());
    
    struct sh_queue* shm_queue;                                               /* This queue structure will be shared.        */
    shm_id = shmget((key_t)SHKEY, sizeof(struct sh_queue), 0666 | IPC_CREAT); /* Allocate shared memory segments for struct. */
    if (shm_id == -1) {
        fprintf(stderr, "shmget fail. \n");
        exit(EXIT_FAILURE);
    }

    /* Make the shared memory accessible to process.*/
    shared_memory = (int*)shmat(shm_id, (void*)0, 0);
    if (shared_memory == (void*)-1) {
        fprintf(stderr, "shmat fail. \n");
        exit(EXIT_FAILURE);
    }

    printf("Produer: memory id: %d\n", (int)shm_id);
    printf("Producer: memory attached at %X\n", (int*)shared_memory);
    
    shm_queue = (struct sh_queue*)shared_memory;
	create_queue(shm_queue);
    
	sem_id = semget((key_t)SKEY, SSIZE, 0666 | IPC_CREAT);                    /* Allocate or get semaphore set if not exist. */
	if (sem_id == -1) {
		fprintf(stderr, "semget fail. \n");
        exit(EXIT_FAILURE);
    }
    
	
	if (shmctl(shm_id, IPC_RMID, 0) == -1) {
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }

    /* Free semaphore set using semctl. */
    
	del_semvalue(sem_id); 
		
	return 0;
}

