/* 
 * Created by robin1001, 2014-11-11
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>

#define MAX 1024
struct element {
	int i,j;
	int val;
};

struct matrix {
	int row, col;
	int num; //none zero element of matrix
	int row_start[MAX]; //row start position
	struct element data[MAX];
};

void matrix_init(struct matrix *p);
void matrix_show(struct matrix *p);
void matrix_show_sparse(struct matrix * p);
void matrix_trans(struct matrix *p, struct matrix *pt);
void matrix_add(struct matrix *pa, struct matrix *pb, struct matrix *pc);
void matrix_mul(struct matrix *pa, struct matrix *pb, struct matrix *pc);

int main()
{
	struct matrix mat_a, mat_b, mat_c, mat_t;
	int op;
	printf("select your option, transpose(0), add(1), mul(2). other for exit\n");
	while (1) {
		scanf("%d", &op);
		switch (op) {
		case 0: //transpose
			printf("please input your matrix\n");
			matrix_init(&mat_a);
			matrix_trans(&mat_a, &mat_t);
			matrix_show(&mat_t);
			matrix_show_sparse(&mat_t);
			break;
		case 1: //add
			printf("please input matrix A\n");
			matrix_init(&mat_a);
			printf("please input matrix B\n");
			matrix_init(&mat_b);
			matrix_add(&mat_a, &mat_b, &mat_c);
			printf("result matrix C is\n");
			matrix_show(&mat_c);
			matrix_show_sparse(&mat_c);
			break;
		case 2: //mul
			break;
		default:
			exit(0);
		}
	}

	return 0;
}

void matrix_init(struct matrix *p)
{
	int i, j, k, val;
	int cur = 0;
	memset(p, 0, sizeof(*p));
	printf("please input row & col\n");
	scanf("%d", &p->row);
	scanf("%d", &p->col);
	printf("please input elements of current matrix\n");
	for (i = 0; i < p->row; i++) {
		for (j = 0; j < p->col; j++) {
			scanf("%d", &val);
			if (val) {
				p->data[cur].i = i;
				p->data[cur].j = j;
				p->data[cur].val = val;
				cur++;
			}
		}
	}
	p->num = cur;
	//calc row start array
	for (k = 0; k < p->num; k++) 
		p->row_start[p->data[k].i+1]++;
	p->row_start[0] = 0;
	for (i = 1; i <= p->row; i++)
		p->row_start[i] += p->row_start[i-1];
	assert(p->num == p->row_start[p->row]);
}

void matrix_show(struct matrix *p)
{
	int i, j;
	int cur = 0;
	printf("matrix in row*col way\n");
	for (i = 0; i < p->row; i++) {
		for (j = 0; j < p->col; j++) {
			if ((cur < p->num) && (p->data[cur].i == i) && (p->data[cur].j == j)) {
				printf("%d ", p->data[cur].val);
				cur++;
			} else {
				printf("0 ");
			}
		}
		printf("\n");
	}
}

void matrix_show_sparse(struct matrix * p)
{
	int k;
	printf("sparse matrix info: row=%d col=%d num=%d\n", p->row, p->col, p->num);
	printf("i, j, val\n");
	for (k = 0; k < p->num; k++) {
		printf("%d %d %d\n", p->data[k].i, p->data[k].j, p->data[k].val);
	}
}

void matrix_trans(struct matrix *p, struct matrix *pt)
{
	int col_start[MAX] = {0};
	int k, j;
	int pos;
	for (k = 0; k < p->num; k++) col_start[p->data[k].j+1]++;
	for (j = 1; j < p->col; j++)
		col_start[j] += col_start[j-1];
	pt->num = p->num;
	pt->row = p->col;
	pt->col = p->row;
	for (k = 0; k < p->num; k++) {
		pos = col_start[p->data[k].j];
		pt->data[pos].i = p->data[k].j;
		pt->data[pos].j = p->data[k].i;
		pt->data[pos].val = p->data[k].val;
		col_start[p->data[k].j]++;
	}
}

void matrix_add(struct matrix *pa, struct matrix *pb, struct matrix *pc)
{
	int cur1 = 0, cur2 = 0;
	int sum;
	if (pa->row != pb->row && pa->col != pb->col) {
		printf("matrix A B row & col match error\n");
		exit(-1);
	}
	pc->row = pa->row;
	pc->col = pa->col;
	pc->num = 0;
	while (cur1 < pa->num && cur2 < pb->num) {
		if ((pa->data[cur1].i < pb->data[cur2].i) || (pa->data[cur1].i == pb->data[cur2].i &&  pa->data[cur1].j < pb->data[cur2].j)) {
			pc->data[pc->num].i = pa->data[cur1].i;
			pc->data[pc->num].j = pa->data[cur1].j;
			pc->data[pc->num].val = pa->data[cur1].val;
			pc->num++;
			cur1++;
		}
		else if (pa->data[cur1].i > pb->data[cur2].i || (pa->data[cur1].i == pb->data[cur2].i &&  pa->data[cur1].j > pb->data[cur2].j)) {
			pc->data[pc->num].i = pb->data[cur2].i;
			pc->data[pc->num].j = pb->data[cur2].j;
			pc->data[pc->num].val = pb->data[cur2].val;
			pc->num++;
			cur2++;
		} 
		else { // == + i==i, j==j
			sum = pa->data[cur1].val + pb->data[cur2].val;
			if (sum) {
				pc->data[pc->num].i = pa->data[cur1].i;
				pc->data[pc->num].j = pa->data[cur1].j;
				pc->data[pc->num].val = sum;
				pc->num++;
			}
			cur1++;
			cur2++;
		}
	}
	while (cur1 < pa->num) {
		pc->data[pc->num].i = pa->data[cur1].i;
		pc->data[pc->num].j = pa->data[cur1].j;
		pc->data[pc->num].val = pa->data[cur1].val;
		pc->num++;
		cur1++;
	}
	while (cur2 < pb->num) {
		pc->data[pc->num].i = pb->data[cur2].i;
		pc->data[pc->num].j = pb->data[cur2].j;
		pc->data[pc->num].val = pb->data[cur2].val;
		pc->num++;
		cur2++;
	}
}

void matrix_mul(struct matrix *pa, struct matrix *pb, struct matrix *pc)
{
	int i, j, row;
	int sum[MAX] = {0};
	if(pa->col != pb->row) {
		printf("error multiply A B col row do not match\n");
		exit(-1);
	}
	pc->row = pa->row;
	pc->col = pb->col;
	pc->num = 0;

	for (row = 0; row < pa->row; row++) {
		for (i = pa->row_start[row]; i < pa->row_start[row+1]; i++) {
			memset(sum, 0, sizeof(int) * pa->col);
		}
	}
}
