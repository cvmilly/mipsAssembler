CC= gcc
CXX= g++

all: clean asm.exe


.c.o:
	$(CC) -c -o $@ $<
.cpp.o:
	$(CXX) -c -o $@ $<  -std=c++11

asm.exe: assem.o
	$(CXX) -o $@ $< -std=c++11
 
clean:
	rm -f my.obj asm.exe *.o *~ \#*\#


