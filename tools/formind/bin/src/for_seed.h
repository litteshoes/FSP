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
// File         			 for_seed.h
// Description  	       In this file are all functions related to seed dispersal,
// establishment and mortality.
//
///////////////////////////////////////////////////////////////////


#ifndef for_seedH
#define for_seedH

#include <iostream>
#include "for_var.h"
#include "random.h"

class DistanceFromDistribution {
public:
	DistanceFromDistribution() {
	};
	virtual double Get(double seedmaxdist, double crowndiameter) = 0;

	virtual ~DistanceFromDistribution() {
	};
};

class DistancefromDistribution_Linear : public DistanceFromDistribution {
	double Get(double seedmaxdist, double crowndiameter) {
		double x, y_norm;
		y_norm = _Random() * 2.0 / seedmaxdist; // y in [0,1] y_norm in [0,y2)
		x = seedmaxdist * (1 - y_norm * seedmaxdist / 2.0);
		if ((x < 0) || (x > seedmaxdist))
			x = seedmaxdist;
		return x;
	}
};

class DistancefromDistribution_Weibull : public DistanceFromDistribution {
public:
	double Get(double seedmaxdist, double crowndiameter) {
		double x, cr;
		cr = crowndiameter / 2.0;
		x = (seedmaxdist + cr) * sqrt(-log(_Random()));
		if (x < 0)
			x = seedmaxdist;
		return x;
	};
};

class DistancefromDistribution_Uniform : public DistanceFromDistribution {
public:
	double Get(double seedmaxdist, double crowndiameter) {
		return (_Random() * seedmaxdist);
	};
};

class DistancefromDistribution_Exponential : public DistanceFromDistribution {
public:
	DistancefromDistribution_Exponential(double meandistance)
		 : meandistance_(meandistance) {
	}

	double Get(double seedmaxdist, double crowndiameter) {
		double x = -meandistance_ * log(_Random());
		if (x > seedmaxdist)
			x = seedmaxdist;
		return x;
	}

private:
	double meandistance_;
};

class DistancefromDistribution_Gaussian : public DistanceFromDistribution {
public:
	DistancefromDistribution_Gaussian(double meandistance)
		 : meandistance_(meandistance) {
	}

	double Get(double seedmaxdist, double crowndiameter) {
		double x = nrand(meandistance_, meandistance_ / 4);
		if (x > seedmaxdist)
			x = seedmaxdist;
		if (x < 0)
			x = 0;
		return x;
	}

private:
	double meandistance_;
};

class ForSeed {
	const int PULSYEAR; // Years between 2 seedpulses (in PULS)

public:
	ForSeed();
	void Init();
	int CheckSeedPoolForSeeds(PlotPointer, int, int);
	void ReduceSeedPool(PlotPointer, int, int, int);
	long CalculateSeedlingInput(double, double, double, double, double, double);
	void ProtocolSeedlingIngrowth(TreePointer, PlotPointer);
	TreePointer ExpandTreeRecord(PlotPointer, TreePointer, int, int, int);
	int GetSeedNumber(double seedpertree);
	void DoEstablishment();
	~ForSeed();

private:
	void DeterminePulsedEstablishment();
	void DetermineGlobalSeedNumber(HecPointer hec);
	void RearangeSeedPool(PlotPointer plot);
	bool DetermineSeedGermination(PlotPointer plot);
	void DetermineSeedGermination_standard(PlotPointer plot);
	void InitSeedTreePlot(PlotPointer plot);
	void ThrowSeeds(PlotPointer plot);
	int ThrowGlobalSeeds2(HecPointer hec, PlotPointer plot, int pft,
		 int globalseed);
	void SeedPoolMortality(PlotPointer plot);
	void DistributeSeeds2TargetPlots_randomly(TreePointer mothertree,
		 double seeds);
	void DistributeSeeds_clumped(TreePointer mothertree, double treex,
		 double treey, double maxdistance, double meandistance, int seed_i,
		 double crowndiameter);
	void DistributeSeeds_single(TreePointer mothertree, double treex,
		 double treey, double maxdistance, double meandistance, int seed_i,
		 double crowndiameter);
	std::vector<DistanceFromDistribution*>distance_from_distribution;

	int PULSCOUNT;
	bool PULS;
};

extern ForSeed for_seed;

#endif
