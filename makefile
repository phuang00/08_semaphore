ifeq ($(DEBUG), true)
	CC = gcc -g
else
	CC = gcc
endif

all: tele_control.o tele_user.o
	$(CC) -o control tele_control.o
	$(CC) -o write tele_user.o

tele_control.o: tele_control.c
	$(CC) -c tele_control.c

tele_user.o: tele_user.c
	$(CC) -c tele_user.c

memcheck:
	valgrind --leak-check:yes ./test_rand

clean:
	rm *.o
	rm *~
