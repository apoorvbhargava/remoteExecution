**************************************************************
			Project 		:	remoteExecution
			Description :	Execute commands remotely using Client-Server methodology
			Author			:	Apoorv Bhargava
			Email				:	apoorvbh@usc.edu
**************************************************************

Execution Instructions:
***********************
=> Please follow the below steps to execute the functionality.
	
	Step-1: Compile the code
					# make
	
	Step-2: Populate the sample file with username, password and allowed commands
					separated by comma
					Example: 
							username1,password2,command1,command2,command3
							username2,password2,command1,command2
							username3,password3
							
	Step-2: Execute the code
			=> Initiate the functionality as Server (Executor). On terminal execute 
					the below command:
				
				# ./server -f <config_filename>

			=> Initiate the functionality as Client (Requester). On another terminal
					execute the below command:
				
				# ./client --server <server ip> --user <username> --password <password> 
									 --exec <program to execute>
	
		Note: Kill the server use "Ctrl+C" and then only close the terminal.
				
	Step 3: Cleaning the executables and output files.
					# make clean

Assumptions :
--------------------
	=> For --exec option : having spaced commands (ex. "ls -la") use quotes to 
		enclose the them.
	
	=> User cannot execute Super User (Sudo) commands
	
	=> All the valid command for a user can have multiple options
	
	=> Config File will have the entry in the following manner:
			username1,password1,command11,command12,.......
			username2,password2,command21,command22,.......
	
	=> Maximum Buffer size and command length can be changed using the global 
		 variable MAXLEN (in global.h)
	
File Description:
-----------------

	1. server.c and server.h
	-------------
		=> Contains the server code.

	2. client.c and client.h
	-------------
		=> Contains the client code.
	
	3. Makefile
	-------------
		=> akka Makefile.
		
	4. README.md:
	-----------
		=> Contains the project details and steps to execute it.
