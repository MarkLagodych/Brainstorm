all: obj bin obj/brainstorm.o obj/main.o
	g++ obj/brainstorm.o obj/main.o -o bin/brainstorm.exe

clean:
	rm -r -f obj
	rm -r -f bin

obj:
	mkdir obj

bin:
	mkdir bin

obj/brainstorm.o:
	g++ -std=c++11 -c brainstorm.cpp -o obj/brainstorm.o

obj/main.o:
	g++ -std=c++11 -c main.cpp -o obj/main.o

