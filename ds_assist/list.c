/*  
 * Created by robin1001, 2014-10-16
 */
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#define MAX_LEN 128

struct list{
	int val;
	struct list * prev;
	struct list * next;
};


struct list * create_list(int * arr, int n)
{
	struct list *head = NULL, *p;
	int i;
	for (i = 0; i < n; i++) {
		struct list * node = (struct list *)malloc(sizeof(struct list));
		if (!node) {
			printf("malloc error\n");
			return;
		}
		node->val = arr[i];
		if (NULL == head) {
			head = node;
		} else {
			p->next = node;
			node->prev = p;
		}
		p = node; 
	}
	p->next = head;
	head->prev = p;
	return head;
}

void traverse_list(struct list * lis)
{
	struct list *head = lis, *p = lis;
	int flag = 1; //flag denotes if the head node is visited or note
	if (NULL == lis) return;
	while (p != head || flag) {
		printf("%d ", p->val);
		p = p->next;
		if (flag) flag = 0;
	}
	printf("\n");
}

struct list * reverse_list(struct list *lis)
{
	struct list *head = NULL, *p = lis, *q;
	int flag = 1;
	if (lis == NULL) return;
	head = p;
    p = p->next;
	while( p != lis) {//利用插头法构造reverse,将p插入新链表头部
		q = p->next;
		head->prev = p;
		p->next = head;
		head = p;
		p = q;
	}
	head->prev = lis;
	lis->next = head;
	return head; 
}

struct list * merge_list(struct list *l1, struct list *l2)
{
	struct list *l3 = NULL, *p3 = NULL, *p1 = l1, *p2 = l2;
	struct list *t1= l1->prev, *t2= l2->prev;
	int flag1 = 1, flag2 = 1;
	while ((p1 != l1 || flag1) && (p2 != l2 || flag2) ){
		if (p1->val <= p2->val) {
			if (p1 == l1) flag1 = 0; //if head node is visited
			if (NULL == p3) p3 = l3 = p1; //if is head node of l3
			else {
				p3->next = p1;
				p1->prev = p3;
				p3 = p1;
			}
			p1 = p1->next;
		} else {
			if (p2 == l2) flag2 = 0;
			if (NULL == p3)	p3 = l3 = p2;
			else {
				p3->next = p2;
				p2->prev = p3;
				p3 = p2;
			}
			p2 = p2->next;
		}
	}
	if (p1 == l1) {
		p3->next = p2;
		p2->prev = p3;
		l3->prev = t2;
		t2->next = l3;
	} else {
		p3->next = p1;
		p1->prev = p3;
		l3->prev = t1;
		t1->next = l3;
	}
	
	return l3;
}

int main()
{
	struct list *list1 = NULL, *list2 = NULL, *list3 = NULL;
	int arr[MAX_LEN], num, i;
	printf("notice: input the list elements in ascend order\n");
	printf("input length of list1\n");
	scanf("%d", &num);
	printf("input elements of list1\n");
	for(i=0; i<num; i++)
		scanf("%d", &arr[i]);
	list1 = create_list(arr, num);
	printf("input length of list2\n");
	scanf("%d", &num);
	printf("input elements of list2\n");
	for(i=0; i<num; i++)
		scanf("%d", &arr[i]);
	list2 = create_list(arr, num);
	//traverse_list(list1);

	list3 = merge_list(list1, list2);
	printf("merge result of list1 & list2, ascend order\n");
	traverse_list(list3);
	
	list3 = reverse_list(list3);
	printf("descent order\n");
	traverse_list(list3);
	return 0;
}
