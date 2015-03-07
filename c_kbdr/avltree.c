#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define max(a, b) (a >= b ? a : b)

typedef struct avl_node{
	int value;
	int height;
	struct avl_node *left, *right;
}AvlNode;

AvlNode * create_node()
{
	AvlNode * node = (AvlNode *)malloc(sizeof(AvlNode));
	memset(node, 0, sizeof(AvlNode));
	return node;
}

int height(AvlNode * node)
{
	if(NULL == node)
		return -1;
	return node->height;
}

AvlNode * ll_rotate(AvlNode *node)
{
	AvlNode * p = node->left;
	node->left = p->right;
	p->right = node;
 	node->height = max(height(node->left), height(node->right))+1;
	p->height = max(height(p->left), height(p->right))+1;
	return p;
}

AvlNode * rr_rotate(AvlNode *node)
{
	AvlNode * p = node->right;
	node->right = p->left;
	p->left = node;
 	node->height = max(height(node->left), height(node->right))+1;
	p->height = max(height(p->left), height(p->right))+1;
	return p;
}

AvlNode * lr_rotate(AvlNode *node)
{
	node->left = rr_rotate(node->left);
	node = ll_rotate(node);
	return node;
}

AvlNode * rl_rotate(AvlNode *node)
{
	node->right = ll_rotate(node->right);
	node = ll_rotate(node);
	return node;
}

AvlNode * insert_tree(AvlNode * node, int val)
{
	if(NULL == node) 
	{
		node = create_node();
		node->value = val;	
	}
	else
	{
		if(val == node->value);
		else if(val < node->value)
		{
			node->left = insert_tree(node->left, val);
			if(2 == height(node->left)-height(node->right))
			{
				if(val < node->left->value)
					node = ll_rotate(node);	
				else
					node = lr_rotate(node);
			}
		}
		else
		{
			node->right = insert_tree(node->right, val);
			if(2 == height(node->right)-height(node->left))
			{
				if(val < node->right->value)
					node = rl_rotate(node);
				else
					node = rr_rotate(node);
		   }
		}
	}
	node->height = max(height(node->left), height(node->right)) + 1;	
	return node;	
}

void print_tree(AvlNode *node)
{
	if(node != NULL)
	{
		print_tree(node->left);
		printf("%d\n", node->value);
		print_tree(node->right);
	}
}


int main()
{
	int i;
	AvlNode * root = NULL;
	int arr[] = {1, 2, 3};

	for(i=0; i<sizeof(arr)/sizeof(int); i++)
		root = insert_tree(root, arr[i]);

	print_tree(root);
	return 0;
}

