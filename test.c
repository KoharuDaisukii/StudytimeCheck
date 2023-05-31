#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

void alarm_to_write(int signum)
{
	alarm(0);
	static int count = 0;
	count++;
	fprintf(stdout, "%d\n", count);
	alarm(2);
}

int main()
{
	signal(SIGALRM, alarm_to_write);
	alarm(2);
	while(1)
	{
		time_t now = time(NULL);
		// fprintf(stdout, "%ld\n", now);
		
	}
}
