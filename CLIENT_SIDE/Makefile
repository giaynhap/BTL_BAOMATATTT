CC=gcc
objects= main.o client.o aes.o sha1.o encrypt.o decrypt.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

client: $(objects)
	$(CC) -o $@ $^ $(CFLAGS)
clean:
	rm -f *.o
	rm -f client
