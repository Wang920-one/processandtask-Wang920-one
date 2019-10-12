#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <sys/shm.h>
#include <ctype.h>
#define HISTORYNUM 10
#define MAX_LINE 80

struct Queue{
	char history[HISTORYNUM][MAX_LINE];
	int front;
	int rear;
}queue;

void add(char *str){
	if(queue.front==-1)
	{
		strcpy(queue.history[queue.rear],str);
		queue.front=queue.rear;
		return;
	}
	queue.rear=(queue.rear+1)%HISTORYNUM;
	strcpy(queue.history[queue.rear],str);
	if(queue.front==queue.rear)
	queue.front=(queue.front+1)%HISTORYNUM; 
}

char *gethistory(int c)
{
	int temp=queue.front;
    while(temp!=queue.rear+1)
	{
        if(queue.history[temp][0]==c)
		return queue.history[temp];
        temp=(temp+1)%HISTORYNUM;
    }
    if(temp==queue.rear+1) 
	return NULL; 
}

char *getnearhistory()
{ 
    if(queue.front==-1) 
	return NULL; 
    else 
	return queue.history[queue.rear];
}
void print(){
	
	if(queue.front==-1){
		printf("no command.");
		return;
	}
	int index=queue.front;
	printf("\n");
	fflush(stdout);
	printf("%s\n",queue.history[index]);
	fflush(stdout);
	index=(index+1)%HISTORYNUM;
	while(index!=(queue.rear+1)&&(!(index==0&&queue.rear==9)))
	{ 
        printf("%s\n",queue.history[index]);
        fflush(stdout);
        index=(index+1)%HISTORYNUM;
    }
} 
void setup(char *inputBuffer,char *args[],int *should_run)
{
	int length;
	int one;
	int next;
	next=0;
	
	length=read(STDIN_FILENO,inputBuffer,MAX_LINE);
	one =-1;
	//read
	if(length==0)
	exit(0);
	else if(length<0){
		perror("error reading the command.");
		args[0]=NULL;
		return;
	}

	char *str;
	if(inputBuffer[0]=='!'){
		if(inputBuffer[0]=='!'&&inputBuffer[1]=='!'&&inputBuffer[2]=='\n')
		{
			str=getnearhistory();
			if(str==NULL){
				perror("No commands in history.");
				return;
			}
			else strcpy(inputBuffer,str);
		}
		else if(inputBuffer[2]=='\n'||inputBuffer[2]=='\t')
		{
			str =gethistory(inputBuffer[1]);
			if(str==NULL)
			{
				perror("No such command in history");
				return;
			}
			else strcpy(inputBuffer,str);
		}
		inputBuffer[strlen(str)]='\n';
		inputBuffer[strlen(str)+1]='\0';
		length=strlen(str)+1;
	}
	int i;
	for(i=0;i<length;i++)
	{
		switch(inputBuffer[i])
		{
			case ' ':
			case '\t':
				if(one!=-1){
					args[next]=&inputBuffer[one];
					next++;
				}
				inputBuffer[i]='\0';
				one=-1;
				break;
			case '\n':
				if(one!=-1){
					args[next]=&inputBuffer[one];
					next++;
				}
				inputBuffer[i]='\0';
				args[next]=NULL;
				break;
			case '&':
				*should_run=1;
				inputBuffer[i]='\0';
				break;
			default:
				if(one==-1)
				one=i;
		}
	}
	add(inputBuffer);
	args[next]=NULL;
}

int main(void)
{
	char inputBuffer[MAX_LINE] ;
	char *args[MAX_LINE/2+1];
	int should_run=1;
	pid_t pid;
	queue.front = -1;
	queue.rear = 0;
	
	char ch;
	ch=getchar();
	if(ch=="history")
	print();
	
	while(should_run)
	{
		printf("osh> ");      
		fflush(stdout);
		setup(inputBuffer,args,&should_run);
		if((pid=fork())==-1){
			exit(-1);
		}
		if(pid==0){
			execvp(args[0],args);
			exit(0);
		}
		if(should_run==0){
			wait(0);
		}
	}
	return 0;
}
