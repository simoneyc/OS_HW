#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h> 

#define MINPID 0
#define MAXPID 128

unsigned int bitmap[4];
pthread_mutex_t mtx;

void print_bitmap(unsigned int array[]){
	int i;
	for(i=0;i<4;i++){
		printf("bitmap[%d] = %u\n",i,array[i]);
	}
}

void input_message(){
	printf("-------------------------------------\n");
	printf("(1) create 100 threads (2) exit\n");
	printf("Please enter your choice : ");
}

unsigned int power(int n,int exp){
	int i;
	unsigned int ans=1;
	for(i=0;i<exp;i++){
		ans*=n;
	}
	return ans;
}

int allocate_map(){
	int i;
	for(i=0;i<4;i++)
	{
		bitmap[i]=0;
	}
	return 1;
} 

int allocate_pid(){
	unsigned int tmp=1,n;
	int i,pid=0,exp=0;
	for(i=0;i<4;i++){
		exp=0;
		n=bitmap[i];
		if(n==power(2,32)-1){
			exp+=31;
			continue;
		}	
		else if(n<power(2,32)-1){
			while(tmp!=0){
				tmp=n%2;
				exp+=1;
				n/=2;	
			}
			break;
		}
	}
	bitmap[i]+=power(2,exp-1);
	pid=i*32+exp-1;
	if(pid>=MAXPID)
		return -1;
	else
		return pid;
}

void release_pid(int pid){
	unsigned int tmp;
	int i,count=0,exp;
	i=pid>>5;
	exp=pid%32;
	tmp=bitmap[i];
	if(power(2,exp)>tmp){
		printf("This PID is not allocated.\n");
	}
	while(tmp!=0){
		if(tmp%2==0 ){
			if(count==exp){
				printf("This PID is not allocated.\n");
				break;
			}				
		}
		else if(tmp%2==1){
			if(count==exp){
				bitmap[i]-=power(2,exp);
				break;
			}		
		}
		tmp/=2;
		count+=1;
	}	
}

int sleep_time(){
	int r;
	r=(rand()%3)+1;
	return r;
}

void *child(void *arg){
	int s;
	pthread_mutex_lock(&mtx);
	int pid = allocate_pid();
	pthread_mutex_unlock(&mtx);
	s = sleep_time();
	printf("pid is %d, will sleep %d seconds\n",pid,s);
	printf("tid is %lu\n",pthread_self());
	sleep(s);
	pthread_mutex_lock(&mtx);
	release_pid(pid);
	pthread_mutex_unlock(&mtx);
	pthread_exit(NULL);
}

int main(){
	int i,choice,map_flag;
	map_flag=allocate_map();
	pthread_t t[100];
	pthread_mutex_init(&mtx,NULL);
	
	input_message();
	scanf("%d",&choice);
	if(choice==1){
		for(i=0;i<100;i++){
			pthread_create(&t[i],NULL,child,NULL);
		}
		sleep(5);
		for(i=0;i<100;i++){
			pthread_join(t[i],NULL);
		}
	}
	else if(choice==2){
		printf("~Exit~\n");
	}
	else{
		printf("Are U kidding me :)\n");
	}
	while(choice!=2){
		input_message();
		scanf("%d",&choice);
		if(choice==1){
			for(i=0;i<100;i++){
				pthread_create(&t[i],NULL,child,NULL);
			}
			sleep(5);
			for(i=0;i<100;i++){
				pthread_join(t[i],NULL);
			}
		}
		else if(choice==2){
			printf("~Exit~\n");
		}
		else{
			printf("Are U kidding me :)\n");
		}
	}
	pthread_mutex_destroy(&mtx);
	return 0;
}

