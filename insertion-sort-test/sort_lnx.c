#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "insertion_sort.h"
/**
 * timespec_diff() - returns time difference in milliseconds for two timespecs.
 * @stop:	time after event.
 * @start:	time before event.
 *
 * Uses difftime() for time_t seconds calcultation.
 */
double timespec_diff(struct timespec *stop, struct timespec *start)
{
	double diff = difftime(stop->tv_sec, start->tv_sec);
	diff *= 1000;
	diff += (stop->tv_nsec - start->tv_nsec) / 1e6;
	return diff;
}
int main(int argc, char *argv[])
{

	unsigned int max_value = 0;
	unsigned int amount_of_points = 0;
	unsigned int interval = 0;
	char *outputfile = "";
	unsigned int seed;
	bool is_verbos = false;

	/* Parse arguments: max value, amount of points and output file for print data and plotting*/
	//if (argc == 1) goto exc_noargs;
	
	if (argc > 1 && argc < 5) {
		max_value = atol(argv[1]);
		amount_of_points = atol(argv[2]);
		if (max_value <= 0 || amount_of_points <= 0) goto exc_wrongnum;
		if(max_value < amount_of_points) goto exc_nomath;
		outputfile = argv[3];
		interval = max_value/amount_of_points;
		if (interval <= 0) goto exc_wrongnum;
	}else goto exc_usage;

	if (argc > 4)
		goto exc_argtoomuch;

	float *TimeArray = malloc(amount_of_points * sizeof(*TimeArray));
	if (NULL == TimeArray) goto exc_malloc;

	/*Random creating*/
	FILE *fp_rand;
	
	if ((fp_rand = fopen("/dev/urandom", "rb")) == NULL) goto exc_fopen;

	fread(&seed, sizeof(seed), 1, fp_rand);
	if (ferror(fp_rand)) goto exc_fread;
	fflush(fp_rand);
	fclose(fp_rand);

	srand(seed);
	if (is_verbos) printf("* Random seed set to: %u\n", seed);

	int *array = 'NULL' ;
	if (NULL == array) goto exc_malloc;

	if (is_verbos) printf("* Initial array:\n");
	
	
	unsigned long num_of_elements = interval;
	unsigned int miniCnt = 0;

	/*Devide X-line on an intervals*/
	for(;num_of_elements <= max_value;num_of_elements += interval)
	{
		/*Memory for a new array with random number*/
		array = malloc(num_of_elements * sizeof(*array));
		if (NULL == array) goto exc_malloc;

		/* Fill array with random numbers */
		for (unsigned int cnt = 0; cnt < num_of_elements; cnt++)
		array[cnt] = rand();

		/* Sort array */
		struct timespec time_now, time_after;

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_now);
		insertion_sort(array, num_of_elements);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_after);

		printf("Sorting %li elements took %g ms \n", 
		       num_of_elements, timespec_diff(&time_after, &time_now));

		/*Fill array with time data*/
		TimeArray[miniCnt] = timespec_diff(&time_after, &time_now);
		miniCnt ++;

		free(array);

	}

	miniCnt = 0;
	FILE *data_File;
	/*Fill output data file with time and number of sorted elemets*/
	if ((data_File = fopen(outputfile,"w")) == NULL) goto exc_fopen;

	for(num_of_elements = interval; num_of_elements <= max_value;num_of_elements += interval)
	{
		
		fprintf(data_File,"%ld	%f\n",num_of_elements,TimeArray[miniCnt]);
		miniCnt++;
	}
	
	free(TimeArray);

	fflush(data_File);
	fclose(data_File);

/* Exception handling */

	return 0;

	exc_noargs:
	printf("Error: no arguments provided\n");
	goto exc_usage;


	exc_wrongnum:
	printf("Error: num_items should be > 0\n");
	goto exc_usage;

	exc_argunknown:
	printf("Error: unknown argument (%s)\n", argv[2]);
	goto exc_usage;

	exc_argtoomuch:
	printf("Error: too much arguments\n");
	/* Notice: no goto here */

	exc_usage:
	printf("Usage:\n <Max value> <Amount of points> <youfileOutput.dat>\n");
	return -1;

	exc_fopen:
	printf("Error: could not open the file\n");
	return -1;

	exc_fread:
	printf("Error: read I/O mismatch\n");
	fclose(fp_rand);
	return -1;

	exc_malloc:
	printf("Error: could not allocate array of size");
	free(array);
	return -1;
	
	exc_nomath:
	printf("Max value must be more than point");
	return -1;
}
