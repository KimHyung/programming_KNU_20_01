#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void ouch(int sig)
{
    printf("OUCH! - I got signal %d\n", sig);
    // (void) signal(SIGINT, SIG_DFL);
}
int main()
{
    (void) signal(SIGINT, ouch);
    (void) signal(SIGTERM, ouch);
    fork();
    while(1){
        // printf("Hello World!\n");
        //sleep(1);
    }
}