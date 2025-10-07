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
//
// File         ranodm.h
// Description  Random number generator
// double Seed: initial value
// int _NRandom(int Val): random natural number [0,val)
// double Random(void):   random natural number [0,1)
// _Randomize(): initiialize seed with random val from Timer
//
///////////////////////////////////////////////////////////////////

/* ------ 2016 comment by Michael Müller -----------------
 Whats there 12.01.2016:
 Formind uses 3 Random Number Generator instances:
 1. MSRand myRand (used in for_var.cpp - default seed only - sure you want that? )
 2. MSRand myRand_Sidar; (uses default seed only -> sure thats what you want? )
 3. a global one used in double _Random()
 The latter gets seeded with _RandomInit(int s), the others have a seed function.
 The function int _NRandom(int Val) derives a random integer between 0 and Val by
 using the global _Random() function.
 Additionally there are several distributions. One of them uses the builtin generator
 rand(): double nrand(double mu, double sigma). The others use _Random().

 Conclusion: A mess. 4 RNG Instances. Only 1 seeded - that means the others
 return the same random numbers each time formind is started.
 The old _Random() function used floating point arithmetic to generate a
 random number. I do not know about the random properties, but I guess they are bad.
 But I know about speed and am sure it is bad.

 My plan would be:
 Step 1:
 #define FORMIND_2016_RNGS switch. If defined one single fast global generator
 is used. Additionally it should be easy to replace it by other generator types.
 But the default type should be fast and good - and approved by some Guru.
 For all other existing generators a dummy is created. So only ONE generator is
 used.
 --> DONE ! 13.01.16 It does not make any notable difference in speed.
 --> Results not checked yet.

 Step 2:
 replace the random distribution functions by fast and reliable functions,
 approved by some Guru.
 --------------------------------------------------------- */

#ifndef  randomH
#define  randomH

#include "for_global.h"
#include "for_var.h"

/* ------------------------------------------------------------
 If you want the new prng make sure FORMIND_2016_RNGS
 is defined.
 The existing interfaces (MSRand, _Random) will use
 one and only one instance of the new prng
 ------------------------------------------------------------ */
// #define FORMIND_2016_RNGS

#ifdef FORMIND_2016_RNGS
/* ------------------------------------------------------------
 The new prng - several ones you can choose from
 (choosing is done at the end of this section)
 ------------------------------------------------------------ */

#include <stdint.h>
#define prng_default_seed 782553245

const double UINT64_MAX_HALF = ((double)UINT64_MAX) * 0.5;
const double UINT32_MAX_ONEBY = 1.0 / ((double)UINT32_MAX);
const double UINT32_MAX_HALF = ((double)UINT32_MAX) * 0.5;

/*
 PRNG Reference: http://xorshift.di.unimi.it/
 Sebastiano Vigna (vigna@acm.org)
 if	for some reason you absolutely want 64 bits of state; otherwise, we
 rather suggest to use a xorshift128+ (for moderately parallel
 computations) or xorshift1024* (for massively parallel computations)
 Passes BigCrush.
 --> good for seeding xorshift1024
 ------
 setSeed function ect. supplemented by michael.mueller@ufz.de
 */
class prng_splitmix64 {
public:
	uint64_t x; /* The state can be seeded with any value. */

	prng_splitmix64() {
		setSeed(prng_default_seed);
	}

	uint64_t getInt() {
		uint64_t z = (x += UINT64_C(0x9E3779B97F4A7C15));
		z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
		z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
		return z ^ (z >> 31);
	}

	void setSeed(uint64_t theseed) {
		x = theseed;
	}

	double getDouble() {
		return getInt() / double(UINT64_MAX);
	}
};

/*
 PRNG Reference: http://xorshift.di.unimi.it/
 Sebastiano Vigna (vigna@acm.org)
 period: 2 by 1024 - 1
 written 2014/2015
 Passes BigCrush.
 ----
 setSeed function ect. supplemented by michael.mueller@ufz.de
 */
class prng_xorshift1024_64 {
public:
	uint64_t s[16];
	int p;
	prng_splitmix64 splitmix; // for seeding

	prng_xorshift1024_64() {
		setSeed(prng_default_seed);
	}

	uint64_t getInt(void) {
		const uint64_t s0 = s[p];
		uint64_t s1 = s[p = (p + 1) & 15];
		s1 ^= s1 << 31; // a
		s[p] = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30); // b,c
		return s[p] * UINT64_C(1181783497276652981);
	}

	void setSeed(uint64_t theseed) {
		p = 0;
		splitmix.setSeed(theseed);
		for (int i = 0; i < 16; i++)
			s[i] = splitmix.getInt();
	}

	double getDouble() {
		return getInt() / double(UINT64_MAX);
	}

	bool getBool() {
		if (getInt() < UINT64_MAX_HALF)
			return true;
		else
			return false;
	}

};

/*
 PRNG Reference: http://xorshift.di.unimi.it/
 Sebastiano Vigna (vigna@acm.org)
 period: 2 by 128 - 1
 written 2014/2015
 Passes BigCrush.
 ----
 setSeed function ect. supplemented by michael.mueller@ufz.de
 */
class prng_xorshift128 {
public:
	uint64_t s[2];
	prng_splitmix64 splitmix; // for seeding

	prng_xorshift128() {
		setSeed(prng_default_seed);
	}

	uint64_t getInt(void) {
		uint64_t s1 = s[0];
		const uint64_t s0 = s[1];
		s[0] = s0;
		s1 ^= s1 << 23; // a
		s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); // b, c
		return s[1] + s0;
	}

	void setSeed(uint64_t theseed) {
		splitmix.setSeed(theseed);
		for (int i = 0; i < 2; i++)
			s[i] = splitmix.getInt();
	}

	double getDouble() {
		return getInt() / double(UINT64_MAX);
	}

	bool getBool() {
		if (rand() < UINT64_MAX_HALF)
			return true;
		else
			return false;
	}
};

/*
 MWC by Georg Marsaglia as posted 20 Jan 1999
 From: George Marsaglia <geo@stat.fsu.edu>
 Subject: Random numbers for C: The END?
 Does pass Diehard. Does not pass BigCrush.
 Still very good for many applications.
 32 bit !!!
 ----
 setSeed function ect. supplemented by michael.mueller@ufz.de
 */
class prng_mwc {
public:
	uint32_t z, w;
	prng_splitmix64 splitmix; // for seeding

	prng_mwc() {
		// z=362436069;
		// w=521288629;
		setSeed(prng_default_seed);
	}

	uint32_t getInt() {
		return (((z = 36969 * (z & 65535) + (z >> 16)) << 16) + (w =
			 18000 * (w & 65535) + (w >> 16)));
	}

	void setSeed(uint32_t theseed) {
		splitmix.setSeed(theseed);
		for (int i = 0; i < 1000000; i++)
			splitmix.getInt();
		z = splitmix.getInt();
		w = splitmix.getInt();
	}

	double getDouble() {
		return getInt() * UINT32_MAX_ONEBY;
	}

	bool getBool() {
		if (getInt() < UINT32_MAX_HALF)
			return true;
		else
			return false;
	}
};

/*
 SHR3 by Georg Marsaglia as posted 20 Jan 1999
 From: George Marsaglia <geo@stat.fsu.edu>
 Subject: Random numbers for C: The END?
 #define SHR3 (jsr^=(jsr<<17), jsr^=(jsr>>13), jsr^=(jsr<<5))
 Does not pass Dieahrd.
 But still perfectly ok for many applications.
 Really fast on 32 bit, even if debugging. If implemented as MACRO
 in some situatins even faster.
 32 bit !!!
 ----
 setSeed function ect. supplemented by michael.mueller@ufz.de
 */
class prng_shr3 {
public:
	uint32_t jsr;
	prng_splitmix64 splitmix; // for seeding

	prng_shr3() {
		setSeed(prng_default_seed);
	}

	uint32_t getInt() {
		return (jsr ^= (jsr << 17), jsr ^= (jsr >> 13), jsr ^= (jsr << 5));
	}

	void setSeed(uint32_t theseed) {
		splitmix.setSeed(theseed);
		// for(int i=0;i<1000;i++)
		// splitmix.getInt();
		jsr = splitmix.getInt();
		// for(int i=0;i<1000;i++)
		// getInt();
	}

	double getDouble() {
		return getInt() * UINT32_MAX_ONEBY;
	}

	bool getBool() {
		if (getInt() < UINT32_MAX_HALF)
			return true;
		else
			return false;
	}
};

/* ------------------------------------------------------------
 --> Here you can easily switch between different prngs. Keep one line
 only without comment:
 ------------------------------------------------------------ */

// typedef prng_shr3 prng_formind_default_type ;  //
// typedef prng_mwc prng_formind_default_type ;
typedef prng_xorshift128 prng_formind_default_type;
// 64-207  32-3 fastest on 64bit Embarcadero

// the one and only instance of the prng - keep that line:
extern prng_formind_default_type formindPRNG;

/* ------------------------------------------------------------
 Dummy functions, global variables used
 ------------------------------------------------------------ */

class MSRand {

public:
	MSRand() {
	}

	int rnd() {
		return formindPRNG.getInt();
	}

	double drnd() {
		return formindPRNG.getDouble();
	}
};

// formind stuff for file i/o
extern int seedUsed; // remember seed used for the output files

inline void _RandomInit(int s) { // initialises the random generator
	seedUsed = s;
	formindPRNG.setSeed(s);
}

inline double _Random(void) { // return random double between 0 and 1
	return formindPRNG.getDouble();
};

inline int _NRandom(int Val) { // return random integer between 0 and Val
	return (int)(_Random() * Val);
}

// random distributions using the global random generator:
double prand(double lambda);
double erand(double beta);
double nrand(double mu, double sigma);
int fak(int val);

// class with random generator
extern MSRand myRand, myRand_Sidar, myRand_Thinning;

/* ------------------------------------------------------------
 The old (before 2016) version:
 ------------------------------------------------------------ */
#else

// sl2014 - general LCG random number generator
class LCGRand {
public:
	void seed(unsigned int s) {
		_seed = s;
	}

protected:
	LCGRand() : _seed(0), _a(0), _c(0), _m(2147483648) {
	} // m is period

	int rnd() {
		return (_seed = (_a * _seed + _c) % _m);
	}

	int _a, _c;
	unsigned int _m, _seed;
};

class MSRand : public LCGRand {
	static const int rmax = 32767;

public:
	MSRand() {
		_a = 214013;
		_c = 2531011;
	}

	int rnd() {
		return LCGRand::rnd() >> 16;
	} // return prn in [0,rmax]

	double drnd() {
		return rnd() / (double)rmax;
	} // return prn in [0,1]
};

extern int seedUsed; // remember seed used for the output files

void _RandomInit(int s); // initialises the random generator
int _NRandom(int Val); // return random integer between 0 and Val
double _Random(void); // return random double between 0 and 1

// random distributions using the global random generator:
double prand(double lambda);
double erand(double beta);
double nrand(double mu, double sigma);
int fak(int val);

// class with random generator
extern MSRand myRand, myRand_Sidar, myRand_Thinning;

#undef   EXTERN
#endif

#endif  /* random.h */
