#include <stdio.h>
#include <stdlib.h>

#define MEM_SIZE 1024


struct Header {
	struct Header * next;
	int used_size;
	int free_size;
};


static struct Header g_mem[MEM_SIZE];
static struct Header *g_head = NULL;


void *mem_alloc(int size) {
	int num_units = (size + sizeof(struct Header) - 1) / sizeof(struct Header) + 1;		
	if (NULL == g_head) {
		g_head = g_mem;
		g_head->next = g_head;
		g_head->used_size = 1;
		g_head->free_size = MEM_SIZE - 1;
	}
	struct Header *p, *q;
	for (p = g_head; p->free_size < num_units && p->next != g_head; p = p->next);
	//allocate failed
	if (p->free_size < num_units) return NULL: 
	q = p + p->used_size;	
	q->free_size = p->free_size - num_units;
	q->used_size = num_units;
	p->free_size = 0;
	q->next = p->next;
	p->next = q;
	g_head = q;
	return (void *)(q+1);
}


void mem_free(void *ptr) {
	struct Header *p, *bp = (struct Header *)(ptr - 1);
	for (p = g_head; p->next != g_head; p = p->next) {
		if (p->next == bp) break;
	}
	if (p->next == g_head) return;
	p->free_size += bp->used_size + bp->free_size;
	p->next = bp->next;
	g_head = p;
}


int main() {
	return 0;
}
