/*
*		Author		-		Apoorv Bhargava
*		File Name	-		client.c
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

#define MAXLEN 1024					/* Max size allowed */
#define SERVER_PORT 45454		/* Server TCP Port */

#define TRUE  1				/* Defining Boolean Values True-1 */
#define FALSE 0				/* Defining Boolean Values False-0 */
typedef int boolean;	/* Defining Boolean */

/* Struct - To handle the command-line arguments */
struct arguments{
	char server[MAXLEN];		/* Server address */
	char username[MAXLEN];	/* Username */
	char password[MAXLEN];	/* Password */
	char command[MAXLEN];		/* Command */
	boolean has_server;			/* True or False - Check if has any server address*/
	boolean has_username;		/* True or False - Check if has any Username */
	boolean has_password;		/* True or False - Check if has any password */
	boolean has_command;		/* True or False - Check if has any execution command */
};

/* Function usage - Prints the Error Description and command details */
void usage();

/*	Function 	- ClientConnection - Creates a TCP connection with SERVER and sends the command to be executed. */
int ClientConnection(char server_ip[MAXLEN],char username[MAXLEN],char password[MAXLEN],char command[MAXLEN]);