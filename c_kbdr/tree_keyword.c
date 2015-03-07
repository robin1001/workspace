#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define MAX_LEN 128

typedef struct node{
	char *word;
	int count;
	struct node * left;
	struct node * right;
}Node;

Node * create_node()
{
	Node * node = (Node *)malloc(sizeof(Node));
	memset(node, 0, sizeof(Node));
	return node;
}

char * copy_str(char *src)
{
	char * dest = (char *)malloc(strlen(src)+1);
	strcpy(dest, src);
	return dest;
}

Node * insert_node(Node * node, char *word)
{
	int cmp;
	if(node == NULL)
	{
		node = create_node();
		node->word = copy_str(word); 
		node->count = 1;
	}
	else
	{
		if((cmp = strcmp(word, node->word)) == 0)
			node->count++;
		else if(cmp < 0)
			node->left = insert_node(node->left, word);
		else
			node->right = insert_node(node->right, word);
	}
	return node;
}

void print_tree(Node * node)
{
	if(node != NULL)
	{
		print_tree(node->left);
		printf("%s\t%d\n", node->word, node->count);
		print_tree(node->right);
	}
}

int main()
{
	char token[MAX_LEN] = {0};
	Node * root = NULL;
	while(scanf("%s", token) != EOF)
		root = insert_node(root, token);
   	
	print_tree(root);	
	return 0;
}

