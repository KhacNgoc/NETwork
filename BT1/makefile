cc=gcc      
MAKE=make
RM =rm

#targets .
all: client.c server.c
	$(cc) -o client client.c	
	$(cc) -o server server.c


#another target for client
client_target:
	./client  


clean:server client
	$(RM) server
	$(RM) client
