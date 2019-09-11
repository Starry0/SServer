CXX = g++

Server:http_conn.o util.o epoll.o log.o time_heap.o main.o
	$(CXX) *.o -o SServer -lpthread
util.o:util.cpp
	$(CXX) -c util.cpp
epoll.o:epoll.cpp
	$(CXX) -c epoll.cpp
log.o:log.cpp
	$(CXX) -c log.cpp
http_conn.o:http_conn.cpp
	$(CXX) -c http_conn.cpp
time_heap.o:time_heap.cpp
	$(CXX) -c time_heap.cpp
main.o:main.cpp
	$(CXX) -c main.cpp

clean:
	rm *.o SServer
