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
// File						for_carbon.h
// Description				calculates carbon fluxes on plot level
//
///////////////////////////////////////////////////////////////////

#ifndef for_carbonH
#define for_carbonH

#include "for_var.h"

bool CalculateCarbonFlux(void);
void InitCarbonPlot(PlotPointer);

#endif  // for_carbonH
