/*
*		Author		-		Apoorv Bhargava
*		File Name	-		server.c
*/

#include "server.h"

/* Assigning default values to the argument struct */
struct arguments argument = {"",FALSE}; 		

struct userRecords *users=NULL;

/*
* Function	:	interruptHandler - Handler to kill all the processes on Ctrl+C
*/

void interruptHandler(int signal, siginfo_t *info, void *dummy) {
	struct userRecords *temp,*old;
	int icounter=0, jcounter;

	/* Relieve the memory -- Start */
	temp=users;

	while(temp != NULL){
		old=temp;
		icounter=0;
		while(temp->commands[icounter] != NULL){
			free(temp->commands[icounter++]);	
		}
		temp=temp->link;
		free(old);
	}
	users=NULL;
	/* Relieve the memory -- End */
	printf("\n Exiting Gracefully!\n");
	if (PPId != getpid()) {
		/*Killing self if not parent */
		exit(0);
	}
	
	if (signal==SIGINT) { /* Kill the children */
		fflush(stdout);
		for (jcounter = 0;jcounter < MAXSERVERCONN; jcounter++) {
		    kill(PId[jcounter], signal);
		}
	}
	
  /* Exiting the main thread */
  exit(0);
}

/* 
*	Function	:	usage - Prints the usage instructions
*	Parameter	:	None
*	Return		:	NULL
*/
void usage(){
	printf("Usage:\n\tserver -f <config_filename>\n");
	printf("\t\tSupports the following command line options.\n");
	printf("\t\t-f, --filename	: Config FileName.\n");
}

/* 
*	Function	:	StoreData - Stores the config data into buffer
*	Parameter	: username,password,commands,count_commands
*	Return		:	void
*/

void StoreData(char *username,char *password,char **commands,int lcounter){

	/* Declaring variable -- Start */	
	struct userRecords *current=NULL,*temp=NULL;
	int icounter=0;
	/* Declaring variable -- End */	

	if(username!=NULL){ 	/* Check - If no username,then don't store the entry */
		current = (struct userRecords *) malloc(sizeof(struct userRecords));

		/* Copy Content into the structure to store it */
		strcpy(current->username,username);

		if(password!=NULL){ 
			strcpy(current->password,password);
		}

		if(commands != NULL){
			for(icounter=0;icounter<=lcounter;icounter++){
				current->commands[icounter] = (char *) malloc(sizeof(commands[icounter]));
				strcpy(current->commands[icounter],commands[icounter]);
			}
		}

		if(users == NULL){	/* First entry */
			current->link=NULL;
			users=current;
		}else{ /* Append entry at the last */
			temp=users;
			while(temp->link != NULL){
				temp=temp->link;
			}
			temp->link=current;
		}
	}
}

/* 
*	Function		:	readFile - Reads the Input File 
*	Parameter		:	inputFile - Input File Pointer
*	Return			:	void
*/

void ReadFile(char *inputFile){

	/* Declaring variable -- Start */	
	FILE *filep;
	char fileData[3*MAXLEN],ch;
	int icounter=0,jcounter=0,kcounter=0,lcounter=0;
	char username[MAXLEN];		/* username */
	char password[MAXLEN];		/* password */
	char command[MAXLEN],*commands[MAXLEN];		/* commands */
	boolean user=FALSE,pass=TRUE,comm=TRUE;
	/* Declaring variable -- End */

	memset(fileData, '\0', sizeof(fileData));
	memset(username, '\0', sizeof(username));
	memset(password, '\0', sizeof(password));
	memset(command, '\0', sizeof(command));

	filep=fopen(inputFile, "r");

	/* Read File Data -- Start */
	if(filep == NULL){
		printf("ERROR: The File %s does not exist.\n",inputFile);
		exit(0);
	}else{
		while((ch = fgetc(filep)) != EOF ){
			fileData[jcounter++] = ch; 
			/* Buffer file into a variable -- reduces I/O operations */
		}
		fclose(filep);
	}
	/* Read File Data -- End */

	/* Separate Users, password and commands -- Start */
	for(icounter = 0; icounter < strlen(fileData); icounter++){
		if(!user){
			if(fileData[icounter] == ',' || fileData[icounter] == '\n'){
				kcounter=0;
				user=TRUE;pass=FALSE;comm=FALSE;
				if(fileData[icounter] == '\n'){
					StoreData(username,NULL,NULL,-1);
					user=FALSE;
					/*Clean-Up -- Start */
					lcounter=0;
					memset(username, '\0', sizeof(username));
					memset(password, '\0', sizeof(password));
					memset(command, '\0', sizeof(command));
					/*Clean-Up -- End */
				}
				continue;
			}
			username[kcounter++]=fileData[icounter];
		}else if(!pass){
			if(fileData[icounter] == ',' || fileData[icounter] == '\n'){
				kcounter=0;
				user=TRUE;pass=TRUE;comm=FALSE;
				if(fileData[icounter] == '\n'){
					StoreData(username,password,NULL,-1);
					user=FALSE;
					/*Clean-Up -- Start */
					lcounter=0;
					memset(username, '\0', sizeof(username));
					memset(password, '\0', sizeof(password));
					memset(command, '\0', sizeof(command));
					/*Clean-Up -- End */
				}
				continue;
			}
			password[kcounter++]=fileData[icounter];
		}else if(!comm){
			if(fileData[icounter] == '\n'){
				kcounter=0;
				user=FALSE;pass=TRUE;comm=TRUE;

				commands[lcounter] = (char *) malloc(sizeof(command));
				strcpy(commands[lcounter],command);
				StoreData(username,password,commands,lcounter);

				/*Clean-Up -- Start */
				lcounter=0;
				memset(username, '\0', sizeof(username));
				memset(password, '\0', sizeof(password));
				memset(command, '\0', sizeof(command));
				/*Clean-Up -- End */

				continue;
			}else if(fileData[icounter] == ','){
				
				if(strlen(command) > 0){
					commands[lcounter] = (char *) malloc(sizeof(command));
					strcpy(commands[lcounter++],command);
				}
				kcounter=0;
				memset(command, '\0', sizeof(command));
				continue;
			}
			command[kcounter++]=fileData[icounter];	
		}
	}
	/* Separate Users, password and commands -- End */
}

/* 
*	Function		:	ServerSetup - Creates a TCP Server and waits for clients request.
*	Parameter		:	None
*	Return			:	NULL
*/
int ServerSetup(){
	/* Declaring variable -- Start */
	int sockfd, newsockfd;
	struct sockaddr_in serverSocketAddress, clientSocketAddress;
	
	char sendBuff[MAXLEN];
	char recvBuff[3*MAXLEN];
	char tempString[MAXLEN];
	char username[MAXLEN];		/* username */
	char password[MAXLEN];		/* password */
	char command[MAXLEN];			/* commands */
	char outFile[MAXLEN];
	
	int icounter=0,jcounter=0,kcounter=0;	
	int bytesReceived, bytesSent;
	boolean ucheck=FALSE,pcheck=FALSE,ccheck=FALSE,validUser=FALSE,validCommand=FALSE;
	char ch;
	socklen_t clientSocketAddressLength;
	
	FILE *fp;
	struct userRecords *temp;
	pid_t pid; /* Child Process Handler */
	/* Declaring variable -- End */
	
	clientSocketAddressLength = sizeof(struct sockaddr_in);
	
	/* Create server socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	/* Check if server socket created */
	if (sockfd < 0){
		printf("ERROR: Unable to open socket.\n");
		return 0;
	}
	
	/* Set server socket properties */
	bzero((char *) &serverSocketAddress, sizeof(serverSocketAddress));
	serverSocketAddress.sin_family = AF_INET;
	serverSocketAddress.sin_addr.s_addr = INADDR_ANY;
	serverSocketAddress.sin_port = htons(SERVER_PORT);
	
	/* Binding Server socket to listen at desired port -- Start */
	if (bind(sockfd, (struct sockaddr *) &serverSocketAddress, sizeof(serverSocketAddress)) < 0){
		printf("ERROR: Unable to bind server socket.\n");
		return 0;
	}
	/* Binding Server socket to listen at desired port -- End */
	
	/* Listening to the port for a client connection request. */
	listen(sockfd,5);
	
	/* Waiting for Client to connect -- Start */
	while(1){
		/* Re-initializing all the values -- Start */
		memset(sendBuff, '\0' ,sizeof(sendBuff));
		memset(recvBuff, '\0' ,sizeof(recvBuff));
		memset(tempString, '\0' ,sizeof(tempString));

		memset(username, '\0' ,sizeof(username));
		memset(password, '\0' ,sizeof(password));
		memset(command, '\0' ,sizeof(tempString));
		memset(outFile, '\0' ,sizeof(outFile));
		
		icounter=0;jcounter=0;kcounter=0;
		/* Re-initializing all the values -- End */
		
		/* Accepting connection from Client -- Start */
		newsockfd = accept(sockfd,(struct sockaddr *)&clientSocketAddress, &clientSocketAddressLength);
		
		/* Check if accepted client connection */
		if (newsockfd < 0){
			printf("ERROR: Unable to accept client connection.\n");
			continue;
		}
		
		/* Creating a new Process to Handle the TCP request*/
		pid=fork();
		/* Accepting connection from Client -- End */
		
		/* Child should be working now - Start */
		if(pid==0){
			close(sockfd);
			
			/* Receiving Data from the Client -- Start */
			bytesReceived = recv(newsockfd, &recvBuff, sizeof(recvBuff), 0);
			
			if(bytesReceived < 0){
				printf("ERROR: Socket unable to read data");
				return 0;
			}
			/* Receiving Data from the Client -- End */
			
			/* Validate Client -- Start */
			icounter=strlen(recvBuff);
			while(jcounter < icounter){
				if(recvBuff[jcounter] == 'u' && recvBuff[jcounter+1] == '`'){
					ucheck =TRUE;
					jcounter+=2;
					kcounter=0;
					continue;
				}else if(recvBuff[jcounter] == 'p' && recvBuff[jcounter+1] == '`'){
					pcheck =TRUE;
					kcounter=0;
					jcounter+=2;
					continue;
				}else if(recvBuff[jcounter] == 'c' && recvBuff[jcounter+1] == '`'){
					ccheck =TRUE;
					kcounter=0;
					jcounter+=2;
					continue;
				}
				
				if(recvBuff[jcounter] == ','){
					ucheck=pcheck=ccheck=FALSE;
				}

				if(ucheck){
					username[kcounter++]=recvBuff[jcounter];
				}else if(pcheck){
					password[kcounter++]=recvBuff[jcounter];
				}else if(ccheck){
					command[kcounter++]=recvBuff[jcounter];
				}			
				jcounter++;
			}

			memset(recvBuff, '\0' ,sizeof(recvBuff));

			temp=users;
			while(temp->link != NULL){
				if(strcmp(temp->username,username) == 0 && strcmp(temp->password,password) == 0){
					validUser=TRUE;
					icounter=0;
					while(temp->commands[icounter] != NULL){
						if(strncmp(command,temp->commands[icounter],strlen(temp->commands[icounter])) == 0){	
							validCommand=TRUE;
							break;
						}
						icounter++;
					}
					if(validCommand){
						temp=NULL;
						break;
					}
				}
				temp=temp->link;
			}
			if(!validUser){
				strcpy(sendBuff,"Invalid User Credentials\n");
			}else if(!validCommand){
				strcpy(sendBuff,"Invalid Command Used\n");
			}
			/* Validate Client -- End */
			
			/* Execute Command And Send data -- Start */
			if(validCommand){
				sprintf(outFile, "file%d.out", pid);
				sprintf(command, "%s > %s", command,outFile);
				fp = popen(command, "r");
				if (fp == NULL) {
					strcpy(sendBuff,"Failed to run command\n");
				}else{
					pclose(fp);
					jcounter=0;
					fp = fopen(outFile, "r");
					while((ch = fgetc(fp)) != EOF){
						tempString[jcounter++] = ch;
						if(jcounter >= MAXLEN){
							/* Sending Data to Client -- Start */
							bytesSent = write(newsockfd,tempString,strlen(tempString));
							if (bytesSent < 0){
								printf("ERROR: Server unable to send data.\n");
								return  0;
							}
							/* Sending Data to Client -- End */
							jcounter=0;
							memset(tempString, '\0' ,sizeof(tempString));
						}
					}
					fclose(fp);
					strcpy(sendBuff,tempString);
				}
			}	
			/* Execute Command and Send data -- End */
			
			/* Sending Remaining Data or Control message to Client -- Start */
			bytesSent= write(newsockfd,sendBuff,strlen(sendBuff));
			if (bytesSent < 0){
				printf("ERROR: Server unable to send data.\n");
				return  0;
			}
			/* Sending Remaining Data or Control message to Client -- End */
			
			/* Notify Client that its over -- Start */
			/* This message should be sent only after previous packets are received by the client, so either we should know RTT or set a sleep timer, but there is a workaround as well if at client we can check the message and strip this message from been displayed -- Extra work for client */
			memset(sendBuff, '\0' ,sizeof(sendBuff));
			strcpy(sendBuff,"***The End***");
			bytesSent= write(newsockfd,sendBuff,strlen(sendBuff));
			if (bytesSent < 0){
				printf("ERROR: Server unable to send data.\n");
				return  0;
			}
			/* Notify Client that its over -- Start */
			
			/* Finished working - Close the connection */
			close(newsockfd);
			
			/* Exiting the current process */
			exit(0);
		}
		/* Child should be working now - End */
	}
	/* Waiting for Client to connect -- End */
	
	return 0; /* Unreachable code - just for sanity */
}
/* 
*	Function	:	main - Initiates server, reads the argument, establishes TCP server and on receiving command replies back to client.
*	Parameter	:	arguments
*	Return		:	0
*/
int main(int argc, char** argv){
	
	int opt = 0;	/* CommandLine argument handler */
	
	/* Added to Kill child and parent process on Ctrl+C - Start */
	struct sigaction act;
	
	act.sa_sigaction= &interruptHandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags=SA_SIGINFO;
	sigaction(SIGINT, &act, NULL);
	
	PPId = getpid();
	/* Added to Kill child and parent process on Ctrl+C - End */

	/* Handling command line arguments */
	static struct option cmdLongOpts[] = {
						{"filename",	required_argument, 0, 'f'},
						{NULL, 0, NULL, 0}
					};

	/* Parse the arguments -- Start */
	while ((opt = getopt_long(argc, argv,"f:",cmdLongOpts, NULL)) != -1){
	    	switch(opt)    	{
			case 'f':
				argument.has_filename=TRUE;
				strncpy(argument.filename, optarg, MAXLEN);
				break;
		    	default:
		   		printf("Please provide valid arguments.\n\n");
				usage();
				exit(0);
	    	}
	}
	/* Parse the arguments -- End */

	/* Check if has arguments -- Start */
	if(!argument.has_filename){
		printf("Please provide Configuration File location.\n\n");
		usage();
		exit(0);
	}
	/* Check if has arguments -- End */

	/* Loading Data from Configuration File -- Start */
	ReadFile(argument.filename);
	/* Loading Data from Configuration File -- End */
	
	/* Create & Start server -- Start */
	ServerSetup();
	/* Creat & Start server -- End */

	/* Wait for the end. After killing all the Child Processes main process will end - Start */
	while (1);
	/* Wait for the end. After killing all the Child Processes main process will end - End */

	return 0;
}
