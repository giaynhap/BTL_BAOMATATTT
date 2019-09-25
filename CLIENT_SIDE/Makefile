CC=gcc
objects= main.o act_socket.o aes.o sha1.o encrypt.o decrypt.o util.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

client: $(objects)
	$(CC) -o $@ $^ $(CFLAGS)
clean:
	rm -f *.o
	rm -f client
