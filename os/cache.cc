/* 实验八 请求分页存储管理设计 fifo使用循环队列实现
 * 1. fifo
 * 2. lru
 * Autor: robin1001
 * date:  2013-05-23
 */
	

#include <stdio.h>
#include <time.h>
#include <stdlib.h>


#define HARD_NUM 4
#define INSTRUCT 320
#define VIRTUAL_NUM 32

int instruct[INSTRUCT]={0};

typedef struct{
	int id;//虚存id
	bool flag;//是否装入
	int count;//最近使用次数
}HM;

typedef struct{
	HM hm[HARD_NUM+1];
	int head;
	int tail;
}FIFO;

typedef struct{
	HM hm[HARD_NUM];
}LRU;


FIFO fifo;
LRU lru;

void init();
void fifoTest();
bool isInFifo(int page);
void lruTest();

int main()
{
	init();
	printf("先进先出fifo...\n");
	fifoTest();

	printf("\n\n");

	printf("最近最少使用fifo...\n");
	lruTest();
	system("pause");
	return 0;
}

void init()
{
	int i,j;
	srand(time(0));
	for(i=0; i<INSTRUCT; i+=5)
	{
		instruct[i] = 1.0 * rand() / RAND_MAX * 320;
		instruct[i+1] = instruct[i] + 1;
		instruct[i+2] = 1.0 *rand() / RAND_MAX * instruct[i];
		instruct[i+3] = instruct[i+2] + 1;
		instruct[i+4] = 319 - 1.0 *rand() / RAND_MAX * instruct[i+3];
	}
}

void fifoTest()
{
	int i,j;
	int lack=0;//缺页次数
	//内存初始化
	for(i=0; i<HARD_NUM; i++)
	{
		fifo.hm[i].id=0;
		fifo.hm[i].flag=false;
		fifo.hm[i].count=0;
	}

	//模拟执行320条指令
	for(i=0; i<INSTRUCT; i++)
	{
		//计算该指令在那一页,判断是否在FIFO中
		int page = instruct[i] % VIRTUAL_NUM;
		bool isIn =isInFifo(page);
		if(isIn)//在实存中
		{
			printf("%d alread in fifo\n",instruct[i]);
		}
		else//缺页，插入队列
		{
			lack ++;
			if((fifo.tail + 1)%(HARD_NUM+1) == fifo.head)//队列已满
			{
				printf("page %d in,page %d out\n",page,fifo.hm[fifo.head].id);
				fifo.hm[fifo.head].id = page;
				fifo.tail =fifo.head;
				fifo.head = (fifo.head + 1) %(HARD_NUM+1);
			}
			else
			{
				printf("page %d in\n",page);
				fifo.hm[fifo.tail].id = page;
				fifo.hm[fifo.tail].flag = true;
				fifo.tail = (fifo.tail+1) %(HARD_NUM+1);
			}
		}
	}

	printf("缺页次数:%d\n",lack);
	printf("命中:%lf\n",1.0 - 1.0 * lack / INSTRUCT);
}

bool isInFifo(int page)
{
	int cur=fifo.head;

	if( cur ==fifo.tail)//队列为空
		return false; 
	while(1)
	{
		if( cur % (HARD_NUM+1) ==fifo.tail)//已经到队列尾
			break;
		if(page == fifo.hm[cur].id && fifo.hm[cur].flag == true)
			return true;
		cur++;
	}

	return false;
}


void lruTest()
{
	int i,j;
	int lack=0;//缺页次数
	//内存初始化
	for(i=0; i<HARD_NUM; i++)
	{
		lru.hm[i].id=0;
		lru.hm[i].flag=false;
		lru.hm[i].count=0;
	}

	//模拟执行320条指令
	for(i=0; i<INSTRUCT; i++)
	{
		//计算该指令在那一页,判断是否在实存中
		int page = instruct[i] % VIRTUAL_NUM;
		bool isIn=false;
		for(j=0; j < HARD_NUM; j++)
			if(lru.hm[j].id==page && lru.hm[j].flag ==true)
			{
				isIn=true;
				lru.hm[j].count ++;
			}

		if(isIn)
			printf("%d alread in...\n",page);
		else
		{
			lack++;
			//查找最近最少使用的项
			int cur=0,count=INT_MAX;
			for(j=0; j < HARD_NUM; j++)
			{
				if(lru.hm[j].count < count)
				{
					cur = j;
					count = lru.hm[j].count;
				}

			}

			if(lru.hm[cur].flag ==false)
			{
				printf("page %d in\n",page);
				lru.hm[cur].flag =true;
			}
			else
				printf("page %d in,page %d out\n",page,lru.hm[cur].id);
			lru.hm[cur].id=page;
			lru.hm[cur].count ++;

		}
	}

	printf("缺页次数:%d\n",lack);
	printf("命中:%lf\n",1.0 - 1.0 * lack / INSTRUCT);
}

