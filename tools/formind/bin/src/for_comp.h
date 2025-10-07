////////////////////////////////////////////////////////////////////
//
// FORMIND � the forest model
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
// File						for_comp.h
// Description				light competition
//
///////////////////////////////////////////////////////////////////
#ifndef for_compH
#define for_compH
#include "for_global.h"
// Avoid pulling optional modules when not available
//#ifdef underconstruction
//#include "for_spat.h"
#include "for_environment.h"
//#endif

#include "for_var.h"
#include <vector>
 using namespace std;
void DoLightCompetition(void);
double CalculateI(double, int);
double CalculateIR(double, double);
void CalculateSpaceConditions(HecPointer, PlotPointer);
void CalculateSpaceConditions(HecPointer, PlotPointer, vector<float>);
void CalculateLAI(HecPointer, PlotPointer);
void CalculateLightAttenuation_new(HecPointer, PlotPointer);
#endif  //__FOR_COMP_HH
