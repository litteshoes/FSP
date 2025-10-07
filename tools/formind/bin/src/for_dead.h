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
// File         		for_dead.h
// Description  	              Functions related to tree mortality
//
///////////////////////////////////////////////////////////////////

#ifndef for_deadH
#define for_deadH

#include "for_global.h"
#include "for_var.h"

class formindDeath {
public:
	void DoMortality(void);
	void DoTreeDeath(int, double, TreePointer, PlotPointer);
	void DeleteTreeCohorte(void);
	void DeleteTreeCohorte(PlotPointer);
	double MoutofD(double, int);
	double MoutofDINC(double, int, double, double);
	double MafterLogging(TreePointer, PlotPointer);

private:
	void CountDeath(int, int, double, int, PlotPointer, double);
	bool SpaceLimitation(PlotPointer plot, TreePointer tree);
	void CalculateTreeMortality_new(TreePointer tree, PlotPointer plot);
	int IndividualDying(PlotPointer plot, TreePointer tree);
	void MortalityandTreeFalling(TreePointer tree, PlotPointer plot,
		 HecPointer hec);
	void DoTreeFalling(TreePointer tree, PlotPointer plot, HecPointer hec,
		 double nFall);
	void DetermineDamages(HecPointer HH, PlotPointer HP, double H, double D,
		 double AC, double crowndiameter, double crownlength);
};

extern formindDeath forDeath;
#endif //for_deadH
