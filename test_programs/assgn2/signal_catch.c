#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
/* #include <thread.h> */

pthread_t user_threadID;
sigset_t new;
void *handler(), interrupt();


struct sigaction act;
void *
handler(char argv1[])
{
act.sa_handler = interrupt;
sigaction(SIGINT, &act, NULL);
pthread_sigmask(SIG_UNBLOCK, &new, NULL);
printf("\n Press CTRL-C to deliver SIGINT\n");
sleep(8); /* give user time to hit CTRL-C */
}
void
interrupt(int sig)
{
printf("thread %d caught signal %d\n", pthread_self(), sig);
}


main( int argc, char *argv[] ) {
sigemptyset(&new);
sigaddset(&new, SIGINT);
pthread_sigmask(SIG_BLOCK, &new, NULL);
pthread_create(&user_threadID, NULL, handler, argv[1]);
pthread_join(user_threadID, NULL);
printf("thread handler, %d exited\n",user_threadID);
sleep(2);
printf("main thread, %d is done\n", pthread_self());
} /* end main */

