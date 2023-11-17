# include <stdio.h>
# include <stdlib.h>
# define MINPID 0
# define MAXPID 128

void show();

unsigned int bitmap[4];
int allocate_map(){
	int i;
	for(i=0;i<4;i++)
	{
		bitmap[i]=0;
	}
	return 1;
}
int allocate_pid(){
	int i,exp,j,sum=1;
	unsigned int temp;
	for(i=0;i<4;i++)
	{	
		exp=0;
		temp=bitmap[i];
		while(temp%2!=0) 
		{
			temp/=2;
			exp++;	
		}
		if(exp==0)
		{
			temp+=1;
			bitmap[i]+=1;
			break;
		}
		else if(exp<32)
		{
			for(j=0;j<exp;j++)
			{
				sum*=2;
			}
			bitmap[i]+=sum;
			break;
		}	
	}
	int n=0;
	n=i*32+exp;
	if(i<4)
	{
		return n; 
	}
	return -1;
}
void release_pid(){
	int pid;
	int flag;
	printf("Please enter the PID you want to delete : ");
	scanf("%d",&pid);
	if(pid<MINPID||pid>MAXPID)
	{
		printf("\nPID out of range!!\n");
		flag = 0;
		return;
	}
	int map=pid/32,loc=pid%32,sum=1,j;
	if((bitmap[map] >> loc)%2==0)
	{
		printf("\nThis Pid is not allocated\n");
		flag = 0;
		return ;
	}
	if(loc==0)
	{
		bitmap[map]-=sum;
		flag = 1;
		return;
	}
	for(j=0,sum=1;j<loc;j++)
	{
		sum*=2;
	}
	bitmap[map]-=sum;
	if (flag!=0)
	{
		printf("\n");
		show();
	}
	
}
void show(){
	int i;
	for(i=0;i<4;i++){
		printf("bitmap[%d] = %u\n",i,bitmap[i]);
	}
}
int main() {
	int i,input=0,pid=0;
	allocate_map();
	printf("-----------Allocating bitmap-----------\n");
	show();
	while(input!=3){
		printf("---------------------------------------\n");
		printf("(1)create a process (2)delete a process  (3)exit\n");
		printf("Please enter your choice : ");
		scanf("%d",&input);
		switch(input){
			case 1:
				pid=allocate_pid();
				if(pid!=-1)
					printf("Successful to allocate PID. The PID of new process : %d\n",pid);
				else 
					printf("bitmap is FULL\n");
				printf("\n");
				show();
				break;
			case 2:
				release_pid();
				break;
			case 3:
				printf("Exit this process !\n");
				break;
			default:
				printf("Please enter a number between 1 to 3!!\n");
		}
		
	}
	return 0;
}

