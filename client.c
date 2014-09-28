/*
*		Author		-		Apoorv Bhargava
*		File Name	-		client.h
*/

#include "client.h"

struct arguments argument= {"","","","",FALSE,FALSE,FALSE,FALSE}; 		
/* Assigning default values to the arguments struct */

/* 
*	Function	:	usage - Prints the usage instructions
*	Parameter	:	None
*	Return		:	NULL
*/

void usage(){
	printf("Usage:\n");
	printf("\tclient --server <server ip [or hostname]> --user <username> --password <password> --exec <program to execute>\n");   
	printf("\tSupports the following command line options.\n");
	printf("\t\t-s, --server 		: Server IP address.\n");
	printf("\t\t-u, --username  	: Username.\n");
	printf("\t\t-p, --password  	: Password.\n");
	printf("\t\t-c, --exec		: Execution Command.\n\n");
}

/* 
*	Function	:	ClientConnection - Creates a TCP connection with SERVER and sends the command it.
*	Parameter : 
*							char server_ip[MAXLEN]
*							char username[MAXLEN]
*							char password[MAXLEN]
*							char command[MAXLEN]
*	Return		:	NULL
*/
int ClientConnection(char server_ip[MAXLEN],char username[MAXLEN],char password[MAXLEN],char command[MAXLEN]){
	/* Declaring variable -- Start */
	int serverConnection;
	int bytesSent=0,bytesReceived=0;
	struct sockaddr_in serverSocketAddress;
	struct hostent *serverIP;
	char sendBuff[3*MAXLEN];
	char recvBuff[3*MAXLEN];
	/* Declaring variable -- End */
	
	/* Create server socket */
	serverConnection = socket(AF_INET, SOCK_STREAM, 0);
	
	/* Check if socket created */
	if (serverConnection < 0){
		printf("ERROR: Unable to open socket.\n");
		return 0;
	}
	
	/* Check if valid IP address -- Start */
  if(inet_pton(AF_INET, server_ip, &(serverSocketAddress.sin_addr)) <= 0){
		/* If invalid IP then the user might have given hostname */
		serverIP = gethostbyname(server_ip);
		if(serverIP == NULL){
			printf("ERROR: Invalid IP address/Host name provided\n");
			return 0;
		}else{
			bcopy((char *)serverIP->h_addr, (char *)&serverSocketAddress.sin_addr.s_addr, serverIP->h_length);
		}
	}
	/* Check if valid IP address -- End */
	
	/* Set serverSocketAddress -- Start */
	serverSocketAddress.sin_family = AF_INET;
  serverSocketAddress.sin_port = htons(SERVER_PORT);
	/* Set serverSocketAddress -- End */
	
	/* Connect to Server -- Start */
	if (connect(serverConnection,(struct sockaddr *) &serverSocketAddress,sizeof(serverSocketAddress)) < 0){
    printf("ERROR: Unable to connect to Server\n");
		return 0;		
	}
	/* Connect to Server -- End */
	
	/* Get Client Socket Details -- Start */
	
	/* Get Client Socket Details -- End */
	
	/* Re-initialize variables -- Start */
	bytesSent=0;bytesReceived=0;
	memset(sendBuff, '\0' ,sizeof(sendBuff));
	memset(recvBuff, '\0' ,sizeof(recvBuff));
	/* Re-initialize variables -- End */
	
	/* Send request to Server -- Start */
	sprintf(sendBuff, "u`%s,p`%s,c`%s", username,password,command);
	bytesSent = write(serverConnection, (void*)&sendBuff, sizeof(sendBuff));
	if (bytesSent < 0){
			printf("ERROR: unable to send request Server.");
			return 0;
	}
	/* Send request to Server -- End */
	
	/* Receive result from Server -- Start */
	while(1){
		memset(recvBuff, '\0' ,sizeof(recvBuff));
		bytesReceived=recv(serverConnection, &recvBuff, sizeof(recvBuff), 0);
		
		if(bytesReceived < 0){
			printf("ERROR: Error in receiving data from Server.");
			return 0;
		}

		if(strstr(recvBuff,"***The End***") != NULL){
			/* If the last message -- Strip the ending message */
			/* Weird way but using backspace reduces string manipulation, though not a scalable way but for current scenario this solution seems to be a fitting one */
			printf("%s\b\b\b\b\b\b\b\b\b\b\b\b\b",recvBuff);
			return 0;
		}
		
		fputs(recvBuff, stdout);
	}
	printf("\n");
	/* Receive result from Server -- End */
	
	/* Closing the TCP connection */
	close(serverConnection);
	return 0;
}

/* 
*	Function	:	main - Initiate client, reads the argument, establishes TCP connection with server and sends command to server.
*	Parameter	:	arguments
*	Return		:	0
*/
int main(int argc, char** argv){
	
	int opt = 0;	/* CommandLine argument handler */

	/* Handling command line arguments */
	static struct option cmdLongOpts[] = {
		{"server",	required_argument, 0, 's'},
		{"user",	required_argument, 0, 'u'},
		{"password",	required_argument, 0, 'p'},
		{"exec",	required_argument, 0, 'c'},
		{NULL, 0, NULL, 0}
	};

	/* Parse the arguments -- Start */
	while ((opt = getopt_long(argc, argv,"s:u:p:c:",cmdLongOpts, NULL)) != -1){
	    	switch(opt)    	{
			case 's':
				argument.has_server=TRUE;
				strncpy(argument.server, optarg, MAXLEN);
				break;
			case 'p':
				argument.has_password=TRUE;
				strncpy(argument.password, optarg, MAXLEN);
				break;
			case 'u':
				argument.has_username=TRUE;
				strncpy(argument.username, optarg, MAXLEN);
				break;
			case 'c':
				if(strlen(optarg) > MAXLEN){
					printf("Please provide Execution Command less than %d.\n\n",MAXLEN);
					usage();
					exit(0);
				}
				argument.has_command=TRUE;
				strncpy(argument.command, optarg, MAXLEN);
				break;
		    	default:
		   		printf("Please provide valid arguments.\n\n");
					usage();
					exit(0);
	    	}
	}
	/* Parse the arguments -- End */

	/* Check if all required arguments provided -- Start */
	if(!argument.has_server){
		printf("Please provide Server Address.\n\n");
		usage();
		exit(0);
	}
	if(!argument.has_username || !argument.has_password){
		printf("Please provide username and password.\n\n");
		usage();
		exit(0);
	}
	if(!argument.has_command){
		printf("Please provide Execution Command.\n\n");
		usage();
		exit(0);
	}
	/* Check if all required arguments provided -- End */

	/* Create connection to server and send request -- Start */
	ClientConnection(argument.server,argument.username,argument.password,argument.command);
	/* Create connection to server and send request -- End */

	return 0;
}