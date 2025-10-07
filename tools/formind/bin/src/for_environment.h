////////////////////////////////////////////////////////////////////
//
// Minimal stub for environment to enable climate-only functionality
// (No external dependencies; uses existing climate reader)
//
////////////////////////////////////////////////////////////////////

#ifndef for_environmentH
#define for_environmentH

#include "for_var.h"
#include "for_misc.h"
#include "for_io.h"

struct Environment {
    bool initialized;
    Environment() : initialized(false) {}
    void DoVariableClimate();
    bool calculate_reduction_factors();
};

extern Environment environment;

// Optional init helpers used elsewhere in guarded blocks
inline void InitEnvironmentPlot(PlotPointer) {}

#endif


