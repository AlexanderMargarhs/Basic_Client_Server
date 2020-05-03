#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "avg_socket"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int succCount=0,allCount=0;

void* average(void*);

int main(int argc,char *argv[])
{
    int socket_server,socket_client,received_length,i,t;
    struct sockaddr_un local,remote;
    pthread_t pid[50]; //Create the maxinum number of clients we can handle with threads.

    system("clear");    
	if ((socket_server=socket(AF_UNIX,SOCK_STREAM,0))==-1) //Error checking.
    {
        perror("socket");
        exit(1);
    }
    local.sun_family=AF_UNIX;
    strcpy(local.sun_path,SOCK_PATH);
    unlink(local.sun_path);
	t=strlen(local.sun_path)+sizeof(local.sun_family);
	if(bind(socket_server,(struct sockaddr*)&local,t)==-1) //Error checking.
    {
        perror("bind");
        exit(1);
    }
	if(listen(socket_server,5)==-1) //Error checking.
	{
    	printf("Listening\n");
	}	
		
    i=0;
	while(1) //Do this until the server is closed.
    {	
		printf("Waiting for a connection...\n"); 
		received_length=sizeof(remote);
		if((socket_client=accept(socket_server,(struct sockaddr*)&remote,&received_length))==-1) //Error checking.
    	{
        	perror("accept");
        	exit(1);
		}
		
		pthread_create(&pid[i++],NULL,average,(void *)&socket_client); //Create a thread and go continue to the function.
	}

	return 0;	
}



void* average(void *socket_arg)
{	
	int socket_client_new=(*(int*) socket_arg);
	int i,N,sum,allCount=0,succCount=0;
	double avg;
	char string[15];

	printf("Connected.\n");

	while(1)
	{
		recv(socket_client_new,&N,sizeof(N),0); //Receive the size of the array.
		if(N==0)
    		break; //In case the N is 0 we end the loop.

		int *intArray;
		intArray=(int*)malloc(N*sizeof(int)); //malloc the array again since it's an array of pointers.
		recv(socket_client_new,intArray,N*sizeof(int),0); //Receive the array.

		sum=0;
		for(i=0;i<N;i++) 
			sum+=intArray[i]; //We find the sum of the array.
		avg=(double)sum/N; //We find the average.
		free(intArray); //We free the second array.

		sprintf(string,"Check Failed");
		if(avg>=10)
			sprintf(string,"Sequence OK");

		send(socket_client_new,string,strlen(string),0); //Send the right message.

		int tmp;
		recv(socket_client_new,&tmp,sizeof(tmp),0); //We receive a temp number so that I won't have the same problem we the send failing.

		if(avg>=10)
    	{
			send(socket_client_new,&avg,sizeof(avg),0); //Send the average in case it's over 10.
			succCount++; //Increase the succeful counter.
		}
		allCount++; //Increase the all counter.
	}
	close(socket_client_new); //We close the client.
	printf("Successful count:%d\n",succCount); //Print the current successful counter. 
	printf("All count:%d\n",allCount); //Print the current all counter.
	pthread_exit(NULL); //Close thread.
	return NULL;
}
