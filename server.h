/*
*		Author		-		Apoorv Bhargava
*		File Name	-		server.h
*/

/* Include standard Library and other header files */

#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h>

/* Handle command-line arguments */
#include <getopt.h>

/* Include socket and Inet headers*/
#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h> 
#include <netinet/in.h> 

/* To handle abrupt kill/mishap */
#include <sys/wait.h> 
#include <signal.h>

#define MAXLEN 1024			/* Max size allowed */
#define MAXSERVERCONN 3			/* Max connection allowed at the server */
#define SERVER_PORT 45454		/* Server TCP Port */

#define TRUE  1		/* Defining Boolean Values True-1 */
#define FALSE 0		/* Defining Boolean Values False-0 */
typedef int boolean;	/* Defining Boolean */

pid_t PPId,PId[MAXSERVERCONN]; /* Child socket Process Handling */

/* Struct - To handle the command-line arguments */
struct arguments{
	char filename[MAXLEN];		/* Config Filename */
	boolean has_filename;			/* True or False - Check if argument has any filename */
};

/* Struct - To handle client request*/
struct userRecords{
	char username[MAXLEN];		/* username */
	char password[MAXLEN];		/* password */
	char *commands[MAXLEN];		/* commands */
	struct userRecords *link;
};

/* Function interruptHandler - Handler to kill all the processes on Ctrl+C */
void interruptHandler(int i, siginfo_t *info, void *dummy);

/*	Function usage - Prints the usage instructions */
void usage();

/* Function StoreData - Stores the config data into buffer */
void StoreData(char *username,char *password,char **commands,int lcounter);

/* Function 	- readFile - Reads the Input File */
void ReadFile(char *inputFile);

/* Function		-	ServerSetup - Creates a TCP Server and waits for clients request */
int ServerSetup();