/*  
 * Created by robin1001, 2014-10-16
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TOKEN_MAX 128
#define TOKEN_LEN 32 //max length of token
#define STACK_MAX 128 
#define TRUE 1
#define FALSE 0

int get_token(char *token);
int in_to_post(char in_arr[][TOKEN_LEN], int n, char post_arr[][TOKEN_LEN]);
int post_eval(char post_arr[][TOKEN_LEN], int n);
int in_eval(char in_arr[][TOKEN_LEN], int n);


int main()
{
	char in_arr[TOKEN_MAX][TOKEN_LEN] = {0};
	char post_arr[TOKEN_MAX][TOKEN_LEN] = {0};
	char token[TOKEN_LEN] = {0};
	int len_in = 0, len_post = 0, i;
	printf("please input the expression,for example 1+2*3\n");

	fflush(stdin);
	while (get_token(token) == TRUE) {
		strcpy(in_arr[len_in++], token);
		printf("%s ", token);
	}
	printf("\n");

	printf("\n/********************postfix evaluation********************/\n");
	len_post = in_to_post(in_arr, len_in, post_arr);
	printf("postfix of expression is:");
	for (i=0; i<len_post; i++) printf("%s ", post_arr[i]);
	printf("\n");
	printf("post result: %d\n", post_eval(post_arr, len_post));

	printf("\n/**********************infix evaluation********************/\n");
	printf("in result: %d\n", in_eval(in_arr, len_in));
	return 0;
}


/*
 * 将输入字符串按运算符，数字分开,并进行错误处理
 * 如 1+2*3，则每次调用全局变量token分别返回1 + 2 * 3
 * 调用成功返回TRUE, 非法字符则返回退出程序，结束符'\n'返回false
 */
int get_token(char *token)
{
	char ch;
	int cur = 0;
	//ignore or the space
	while (' ' == (ch = getchar())) continue;
	if ('+' == ch || '-' == ch || '*' == ch || '/' == ch || '(' == ch || ')' == ch) {
		token[cur++] = ch;
	}
	else if (isdigit(ch)) {
		token[cur++] = ch;
		while(isdigit(ch = getchar())) token[cur++] = ch;
		ungetc(ch, stdin); //将多读取的非数字字符回退到stdin中
	}
	else if ('\n' == ch) return FALSE;
	else {
		printf("error:unexpected char %c!!!!!\n", ch);
		exit(-1); 
	}
	
	token[cur] = '\0';
	return TRUE;
}
/*判断token是操作数数字或是运算符*/
int is_num(const char *token)
{
	if (isdigit(token[0])) return TRUE;
	return FALSE;
}

char get_op(const char *token)
{
	return token[0];
}

int get_priority(char a, char b)
{
	if ('(' == a || '(' == b )
		return 1;
	if (')' == a )
		return -1;
	if ('+' == b || '-' == b)
		return 1;
	if ('+' == a || '-' == a)
		return -1;
	return 1;
}

/*中缀转后缀
 *返回值: 转换后后缀表达式中token的个数, 所有token存放于post_arr中
 */
int in_to_post(char in_arr[][TOKEN_LEN], int n, char post_arr[][TOKEN_LEN])
{
	char op_stack[STACK_MAX] = {0}; //运算符栈
	int top = -1, i; //top, top of op stack
	int cur = 0; //cur to post_arr
	char ch;
	for (i = 0; i < n; i++) {
		if (is_num(in_arr[i])) //数字，直接进后序数组
		   strcpy(post_arr[cur++], in_arr[i]);
		else { //运算符判断优先级，进行栈操作
			ch = get_op(in_arr[i]);
			if ( top < 0) { //stack is empty, push
				top = 0;
				op_stack[0] = ch;
			} else {
				if (get_priority(ch, op_stack[top]) > 0) //push
					op_stack[++top] = ch;
				else { //pop
					if (')' == ch) {//pop until '('
						while (top > 0 && op_stack[top] != '(') {
							post_arr[cur][0] = op_stack[top];
							post_arr[cur][1] = '\0';
							top--;
							cur++;
						}
						if (op_stack[top] != '(') {
							printf("error: bracket doesn't match!!!!!\n");
							exit(-1);
						}
						top--;
					} else { //pop top, push ch
						post_arr[cur][0] = op_stack[top];
						post_arr[cur][1] = '\0';
						cur++;
						op_stack[top] = ch;
					}
				}
			}

		}
	}
	while (top >= 0) {
		post_arr[cur][0] = op_stack[top--];
		post_arr[cur][1] = '\0';
		cur++;
	}

	return cur;
}

int calc(int num1, int num2, char ch)
{
	int result = 0;
	switch(ch) {
	case '+':
		result  = num1 + num2;
		break;
	case '-':
		result = num1 - num2;
		break;
	case '*':
		result = num1 * num2;
		break;
	case '/':
		if (0 == num2) {
			printf("error: devide 0\n");
			exit(-1);
		}
		result = num1 / num2;
		break;
	default:
		break;
	}
	return result;
}

/* 对后缀表达式进行求值*/
int post_eval(char post_arr[][TOKEN_LEN], int n)
{
	int i, top = -1;
	int num_stack[STACK_MAX] = {0};
	char ch;
	for (i = 0; i < n; i++) {
		if (is_num(post_arr[i])) 
			num_stack[++top] = atoi(post_arr[i]);
		else {
			ch = get_op(post_arr[i]);
			//the same as num2=pop() num1=pop() push(num1 op num2) 
			num_stack[top-1] = calc(num_stack[top-1], num_stack[top], ch);
			top = top - 1;
		}
	}
	return num_stack[0];
}

/* 对中缀表达式进行求值 */
int in_eval(char in_arr[][TOKEN_LEN], int n)
{
	int i, op_top = -1, num_top = -1;
	char op_stack[STACK_MAX];
    int num_stack[STACK_MAX];
	char ch;
	for (i = 0; i < n; i++) {
		if (is_num(in_arr[i]))
			num_stack[++num_top] = atoi(in_arr[i]);
		else {
			ch = get_op(in_arr[i]);
			if ( op_top < 0) { //op stack is empty, push
				op_top = 0;
				op_stack[0] = ch;
			} else {
				if (get_priority(ch, op_stack[op_top]) > 0) //push
					op_stack[++op_top] = ch;
				else {
					if (')' == ch) { //pop utill (
						while (op_top > 0 && op_stack[op_top] != '(') {
							num_stack[num_top-1] = calc(num_stack[num_top-1], num_stack[num_top], op_stack[op_top]);
							num_top = num_top - 1;
							op_top--;
						}
						if (op_stack[op_top] != '(') {
							printf("error: bracket doesn't match!!!!!\n");
							exit(-1);
						}
						op_top--;
					} else {
						//equals pop top, calc, push ch
						num_stack[num_top-1] = calc(num_stack[num_top-1], num_stack[num_top], op_stack[op_top]);
						num_top = num_top - 1;
						op_stack[op_top] = ch;
					}
				}
			}
		}
	}
    //calc left op in op_stack
	while (op_top >= 0) {
		num_stack[num_top-1] = calc(num_stack[num_top-1], num_stack[num_top], op_stack[op_top]);
		num_top = num_top - 1;
		op_top--;
	}
	
	return num_stack[0];
}
