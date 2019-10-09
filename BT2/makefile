cc=gcc      
MAKE=make
RM =rm

#targets .
all: Client/client.c Server/server.c
	$(cc) -o Client/client Client/client.c	
	$(cc) -o Server/server Server/server.c

#another target for client
client_target:
	./client  

clean:server client
	$(RM) server
	$(RM) client
