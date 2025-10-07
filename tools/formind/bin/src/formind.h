
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
// File						formind.h
// Description				initializes and combines all processes of formind
//
///////////////////////////////////////////////////////////////////

#ifndef formindH
#define formindH

#include "for_var.h"
#include "json.hpp"

class Formind {
public:
	Formind();
	~Formind();
	void ReadCommandLineParams(int argc, char*argv[], bool doNotAsk);
	void ReadCommandLineParams(int argc, char*argv[]);
	int Start();
	bool Step(); // run one timestep
	bool Run(); // run many timesteps
	void PrintInfo();
	int WriteResults();

	bool UseLidar() {
		return (myResultFileSwitch.lidarpc && N_Par.LidarPostProcessing);
	};
	bool InitSimulation(); // initialyze formind core. Is called by Start().
	void InitEachYear();
	bool PlotLoop_modular();
	void WriteBlenderJson();
	
private:
	bool FreeMemory();
	void CreateOutputArrays();
	bool FreeOutputArrays();
	void InitEnvironment();
	bool InitPlot(PlotPointer plot);
	void InitConst();
	bool InitTrees(PlotPointer plot);
};

bool MakeOutput(int where);

#endif  // __FORMIND_H
