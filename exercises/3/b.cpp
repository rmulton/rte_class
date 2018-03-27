#include <signal.h>
#include <time.h>
#include <iostream>

/* Timer */
class Timer {
    // call_callback est static pour permettre d'être passée à l'API POSIX
    private:
        timer_t tid;
        static void call_callback(int sig, siginfo_t* si, void* user);
    protected:
        virtual void callback() = 0; // virtual permet d'appeler les méthodes des classes héritées, 0 empêche la classe mère de l'appeler 
    public:
        Timer();
        ~Timer();
        void start(double duration_ms);
        void stop();
};

// Constructors
Timer::Timer(){
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = call_callback; // Partout où on doit donner une fonction c (par ex dans les API POSIX), il faut utiliser des méthodes statiques pour que la signature de la fonction ne contienne pas un objet
    sigemptyset(&sa.sa_mask);
    sigaction(SIGRTMIN, &sa, NULL);

    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = this; // on passe l'objet ainsi

    timer_create(CLOCK_REALTIME, &sev, &tid);
}

// Destructor
Timer::~Timer(void){
    timer_delete(tid);
}

// Starter
void Timer::start(double duration_ms){
    itimerspec its;
    int tv_sec = (int) duration_ms/1000;
    int nsec = (int) duration_ms * 1000000;
    int tv_nsec = nsec - tv_sec * 1000000000;
    std::cout << "Starting timer with tv_sec = " << tv_sec << ", tv_nsec = " << tv_nsec << std::endl;
    its.it_value.tv_sec = tv_sec;
    its.it_value.tv_nsec = tv_nsec;
    its.it_interval.tv_sec = tv_sec;
    its.it_interval.tv_nsec =  tv_nsec;
    timer_settime(tid, 0, &its, NULL);
}

// call_callback static method
void Timer::call_callback(int sig, siginfo_t* si, void*){
    Timer* timer = (Timer*) si->si_value.sival_ptr;
    timer->callback();
    return;
}

// Stopper
void Timer::stop(){
    // TODO
    // timer_settime avec un its qui a sec = 0, nsec = 0
}

/* Test timer */
class TestTimer: public Timer {
    protected:
        void callback();
};

// Callback
void TestTimer::callback(){
    std::cout << "Test timer callback called" << std::endl;
    return;
}

int main() {
    std::cout << "Starting" << std::endl;
    TestTimer timer; // on utilise new quand on veut mettre un objet dans la pile et le partager à travers le code
    timer.start(0);
    // Ajouter un while
    return 0;
}