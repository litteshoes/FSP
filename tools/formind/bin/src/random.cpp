////////////////////////////////////////////////////////////////////
//
// FORMIND – the forest model
// Contact: info@formind.org
// http://www.formind.org/
//
// Author: FORMIND model developer team
// Maintainer: FORMIND model developer team
// Copyright: Helmholtz Centre for Environmental Research - UFZ
//            and FORMIND model developer team.
// License: AGPL (>= 3)
//
///////////////////////////////////////////////////////////////////
//
// File						random.cpp
// Description				Random generator
//
///////////////////////////////////////////////////////////////////


#include "random.h"
#include <math.h>

using namespace std;

#ifdef FORMIND_2016_RNGS

prng_formind_default_type formindPRNG;
int seedUsed; // remember the seed used for the output files.
MSRand myRand, myRand_Sidar, myRand_Thinning;

#pragma warn -8004

double prand(double lambda)

// -----------------------------------------------------------------------------
	 /* !
	  \brief          Calculates poisson-distributed random numbers
	  \param	      	double lambda
	  \return         double prand
	  \details		  	Algorithm from Kemp 1990; only recomended for lambda < 1.
	  */

{

	double u, s = 0, r, ppi, psi, p, f, h = 0, q;
	r = floor(lambda + 1);
	vector<double>cc(r + 1, 0);
	u = _Random();
	r = floor(lambda + 1);
	s = 2 * exp(-r) * pow(r, r) / fak(r);
	ppi = r * s / (r + lambda);
	if (u > s)
		goto five;
	if (u < ppi)
		return r - 1;
	else
		return r;
five:
	psi = s - ppi;
	for (int j = 1; j <= r - 1; j++) {
		cc[r - j] = ppi;
		ppi = (r - j) * ppi / lambda;
		s = s + ppi;
		if (u <= s)
			return r + j;
	}
	cc[0] = ppi;
	cc[r] = psi;
	p = exp(-lambda);
	f = p / ppi;
	h = f * s;
	q = f * psi;
	if (u > h)
		goto twelve;
	s = s / (f - 1);
	u = u / (f - 1);
	for (int j = 0; j <= r - 1; j++) {
		s = s + cc[r - 1 - j];
		if (u <= s)
			return r - 1 - j;
		s = s + cc[r + j];
		if (u <= s)
			return r + j;
	}
twelve:
	for (int j = 2 * r; j <= 500; j++) {
		q = lambda * q / j;
		h = h + q;
		if (u <= h)
			return j;
	}
	return 0;
}
#pragma warn .8004


double erand(double beta)

// -----------------------------------------------------------------------------
	 /* !
	  \brief		Exponential random number
	  \param		beta (mean = 1/beta)
	  \return	double random number
	  \details 	If x is a (0,1) uniformly distributed random number, then -1/beta * ln(x)
					is a exponentially distributed random number with parameter beta
	  */
{
	double ran = _Random();
	if (ran == 0.) {
		ran = 0.001;
	}
	return -log(ran) / beta;
}

// --------------------------------------------------------------------------------------------------
/* !
 \brief          Calculates a number's factorial.
 \param	        int val
 \return         int
 */

int fak(int val)

{
	if (val > 1) {
		return fak(val - 1) * val;
	}
	else
		return 1;
}

double nrand(double mu, double sigma)

// -----------------------------------------------------------------------------
	 /* !
	  \brief       gaussian random number,
	  \param       mu (mean)
	  \param       sigma (standart deviation)
	  \return      double - gaussian random number
	  \details     The "twelve-rule" (german: "Zwölferregel") describes a method to create approximately normal
						distributed random numbers. If y is (0,1) normal distributed,
						then z = sy+m is a normal distributed random number with mean m and
						variance s2
	  */

{
	double summe = -6;
	for (int i = 1; i <= 12; i++) {
		double ran = _Random();
		summe += ran;
	}
	return sigma * summe + mu;
}

#else

MSRand myRand, myRand_Sidar, myRand_Thinning;

double Seed = 1.0;
// the seed variable which is used inside the random generator

int seedUsed; // remember the seed used for the output files.

void _RandomInit(int s) {
	seedUsed = s;
	Seed = (double)s;
}

double _Random(void)

// -----------------------------------------------------------------------------
  	 /* !
	  \brief          Calculates random numbers between 0 and 1
	  \param	      	void
	  \return         double
	  */

{

	static double a = 16807.0;
	static double q = 127773.0;
	static double r = 2836.0;
	static double m = 2147483647.0;
	double Test, Hi, Lo;

	do {
		if (Seed == 0)
			Seed = 0.1;
		Hi = floor(Seed / q);
		Lo = Seed - q * Hi;
		Test = a * Lo - r * Hi;
		if (Test > 0.0)
			Seed = Test;
		else
			Seed = Test + m;

		if ((Seed >= m) || (Seed < 0.0))
			cout << "WARNING: variable warning \tfile: " << __FILE__ <<
				 "\tfunction: _Randomize\t line:" << __LINE__ <<
				 "\t seed number extremly large or smaller than zero: " <<
				 Seed << endl;
	}
	while ((Seed >= m) || (Seed < 0.0));

	return Seed / m;


}

int _NRandom(int VAL)

// -----------------------------------------------------------------------------
	 /* !
	  \brief          Calculates random number between 0 and VAL - 1.
	  \param	      	int VAL
	  \return         int
	  */

{

	return (int)(_Random() * ((long)VAL));
}

#pragma warn -8004

double prand(double lambda)

// -----------------------------------------------------------------------------
	 /* !
	  \brief          Calculates poisson-distributed random numbers
	  \param	      	double lambda
	  \retrun         double prand
	  \details		  	Algorithm from Kemp 1990; only recomended for lambda < 1.
	  */

{

	double u, s = 0, r, ppi, psi, p, f, h = 0, q;
	r = floor(lambda + 1);
	vector<double>cc(r + 1, 0);
	u = _Random();
	r = floor(lambda + 1);
	s = 2 * exp(-r) * pow(r, r) / fak(r);
	ppi = r * s / (r + lambda);
	if (u > s)
		goto five;
	if (u < ppi)
		return r - 1;
	else
		return r;
five:
	psi = s - ppi;
	for (int j = 1; j <= r - 1; j++) {
		cc[r - j] = ppi;
		ppi = (r - j) * ppi / lambda;
		s = s + ppi;
		if (u <= s)
			return r + j;
	}
	cc[0] = ppi;
	cc[r] = psi;
	p = exp(-lambda);
	f = p / ppi;
	h = f * s;
	q = f * psi;
	if (u > h)
		goto twelve;
	s = s / (f - 1);
	u = u / (f - 1);
	for (int j = 0; j <= r - 1; j++) {
		s = s + cc[r - 1 - j];
		if (u <= s)
			return r - 1 - j;
		s = s + cc[r + j];
		if (u <= s)
			return r + j;
	}
twelve:
	for (int j = 2 * r; j <= 500; j++) {
		q = lambda * q / j;
		h = h + q;
		if (u <= h)
			return j;
	}
	return 0;
}
#pragma warn .8004

double erand(double beta)

// -----------------------------------------------------------------------------
	 /* !
	  \brief			Exponential random number
	  \param			beta (mean = 1/beta)
	  \return		double random number
	  \details     if x is a (0,1) equally distributed random number,  then
						-1/beta * ln(x) is a exponentially distributed random number
						with parameter beta
	  */
{
	double ran = _Random();
	if (ran == 0.) {
		ran = 0.001;
	}
	return -log(ran) / beta;
}

// --------------------------------------------------------------------------------------------------
/* !
 \brief          Calculates a number's factorial.
 \param	     	  int val
 \result         int
 */
int fak(int val)

{
	if (val > 1) {
		return fak(val - 1) * val;
	}
	else
		return 1;
}

double nrand(double mu, double sigma)

// -----------------------------------------------------------------------------
	 /* !
	  \brief       gaussian random number,
	  \param       mu (mean)
	  \param       sigma (standart deviation)
	  \return      double - gaussian random number
	  \details     The "twelve-rule" (german: "Zwölferregel) describes a method to create approximately normally
						distributed random number. If y is (0,1)-normally distrbuted random
						number, then z = sy+m is a normally distributed random number
						with mean m and variance s^2
	  */

{
	double summe = -6;
	for (int i = 1; i <= 12; i++) {
		double ran = (double)rand() / RAND_MAX;
		summe += ran;
	}
	return sigma * summe + mu;
}
#endif

// -----------------------------------------------------------
// ----------------- end of for_random.cpp -------------------
// -----------------------------------------------------------
