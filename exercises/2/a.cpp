#include <sstream>
#include <iostream>
#include <pthread.h>

struct counterStruct { volatile double* pCounter; unsigned int nLoops; };

/* Single increment function */
void* incrementer(void* pointer){
    // Get the counter
    counterStruct* p_counter_struct = (counterStruct*) pointer;
    volatile double* pCounter = p_counter_struct->pCounter;
    unsigned int nLoops = p_counter_struct->nLoops;

    while ( *pCounter < nLoops ){
        *pCounter = *pCounter + 1.0;
    }
    return pointer;
}

/* Increment function */
void incr(unsigned int nLoops, unsigned int nTasks, double* pCounter) {
    // Create the counter
    counterStruct counter_struct = { pCounter, nLoops };
    // Create the threads
    pthread_t incrementThreads [nTasks];
    for (unsigned int i = 0; i<nTasks ; i++){
        pthread_create(&incrementThreads[i], NULL, incrementer, &counter_struct);
    }
    // Wait for every thread to be done
    for (unsigned int i = 0; i<nTasks ; i++){
        pthread_join(incrementThreads[i], NULL);
    }
    
    return;
}

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
int getArgv(int i, char* argv[]) {
    int output;
    std::istringstream iss(argv[i]);
    iss >> output;
    return output;
}

/* Main function */
int main(int argc, char* argv[]) {

    // Check the number of arguments
    if (argc != 3) {
        printf("Please enter two argument, not %d\n", argc-1);
        return -1;
    }

    // Initiate the clock
    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &start);

    // Get nLoops : convert argv[1] to an int
    int nLoops = getArgv(1, argv);
    // Get nTasks : convert argv[2] to an int
    int nTasks = getArgv(2, argv);
    // Display the result
    std::cout << "Received: nLoops=" << nLoops << ", nTasks=" << nTasks << std::endl;

    // Create the counter
    double counter = 0.0;

    // Increment it with nTasks threads
    incr(nLoops, nTasks, &counter);

    // Print its value
    // std::cout << "Counter has value: " << counter;
    printf("Counter has value: %f", counter);

    // Print the time
    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec elapsed = diff(start, end);
    std::cout << "\n" << elapsed.tv_sec + elapsed.tv_nsec/1000000000. << " seconds\n";

    return 0;
}