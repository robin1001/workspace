#include <stdio.h>

#define STACK_SIZE  (1024 * 64) 

typedef unsigned int uint;
typedef struct Tcb{
	uint *pStack;
}TCB;

TCB * tcb_cur;
TCB tcb_arr[2] = {0};
int cur = 0;

uint stack1[STACK_SIZE];
uint stack2[STACK_SIZE];

void TaskCreate(TCB * ptcb,void (* task)(void), uint * pstack)
{
	//init task
	uint * stk;
	stk = (uint *)pstack;         /* Simulate call to function with argument                 */  
	*--stk = (uint)0;
	*--stk = (uint)0X00000000;	//子程序是从当前esp＋4处取得传入的参数，所以此处要空出4个字节
	*--stk = (uint)task;          /* Put pointer to task   on top of stack                   */
	*--stk = (uint)0x00000202;				/* EFL = 0X00000202												*/
	*--stk = (uint)0xAAAAAAAA;                /* EAX = 0xAAAAAAAA                                              */
	*--stk = (uint)0xCCCCCCCC;                /* ECX = 0xCCCCCCCC                                             */
	*--stk = (uint)0xDDDDDDDD;                /* EDX = 0xDDDDDDDD                                             */
	*--stk = (uint)0xBBBBBBBB;                /* EBX = 0xBBBBBBBB                                             */
	*--stk = (uint)0x00000000;                /* ESP = 0x00000000  esp可以任意，因为                            */
	*--stk = (uint)0x11111111;                /* EBP = 0x11111111                                             */
	*--stk = (uint)0x22222222;                /* ESI = 0x22222222                                             */
	*--stk = (uint)0x33333333;                /* EDI = 0x33333333												*/

	ptcb->pStack = stk;
}

void TaskSwitch()
{
	_asm{
		lea	 eax, nextstart	;任务切换回来后从nextstart开始
			push eax
			pushfd				;标志寄存器的值
			pushad				;保存EAX -- EDI		
			mov ebx, [tcb_cur]
			mov [ebx], esp		;把堆栈入口的地址保存到当前TCB结构中
	}

	tcb_cur = &(tcb_arr[(cur++) % 2]);

	_asm{
		mov ebx, [tcb_cur]
		mov esp, [ebx]		;得到OSTCBHighRdy的esp
		popad				;恢复所有通用寄存器，共8个
		popfd				;恢复标志寄存器
		ret					;跳转到指定任务运行
	}
nextstart:			//任务切换回来的运行地址
	return;
}

void delay(long n)
{
	while(n--);
}

void task1Fxn()
{
	for(;;)
	{
		printf("i am task 1\n");
		delay(0xFFFFFFF);
		TaskSwitch();
	}
}

void task2Fxn()
{
	for(;;)
	{
		printf("i am task 2\n");
		delay(0xFFFFFFF);
		TaskSwitch();
	}
}


void TaskStart()
{
	_asm{
		mov ebx, [tcb_cur]
		mov esp, [ebx]		;得到OSTCBHighRdy的esp

		popad				;恢复所有通用寄存器，共8个
		popfd				;恢复标志寄存器
		ret					;跳转到指定任务运行
	}
}


int main()
{
	TaskCreate(&tcb_arr[0], task1Fxn, &stack1[STACK_SIZE-1]);
	TaskCreate(&tcb_arr[1], task2Fxn, &stack2[STACK_SIZE-1]);
	tcb_cur = &tcb_arr[0];

	TaskStart();
	return 0;
} 
