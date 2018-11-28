#include "shmem.h"
#include "semaphore.h"

#define SKEY  1      /* semaphore key         */
#define SSIZE 3      /* semaphore array size. */
#define SHKEY 2      /* shared memory key     */
#define TRUE 1
#define FALSE 0

/* sem_num = 0 is the sem_mutex 
 * sem_num = 1 is the sem_empty
 * sem_num = 2 is the sem_full. */

int main (void) {
	static int sem_id;    /* semaphore id           */
    static int shm_id;    /* shared memory block id */
    void* shared_memory = (void*)0; /* */
    	
	srand((unsigned int)getpid());

    struct sh_queue* shm_queue;                                                /* This queue structure will be shared.        */
    shm_id = shmget((key_t)SHKEY, sizeof(struct sh_queue), 0666 | IPC_CREAT);  /* Allocate shared memory segments for struct. */
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

    printf("Consumer: memory id: %d\n", (int)shm_id);
    printf("Consumer: memory attached at %X\n", (int*)shared_memory);

    shm_queue = (struct sh_queue*)shared_memory;  

	sem_id = semget((key_t)SKEY, SSIZE, 0666 | IPC_CREAT);                    /* Allocate or get semaphore set if not exist. */
	if (sem_id == -1) {
		fprintf(stderr, "semget fail. \n");
        exit(EXIT_FAILURE);
    }
	
	/* Initialize the 3 semaphores.
	 * sem_mutex begin with 0 
	 * sem_empty begin with STACK_SIZE
	 * sem_full begin with 0 
	 * respectively defined in producer file.
     * Initially the producer will set the semaphore.
   	 * for (int i = 0; i < SSIZE; ++i) 
	 *     set_semvalue(sem_id, i);	                  */ 

    while (TRUE) {
		if (!isEmpty(shm_queue)) {
			if(!sem_wait(sem_id, 0))
				exit(EXIT_FAILURE);
			
			printf("consuming... \n"); fflush(stdout);								
			int break_time = rand() % 2;              //  Critical region   
			//int break_time = 3;			
			show_queue(shm_queue);
			pop(shm_queue);
			
			if (!sem_wait(sem_id, 2))                 // --sem_full, ++sem_empty, free sem_mutex 
				exit(EXIT_FAILURE);
			if (!sem_signal(sem_id, 1))
				exit(EXIT_FAILURE);
			if (!sem_signal(sem_id, 0))
				exit(EXIT_FAILURE);
				
			sleep(break_time);
		} else {
			printf("Queue is empty, waiting for producer...\n"); fflush(stdout);
			sleep(3);			
			//sem_wait(sem_id, 2);
		} 
	} 
    
	/* Free semaphore set using semctl. 
	 * del_semvalue(sem_id); */
		
	return 0;
}
