#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> 
#include <time.h>

#define MINPID 0
#define MAXPID 128

int count[100];
unsigned int bitmap[4];
int allocate_map(){

	int i;
	for(i=0;i<4;i++){
		bitmap[i]=0;
	}
	return 1;
}

int allocate_pid(){

	int i,exp,j,sum=1;
	unsigned int temp;
	for(i=0;i<4;i++){	

		exp=0;
		temp=bitmap[i];
		while(temp%2!=0){

			temp/=2;
			exp++;	
		}

		if(exp==0){
			temp+=1;
			bitmap[i]+=1;
			break;
		}
		else if(exp<32){
			for(j=0;j<exp;j++){
				sum*=2;
			}
			bitmap[i]+=sum;
			break;
		}
	}

	int n=0;
	n=i*32+exp;
	if(i<4){
		return n; 
	}
	return -1;
}

void release_pid(int pid){

	int map=pid/32,loc=pid%32,sum=1,j;

	if(loc==0){
		bitmap[map]-=sum;
		return;
	}
	for(j=0,sum=1;j<loc;j++){
		sum*=2;
	}
	bitmap[map]-=sum;
}

void show(){

	int i;
	for(i=0;i<4;i++){
		printf("bitmap[%d] = %u\n",i,bitmap[i]);
	}
}

void *child(void *arg){

	int pid,time;
	
	pid=allocate_pid();
	
	time=rand()%10+1;
	sleep(time);
	printf("Thread ID=%lu\npid=%d,this will sleep %d seconds\nRelease Pid %d\n",pthread_self(),pid,time,pid);
	//count[pid]++;
	release_pid(pid);
	

	pthread_exit(NULL);

}

int main() {

	int i,input=0,pid=0;
	allocate_map();
	printf("------Allocating bitmap------\n");
	show();
	pthread_t thread[100];

	while(1){
	
		//for(int j=0;j<100;j++){
		//	count[j]=0;
		//}	
		printf("-----------------------------\n");
		printf("(1)create 100 process (2)exit\n");
		printf("Please enter your choice : ");
		scanf("%d",&input);
		switch(input){

			case 1:
				for(i=0;i<100;i++){
					pthread_create(&thread[i],NULL,child,NULL);
				}
				
				show();
				
				for(i=0;i<100;i++){
					pthread_join(thread[i],NULL);
				}
				
				sleep(3);
				show();
				
				
				//for(int k=0;k<100;k++){
				//	printf("pid %d count: %d\n",k,count[k]);
				//}
				break;

			case 2:
				printf("Exit success!\n");
				return 0;
      
			default:
				printf("Please enter a number between 1 to 2!!\n");
		}
	}
	
	return 0;
}

