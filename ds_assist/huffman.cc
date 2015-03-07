//Created by robin1001, 2014-12-01
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <vector>
#include <queue>
#include <stack>

using namespace std;

struct Node {
	int left, right, parent;
	int w;
	char ch;
	int id;
	Node(): left(-1), right(-1), parent(-1), w(0) {}
    Node(int l, int r, int ww): left(l), right(r), w(ww), parent(-1) {}
};

struct NodeCmp{
	bool operator () ( const Node *n1, const Node *n2 ) {
		return n1->w > n2->w;
	}
};

int main() 
{
	int num, freq;
	char chs[10]; 
	scanf("%d", &num);

	vector<Node> node_arr(2*num+1);
	for (int i = 0; i < num; i++) {
		fflush(stdin);
		scanf("%s%d", chs, &freq);
		node_arr[i].id = i;
		node_arr[i].ch = chs[0];
		node_arr[i].w = freq;
	}
	
	priority_queue<Node *, vector<Node *>, NodeCmp> que;
	for (int i = 0; i < num; i++) 
		que.push(&node_arr[i]);
	int cur = num;
	while (!que.empty()) {
		Node *n1 = que.top();
		que.pop();
		if (que.empty()) {
			break;
		}
		Node *n2 = que.top();
		que.pop();
	
		node_arr[cur] = Node(n1->id, n2->id, n1->w + n2->w);
		Node *n3 = &node_arr[cur];
		n3->id = cur;
		n3->left = n1->id;
		n3->right = n2->id;
		n1->parent = n3->id;
		n2->parent = n3->id;
		//assert(n1 == &node_arr[n1->id]);
		//printf("%d %d\n", (int)&(*n1), (int)&(node_arr[n1->id]));
		//printf("%d %d %d\n", n1->id, n1->parent, node_arr[n1->id].parent);
		//printf("n1: %d %d n2: %d %d n3:%d %d\n", n1->id, n1->w, n2->id, n2->w, n3->id, n3->w);
		que.push(n3);
		cur++;
	}

	for (int i = 0; i < num; i++) {
		Node n = node_arr[i];
		stack<char> coding;
		while (n.parent != -1 ) {
			if (n.id == node_arr[n.parent].left) {
				coding.push('0');
			} else {
				coding.push('1');
			}
			n = node_arr[n.parent];
		}
		
		printf("%c\t", node_arr[i].ch);
		while (!coding.empty()) {
			printf("%c", coding.top());
			coding.pop();
		}
		printf("\n");
	}			

	return 0;
}
