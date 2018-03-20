#include <sstream>
#include <signal.h>
#include <time.h>
#include <limits.h>

/* Increment function */
unsigned incr(unsigned int nLoops, double* pCounter, bool* pStop) {
    unsigned int k = 0;
    for (unsigned int i = 0; i < nLoops; i++){
        if (*pStop){
            return k;
        }
        *pCounter = *pCounter + 1.0;
        k++;
    }
    return k;
}

void flip(int sig, siginfo_t* si, void*){
    printf("Setting stop to true\n");
    bool* pStop = (bool*) (*si).si_value.sival_ptr;
    *pStop = true;
    return;
}

void create_timer(bool* pStop, int sec){
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &flip;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGRTMIN, &sa, NULL);

    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = pStop;

    timer_t tid;
    timer_create(CLOCK_REALTIME, &sev, &tid);
    itimerspec its;
    its.it_value.tv_sec = sec;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = sec;
    its.it_interval.tv_nsec =  0;
    timer_settime(tid, 0, &its, NULL);

}

int main(int argc, char* argv[]) {
    // Check the number of arguments
    if (argc != 2) {
        printf("Please enter one argument, not %d\n", argc-1);
        return -1;
    }

    // Convert argv[1] to an int
    int timer;
    std::istringstream iss(argv[1]);
    iss >> timer;


    // Declare the stop variable
    bool stop = false;

    // Create the counter
    double counter = 0.0;

    // Create the timer
    create_timer(&stop, timer);
    
    // Use the increment function
    unsigned int nLoops = UINT_MAX;
    unsigned int k = incr(nLoops, &counter, &stop);
    printf("%d loops completed\n", k);

}