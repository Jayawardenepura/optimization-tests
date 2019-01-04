#include "pi_number_counter.h"

unsigned long roundDiv(unsigned long q, unsigned long r) {
	unsigned long ret, doubleMod;
	if (q<r)
		return 0;
	
	ret = q / r;
	doubleMod = 2*(q % r);
	if (doubleMod >= r)
		return (ret+1);
	else
		return ret;
}

 void mpz_fact(mpz_t ret, mpz_t n) {
	int cmp;
	mpz_t result, m;
	mpz_init(result);

	mpz_set_ui(result, 1);
	if (mpz_cmp_ui(n, 2u) < 0) {
		mpz_set(ret, result);
		return;
	}

	mpz_init_set(m, n);
	while (mpz_sgn(m)) {
		mpz_mul(result, result, m);
		mpz_sub_ui(m, m, 1);
	}
	
	mpz_set(ret, result);
}


void chudnovsky(mpf_t rop, unsigned long precision, unsigned long iterStart, unsigned long iterFinish) {
	unsigned long k1;
	mpz_t k2;
	mpf_t sum, fraction; 
	mpz_t numerator, denominator_part1; 
	mpf_t numerator_f, denominator_part1_f;
	mpz_t tokenNeg1_powK, token1359, token5451, token6403_pow3, token6403;
	mpf_t token6403_NegSqrt;

	k1 = iterStart;
	mpz_init_set_ui(k2, iterStart);

	mpf_init2(sum, precision);
	mpf_init2(fraction, precision);
	
	mpz_init(numerator);
	mpf_init2(numerator_f, precision);
	mpz_init(denominator_part1);
	mpf_init2(denominator_part1_f, precision);

	mpz_init_set_ui(tokenNeg1_powK, 1u);
	mpz_init_set_ui(token1359, 13591409u); 
	mpz_init_set_ui(token5451, 545140134u); 
	mpz_init_set_str(token6403_pow3, "262537412640768000", 10);
	mpz_init_set_ui(token6403, 640320u);

	mpf_init2(token6403_NegSqrt, precision);
		mpf_set_ui(token6403_NegSqrt, 640320u);
		mpf_sqrt(token6403_NegSqrt, token6403_NegSqrt);
		mpf_ui_div(token6403_NegSqrt, 1u, token6403_NegSqrt);


	mpz_t k_mul6_fact, k_mul3_fact, k_fact_cube;
	mpz_init(k_mul6_fact);
	mpz_init(k_mul3_fact);
	mpz_init(k_fact_cube);


	while (k1<iterFinish) {
		mpz_mul_ui(k_mul6_fact, k2, 6u);
		mpz_fact(k_mul6_fact, k_mul6_fact);

		mpz_mul_ui(k_mul3_fact, k2, 3u);
		mpz_fact(k_mul3_fact, k_mul3_fact);

		mpz_fact(k_fact_cube, k2);
		mpz_pow_ui(k_fact_cube, k_fact_cube, 3u);

		// numerador
		mpz_mul(numerator, token5451, k2);
		mpz_add(numerator, numerator, token1359);

		mpz_mul(numerator, numerator, k_mul6_fact);
		
		mpz_mul(numerator, numerator, tokenNeg1_powK);
	
		// denominador
		mpz_pow_ui(denominator_part1, token6403_pow3, k1);

		mpz_mul(denominator_part1, denominator_part1, token6403);
		
		mpz_mul(denominator_part1, denominator_part1, k_fact_cube);

		mpz_mul(denominator_part1, denominator_part1, k_mul3_fact);

		
		// fraccao
		mpf_set_z(numerator_f, numerator);
		mpf_set_z(denominator_part1_f, denominator_part1);

		mpf_div(fraction, numerator_f, denominator_part1_f);
		
		mpf_mul(fraction, fraction, token6403_NegSqrt);

		// soma
		mpf_add(sum, sum, fraction);

		k1++;
		mpz_add_ui(k2, k2, 1u);
		mpz_neg(tokenNeg1_powK, tokenNeg1_powK);
	}


	// multiplicar por 12
	mpf_mul_ui(sum, sum, 12u);
	mpf_set(rop, sum);
}






void *doChudnovsky(void *arg) {
	unsigned long workerId;
	worker_t *me;

	workerId = *((int*) arg);
	me = &(workInfo.workers[workerId]);

//	printf("-> thread %u\n", workerId);
	chudnovsky( me->rop, workInfo.precision, me->iterStart, me->iterFinish);
//	printf("thread <-%u\n", workerId);

	return NULL;
}



void threadedChudnovsky(unsigned long threadCount, mpf_t rop, unsigned long precision, unsigned long itercount) {
	memset( &workInfo, 0, sizeof(workInfo_t) );


	// inicializacao e criacao dos workers
	workInfo.precision = precision;
	workInfo.workers = malloc(sizeof(worker_t) * threadCount);
	memset(workInfo.workers, 0, sizeof(worker_t) * threadCount);

	for (unsigned long i=0; i<(threadCount-1); i++) {
		mpf_init2( workInfo.workers[i].rop, precision );
		workInfo.workers[i].iterStart = roundDiv(itercount, threadCount) * i;
		workInfo.workers[i].iterFinish = roundDiv(itercount, threadCount) * (i+1);

		unsigned long *workerId = malloc(sizeof(unsigned long));
		*workerId = i;
		pthread_create( &(workInfo.workers[i].thread), NULL, doChudnovsky, workerId );
	}
	
	mpf_init2( workInfo.workers[threadCount-1].rop, precision );
	workInfo.workers[threadCount-1].iterStart = roundDiv(itercount, threadCount) * (threadCount-1);
	workInfo.workers[threadCount-1].iterFinish = itercount;
		
	unsigned long *workerId = malloc(sizeof(*workerId));
	*workerId = threadCount-1;
	pthread_create( &(workInfo.workers[threadCount-1].thread), NULL, doChudnovsky, workerId );
	


	// esperar que cada worker acabe
	for (unsigned long i=0; i<threadCount; i++) {
		pthread_join( workInfo.workers[i].thread, NULL );
		mpf_add(rop, rop, workInfo.workers[i].rop);
	}

	// temos (1/PI); inverter
	mpf_ui_div(rop, 1, rop);
}
