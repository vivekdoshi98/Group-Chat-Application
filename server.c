#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_NUM_CLIENTS 5

//sends message to client and gives error if any
void sendToClient(int fd,char * msg){
	if(send(fd,msg,strlen(msg),0)<0){
		printf("ERROR IN SENDING MSG");
		exit(1);
	}
}
//--FUNCTIONS RELATED TO FILES--
//checks username in the given filename
int checkUserExists(char *filename,char *username){
	FILE* ptr;
	char filepath[50];
	sprintf(filepath,"./Utility/%s",filename);
	ptr = fopen(filepath, "r");
	if(ptr==NULL){
		printf("%s not found",filepath);
		exit(1);
	}
	char line[50];
	
	while (fgets(line, sizeof(line), ptr)) 
	{
		//for file users.txt where username does not have #index
		//so spliting with delim \n
		char *tmp1=strtok(line, "\n");
		char un1[50];
		strcpy(un1,tmp1);
		if(!strcmp(un1,username)){
	    	printf("\n%s exists in system\n",username);
	    	fclose(ptr);
	    	return 0;
	    }
	    //for file groupname_Users.txt where username have #index
	    //so splitting with delim #
	    char *tmp=strtok(line, "#");
		char un[50];
		strcpy(un,tmp);
	    if(!strcmp(un,username)){
	    	printf("\n%s exists in grp\n",username);
	    	fclose(ptr);
	    	return 0;
	    }
    }
    fclose(ptr);
    return 1;
}
//to check if username exists in users.txt if not add it
int validateUser(char *username){
	int ex=checkUserExists("users.txt",username);
	if(ex==0)
		return 0;
	else{
		FILE* ptr;
	    ptr = fopen("./Utility/users.txt", "a+");
	    fputs(username, ptr);
	    fputs("\n", ptr);
	    printf("\nUSER ADDED\n");
	    fclose(ptr);
	    return 1;
	}
}
//checks and adds the user to grpname with #index 0
int addUserToGrp(char *username,char *grpname){
	char path[60];
	sprintf(path,"%s_Users.txt",grpname);
	int ex=checkUserExists(path,username);
	if(ex==0){
		return 0;
	}
	FILE* ptr;
	char filepath[50];
	sprintf(filepath,"./Utility/%s_Users.txt",grpname);
	ptr = fopen(filepath, "a+");
	if(ptr==NULL){
		printf("%s not found",filepath);
		exit(1);
	}
	char line[60];
	sprintf(line,"%s#0\n",username);
	fputs(line, ptr);
    printf("\nADDED to grp\n");
    fclose(ptr);
    return 1;
}
//returns the last message index from grpname_Msg.tzt
int getMsgLastIndex(char *grpname){
	FILE* ptr;
	char filepath[50],m[1024];
	sprintf(filepath,"./Utility/%s_Msg.txt",grpname);
	ptr = fopen(filepath, "r");
	if(ptr==NULL){
		printf("%s not found",filepath);
		exit(1);
	}
	char line[1024];
	int finalIndex=0;
	while (fgets(line, sizeof(line), ptr)) 
	{
		char *tmp=strtok(line, "#");
		tmp=strtok(NULL,"\n");
		char i[5];
		strcpy(i,tmp);
		finalIndex=atoi(i);
    }
    return finalIndex;
}
//returns given users msg index from grp_Users.txt
int getUserMsgIndex(char *username,char *grp){
	FILE *ptr;
	char filepath[50],m[1024];
	sprintf(filepath,"./Utility/%s_Users.txt",grp);
	ptr = fopen(filepath, "a+");
	if(ptr==NULL){
		printf("%s not found",filepath);
		exit(1);
	}
	char line[60];
	size_t len=0;
	while (fgets(line, sizeof(line), ptr)) 
	{
		char *tmp=strtok(line, "#");
		char un[50];
		strcpy(un,tmp);
		tmp=strtok(NULL,"\n");
		char i[5];
		strcpy(i,tmp);
		int index=atoi(i);
		if(!strcmp(un,username))
	    	return index;
    }
    return 0;
}
//sends the unread messages to client
void getMessages(char *grp,int con,char *username){
	int currentIndex=getUserMsgIndex(username,grp);
	FILE* ptr;
	char filepath[50],m[1024];
	sprintf(filepath,"./Utility/%s_Msg.txt",grp);
	ptr = fopen(filepath, "r");
	if(ptr==NULL){
		printf("%s not found",filepath);
		exit(1);
	}
	char line[1024];
	size_t len=0;
	int flag=0;
	while (fgets(line, sizeof(line), ptr)) 
	{
		char *tmp=strtok(line, "#");
		tmp=strtok(NULL,"\n");
		char i[5];
		strcpy(i,tmp);
		int mid=atoi(i);
		if(mid>=currentIndex){
			char l[1024];
			sprintf(l,"%s\n",line);
			sendToClient(con,l);
			flag=1;
		}
    }
    if(flag==0)
   		sendToClient(con,"No New Messages!");
}
//to add message in gname_Msg.txt in format
//um:msg#index
void addMsg(char *gname,char *msg,char *um,int index){
	FILE* ptr;
	char filepath[50],m[1024];
	sprintf(filepath,"./Utility/%s_Msg.txt",gname);
	ptr = fopen(filepath, "a+");
	if(ptr==NULL){
		printf("%s not found",filepath);
		exit(1);
	}
	sprintf(m,"%s:%s#%d\n",um,msg,index);
	fputs(m, ptr);
    printf("\nMSG ADDED to grp\n");
    fclose(ptr);
    return;
}
//to update the user msg index in grp_Users.txt
void updateUserMsgIndex(char *grp,char *username,int index){
	FILE* ptr,*tmpFile;
	char filepath[50],updateLine[50];
	sprintf(filepath,"./Utility/%s_Users.txt",grp);
	sprintf(updateLine,"%s#%d\n",username,index);
	ptr = fopen(filepath, "r");
	tmpFile = fopen("./Utility/tmp.txt", "w+");
	if(ptr==NULL || tmpFile==NULL){
		printf("FILE not found");
		exit(1);
	}
	char line[50],org[50];
	while (fgets(line, sizeof(line), ptr)) 
	{
		strcpy(org,line);
		char *tmp=strtok(line, "#");
		char un[50];
		strcpy(un,tmp);
		
		if(!strcmp(un,username)){
	    	fputs(updateLine,tmpFile);
	    }
	    else{
	    	fputs(org,tmpFile);
	    }
    }
    fclose(ptr);
    fclose(tmpFile);
    remove(filepath);
    rename("./Utility/tmp.txt",filepath);
}
//removes the username from gname_Users.txt
void exitGrp(char *gname,char *username){
	FILE* ptr,*tmp;
	char filepath[50];
	sprintf(filepath,"./Utility/%s_Users.txt",gname);
	
	ptr = fopen(filepath, "r");
	tmp = fopen("./Utility/tmp.txt", "w+");
	if(ptr==NULL || tmp==NULL){
		printf("FILE not found");
		exit(1);
	}

	char line[50],org[50];
	while (fgets(line, sizeof(line), ptr)) 
	{
		strcpy(org,line);
		char *sp=strtok(line, "#");
		char un[50];
		strcpy(un,sp);

	    if(!strcmp(un,username)){
	    	continue;
	    }
	    else{
	    	fputs(org,tmp);
	    }
    }
    fclose(ptr);
    fclose(tmp);
    remove(filepath);
    rename("./Utility/tmp.txt",filepath);
    printf("\nUSER REMOVED");
    return;

}

//--FUNCTIONS RELATED TO FILES ENDS HERE--


//to create socket -> bind -> listen and return that socket
int createSocket(int port,struct sockaddr_in *addPtr){
	int server_fd,newcon_fd;
	server_fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	//AF_INET - IPV4, SOCK_STREAM - TCPIP, IPPROTO_TCP - TCP connection
	if(server_fd<0){
		printf("[-]SOCKET CREATION ERROR");
		exit(1);
	}

	//Socket address and properties
	addPtr->sin_family=AF_INET; //IPV4
	addPtr->sin_addr.s_addr=INADDR_ANY; //Any local ip
	addPtr->sin_port=htons(port); //converts port value to correct type

	//binding socket to port -returns -1 if error
	if(bind(server_fd,(struct sockaddr *)addPtr,sizeof(*addPtr))<0){
		printf("[-]BIND ERROR");
		exit(1);
	}
	else{ printf(" [+]BIND SUCCESSFUL\n");}

	//listen : waiting for client to make connection
	if(listen(server_fd,MAX_NUM_CLIENTS)<0){ 
		printf("[-]LISTEN ERROR");
		exit(1);
	}
	else{ printf(" [+]SERVER IN LISTEN MODE\n");}

	return server_fd;
}

//main function for a group process
void performGroupActions(char *groupName,int port,int readFromParent){
	
	struct sockaddr_in address;
	int server_fd=createSocket(port,&address);
	int newcon_fd,addrlen = sizeof(address);
	while(1){
		if((newcon_fd=accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen))<0){
			printf("ACCEPT ERROR IN %s",groupName);
			exit(1);
		}
		else{ printf(" [+]CLIENT ACCEPTED IN %s\n",groupName);}
		printf(" [-]WAITING FOR USERNAME\n");
		//reading username from parent process through pipe[0][0];
		char username[50];
		memset(username,0,50);
		read(readFromParent,username,50);
		printf("\n [+]%s JOINED Group1\n",username);

		
		char welcome[1024];
		sprintf(welcome,"\nWelcome to %s %s.\nEnter HELP to see commands\n",groupName,username);
		sendToClient(newcon_fd,welcome);
		//send unread messages
		sendToClient(newcon_fd,"Messages so far:-\n");
		getMessages(groupName,newcon_fd,username);
		while(1){
			char buffer[1024];
			memset(buffer,0,50);
			read(newcon_fd,buffer,50);
			printf("CLIENT %s: %s\n",username,buffer);	
			if(!strcmp(buffer,"HELP")){
				sendToClient(newcon_fd,"COMMANDS:-\n1)ADD MSG\n2)UNJOIN\n3)DISCONNECT\n");
			}
			else if(!strcmp(buffer,"DISCONNECT")){
				sendToClient(newcon_fd,"Disconnect from server");
				break;
			}
			else if(!strcmp(buffer,"ADD MSG")){
				int index=getMsgLastIndex(groupName);
				while(1){
					sendToClient(newcon_fd,"Enter Message (END to terminate)\n");
					memset(buffer,0,1024);
					read(newcon_fd,buffer,1024);
					if(!strcmp(buffer,"END")){
						sendToClient(newcon_fd,"Messages saved\n");
						break;
					}
					index++;
					addMsg(groupName,buffer,username,index);
				}		
				updateUserMsgIndex(groupName,username,index);
			}
			else if(!strcmp(buffer,"UNJOIN")){
				exitGrp(groupName,username);
				sendToClient(newcon_fd,"You have left the group\n");
				sendToClient(newcon_fd,"Disconnect from server");
				break;
			}
			else
				sendToClient(newcon_fd,"COMMAND NOT FOR GROUP\n");
		}
	}
}

void main(){
	int pipes[3][2];
	for(int i=0;i<2;i++)
		pipe(pipes[i]);
	if(fork()==0){
		//-----------------------THIS IS GROUP1-----------------------
		//closing extra pipes
		close(pipes[1][0]);
		close(pipes[1][1]);
		close(pipes[2][0]);
		close(pipes[2][1]);
		
		//main function for group
		performGroupActions("Group1",8081,pipes[0][0]);
		close(pipes[0][1]);
		close(pipes[0][0]);
	}
	else{
		if(fork()==0){
			//-----------------------THIS IS GROUP2-----------------------			
			//closing extra pipes
			close(pipes[0][0]);
			close(pipes[0][1]);
			close(pipes[2][0]);
			close(pipes[2][1]);
			
			//main function for group
			performGroupActions("Group2",8082,pipes[1][0]);
			close(pipes[1][1]);
			close(pipes[1][0]);
		}
		else{
			if(fork()==0){
				//-----------------------THIS IS GROUP3-----------------------		
				//closing extra pipes
				close(pipes[0][0]);
				close(pipes[0][1]);
				close(pipes[1][0]);
				close(pipes[1][1]);
				
				//main function for group
				performGroupActions("Group3",8083,pipes[2][0]);
				close(pipes[2][1]);
				close(pipes[2][0]);
			}
			else{
				//-----------------------THIS IS PARENT-----------------------	
				close(pipes[0][0]);
				close(pipes[1][0]);
				close(pipes[2][0]);

				struct sockaddr_in address;
				int server_fd=createSocket(PORT,&address);
				int newcon_fd;
				int addrlen = sizeof(address);
				
				while(1){
					if((newcon_fd=accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen))<0){
						printf("ACCEPT ERROR");
						exit(1);
					}
					else{ printf(" [+]CLIENT ACCEPTED\n");}

					sendToClient(newcon_fd,"Enter username:");
					char username[50];
					memset(username,0,50);
					read(newcon_fd,username,50);

					int exists=validateUser(username);
					
					sendToClient(newcon_fd,"Groups:-\n1)Group1\n2)Group2\n3)Group3\nEnter number to join group:");

					char grpNum[50];
					memset(grpNum,0,50);
					read(newcon_fd,grpNum,50);
					int gnum=atoi(grpNum);
					if(gnum==1){
						addUserToGrp(username,"Group1");
						write(pipes[0][1],username,50);
						sendToClient(newcon_fd,"8081");
					}
					else if(gnum==2){
						addUserToGrp(username,"Group2");
						write(pipes[1][1],username,50);
						sendToClient(newcon_fd,"8082");
					}
					else{
						addUserToGrp(username,"Group3");
						write(pipes[3][1],username,50);
						sendToClient(newcon_fd,"8083");
					}
				}
				close(pipes[0][1]);
				close(pipes[1][1]);
				close(pipes[2][1]);
			}
		}
		
	}

}