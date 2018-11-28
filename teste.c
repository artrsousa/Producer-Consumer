#include "shmem.h"
#include "semaphore.h"
#define SKEY  1      /* semaphore key         */
#define SSIZE 3      /* semaphore array size. */
#define SHKEY 2      /* shared memory key     */
#define TRUE 1
#define FALSE 0
void queue_stat (struct sh_queue* queue) {
	printf("queue capacity: %d \n", queue->capacity);
	printf("queue filled: %d \n", queue->filled);
	printf("queue tail: %d \n", queue->tail);
	printf("queue head: %d \n\n", queue->head);
}

int main (void) {

	static int sem_id;    /* semaphore id           */
    static int shm_id;    /* shared memory block id */
    void* shared_memory = (void*)0; /* */
	
	srand((unsigned int)getpid());
    	
    struct sh_queue* queue;
    
    create_queue(queue);
   // struct sh_queue* queue = create_queue();                                  /* This queue structure will be shared.        */
    shm_id = shmget((key_t)SHKEY, sizeof(struct sh_queue), 0666 | IPC_CREAT); /* Allocate shared memory segments for struct. */
    if (shm_id == -1) {
        fprintf(stderr, "shmget fail. \n");
        exit(EXIT_FAILURE);
    }

    /* Make the shared memory accessible to process.*/
    shared_memory = shmat(shm_id, (void*)0, 0);
    if (shared_memory == (void*)-1) {
        fprintf(stderr, "shmat fail. \n");
        exit(EXIT_FAILURE);
    }

    printf("Memory attached at %X\n", (int*)shared_memory);    


	push(queue, 'a');
	push(queue, 'z');
	push(queue, 'v');
	push(queue, 'h');
	push(queue, 'g');
	push(queue, 's');
	push(queue, 'f');
	push(queue, 'd');
	show_status(queue);
	pop(queue);
	pop(queue);
	pop(queue);
	show_status(queue);
	push(queue, 'd');
	show_status(queue);
	push(queue, 'a');
	push(queue, 't');
	push(queue, 'x');
	show_status(queue);
	pop(queue);
	pop(queue);
	pop(queue);
	show_status(queue);
	pop(queue);
	pop(queue);
	pop(queue);
	show_status(queue);
	push(queue, 'x');
	show_status(queue);
		pop(queue);
	pop(queue);
	show_status(queue);
	
	sem_id = semget((key_t)SKEY, SSIZE, 0666 | IPC_CREAT);                    /* Allocate or get semaphore set if not exist. */
	if (sem_id == -1) {
		fprintf(stderr, "semget fail. \n");
        exit(EXIT_FAILURE);
    }
del_semvalue(sem_id);
    /* Free semaphore set using semctl. 
	 * del_semvalue(sem_id); */
		
	return 0;
}

