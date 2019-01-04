#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <pthread.h>
#include <stdint.h>
#include <gmp.h> // sudo apt-get install libgmp3-dev

typedef struct {
	pthread_t thread;
	mpf_t rop;
	unsigned long iterStart;	
	unsigned long iterFinish;
} worker_t;
	

typedef struct {
	unsigned long precision;
	worker_t *workers;
} workInfo_t;

workInfo_t workInfo;

void pi_digits(long);
unsigned long roundDiv(unsigned long, unsigned long);
void mpz_fact(mpz_t ret, mpz_t n);
void chudnovsky(mpf_t rop, unsigned long precision, unsigned long iterStart, unsigned long iterFinish);
void *doChudnovsky(void *arg);
void threadedChudnovsky(unsigned long threadCount, mpf_t rop, unsigned long precision, unsigned long itercount);
