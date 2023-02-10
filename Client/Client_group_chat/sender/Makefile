#makefile sender

TARGET= sender.out
CC= gcc
CFLAGS= -ansi -g -pedantic
OBJS= sender.o  


$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

sender.o: sender.c
	$(CC) $(CFLAGS) -c  sender.c


clean:
	rm -f $(TARGET) $(OBJS)