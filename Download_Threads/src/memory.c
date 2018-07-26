#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<ctype.h>
#include<pthread.h>
#include<malloc.h> 

#define PROCESS_NAME_LEN 32  //进程名长度
#define	MIN_SLICE 10          //最小碎片大小
#define DEFAULT_MEM_SIZE 1024   //内存大小
#define DEFAULT_MEM_START 0     //起始位置
#define MA_FF 1
#define MA_BF 2
#define MA_WF 3

int mem_size = DEFAULT_MEM_SIZE;    //内存大小
int ma_algorithm = MA_FF;           //当前分配大小
int pid;
int flag = 0;

/*描述每一个空闲的数据结构*/
struct free_block_type 
{
	int size;
	int start_addr;
	struct free_block_type *next;
};

/*指向内存中空闲块链表的首指针*/
struct free_block_type *free_block;

/*每个进程分配到的内存块描述*/
struct allocated_block 
{
	int pid;
	int size;
	int start_addr;
	char process_name[PROCESS_NAME_LEN];
	struct allocated_block *next;
};

/*进程分配内存块链表的首指针*/
struct allocated_block *allocated_block_head = NULL;

void display_menu()
{
	printf("\n");
	printf("1 -Set memory size (default=%d)\n",DEFAULT_MEM_SIZE);
	printf("2 -Select memory allocation algorithm\n");
	printf("3 -New process\n");
	printf("4 -Terminate a process \n");
	printf("5 -Display memory usage\n");
	printf("0 -Exit\n");
}


struct free_block_type* init_free_block(int mem_size)//初始化空闲块
{
	struct free_block_type *fb;
	fb = (struct free_block_type*)malloc(sizeof(struct free_block_type));    
	if (fb == NULL) 
    {
		printf("NO mem \n");
		return NULL;
	}
	fb->size = mem_size;
	fb->start_addr = DEFAULT_MEM_START;
	fb->next = NULL;                       
	return fb;
}
set_mem_size()       //设置内存大小
{
	int size;
	if (flag != 0) 
    {
		printf("Cannot set memory size again\n");
		return 0;
	}
	printf("Total memory size=");
	scanf("%d",&size);
	if (size > 0) 
    {
		mem_size = size;
		free_block->size = mem_size;
	}
}





rearrange_FF()
{
}
rearrange_BF()
{
}
rearrange_WF()
{
}
rearrange(int algorithm)
{
	switch(algorithm){
		case MA_FF:rearrange_FF();break;
		case MA_BF:rearrange_BF();break;
		case MA_WF:rearrange_WF();break;
	}
}

set_algorithm(){              //设置当前分配算法
	int algorithm;
	printf("\t1-First Fit\n");
	printf("\t2-Best Fit\n");
	printf("\t3-Worst Fit\n");
	scanf("%d",&algorithm);
	if(algorithm>=1&&algorithm<=3)
		ma_algorithm=algorithm;
		rearrange(ma_algorithm);
}

int allocate_mem(struct allocated_block *ab)
{
	struct free_block_type *fbt,*pre;
	int request_size=ab->size;
	fbt=pre=free_block;
	if(ma_algorithm==MA_BF)
	{
		if(fbt->next==NULL&&fbt->size>=request_size)  //只有一个内存块
		{    
			ab->size=request_size;
			ab->start_addr=fbt->start_addr;
			fbt->size=fbt->size-request_size;
			if(fbt->size==0)                      //分配完内存为0
			{
				free_block=NULL;
				return 1;
			}
			fbt->start_addr=fbt->start_addr+request_size;   
			return 1;
		}
		else if(fbt->next==NULL&&fbt->size<request_size)
			return -1; 
		while(fbt!=NULL)                    //寻找分配
		{
			if(fbt->size>=request_size)
			{     
				ab->size=request_size;
				ab->start_addr=fbt->start_addr;
				fbt->size=fbt->size-request_size; 
				if(pre!=fbt) 
					pre->next =fbt->next;  
				else                                 //处理头节点
						free_block=fbt->next; 
					
				if(fbt->size==0)
				{
					free(fbt);
					return 1;
				}
				fbt->start_addr=fbt->start_addr+request_size;  //插入fbt
				pre=free_block;
				if(pre->size>fbt->size)						//处理头节点
				{
					fbt->next=pre;
					free_block=fbt;
					return 1;
				}
				while(pre->next!=NULL)
				{
					if(pre->next->size>fbt->size)
					{
						fbt->next=pre->next;
						pre->next=fbt;
						return 1;
					}
					pre=pre->next;
				}
				if(pre->next==NULL)
				{
					pre->next=fbt;
					fbt->next=NULL;
				}
				return 1;
			}
			pre=fbt;
			fbt=fbt->next;
		}
		return -1;
	}
	
	
	
	if(ma_algorithm==MA_FF)
	{
		if(fbt->next==NULL&&fbt->size>=request_size)  //只有一个内存块
		{    
			ab->size=request_size;
			ab->start_addr=fbt->start_addr;
			fbt->size=fbt->size-request_size;
			if(fbt->size==0)              // 内存分配完
			{
				free_block=NULL;
				return 1;
			}
			fbt->start_addr=fbt->start_addr+request_size;
			return 1;
		}
		else if(fbt->next==NULL&&fbt->size<request_size)
			return -1; 
		while(fbt!=NULL)               //寻找分配内存块
		{
			if(fbt->size>=request_size)
			{     
				ab->size=request_size;
				ab->start_addr=fbt->start_addr;
				fbt->size=fbt->size-request_size; 
				if(fbt->size>0)
				{
					fbt->start_addr=fbt->start_addr+request_size;
					return 1;
				}
				else if(fbt->size==0&&pre==fbt)
				{
					free_block=fbt->next;
					free(fbt);
					return 1;
				}
				else if(fbt->size==0&&pre!=fbt)
				{
					pre->next=fbt->next;
					free(fbt);
					return 1; 
				}
			}
			pre=fbt;
			fbt=fbt->next;
		}
			return -1;	
	} 
	
	
	
	
}
new_process()       //创建新进程
{
	struct allocated_block *ab;
	int size;
	int ret;
	ab=(struct allocated_block *)malloc(sizeof(struct allocated_block));
	if(!ab) exit(-5);
	ab->next=NULL;
	pid++;
	sprintf(ab->process_name,"PROCESS-%02d",pid);
	ab->pid=pid;
	printf("Memory for %s:",ab->process_name);
	scanf("%d",&size);
	if(size>0)
		ab->size=size;
	ret=allocate_mem(ab);
	if((ret==1)&&(allocated_block_head==NULL)){
		allocated_block_head=ab;  
		return 1;
	}
	else if(ret==1){
		ab->next=allocated_block_head;
		allocated_block_head=ab;
		return 2;
	}
	else if(ret==-1){
		printf("Allocation fail\n");
		free(ab);
		return -1;
	}
}










struct allocated_block* find_process(int pid)
{
	struct allocated_block *p;
	p=allocated_block_head;
	while(p!=NULL)
	{
		if(p->pid==pid)
			return p;
		p=p->next;
	}
	return NULL;
}

int dispose(struct allocated_block *free_ab)
{
	struct allocated_block *pre,*ab;
	if(free_ab==allocated_block_head){
		allocated_block_head=allocated_block_head->next;
		free(free_ab);
		return 1;
	}
	pre=allocated_block_head;
	ab=allocated_block_head->next;
	while(ab!=free_ab){
		pre=ab;
		ab=ab->next;
	}
	pre->next=ab->next; 
	free(ab);	
}
int free_mem(struct allocated_block *ab)      //将ab所表示的以分配区归还，并进行可能合并
{
	int algorithm=ma_algorithm;
	struct free_block_type *fbt,*pre,*work;
	fbt=(struct free_block_type*)malloc(sizeof(struct free_block_type));
	if(!fbt)
		return -1;
	if(ma_algorithm==MA_BF)
	{
		fbt->size=ab->size;
		fbt->start_addr=ab->start_addr;
		work=free_block;
		if((fbt->size+fbt->start_addr)==work->start_addr)           //合并右边
		{
			fbt->size=fbt->size+work->size;    //处理头节点
			if(work->next==NULL)    //只有一个内存块
			{
				fbt->next=NULL;
				free_block=fbt;
				return 1;
			}             
			free_block=work->next;
		}
		else{
				while(work!=NULL)             //合并右边（不是头节点）
				{
					if((fbt->size+fbt->start_addr)==work->start_addr)
					{	
						fbt->size=fbt->size+work->size;
						if(work->next==NULL)   //尾节点
						{
							pre->next=NULL;
							break;
						}
						else
						{
							pre->next=work->next;
							break;
						}
						free(work);
					}
					pre=work;
					work=work->next;
				}
		}
	
		pre=work=free_block;
		while(work!=NULL)        // 合并左边
		{
			if((work->size+work->start_addr)==fbt->start_addr)
			{
				work->size=work->size+fbt->size;
				if(pre==work&&pre->next==NULL) //头节点且只有头节点
				{
					free_block=work;
					return 1;
				}
				if(pre==work)	//头节点
				{
					free_block=work->next;
					fbt=work;
					break;
				}
				if(work->next==NULL)  //尾节点
				{
					pre->next=NULL;
					fbt=work;
					break;
				}
				pre->next=work->next;
				fbt=work;	
			}
			pre=work;
			work=work->next;	
		}
		pre=free_block;    //排序 插入
		if(pre->size>fbt->size)  //头节点
		{ 
			fbt->next=pre;
			free_block=fbt;
			return 0;
		}
		while(pre->next!=NULL)   //非头节点
		{	
			if(pre->next->size>fbt->size)
			{
				fbt->next=pre->next;
				pre->next=fbt;
				return 0;
			}
			pre=pre->next;
		}
		if(pre->next==NULL)  //比较完 插入到最后
		{ 
			pre->next=fbt;
			fbt->next=NULL;
		}
	return 1;
	}
	
	
	if(ma_algorithm==MA_FF)
	{
		fbt->size=ab->size;
		fbt->start_addr=ab->start_addr;
		work=free_block;
		if((fbt->size+fbt->start_addr)==work->start_addr)   //合并右边 /头节点
		{                                                               
			fbt->size=fbt->size+work->size;
			if(work->next==NULL)   //只有一个内存块
			{
				fbt->next=NULL;
				free_block=fbt;
				return 1;
			}
			 free_block=work->next;
		
		}
		else{          //合并右边 非头节点
			while(work!=NULL)
			{
				if((fbt->size+fbt->start_addr)==work->start_addr)
				{	
					fbt->size=fbt->size+work->size;
					if(work->next==NULL)   //尾节点
					{
						pre->next=NULL;
						break;
					}
					else
					{
						pre->next=work->next;
						break;
					}
					free(work);
				}
				pre=work;
				work=work->next;
			}
		}
		work=free_block;
		while(work!=NULL)              //合并左边
		{            
			if((work->size+work->start_addr)==fbt->start_addr)
			{  
				work->size=work->size+fbt->size;
				return 1;
			}
			work=work->next;	
		}
		pre=free_block;     //未找到合并  按地址插入
		if(pre->start_addr>fbt->start_addr)       //头节点
		{ 
			fbt->next=pre;           
			free_block=fbt;
			return 0;
		}
		while(pre->next!=NULL) 
		{	      
			if(pre->next->start_addr>fbt->start_addr)
			{   
				fbt->next=pre->next;
				pre->next=fbt;
				return 1;
			}
			pre=pre->next;
		}
		if(pre->next==NULL)   //未找到合适  插入到最后
		{
			pre->next=fbt;
			fbt->next=NULL;
		}
	return 1;
	}
			

}
kill_process() //删除进程
{
	struct allocated_block *ab;
	int pid;
	printf("KILL Process,pid=");
	scanf("%d",&pid);
	ab=find_process(pid);
	if(ab!=NULL){
		free_mem(ab);
		dispose(ab);
	}
}




display_mem_usage()
{
	struct free_block_type *fbt=free_block;             
	struct allocated_block *ab=allocated_block_head;
	if(fbt==NULL)
		return -1;
	printf("---------------------------------------------------\n");
	/*显示空闲区*/
	printf("Free Memory:\n");
	printf("%20s  %20s\n","start_addr","size");
	while(fbt!=NULL)
	{
		printf("%20d %20d\n",fbt->start_addr,fbt->size);
		fbt=fbt->next;
	}
	/*显示已分配区*/
	printf("\nUsed Memory:\n");
	printf("%10s %20s %10s %10s\n","PID","ProcessName","start_addr","szie");
	while(ab!=NULL)
	{
		printf("%10d %20s %10d %10d\n",ab->pid,ab->process_name,ab->start_addr,ab->size);
		ab=ab->next;
	}
	printf("---------------------------------------------------\n");
	return 0;
}




int main()
{
	char choice ;
	pid =0;
	free_block=init_free_block(mem_size); //初始化空闲块
	while(1)
	{
		display_menu(); //显示菜单
		fflush(stdin);
		choice=getchar(); //获取用户输入
		switch(choice){
			case '1':set_mem_size();break;    //设置内存大小
			case '2':set_algorithm();flag=1;break;   //设置算法
			case '3':new_process();flag=1;break;    //创建新进程
			case '4':kill_process();flag=1;break;  //删除进程
			case '5':display_mem_usage();flag=1;break; //显示内存
			case '0':exit(0);//释放链表并退出
		}		
		getchar();
	}
}

