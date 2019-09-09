CXX = g++

Server:main.o http_conn.o util.o epoll.o log.o
	$(CXX) *.o -o SServer -lpthread
main.o:main.cpp
	$(CXX) -c main.cpp
util.o:util.cpp
	$(CXX) -c util.cpp
epoll.o:epoll.cpp
	$(CXX) -c epoll.cpp
log.o:log.cpp
	$(CXX) -c log.cpp

http_conn.o:http_conn.cpp
	$(CXX) -c http_conn.cpp

clean:
	rm *.o SServer
