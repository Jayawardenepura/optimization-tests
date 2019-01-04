#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define THREAD 4
#define CNT_ITERATION 1000
#include <time.h>

#include "pi_number_counter.h"
/**
 * timespec_diff() - returns time difference in milliseconds for two timespecs.
 * @stop:	time after event.
 * @start:	time before event.
 *
 * Uses difftime() for time_t seconds calcultation.
 */


struct results{
    double time;
    int digitals;
};

struct results data[sizeof(struct results)];

double timespec_diff(struct timespec *stop, struct timespec *start)
{
	double diff = difftime(stop->tv_sec, start->tv_sec);
	diff *= 1000;
	diff += (stop->tv_nsec - start->tv_nsec) / 1e6;
	return diff;
}

int main(int argc, char *argv[])
{
	unsigned long threadCount;
	unsigned long itercount;
	unsigned long precision;

	unsigned int number_of_digits;
	char *outputfile = "";
	unsigned int seed;

	
	threadCount = THREAD;
	itercount = CNT_ITERATION;
	/* Parse arguments: max value, amount of points and output file for print data and plotting*/
	if (argc == 1) goto exc_noargs;
	
	if (argc > 1 && argc < 4) {
		number_of_digits = atol(argv[1]);
		outputfile = argv[2];

	}else goto exc_usage;
	
	if (argc > 4)
		goto exc_argtoomuch;
	
	mpf_t pi;

	unsigned long num_of_elements = 16;
	int miniCnt_of_elements = 0;
	int significantDigits;
	char *pi_str;


	
	for(;num_of_elements < number_of_digits;num_of_elements *= 2)
	miniCnt_of_elements++;

	data[miniCnt_of_elements];

	//struct results res[miniCnt];
	/*Array of array for save data*/
	//double *Amout_plus_time = malloc(2*(miniCnt) * sizeof(*Amout_plus_time));

	miniCnt_of_elements = 0;
	num_of_elements = 16;// describe 
			//	  \/
	

	/*number of digits has equivalent to 2^(n-1) */
	/*if you arguments like  1200 -> 1024 (then program will eat only 1024 and display an equivalent) */
	for(;num_of_elements < number_of_digits;num_of_elements *= 2) // 16,32,64,128...etc 
	{	
		precision = 8 * num_of_elements; // ?? from Author
		mpf_init2(pi, precision); 

		struct timespec time_now, time_after;
		
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_now);
		threadedChudnovsky(threadCount, pi, precision, itercount);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_after);
		
		gmp_asprintf( &pi_str, "%.Ff", pi );
		significantDigits = strlen(pi_str) - 2; //Like amount at step

		printf("Calculating %i number took %g ms \n", 
		       significantDigits, timespec_diff(&time_after, &time_now));

		//fill arrays of data
		data[miniCnt_of_elements].digitals = significantDigits;
		data[miniCnt_of_elements].time = timespec_diff(&time_after, &time_now);
		miniCnt_of_elements ++;
	}
	FILE *data_File;
	/*Fill output data file with time and number of sorted elemets*/
	if ((data_File = fopen(outputfile,"w")) == NULL) goto exc_fopen;

	for(int i = 0; i < miniCnt_of_elements;i++) fprintf(data_File,"%d	%f\n",data[i].digitals,data[i].time);

	//print info for GNUPLOT:    AMOUNT  	TIME (ms)
	fflush(data_File);
	fclose(data_File);

/* Exception handling */

	return 0;

	exc_noargs:
	printf("Error: no arguments provided\n");
	goto exc_usage;

	exc_argtoomuch:
	printf("Error: too much arguments\n");
	/* Notice: no goto here */

	exc_usage:
	printf("Usage:\n <Binar number of digit> <youfileOutput.dat>\n");
	return -1;

	exc_fopen:
	printf("Error: could not open the file\n");
	return -1;

	exc_malloc:
	printf("Error: could not allocate array of size");
	return -1;
	
}
