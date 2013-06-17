CC = gcc
CCP = clang++
CFLAGS = -std=c99
CPPFLAGS = -std=c++11

all: tibast.exe msg_injector.dll pill.dll

tibast.exe: tibast.cpp msg_injector.dll thread_injector.dll Makefile
	$(CCP) $(CPPFLAGS) -lmsg_injector -lthread_injector -L. $< -o $@

thread_injector.dll: thread_injector.c Makefile
	$(CC) $(CFLAGS) --shared $< -o $@

msg_injector.dll: msg_injector.c Makefile
	$(CC) $(CFLAGS) --shared $< -o $@

pill.dll: pill.c Makefile
	$(CC) --shared $< -o $@

clean:
	rm *.exe *.dll
