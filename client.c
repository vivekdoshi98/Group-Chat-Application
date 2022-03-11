#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 8080
void sendToServer(int fd,char * msg){
	if(send(fd,msg,strlen(msg),0)<0){
		printf("[-]ERROR IN SENDING MSG");
		exit(1);
	}
}
//to create and connect to socket with the port provided
int createAndConnectSocket(int port){
	int sk=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	//AF_INET - IPV4, SOCK_STREAM - TCPIP, IPPROTO_TCP - TCP connection
	if(sk<0){
		printf("SOCKET CREATION ERROR");
		exit(1);
	}	
	//socket address
	struct sockaddr_in serv_addr;
	serv_addr.sin_family=AF_INET; //for IPV4
	serv_addr.sin_port=htons(port); //htons converts port number to correct type.
	//in client we need to provide IP which inet_pton function converts and stores in sin_addr
	if(inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr)<=0){
		printf("INVALID IP");
		exit(1);
	}
	//connecting socket - in client bindind is not required
	if(connect(sk,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0){
		printf("CONNECT ERROR");
		exit(1);
	}
	return sk;
}

void main(){
	int client_fd;
	char buffer[1024];
	client_fd=createAndConnectSocket(PORT);
	memset(buffer,0,1024);
	if(read(client_fd,buffer,1024)<0){
		printf("ERROR WHILE READING DATA");
		exit(1);
	}
	printf("%s\n",buffer);
	//sending username
	char *msg;
	gets(msg);
	sendToServer(client_fd,msg);

	memset(buffer,0,1024);
	if(read(client_fd,buffer,1024)<0){
		printf("ERROR WHILE READING DATA");
		exit(1);
	}
	printf("%s\n",buffer);
	//sending group number to server
	gets(msg);
	sendToServer(client_fd,msg);

	//getting port number
	memset(buffer,0,1024);
	if(read(client_fd,buffer,1024)<0){
		printf("ERROR WHILE READING DATA");
		exit(1);
	}

	int port=atoi(buffer);
	//printf("PORT NUMBER: %d",port);
	close(client_fd);
	int grp_fd=createAndConnectSocket(port);

	int pid=fork();
	if(pid==0){
		//----------------PROCESS TO SEND DATA TO SERVER------------
		while(1){
			char *msg;
			gets(msg);
			if(send(grp_fd,msg,strlen(msg),0)<0){
				printf("ERROR WHILE SENDING MESSAGE!");
				exit(1);
			}
			if(!strcmp(msg,"DISCONNECT") || !strcmp(msg,"UNJOIN")){
				//closing the socket discripter
				close(grp_fd);
				printf("Exit Successful\n");
				break;
			}
		}
		return;
	}
	else{
		while(1){
			memset(buffer,0,1024);
			if(read(client_fd,buffer,1024)<0){
				printf("ERROR WHILE READING DATA");
				exit(1);
			}
			if(strncmp(buffer,"Disconnect from server",22) == 0){
				break;
			}
			printf("%s\n",buffer);
		}
	}
}