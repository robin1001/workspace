#include <stdio.h>

#include <vector>


#define error_msg(x) ()

class Deque {
public:
	Deque(int map_size = 10, int bucket_size = 2):map_size_(map_size), 
		bucket_size_(bucket_size), map_(map_size, NULL) {
		start_ = end_ = map_size_/2;
		start_cur_ = end_cur_ = bucket_size_ / 2;
		for (int i = 0; i < map_size_; i++) {
			map_[i] = new int[bucket_size_];
		}
	}
	~Deque() {
		for (int i = 0; i < map_size_; i++) {
			delete [] map_[i];
			map_[i] = NULL;
		}
	}
	int pop_front() {
		int x = map_[i][start_cur_];
		start_cur_ ++;
		if (start_cur_ >= bucket_size_) {
			start_++;
			if (start_ == map_size_) error_msg(""); 
			start_cur_ = 0;
		}
		return x;
	}
	void push_front(int x) {
		start_cur_--;	
		if (start_cur_ < 0) {
			start_--;
			if (start < 0) error_msg("");
			start_cur_ = bucket_size_;
		}
		map[start_][start_cur_] = x;
	}
	int pop_back() {
		/* the same as pop front */
	}
	void push_back(int x) {
		/* the same as push front */
	}
	void at(int i) {
		int num1 = bucket_size_ - start_cur_;			
		if (i < num1) {
			return map_[start_][start_cur_+i];
		}
		int num_node = (i - num1) % bucket_size_ + 1;
		int left = (i - num1) / bucket_size_;

		return map_[start_+num_node][left];
	}

	void remove(int i) {
		/* judge start move or end move */
	}
	void insert(int i, int x) {
	}
protected:
	int start_;
	int end_;
	int start_cur_;
	int end_cur_;
	int map_size_;
	int bucket_size_;
	std::vector<int *> map_;
}




int main() {
}
