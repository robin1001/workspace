//Created by robin1001, 2014-12-01
#include <iostream>
#include <stdio.h>
#include <vector>
#include <queue>

#define MAX 65535

using namespace std;

class Arc{
public:
	int node_id_;
	Arc *next_;
	int weight_;
	Arc(int id, Arc *next, int weight): node_id_(id), next_(next), weight_(weight) {}
};

class Node{
public:
	int data_;
	Arc * arc_;
	Node(int data, Arc *arc): data_(data), arc_(arc) {}
	Node(): data_(0), arc_(NULL) {}
};

class Graph{
	vector<Node> nodes_;
	int node_num_;
	int arc_num_;
public:
	Graph(): node_num_(0), arc_num_(0) {}

	void AddNode() {
		nodes_.push_back(Node(0, NULL));
		node_num_++;
	}

	void AddArc(int src, int dest, int weight) {
		Arc * arc = nodes_[src].arc_;
		Arc * parc = new Arc(dest, arc, weight);
		nodes_[src].arc_ = parc;
		arc_num_++;
	}

	void SetNumOfNode(int n) {
		nodes_.resize(n);
		node_num_ = n;
	}

	void Output() {
		printf("nodes: %d\tarcs: %d\n", node_num_, arc_num_);
		printf("adjcent table:\n");
		for (int i = 0; i < node_num_; i++) {
			Node node = nodes_[i];
			printf("%d\t", i);
			for ( Arc *arc = node.arc_; arc; arc = arc->next_) 
				printf("%d:%d\t", arc->node_id_, arc->weight_);
			printf("\n");
		}
	}

	struct Distance {
		int node_id;
		int dis;
		friend bool operator < (Distance d1, Distance d2) {
			return d1.dis > d2.dis;
		}
		Distance() {}
	};
		
	void ShortestPath(int s) {
	    vector<bool> visited(node_num_, false);
		priority_queue<Distance> que;
		vector<Distance> dis_arr(node_num_);
		for (int i = 0; i < dis_arr.size(); i++) {
			dis_arr[i].node_id = i;
			dis_arr[i].dis = MAX;
		}
		dis_arr[s].dis = 0;
		que.push(dis_arr[s]);

		while (!que.empty()) {
			Distance d = que.top();
			que.pop();
			printf("pop:%d\t%d\n", d.node_id, d.dis);
			int id = d.node_id;
			if (visited[id]) 
				continue;
			visited[id] = true;
			
			for (Arc *arc = nodes_[id].arc_; arc; arc = arc->next_) {
				int v = arc->node_id_;
				if(!visited[v] && d.dis + arc->weight_ < dis_arr[v].dis) {
					dis_arr[v].dis = d.dis + arc->weight_;
					que.push(dis_arr[v]);
				}
			}
		}

		for (int i = 0; i < dis_arr.size(); i++) {
			printf("%d\t%d\n", dis_arr[i].node_id, dis_arr[i].dis);
		}
	}
};


int main()
{
	int node_num, arc_num;
	Graph graph;
	int src, dest, weight;
	scanf("%d%d", &node_num, &arc_num);
	graph.SetNumOfNode(node_num);
	for (int i = 0; i <arc_num; i++) {
		scanf("%d%d%d", &src, &dest, &weight);
		graph.AddArc(src, dest, weight);
	}

	graph.Output();
	graph.ShortestPath(0);
	return 0;
}
