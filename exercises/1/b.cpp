#include <sstream>

/* Increment function */
void incr(unsigned int nLoops, double* pCounter) {
    for (unsigned int i = 0; i < nLoops; i++){
        *pCounter = *pCounter + 1.0;
    }
    return;
}

// Returns the difference between two timespecs
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

int main(int argc, char* argv[]) {
    // Check the number of arguments
    if (argc != 2) {
        printf("Please enter one argument, not %d\n", argc-1);
        return -1;
    }
    // Initiate the clock
    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &start);

    // Convert argv[1] to an int
    int nLoops;
    std::istringstream iss(argv[1]);
    iss >> nLoops;

    // Create the counter
    double pCounter = 0.0;
    // Increment it
    incr(nLoops, &pCounter);
    // Print its value
    printf("%f\n", pCounter);

    // Print the time
    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec elapsed = diff(start, end);
    printf("%ld.%06ld seconds\n", elapsed.tv_sec, elapsed.tv_nsec);

    return 0;
}