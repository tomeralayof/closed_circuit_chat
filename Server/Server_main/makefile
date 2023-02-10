#makefile main_client 

VLG = valgrind --leak-check=yes --track-origins=yes
TARGET= main_server.out
CC= gcc
CFLAGS= -ansi -g -pedantic
LFLAGS= -L../../lib/ -lddslib -lm -I../../Include/
OBJS= server_app_test.o server_app.o net_server.o user.o user_manager.o  group.o group_manager.o protocol.o
SERVER_NET_PATH= ../net_server/
SERVER_APP_PATH= ../Server_app/
SERVER_USER_PATH= ../User/

SERVER_GROUP_PATH = ../Group/
SERVER_GROUP_MANAGER_PATH =../Group_manager/

SERVER_USER_MANAGER_PATH= ../user_manager/
PROTOCOL_PATH= ../../Protocol/
#-ansi
all: build run

vlg: $(TARGET)
	$(VLG) ./$(TARGET)
run:
	./$(TARGET)
	
build: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LFLAGS)

server_app_test.o: server_app_test.c
	$(CC) $(CFLAGS) -c server_app_test.c -lm

server_app.o: $(SERVER_APP_PATH)server_app.c
	$(CC) $(CFLAGS) -c $(SERVER_APP_PATH)server_app.c -lm

net_server.o: $(SERVER_NET_PATH)net_server.c
	$(CC) $(CFLAGS) -c $(SERVER_NET_PATH)net_server.c -lm

#change
group.o: $(SERVER_GROUP_PATH)group.c
	$(CC) $(CFLAGS) -c $(SERVER_GROUP_PATH)group.c -lm

group_manager.o: $(SERVER_GROUP_MANAGER_PATH)group_manager.c
	$(CC) $(CFLAGS) -c $(SERVER_GROUP_MANAGER_PATH)group_manager.c -lm
#change

user.o: $(SERVER_USER_PATH)user.c
	$(CC) $(CFLAGS) -c $(SERVER_USER_PATH)user.c -lm

user_manager.o: $(SERVER_USER_MANAGER_PATH)user_manager.c
	$(CC) $(CFLAGS) -c $(SERVER_USER_MANAGER_PATH)user_manager.c -lm

protocol.o: $(PROTOCOL_PATH)protocol.c
		$(CC) $(CFLAGS) -c $(PROTOCOL_PATH)protocol.c

clean:
	rm -f $(TARGET) $(OBJS)