all : s c
.PHONY : clean

s : server.o
	g++ server.cpp Epoll.cpp Task.h ThreadPool.cpp TaskSyncQueue.cpp TimerWheel.cpp TimerClass.h -o s -lpthread

c : client.o
	g++ client.cpp downLoad.cpp -o c -lpthread

clean : 
	rm *.o