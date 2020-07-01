all: run

run: hull.out
	./hull.out ./maps/c4a1a.bsp

hull.out: main.o view.o brush.o proc.o
	g++ -O3 main.o view.o brush.o proc.o -o hull.out -lGL -lGLU -lglut -lm

main.o: main.cpp hull.h
	g++ -O3 -c  main.cpp -o main.o

view.o: view.cpp hull.h
	g++ -O3 -c  view.cpp -o view.o

brush.o: brush.cpp hull.h
	g++ -O3 -c  brush.cpp -o brush.o

proc.o: proc.cpp hull.h
	g++ -O3 -c proc.cpp -o proc.o