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
	
	/* Initialize the 3 semaphores.
	 * sem_mutex begin with 0 
	 * sem_empty begin with STACK_SIZE
	 * sem_full begin with 0 
	 * respectively defined in set_semvalue(). 
     * Initially the producer will set the semaphore. */
    
	for (int i = 0; i < SSIZE; ++i) 
	    set_semvalue(sem_id, i);	
    
    init_vet();
	while (TRUE) {
		if (!isFull(shm_queue)) {                   //  If not full      
			if (!sem_wait(sem_id, 0))			  	// Block sem_mutex   
				exit(EXIT_FAILURE);
				
			int aux = rand() % 26;
			int break_time = rand() % 4;
			//int break_time = 3;				
			push(shm_queue, char_vet[aux]);			 // Critical region  
			show_queue(shm_queue);
			
			if (!sem_signal(sem_id, 2))              // ++sem_full, --sem_empty, free sem_mutex 
				exit(EXIT_FAILURE);
			if (!sem_wait(sem_id, 1))
				exit(EXIT_FAILURE);
			if (!sem_signal(sem_id, 0))
				exit(EXIT_FAILURE);
				
			sleep(break_time);
		} else {
			printf("Queue is full, waiting for consumer...\n"); fflush(stdout);
			sleep(3);			
			//sem_wait(sem_id, 1);
		} 
	}  
	
	if (shmctl(shm_id, IPC_RMID, 0) == -1) {
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }

    /* Free semaphore set using semctl. */
    
	//del_semvalue(sem_id); 
		
	return 0;
}

void init_vet() {
	int aux = 97;
	for (int i = 0; i < CSIZE; ++i) {
		char_vet[i] = aux;
		++aux;
	}
}
