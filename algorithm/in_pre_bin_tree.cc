#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct tree_node{
	char ch;
	struct tree_node *left, *right;
}TreeNode;

char prefix[128] = "ABECDFGHIJ";
char midfix[128] = "EBCDAFHIGJ";


void CreateTree(TreeNode ** ptr,int start, int end)
{
	int i, j;
	int mid = -1;
	if(start == end)
	{
		TreeNode * node = (TreeNode *)malloc(sizeof(TreeNode));
		node->ch = midfix[start];
		node->left = NULL;
		node->right = NULL;
		printf("%c\n", node->ch);
		*ptr = node;	
		return;
	}
	else
	{
		//在先序中查找第一个出现的字符，即中序中间字符
		for(i=0; i<strlen(prefix); i++)
		{
			for(j=start; j<=end; j++)
			{
				if(prefix[i] == midfix[j])
				{
					mid = j;
					break;
				}
			}
			if(mid != -1)
				break;
		}
		//create node
		TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
		node->ch = midfix[mid];
		node->left = NULL;
		node->right = NULL;
		printf("%c\n", node->ch);
		*ptr = node;
		if(mid > start) CreateTree(&(node->left), start, mid-1);
		if(mid < end) 	CreateTree(&(node->right), mid+1, end);		
	}
}

void PrefixTransfer(TreeNode * tree)
{
	printf("%c", tree->ch);
	if(tree->left) 	PrefixTransfer(tree->left);
	if(tree->right) PrefixTransfer(tree->right);
}
void InfixTransfer(TreeNode * tree)
{
	if(tree->left) 	InfixTransfer(tree->left);
	printf("%c", tree->ch);
	if(tree->right) InfixTransfer(tree->right);
}

int main()
{
	TreeNode *tree;
	CreateTree(&tree, 0, strlen(midfix)-1);
	PrefixTransfer(tree);
	printf("\n");
	InfixTransfer(tree);	
	getchar();
	return 0;
}


