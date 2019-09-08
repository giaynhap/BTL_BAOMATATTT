CC=gcc
objects= main.o client.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

client: $(objects)
	$(CC) -o $@ $^ $(CFLAGS)
clean:
	rm -f *.o
	rm -f client