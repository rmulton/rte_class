#include <signal.h>
#include <time.h>
#include <iostream>

// new déclare une variable qui doit être utilisée en dehors de la fonction
// pour chaque new il faut un delete

void myHandler(int sig, siginfo_t* si, void*) {
    int* counter = (int *) (*si).si_value.sival_ptr;
    printf("Counter: %d\n", *counter);
    *counter += 1;
}

int main() {
    // Create the counter
    int counter = 0;

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = myHandler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGRTMIN, &sa, NULL);

    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &counter;

    timer_t tid;
    timer_create(CLOCK_REALTIME, &sev, &tid);
    itimerspec its;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 500 * 1000 * 1000;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec =  500 * 1000 * 1000;
    timer_settime(tid, 0, &its, NULL);

    while (counter < 16) { }

    timer_delete(tid);

    return 0;
}