#ifndef SHMEM_H
#define SHMEM_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>

#define QUEUE_SIZE 10

struct sh_queue {
	int head;
	int tail;
	int capacity;
	int filled;
	char array[QUEUE_SIZE*sizeof(char)];
};

void create_queue (struct sh_queue* queue) {
	//struct sh_queue* queue = (struct sh_queue*) malloc(sizeof(struct sh_queue));
	queue->capacity = QUEUE_SIZE;
	queue->head = queue->filled = queue->tail = 0;
	//queue->array    = (char*) malloc(sizeof(queue->capacity * sizeof(char)));
	
	for (int i = 0; i < queue->capacity; ++i)
		queue->array[i] = '0';
	
	//return queue;
};

int isFull (struct sh_queue* queue) {
	return queue->filled == queue->capacity;
};

int isEmpty (struct sh_queue* queue) {
	return queue->filled == 0;
};

void push (struct sh_queue* queue, char pChar) {
	if (isFull(queue))
		printf("Queue is full. \n");
	else {
		if (isEmpty(queue)) {
			queue->array[queue->tail] = pChar;
		} else {
			++queue->tail;
			if (queue->tail == queue->capacity) {
				queue->tail = 0;
			} 
			queue->array[queue->tail] = pChar;
		}
		++queue->filled;
	}
};

void pop (struct sh_queue* queue) {
	if (isEmpty(queue))
		printf("Queue is empty. \n");
	else {
		queue->array[queue->head] = '0';	
		++queue->head;
		if ((queue->head == queue->capacity) && queue->array[0] == '0') {
			queue->head = 0;
		}
		--queue->filled;
		if (queue->filled == 0)
			queue->head = queue->tail = 0;
	}
};

void show_queue (struct sh_queue* queue) {
	if (isEmpty(queue))
		printf("Queue is empty. \n");
	else {
		int aux = queue->head;
        while (aux != queue->tail) {
            printf("| %c ", queue->array[aux]);
            ++aux;
            if (aux == queue->capacity)
                aux = 0;             
        }
        printf("| %c |\n\n", queue->array[aux]);
	}
};

char front (struct sh_queue* queue) {
	if (isEmpty(queue)) {
		printf("Queue is empty. \n");
	} 
		
	return queue->array[queue->head];
};
	
#endif /* SHMEM_H */
