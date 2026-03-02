CC = g++
CFLAGS = -Wall -Werror
all: main
main: main.o LoadBalancer.o Request.o RequestGenerator.o Webserver.o
	$(CC) $(CFLAGS) -o main main.o LoadBalancer.o Request.o RequestGenerator.o Webserver.o

main.o: main.cpp 
	$(CC) $(CFLAGS) -c main.cpp

LoadBalancer.o: LoadBalancer.cpp LoadBalancer.h
	$(CC) $(CFLAGS) -c LoadBalancer.cpp

Request.o: Request.cpp Request.h
	$(CC) $(CFLAGS) -c Request.cpp

RequestGenerator.o: RequestGenerator.cpp RequestGenerator.h
	$(CC) $(CFLAGS) -c RequestGenerator.cpp

Webserver.o: Webserver.cpp Webserver.h
	$(CC) $(CFLAGS) -c Webserver.cpp

clean:
	rm -f main *.o