#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "avg_socket"

int main(int argc ,char *argv[])
{
    int socket_client,received_length,N,*intArray,i,t;
    double avg;
    struct sockaddr_un client;
    char recStr[20];

    system("clear");
    if ((socket_client=socket(AF_UNIX,SOCK_STREAM,0))==-1)
    {
        perror("socket");
        exit(1);
    }
    printf("Trying to connect...\n\n");
    client.sun_family=AF_UNIX;
    strcpy(client.sun_path,SOCK_PATH);
    t=strlen(client.sun_path)+sizeof(client.sun_family);
    if(connect(socket_client,(struct sockaddr *)&client,t)==-1)
    {
        perror("connect");
        exit(2);
    }
    printf("Connected.\n\n");

    while(1)
    {
        printf("Give how many numbers are you going to input, if you want to terminate the connection, give 0: "); //Read how many numbers the user will input.
    	scanf("%d",&N);
        send(socket_client,&N,sizeof(N),0); //Send the size of the Array we will have.
        if(!N) //In case the size is 0 we end the client program.
            break;

        printf("\nInput %d numbers now:\n",N); 
        intArray=(int*)malloc(sizeof(int)); //Malloc for the size needed.
		for(i=0;i<N;i++)
        {
            printf("Give number #%d: ",i+1); 
            scanf("%d",&intArray[i]); //Read the numbers that we will have inside the array.
		}
		send(socket_client,intArray,N*sizeof(int),0); //Send the array pointer to the Server.
		free(intArray); //Free the array.
        system("clear");

		received_length=recv(socket_client,recStr,20,0);
        recStr[received_length]='\0';
        printf("%s\n\n",recStr);

        int temp=1;
        send(socket_client,&temp,sizeof(temp),0); //We send something extra because I had an problem when I tried to receive twice in a row. 

        if(!(strcmp(recStr,"Sequence OK"))) //In case the average is over 10 we receive the avergae from the server.
        {
            recv(socket_client,&avg,sizeof(avg),0);
            printf("The average number is: %.2f\n",avg); //Print the average.
        }
	}
    close(socket_client); //Close the client at the end.

    return 0;
}

//TODO input checking, colours, comments, change prints, add counter to server
