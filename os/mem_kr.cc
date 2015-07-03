#include <stdio.h>

#define BLOCK_SIZE 512

struct header {
	struct header *prev, *next;
	int size;
};

static struct header *head = NULL;
static struct header base;


struct header *mem_more(int n); 
void mem_free(void *ptr);
void *mem_alloc(int n);

struct header *mem_more(int n) {
	int size = n > BLOCK_SIZE ? n : BLOCK_SIZE;
	struct head * p = (struct header *)malloc(size * sizeof(header));
	if (p == NULL) return NULL:
	else {
		p->size = size;
		free(p+1);
		return p;
	}
}


void *mem_alloc(int n) {
	struct header *p;
	int num_units = (n + sizeof(struct header) - 1) / sizeof(struct header) + 1;
	if (head == NULL) {
		base.size = 0;
		head = &base;
		head->next = head->prev = head;
	}
	for (p = head; ;p = p->next) {
		if (p->size >= num_units) { //find big enough free block
			if (p->size == num_units) {
				p->prev->next = p->next;
				p->next->prev = p->prev;
			} 
			else {
				p->size -= num_units;		
				p += p->size;
				p->size = num_units;
			}
			head = p->next;
			return (p+1);
		}
		if (p->next == head) { //not find, apply for more

		}
	}
}

void mem_free(void *ptr) {
	struct header *q = (struct header *)ptr - 1;		
	struct header *p, *n;  		
	for (p = head; p->next != head; p = p->next) {
		if ((q > p && q < p->next) || p > p->next)
			break;
	}
	n = p->next;
	//combine with prev
	if (p + p->size = q) {
		p->size += q->size;
		q = p;

	} else {
		q->prev = p;
		p->next = q;
	}
	//combine with next
	if (q + q->size = n) {
		q->size += n->size;
	} else {
		n->prev = q;	
		q->next = n;
	}
}

