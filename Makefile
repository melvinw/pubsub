CFLAGS=-Wall -Werror -O0 -g3 -ggdb3

all: message.h message.c subscriber.h subscriber.c broker.h broker.c test_subscriber.c test_publisher.c
	gcc -c $(CFLAGS) -Wall -Werror -o message.o message.c
	gcc -c $(CFLAGS) -Wall -Werror -o publisher.o publisher.c
	gcc -c $(CFLAGS) -Wall -Werror -o subscriber.o subscriber.c
	gcc $(CFLAGS) -o test_publisher test_publisher.c publisher.o message.o
	gcc $(CFLAGS) -o test_subscriber test_subscriber.c subscriber.o message.o
	gcc $(CFLAGS) -Wall -Werror -o broker broker.c

.PHONY: clean

clean:
	rm -f message.o publisher.o subscriber.o test_subscriber test_publisher broker
