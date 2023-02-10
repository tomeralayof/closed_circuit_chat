#makefile listener


TARGET= listener.out
CC= gcc
CFLAGS= -ansi -g -pedantic
OBJS= listener.o  



$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

listener.o: listener.c
	$(CC) $(CFLAGS) -c  listener.c


clean:
	rm -f $(TARGET) $(OBJS)