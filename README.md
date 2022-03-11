# Group-Chat-Application
A basic group chat application using socket programming over TCP and fork/pipe in C.


At a time client join a group with a username and enter as many messages he/she wants and disconnect.New client can join the same group and send his/her messages.

When a client returns to same group he/she will get all the unread message that came after he/she disconnected the first time.


## Code explanation
In server.c, firstly, three processes are created using fork() and each has 1 pipe to read username from the parent process. 
Individual sockets are created with different port numbers and those sockets are in listening state. 
In the parent process again a socket is created to which the client will connect.


Client connects to the server and is prompted to enter the username. 
Username is added to thelist if not present, server asks which group to join and based on client answer, server sends the
port number of that group to client and writes the username to that group’s pipe.


Client, on receiving the port number closes the connection with server and again makes aconnection request with the new port number which is of the group process.
On connecting the group socket all the unread messages are sent to the client and then client can run any of the following commands:-


HELP - prints all the available commands

ADD MSG - User can send any message to the group server and it will be stored.

UNJOIN - User is removed from the group he/she is connected to.

DISCONNECT - User is disconnected from the server.

## File System - how username and messages are stored and how Read/Unread
mechanism works.


Mainly 3 files are used - users.txt, groupname_Users.txt, groupname_Msg.txt
Each group will have their own groupname_Users.txt, groupname_Msg.txt.


1) When the client connects to the server, the parent server process checks if the
username exists in users.txt, if not, username is added to the file. Also checks if the
username is present in groupname_Users.txt and adds it in format username#0.


2) When client connects to group process socket,it gets the username from parent and
extracts the last message index (value after #).


3) Based on the user index process, it checks the messages from groupname_Msg.txt and
prints all the messages with an index greater than the user's index.


4) When the client runs ADD MSG command, all the messages get stored in
groupname_Msg.txt in the format username:message#INDEX.


5) After the client ENDs the ADD MSG command his/her record is updated with the last
message index in groupname_Users.txt


6) Finally when UNJOIN command is run, that username is removed from the
groupname_Users.txt

## Output:-


![ss1](https://user-images.githubusercontent.com/80559953/157881105-33eb0fea-10b9-4a8e-84dc-9ea8a8cb5a03.png)


![ss2](https://user-images.githubusercontent.com/80559953/157881107-e45c573d-3449-479d-92a4-87f5c7b1d67f.png)


![ss3](https://user-images.githubusercontent.com/80559953/157881111-c314abfe-8173-4359-ba2b-58d26697e84f.png)


![ss4](https://user-images.githubusercontent.com/80559953/157881093-6ab027fe-4884-4ff3-a91c-670d9d599425.png)


![ss5](https://user-images.githubusercontent.com/80559953/157881101-352b3195-dfc2-4cef-9815-1a0e19ea47bf.png)




