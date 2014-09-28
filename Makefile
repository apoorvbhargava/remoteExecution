all:client server

client:client.c
	cc -o client client.c -Wall -lnsl -lresolv

server:server.c
	cc -o server server.c -Wall -lnsl -lresolv

clean: 
	rm client server
	
