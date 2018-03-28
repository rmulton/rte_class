#include <signal.h>
#include <time.h>
#include <iostream>
#include <sstream>

/* Helper function to get command line arguments as integers */
int getArgvInt(int i, char* argv[]) {
    int output;
    std::istringstream iss(argv[i]);
    iss >> output;
    return output;
}

/* Timer */
class Timer {
    // call_callback est static pour permettre d'être passée à l'API POSIX
    private:
        timer_t tid;
	int counter;
        static void call_callback(int sig, siginfo_t* si, void*);
	void increment();
    protected:
        virtual void callback() = 0; // virtual permet d'appeler les méthodes des classes héritées, 0 empêche la classe mère de l'appeler 
    public:
        Timer();
        ~Timer();
        void start(double duration_ms);
        void stop();
	int* getPCounter();
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

    // Initiate the counter
    counter = 0;
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
    timer->increment();
    return;
}

void Timer::increment(){
    counter++;
}

// Stopper
void Timer::stop(){
    // TODO
    // timer_settime avec un its qui a sec = 0, nsec = 0
}

// Getter for the counter
int* Timer::getPCounter(){
    return &counter;
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

int main(int argc, char* argv[]) {
    // Get the inputs
    if (argc!=3) {
        std::cout << "Please input <program_bin> <duration> <repetitions>" << std::endl;
        return -1;
    }
    int duration = getArgvInt(1, argv);
    if (duration <= 0) {
        std::cout << "Please enter a stricly positive duration" << std::endl;
        return -1;
    }
    int repetitions = getArgvInt(2, argv);
    if (repetitions <= 0) {
        std::cout << "Please enter a stricly positive number of repetitions" << std::endl;
        return -1;
    }
    // Start the timer
    std::cout << "Starting the timer for " << duration << " milliseconds" << std::endl;
    TestTimer timer; // on utilise new quand on veut mettre un objet dans la pile et le partager à travers le code
    timer.start(duration);
    int* pcounter = timer.getPCounter();
    // Wait for the timer to finish
    while (*pcounter<repetitions){}
    return 0;

}
