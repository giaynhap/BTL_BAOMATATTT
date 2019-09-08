CC=gcc
CFLAGS = -pthread

objects= main.o


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(objects)
	$(CC) -o $@ $^ $(CFLAGS)
clean:
	rm -f *.o
	rm -f server