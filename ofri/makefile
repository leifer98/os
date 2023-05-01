CC = gcc
CFLAGS = -Wall

all: codecA codecB cmp copy basic_shell encode decode

codecB: codecB.so
codecA: codecA.so
	
codecB.so: codecB.o 
	$(CC) -shared -fPIC -o codecB.so codecB.o
	
codecA.so: codecA.o 
	$(CC) -shared -fPIC -o codecA.so codecA.o

cmp: cmp.o
	$(CC) $(CFLAGS) cmp.o -o cmp

cmp.o: cmp.c
	$(CC) $(CFLAGS) -c cmp.c

copy: copy.o
	$(CC) $(CFLAGS) copy.o -o copy

copy.o: copy.c
	$(CC) $(CFLAGS) -c copy.c
	
basic_shell: basic_shell.o
	$(CC) $(CFLAGS) basic_shell.o -o basic_shell

basic_shell.o: basic_shell.c
	$(CC) $(CFLAGS) -c basic_shell.c

encode: encode.o
	$(CC) $(CFLAGS) encode.o -o encode

encode.o: encode.c
	$(CC) $(CFLAGS) -c encode.c

decode: decode.o
	$(CC) $(CFLAGS) decode.o -o decode

decode.o: decode.c
	$(CC) $(CFLAGS) -c decode.c

.PHONY: clean all codecA codecB

clean: 
	rm -f *.o *.a *.so cmp basic_shell copy codecA.so codecB.so encode decode
