CXX = g++

Server:main.o http_conn.o util.o epoll.o
	$(CXX) *.o -o SServer -lpthread
main.o:main.cpp
	$(CXX) -c main.cpp
util.o:util.cpp
	$(CXX) -c util.cpp
epoll.o:epoll.cpp
	$(CXX) -c epoll.cpp

http_conn.o:http_conn.cpp http_conn.h
	$(CXX) -c http_conn.cpp

clean:
	rm *.o SServer
