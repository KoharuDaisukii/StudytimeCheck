all: StudytimeCheck
	
StudytimeCheck: StudytimeCheck.o login.o stats.o
	gcc -o StudytimeCheck StudytimeCheck.o login.o stats.o -lcurses
	
StudytimeCheck.o: StudytimeCheck.c studytimecheck.h varlib.h login.h
	gcc -c StudytimeCheck.c
	
login.o: login.c login.h
	gcc -c login.c
	
stats.o: stats.c varlib.h studytimecheck.h
	gcc -c stats.c
clean:
	rm -f *.o
