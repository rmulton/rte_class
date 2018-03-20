#include <sstream>
#include <iostream>
#include <pthread.h>
#include <mutex>

/* Threads*/

/* Structure that contains everything a thread need to run */
struct counterStruct { volatile double* pCounter; unsigned int nLoops; pthread_mutex_t mutex; };

/* Single increment function */
void* call_incr(void* pointer){
    // Get the counter
    counterStruct* p_counter_struct = (counterStruct*) pointer;
    volatile double* pCounter = p_counter_struct->pCounter;
    unsigned int nLoops = p_counter_struct->nLoops;

    // Increment nLoops time the counter
    for (unsigned int i ; i < nLoops ; i++ ){
        pthread_mutex_lock(&p_counter_struct->mutex);
        *pCounter = *pCounter + 1.0;
        pthread_mutex_unlock(&p_counter_struct->mutex);
    }
    return pointer;
}

counterStruct getCounterStruct(double* pCounter, unsigned int nLoops, bool mutex){
    if (mutex==true){
        std::cout << "Using a mutex" << std::endl;
        // Create the counter with a mutex
        pthread_mutex_t counterMutex;
        counterStruct counter_struct = { pCounter, nLoops, counterMutex };
        pthread_mutex_init(&counter_struct.mutex, NULL);
        return counter_struct;
    } else {
        std::cout << "Using no mutex" << std::endl;
        // Create the counter with no mutex
        counterStruct counter_struct = { pCounter, nLoops };
        return counter_struct;
    }
}

void destroyMutex(counterStruct* counter_struct){
    pthread_mutex_destroy(&counter_struct->mutex);
    return;
}

/* Increment function with a mutex */
void incr(unsigned int nLoops, unsigned int nTasks, double* pCounter, bool mutex=false) {
    // Create the counter
    counterStruct counter_struct = getCounterStruct(pCounter, nLoops, mutex);
    // Create the threads
    pthread_t incrementThreads [nTasks];
    for (unsigned int i = 0; i<nTasks ; i++){
        pthread_create(&incrementThreads[i], NULL, call_incr, &counter_struct);
    }
    // Wait for every thread to be done
    for (unsigned int i = 0; i<nTasks ; i++){
        pthread_join(incrementThreads[i], NULL);
    }
    // Destroy the mutex
    // if (mutex==true){
    destroyMutex(&counter_struct);
    // }
    return;
}

/* Helpers */

/* Returns the difference between two timespecs */
timespec diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

/* Helper function to get command line arguments as integers */
int getArgvInt(int i, char* argv[]) {
    int output;
    std::istringstream iss(argv[i]);
    iss >> output;
    return output;
}

/* Helper function to get command line arguments as bools */
bool getArgvBool(int i, char* argv[], unsigned int argc, std::string trueValue) {
    // If there is not enough arguments return false
    if (i>argc-1){
        return false;
    }
    // Otherwise, check that the target argument has the expected content
    std::string input;
    std::istringstream iss(argv[i]);
    iss >> input;
    if (input==trueValue){
        return true;
    } else {
        return false;
    }
}

/* Main */

int main(int argc, char* argv[]) {

    // Check the number of arguments
    if (argc != 4 && argc!=3) {
        printf("Please enter two (nLoops, nTasks) or three arguments (nLoops, nTasks, mutex), not %d\n", argc-1);
        return -1;
    }

    // Initiate the clock
    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &start);

    // Get nLoops : convert argv[1] to an int
    int nLoops = getArgvInt(1, argv);
    // Get nTasks : convert argv[2] to an int
    int nTasks = getArgvInt(2, argv);
    // Get mutex : convert argv[3] to a string
    bool mutex = getArgvBool(3, argv, argc, "mutex");
    // Display the result
    std::cout << "Received: nLoops=" << nLoops << ", nTasks=" << nTasks << ", mutex=" << mutex << std::endl;

    // Create the counter
    double counter = 0.0;

    // Increment it with nTasks threads
    incr(nLoops, nTasks, &counter, mutex);

    // Print its value
    // std::cout << "Counter has value: " << counter;
    printf("Counter has value: %f\n", counter);

    // Print the time
    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec elapsed = diff(start, end);
    std::cout << elapsed.tv_sec + elapsed.tv_nsec/1000000000. << " seconds" << std::endl;

    return 0;
}