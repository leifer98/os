CC = gcc
CFLAGS = -Wall -Werror -std=c11

all: mains maindloop maindrec recursived loopd recursives loops cmp copy basic_shell
	
loops: libclassloops.a
recursives: libclassrec.a
loopd: libclassrec.so
recursived: libclassloops.so

mains: libclassrec.a main.o
	gcc -Wall -g main.o ./libclassrec.a -L. -o mains
	
maindloop: libclassloops.so main.o 
	gcc -Wall -g main.o ./libclassloops.so -L. -o maindloop
	
maindrec: libclassrec.so main.o 
	gcc -Wall -g main.o ./libclassrec.so -L. -o maindrec

libclassloops.a: basicClassification.o advancedClassificationLoop.o NumClass.h
	ar -rcs libclassloops.a basicClassification.o advancedClassificationLoop.o 
	
libclassrec.a: basicClassification.o advancedClassificationRecursion.o NumClass.h
	ar -rcs libclassrec.a basicClassification.o advancedClassificationRecursion.o 
	
libclassrec.so: basicClassification.o advancedClassificationRecursion.o NumClass.h
	gcc -shared -fPIC -o libclassrec.so basicClassification.o advancedClassificationRecursion.o
	
libclassloops.so: basicClassification.o advancedClassificationLoop.o NumClass.h
	gcc -shared -fPIC -o libclassloops.so basicClassification.o advancedClassificationLoop.o

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

.PHONY: clean all loops recursives loopd recursived

clean: 
	rm -f *.o *.a *.so mains maindloop maindrec cmp cmp.o basic_shell basic_shell.o copy copy.o
