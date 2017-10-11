all: client  server

client : uniclient.c
	gcc -Wall uniclient.c -o uniclient

server : uniserver.c
	gcc -Wall uniserver.c -o uniserver

clean  :
	$(RM) uniclient
	$(RM) uniserver
