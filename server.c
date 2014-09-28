/*
*		Author		-		Apoorv Bhargava
*		File Name	-		server.d
*/

#include "server.h"

/* Assigning default values to the argument struct */
struct arguments argument = {"",FALSE}; 		

struct userRecords *users=NULL;

/*
* Function	:	intHandler - Handler to kill all the processes on Ctrl+C
*/

void intHandler(int i, siginfo_t *info, void *dummy) {
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
	
	if (PPId != getpid()) {
		/*Killing self if not parent */
		exit(0);
	}
	
	if (i==SIGINT) { /* Kill the children */
		fflush(stdout);
		for (jcounter = 0;jcounter < MAXSERVERCONN; jcounter++) {
		    kill(PId[jcounter], i);
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
	printf("server -f <config_filename>\n");
	printf("\tSupports the following command line options.\n");
	printf("\t-f, --filename	: Config FileName.\n");
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
		printf("The File %s does not exist.\n",inputFile);
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
*	Function	:	main - Initiates server, reads the argument, establishes TCP server and on receiving command replies back to client.
*	Parameter	:	arguments
*	Return		:	0
*/
int main(int argc, char** argv){
	
	int opt = 0;	/* CommandLine argument handler */
	
	/* Added to Kill child and parent process on Ctrl+C - Start */
	struct sigaction act;
	
	act.sa_sigaction= &intHandler;
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
				ErrorDescription();
				exit(0);
	    	}
	}
	/* Parse the arguments -- End */

	/* Check if has arguments -- Start */
	if(!argument.has_filename){
		printf("Please provide Configuration File location.\n\n");
		ErrorDescription();
		exit(0);
	}
	/* Check if has arguments -- End */

	/* Loading Data from Configuration File -- Start */
	ReadFile(argument.filename);
	/* Loading Data from Configuration File -- End */

	/* Wait for the end. After killing all the Child Processes main process will end - Start */
	while (1);
	/* Wait for the end. After killing all the Child Processes main process will end - End */

	return 0;
}
