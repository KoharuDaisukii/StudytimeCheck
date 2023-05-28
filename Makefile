all: StudytimeCheck
	
StudytimeCheck: main.o login.o stats.o settings.o
	gcc -o StudytimeCheck main.o login.o stats.o settings.o -lcurses
	
main.o: main.c studytimecheck.h varlib.h login.h
	gcc -c main.c
	
login.o: login.c login.h
	gcc -c login.c
	
stats.o: stats.c studytimecheck.h varlib.h
	gcc -c stats.c
	
settings.o: settings.c studytimecheck.h varlib.h	
	gcc -c settings.c
	
clean:
	rm -f *.o
