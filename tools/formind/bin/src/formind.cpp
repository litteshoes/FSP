////////////////////////////////////////////////////////////////////
//
// FORMIND – the forest model
// Contact: info@formind.org
// http://www.formind.org/
//
// Author: FORMIND model developer team
// Maintainer: FORMIND model developer team
// Copyright: Helmholtz Centre for Environmental Research - UFZ
// and FORMIND model developer team.
// License: AGPL (>= 3)
//
///////////////////////////////////////////////////////////////////
//
// File						formind.cpp
// Description				initializes and combines all processes of formind
//
///////////////////////////////////////////////////////////////////

#include <fstream> // 用于文件写入
#include <cmath>   // 用于 sqrt 和 M_PI
#include "json.hpp"
#include "formind.h"
#include "for_iommparset.h"

// Minimal environment is always available for climate-only mode
#include "for_environment.h"
#include "for_grow.h"
#include "for_comp.h"
#include "for_res.h"

#include "for_misc.h"
#include "for_dead.h"
#include "for_io.h"
#include "for_carbon.h"
#include "for_seed.h"
#include <cstring>
#include <string>
#include "MMParSet/MMErrorMessage.h"
extern forFileNames fileNames; 

using namespace std;

/* !
 \brief   constructor
 */
Formind::Formind() {

}

// ----------------------------------------------------------------

/* !
 \brief    destructor
 */
Formind::~Formind() {
	FreeMemory();
}

// ----------------------------------------------------------------

/* !
 \brief     prints information about the simulation (e.g. from *.par file) into
 the console
 \param     void
 \return    void
 */
void Formind::PrintInfo() {
	string datestamp(__DATE__); // date of compilation
	string timestamp(__TIME__); // time of compilation
	time_t tmpCurrentTime;
	time(&tmpCurrentTime);
	cout << "=================== Start Simulation======================="
		 << endl;
#ifdef underconstruction
	cout << "FORMIND Version: " << FORMIND_VERSION_REVISION << "   Date: " <<
		 datestamp << ", Time: " << timestamp << endl;
	cout << "==========================================================="
		 << endl;
#endif
	cout << "              time now: " << ctime(&tmpCurrentTime);
	cout << "======================= Switches ============================="
		 << endl;
	cout << "Closed boundary conditions:           " <<
		 N_Par.Closed_boundary << endl;
	cout << "Regional seed ingrowth:               " <<
		 N_Par.Globalseeds << endl;
	cout << "Local seed dispersal by mother trees: " << N_Par.Seedtree << endl;
	cout << "Pulsed ingrowth of seeds:             " << N_Par.Puls << endl;
	cout << "Density regulation of seeds:          " << N_Par.Densityreg << endl;
	cout << "Treefalling & damages:                " << N_Par.Treefall << endl;
	cout << "No growth of trees:                   " << N_Par.Nogrow << endl;
	cout << "Space limitation:                     " <<
		 N_Par.Spacelimitation << endl;
	cout << "Background tree mortality:            " <<
		 N_Par.Flag_BackgroundMortality << endl;
	cout << "Diameter-dependent tree mortality:    " <<
		 N_Par.Flag_DbhMortality << endl;
	cout << "Diameter-increment-dependent tree mortality: " <<
		 N_Par.Flag_DincMortality << endl;

#ifdef underconstruction
	cout << "Advanced Climate: Water               " << N_Par.Water_ON << endl;
	cout << "Advanced Climate: Temperature         " <<
		 N_Par.Temperature_ON << endl;
	cout << "Advanced Climate: variable Irradiance " <<
		 N_Par.variable_Irradiance_ON << endl;
	cout << "Advanced Climate: Daylength           " <<
		 N_Par.Daylength_ON << endl;
	cout << "Advanced Climate: Vegetation Periode  " <<
		 N_Par.Veg_period_ON << endl;
	cout << "Advanced Climate: CO2 (2=simpel,1=complex) " <<
		 N_Par.CO2_dependency << endl;
	cout << "Seed predation mortality:             " << N_Par.Predmort << endl;
	cout << "Fragmentation Effects:                " <<
		 N_Par.Fragmentation << endl;
	cout << "High mortality for big trees (Frag.): " <<
		 N_Par.Frag_highmortbigtree << endl;
	cout << "Low seeds (Frag.):                    " <<
		 N_Par.Frag_lowseed << endl;
	cout << "Temperature effects (Frag.):          " <<
		 N_Par.Frag_TemperatureEffects << endl;
	cout << " Spin up Time:                        " << SpinupTime << endl;
	cout << "Fire Module:                          " << N_Par.Fire << endl;
	cout << "Grassmind:                            " << N_Par.GRASSMIND << endl;
	cout << "CENTURY coupling:                     " << N_Par.Century_ON << endl;
	cout << "CANDY coupling:                       " << N_Par.Candy_ON << endl;
	cout << "Trait Distributions:                  " <<
		 N_Par.TraitDist_ON << endl;
	cout << "Spatial explicit:                     " << N_Par.spatial << endl;
	cout << "Species Number Code:                  " <<
		 N_Par.Flag_SpeciesNumber << endl;
	cout << "Logging:                              " << Logging.DoIt << endl;
	cout << "Thinning:                             " << Logging.Thinning << endl;
	cout << "Liana:                             " << N_Par.Liana << endl;
#endif
	cout << "================== Simulation Info ========================"
		 << endl;
	cout << " Time step:                           " << T.D << endl;
	cout << " Time end:                            " << T.End << endl;
	cout << " Time step of output:                 " << T.Out << endl;
	cout << " Threshold diameter for output:       " << Switch.Schwelle << endl;
	cout << " Light climate: vertical discretisation [m]: " <<
		 Switch.DLYR << endl;
	cout << " Simulated area [hectare]:            " << SQR(Switch.Ha) *
		 (Switch.Hectare / 10000.0) << endl;
	cout << " Number of plots per hectare:         " << Switch.Maxplot << endl;
	cout << " Stochasticity:                       ";
	cout.precision(32);
	if (N_Par.random_seed_ON)
		cout << "ON. Seed: " << seedUsed;
	else
		cout << "OFF. Seed: " << seedUsed;
	cout << endl;
	cout.precision(6);
}

// ----------------------------------------------------------------

/* !
 \brief		Creates empty output arrays and vectors
 \details   arrays either per stem diameter class or PFT,
 included in Formind::Start()
 */
void Formind::CreateOutputArrays() {
	int maxclass = (int) floor(DMAX / (float)N_Par.Div_DiaClassWidth + 1.0);

	Out.DIAandGRP_new = new double*[maxclass];
	Out.TH_DIAandGRP_new = new double*[maxclass];

	Out.BADIAandGRP_new = new double*[maxclass];
	Out.TH_BADIAandGRP_new = new double*[maxclass];

	Out.DIASUM = new double[maxclass];
	Out.TH_DIASUM = new double[maxclass];

	Out.DIADEATH = new double[maxclass];
	Out.DIADEATH_PFT = new double*[maxclass];

	Out.BADIASUM_new = new double[maxclass];
	Out.TH_BADIASUM_new = new double[maxclass];

	Out.SVandGRP = new double[MAXGRP];
	Out.BVandGRP = new double[MAXGRP];
	Out.BAandGRP = new double[MAXGRP];
	Out.NGRP = new double[MAXGRP];
	Out.BTandGRP = new double[MAXGRP];
	Out.TH_SVandGRP = new double[MAXGRP];
	Out.TH_BVandGRP = new double[MAXGRP];
	Out.TH_BAandGRP = new double[MAXGRP];
	Out.TH_NGRP = new double[MAXGRP];
	Out.TH_BTandGRP = new double[MAXGRP];

	for (int i = 0; i < maxclass; i++) {
		Out.DIAandGRP_new[i] = new double[MAXGRP];
		Out.DIADEATH_PFT[i] = new double[MAXGRP];
		Out.TH_DIAandGRP_new[i] = new double[MAXGRP];
		Out.BADIAandGRP_new[i] = new double[MAXGRP];
		Out.TH_BADIAandGRP_new[i] = new double[MAXGRP];
	}

	Out.DIAFALL.resize(maxclass);
	Out.DIAFALL.assign(maxclass, 0);
	Out.SVDIASUM.resize(maxclass);
	Out.GROWTH_COUNTER_SUM.resize(maxclass);
	Out.TH_GROWTH_COUNTER_SUM.resize(maxclass);
	Out.GROWTHSUM.resize(maxclass);
	Out.TH_GROWTHSUM.resize(maxclass);

	Out.GROWTH_COUNTER_andGRP.resize(maxclass);
	Out.TH_GROWTH_COUNTER_andGRP.resize(maxclass);
	Out.GROWTHandGRP.resize(maxclass);
	Out.TH_GROWTHandGRP.resize(maxclass);
	for (int i = 0; i < maxclass; i++) {
		Out.GROWTH_COUNTER_andGRP[i].resize(MAXGRP);
		Out.TH_GROWTH_COUNTER_andGRP[i].resize(MAXGRP);
		Out.GROWTHandGRP[i].resize(MAXGRP);
		Out.TH_GROWTHandGRP[i].resize(MAXGRP);
	}

}

// ----------------------------------------------------------------

/* !
 \brief		Deletes values in output arrays and vectors
 \details   included in FreeMemory()
 \return		bool true if no error occurs
 */
bool Formind::FreeOutputArrays() {
	int maxclass = (int) floor(DMAX / (float)N_Par.Div_DiaClassWidth + 1.0);

	for (int i = 0; i < maxclass; i++) {
		delete[]Out.DIAandGRP_new[i];
		delete[]Out.DIADEATH_PFT[i];
		delete[]Out.TH_DIAandGRP_new[i];

		delete[]Out.BADIAandGRP_new[i];
		delete[]Out.TH_BADIAandGRP_new[i];
		delete[]diagrp[i];
	}

	delete[]Out.DIAandGRP_new;
	delete[]Out.DIADEATH_PFT;
	delete[]Out.TH_DIAandGRP_new;

	delete[]Out.BADIAandGRP_new;
	delete[]Out.TH_BADIAandGRP_new;

	delete[]Out.DIASUM;
	delete[]Out.TH_DIASUM;

	delete[]Out.DIADEATH;

	delete[]Out.BADIASUM_new;
	delete[]Out.TH_BADIASUM_new;

	delete[]Out.SVandGRP;
	delete[]Out.BVandGRP;
	delete[]Out.BAandGRP;
	delete[]Out.NGRP;
	delete[]Out.BTandGRP;
	delete[]Out.TH_SVandGRP;
	delete[]Out.TH_BVandGRP;
	delete[]Out.TH_BAandGRP;
	delete[]Out.TH_NGRP;
	delete[]Out.TH_BTandGRP;

	delete[]diadd;
	delete[]diagrp;

	return true;
}

// ----------------------------------------------------------------

/* !
 \brief    deletes values of every tree, plot and hectar and output arrays
 \return	  bool true if no error occurs
 */
bool Formind::FreeMemory() {

	int maxclass = (int) floor(DMAX / (float)N_Par.Div_DiaClassWidth + 1.0);
	PlotPointer helpplot, plot;
	TreePointer tree, helptree;
	HecPointer hec, hecnext;

	hec = FirstHec;
	while (hec != NULL) {
		hecnext = hec->next;
		plot = hec->FirstPlot;
		while (plot != NULL) {
			helpplot = plot->next;
			tree = plot->FirstTree;
			while (tree != NULL) {
				helptree = tree->next;
				delete tree;
				tree = helptree;
			}

			for (int i = 0; i < maxclass; i++) {
				delete[]plot->DiaGrp[i];
			}
			delete[]plot->DiaSum;
			delete[]plot->DiaGrp;

			delete plot;
			plot = helpplot;
		}

		delete hec;
		hec = hecnext;
	}
	if (!FreeOutputArrays())
		goto E_EXIT;
	return true;
E_EXIT:
	return false;
}

// ----------------------------------------------------------------

/* !
 \brief    Input of daily climate variability and GRASSMIND management
 */
void Formind::InitEnvironment() {

	if (N_Par.variable_Irradiance_ON || N_Par.Water_ON || N_Par.Temperature_ON ||
		 N_Par.Veg_period_ON || N_Par.Daylength_ON || (N_Par.CO2_dependency > 0))
	{
		try {
			environment.DoVariableClimate();
		} catch (...) {
			std::cerr << "ERROR: environment.DoVariableClimate() threw exception. "
					  << "Check Climate_File path and file contents."
					  << std::endl;
		}
	}
}

// ----------------------------------------------------------------

/* !
 \brief    initializes FORMIND simulation
 \details  defines number of PFTs, defines starting point of random number generator,
 imports *.pin file, creates empty output arrays
 \return	  bool true if no error occurs
 */
int Formind::Start() {

	MAXGRP = N_Par.Div_MAXGRP;

	if (N_Par.random_seed_ON)
		_RandomInit(time(NULL));
	else
		_RandomInit(RandomInit);

	// Do initialization from pin file
	// Exclude the following two cases:
	// 1) initialization should be done from inv file (tree list input) or
	// 2) no initialization is desired (no PinFileNameX in par file)
	if ((!N_Par.TreeListInput) || (PinFileNameX != "")) {
		strcpy(PinFileName, fileNames.getPinFileNameAbsolutePath().c_str());
		PINFileReader PINReader(false);
		// Trow and Error if a PinFileNameX is given but the file does not exist
		if (!PINReader.readPINFile(PinFileName))
			goto E_EXIT;
	}

	if (!Result.InitFile())
		goto E_EXIT;

#ifdef underconstruction
	if (N_Par.Landslide) {
		InitLandslide();
	}

	if (Logging.DoIt) {
		InitLogging();
		Result.LastLogInit();
	}

	if (Logging.Thinning)
		InitThinning();
#endif

	if (!InitSimulation())
		goto E_EXIT;

	CreateOutputArrays();

	InitEnvironment();

EXIT:
	return 0;
E_EXIT:
	cerr << "ERROR: undefined error \tfile: " << __FILE__ <<
		 "\tfunction: InitComplete\t line:" << __LINE__ <<
		 "\t Initialization error." << endl;
	return 1;
}

// ----------------------------------------------------------------

/* !
 \brief		writes additional results and saves corresponding parameter file
 \return		bool true if no error occurs
 */
int Formind::WriteResults() {

#ifdef underconstruction
	if (Logging.DoIt)
		WriteLogging();
#endif

	Result.Close();

	if (myResultFileSwitch.save_parameter_files) {
		Result.Save_parameters();
	}

EXIT:
	return 0;
E_EXIT:
	cerr << "ERROR: undefined error \tfile: " << __FILE__ <<
		 "\tfunction: Results_1\t line:" << __LINE__ <<
		 "\t undefined error." << endl;
	return 1;
}

// ----------------------------------------------------------------

/* !
 \brief		initializes constant model parameters
 \details   set initial values of several parameters, e.g. starting time,
 maximal height, dead biomass; included in InitSimulation()
 */
void Formind::InitConst() {
	double h, hmax;

	MAXLAI = 3;
	Logging.Done = 0.0;
	BINC[0] = 0;
	BINC[1] = 0;
	AREASUM = 0.0;
	T.T = T.Start;
	T.FileOut = 0.0;
	HMAX = 0.0;
	DMAX = 0.0;
	Out.GAPSUM = 0;
	Out.GAPNO = 0;
	Out.GAPDAMAGE = 0;
	Out.TH_GAPDAMAGE = 0;

	for (int i = 0; i < 9; i++) {
		Out.DEATHCUM[i] = 0;
		Out.TH_DEATHCUM[i] = 0;
		Out.BiomassDEATHCUM[i] = 0;
		Out.TH_BiomassDEATHCUM[i] = 0;
		Out.DEATHCUMrate[i] = 0;
		Out.TH_DEATHCUMrate[i] = 0;
		Out.BiomassDEATHCUMrate[i] = 0;
		Out.TH_BiomassDEATHCUMrate[i] = 0;
	}
	hmax = 0.0;
	for (int pft = 0; pft < MAXGRP; pft++) {

		if (N_Par.GRASSMIND)
			h = N_Par.Est_DS;
		else
			h = forGrow.HoutofDFunc->Calculate(N_Par.Est_DS, pft);

		if (hmax < h)
			hmax = h;
	}
	SAEMLINGSKRONE = (int)floor(hmax / Switch.DLYR);

	for (int pft = 0; pft < MAXGRP; pft++) {
		if (N_Par.Geo_FUNCTION_7[6] == 0) {
			if (HMAX < N_Par.Geo_HMmean_5[pft])
				HMAX = N_Par.Geo_HMmean_5[pft];

			if (N_Par.Geo_FUNCTION_7[0] == 1 && N_Par.Geo_HMmean_5[pft] >
				 N_Par.Geo_HD_39[1][pft]) {
				std::string message = "Geo_HMmean_5 (maximal height) of PFT" +
					 std::to_string(pft + 1) +
					 " is larger than h1 of Par.Geo_HD_39 of the same PFT.\n";
				MMErrorMessage(message, N_Par.ErrorType);
			}
			if (N_Par.Geo_FUNCTION_7[0] == 3 && N_Par.Geo_HMmean_5[pft] >
				 N_Par.Geo_HD_39[0][pft]) {
				std::string message = "Geo_HMmean_5 (maximal height) of PFT" +
					 std::to_string(pft + 1) +
					 " is larger than h0 of Par.Geo_HD_39 of the same PFT.\n";
				MMErrorMessage(message, N_Par.ErrorType);
			}

			if (DMAX < forGrow.DoutofHFunc->Calculate
				 (N_Par.Geo_HMmean_5[pft], pft))
				DMAX = forGrow.DoutofHFunc->Calculate(N_Par.Geo_HMmean_5[pft], pft);
		}
		else {
			const double dtmp = forGrow.HoutofDFunc->Calculate
				 (N_Par.Geo_HMmean_5[pft], pft);
			if (HMAX < dtmp)
				HMAX = dtmp;
			if (DMAX < N_Par.Geo_HMmean_5[pft])
				DMAX = N_Par.Geo_HMmean_5[pft];
		}
	}

	HIGHESTLAYERNUMBER = (int)ceil(HMAX / Switch.DLYR);

}

// -----------------------------------------------------------

/* !
 \brief     Initialization of plots
 \return		bool true if no error occurs
 */
bool Formind::InitPlot(PlotPointer plot) {
	plot->StemNumber = 0;
	int maxclass = (int) floor(DMAX / (float)N_Par.Div_DiaClassWidth + 1.0);
	plot->DiaSum = new double[maxclass];
	plot->DiaGrp = new double*[maxclass];
	for (int j = 0; j < maxclass; j++) {
		plot->DiaGrp[j] = new double[MAXGRP];
	}

	plot->Area = (plot->LocXH - plot->LocXL) * (plot->LocYH - plot->LocYL);
	if (plot->landcode > 0)
		AREASUM += plot->Area;

	if (!ForTools::FindNeighbours(plot))
		goto E_EXIT;
	if (!ForTools::FindEightNeighbours(plot))
		goto E_EXIT;

	InitCarbonPlot(plot);

	plot->IRFloor = 0.0;
	plot->Plotmax = 0.0;
	for (int zz = 0; zz <= MAXLYR; zz++) {
		plot->ATSum[zz] = 0.0;
		plot->ATold[zz] = 0.0;
		plot->LayerReductionFac[zz] = 1.0;
		plot->LAI[zz] = 0.0;
		plot->LAIK[zz] = 0.0;
	}

	plot->PCTCA = 0.0;

	// Keep optional modules disabled except environment
	if (N_Par.variable_Irradiance_ON || N_Par.Water_ON || N_Par.Temperature_ON ||
		 N_Par.Veg_period_ON || N_Par.Daylength_ON || (N_Par.CO2_dependency > 0))
		InitEnvironmentPlot(plot);

	return true;

E_EXIT:
	cerr << "ERROR: undefined error \tfile: " << __FILE__ <<
		 "\tfunction: InitPlot\t line:" << __LINE__ <<
		 "\t undefined error." << endl;
	return false;
}

// -----------------------------------------------------------

/* !
 \brief     Initialization of the trees
 \details   as described in *.pin file, distinction between commercial and
 noncommercial trees
 \return		bool true if no error occurs
 */
bool Formind::InitTrees(PlotPointer plot) {

	if (!N_Par.TraitDist_ON) {
		double dice;
		TreePointer ntree, tree;
		plot->FirstTree = NULL;
		ntree = NULL;

		for (int dd = MAXDD - 1; dd >= 0; dd--) {
			for (int pft = 0; pft < MAXGRP; pft++) {
				int N0 = (int)plot->N0[pft][dd];
				if (N0 > 0) { // PIN file: tree exists
					if (std::atof(DCLASS[dd].c_str()) > DM[pft]) {
						std::string message =
							 "DBH of initialized tree in .pin file is larger than the maximum possible defined value in .par file (N_Par.Geo_HMmean_5): PFT" +
							 std::to_string(pft + 1) +
							 ", maximum possible diameter: " + std::to_string
							 (DM[pft]) + " m";
						MMErrorMessage(message, N_Par.ErrorType);
					}

					if ((std::atof(DCLASS[dd].c_str()) < N_Par.Est_DS)
						 && N_Par.Div_Liana[pft] == 0)
					{ // allow smaller DBH for Lianas
						std::string message =
							 "DBH of initialized tree in .pin file is smaller than the minimum DBH of seedlings in .par file (N_Par.Est_DS):" +
							 std::to_string(N_Par.Est_DS) + " m";
						MMErrorMessage(message, N_Par.ErrorType);
					}

					int trees_com = (int)(N0 * N_Par.Div_COMMERCIAL_A[pft]);
					// number commercial trees
					int trees_noncom = (int)(N0 * (1 - N_Par.Div_COMMERCIAL_A[pft]));
					// number noncommercial trees
					int rest = N0 - trees_com - trees_noncom;

					if (rest > 0) {
						dice = _Random();
						if (dice < N_Par.Div_COMMERCIAL_A[pft])
							trees_com = trees_com + rest;
						else
							trees_noncom = trees_noncom + rest;
					}

					if (trees_com > 0) {
						if (N_Par.Cohort) {
							tree = new TREE(pft, std::atof(DCLASS[dd].c_str()),
								 trees_com, plot, 1, N_Par.Div_Liana[pft]);
							if (plot->FirstTree == NULL)
								plot->FirstTree = tree;
							else
								ntree->next = tree;
							ntree = tree;
							ntree->next = NULL;
						}
						else {
							for (int j = 0; j < trees_com; j++) {
								tree = new TREE(pft, std::atof(DCLASS[dd].c_str()), 1,
									 plot, 1, N_Par.Div_Liana[pft]);
								if (plot->FirstTree == NULL)
									plot->FirstTree = tree;
								else
									ntree->next = tree;
								ntree = tree;
								ntree->next = NULL;
							}
						}

					}

					if (trees_noncom > 0) {
						if (N_Par.Cohort) {
							tree = new TREE(pft, std::atof(DCLASS[dd].c_str()),
								 trees_noncom, plot, 0, N_Par.Div_Liana[pft]);
							if (plot->FirstTree == NULL)
								plot->FirstTree = tree;
							else
								ntree->next = tree;
							ntree = tree;
							ntree->next = NULL;
						}
						else {
							for (int j = 0; j < trees_noncom; j++) {
								tree = new TREE(pft, std::atof(DCLASS[dd].c_str()), 1,
									 plot, 1, N_Par.Div_Liana[pft]);
								if (plot->FirstTree == NULL)
									plot->FirstTree = tree;
								else
									ntree->next = tree;
								ntree = tree;
								ntree->next = NULL;
							}
						}

					}

					if (tree == NULL) {
						cerr << "ERROR: system error \tfile: " << __FILE__ <<
							 "\tfunction: InitTrees\t line:" << __LINE__ <<
							 "\t not enough memory for a tree." << endl;
						goto E_EXIT;
					}
				} // if (plot->N0[pft][dd] > 0)
			} // for(pft=0; pft<MAXGRP; pft++)
		} // for(dd=MAXDD; dd>=0; dd--)
	}
#ifdef underconstruction
	else {
		InitTreesTraitDist(plot);
	}
#endif
	return true;

E_EXIT:
	return false;
} // InitTrees

// -----------------------------------------------------------

/* !
 \brief     Initializes Simulation
 \details   defines start parameter and allometries, initializes trees per plot
 and hectar, defines starting time of result files
 \return		bool true if no error occurs
 */
bool Formind::InitSimulation() {
	PlotPointer plot;
	HecPointer hec;

	forGrow.InitDB();
	forGrow.InitDH();
	forGrow.InitMaximumDiameter();
	if (N_Par.Geo_FUNCTION_7[0] == 3 || N_Par.Geo_FUNCTION_7[0] == 0)
		forGrow.InitLookupVectors();

	if (!N_Par.GRASSMIND)
		forGrow.InitMaintenanceRespiration();
	InitConst();

	forGrow.InitA4();

	if (N_Par.Geo_FUNCTION_7[0] != 2 && N_Par.Geo_FUNCTION_7[3] != 1)
		forGrow.InitB2D();

#ifdef underconstruction
	if (N_Par.Fire)
		InitFireAttributes();
#endif

	for_seed.Init();

	// hec - plot - tree initialisation =======================================

#ifdef underconstruction
	// Create an empty forest, which is required for initialization without pin file, i.e., in the following two cases:
	// 1) initialization should be done from inv file (tree list input) or
	// 2) no initialization is desired (no PinFileNameX in par file)
	if (N_Par.TreeListInput || (PinFileNameX == "")) {
		CreateEmptyForest();
	}
#endif

	int maxclass = (int) floor(DMAX / (float)N_Par.Div_DiaClassWidth + 1.0);
	diadd = new double[maxclass];
	diagrp = new double*[maxclass];
	for (int i = 0; i < maxclass; i++) {
		diagrp[i] = new double[MAXGRP];
	}

	hec = FirstHec;
	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {

			if (!InitPlot(plot))
				goto E_EXIT;
			if(!N_Par.TreeListInput)
				if (!InitTrees(plot))
					goto E_EXIT;

			Out.PlotCorner[hec->HecNo - 1][plot->No - 1][0] = plot->LocXL;
			Out.PlotCorner[hec->HecNo - 1][plot->No - 1][1] = plot->LocXH;
			Out.PlotCorner[hec->HecNo - 1][plot->No - 1][2] = plot->LocYL;
			Out.PlotCorner[hec->HecNo - 1][plot->No - 1][3] = plot->LocYH;
			plot = plot->next;
		}
		hec = hec->next;
	}

#ifdef underconstruction
	if (N_Par.TreeListInput) {
		ReadTreeList();
	}

	if (N_Par.InitPools) {
		ReadInitPools();
	}

	if (N_Par.spatial)
		InitSpatial();
#endif

	NextTimeOut = OutputStart;

	if (myResultFileSwitch.res || myResultFileSwitch.res_th ||
		 myResultFileSwitch.res_th || myResultFileSwitch.cohort ||
		 myResultFileSwitch.cohort_th) {
		TimeTreeListOut = N_Par.TreeListOutputStart;
	}

#ifdef underconstruction
	InitLidarOutput();

	if (N_Par.Flag_SpeciesNumber) {
		InitSpecies();
	}
#endif

	return true;

E_EXIT:
	cerr << "ERROR: undefined error \tfile: " << __FILE__ <<
		 "\tfunction: InitSim\t line:" << __LINE__ <<
		 "\t undefined error." << endl;
	return false;
} // InitSim

// -----------------------------------------------------------

/* !
 \brief     Initializes each year
 \details   sets stochastic processes to 0, e.g. death rate, seed rain,
 DBH dependend death rate per PFT
 */
void Formind::InitEachYear() {
	Out.DEATH = 0.0;
	Out.TH_DEATH = 0.0;
	Out.SPACEDEATH = 0.0;
	Out.TH_SPACEDEATH = 0.0;
	Out.InTH = 0.0;
	Out.SEEDLINGSUM = 0.0;
	Out.SEEDRAINSUM = 0.0;
	Out.SEEDRAINandCOM[0] = 0.0;
	Out.SEEDRAINandCOM[1] = 0.0;
	MAXARRAY = (int) floor(DMAX / (float)N_Par.Div_DiaClassWidth + 1.0);

	for (int j = 0; j < MAXARRAY; j++) {
		for (int pft = 0; pft < MAXGRP; pft++) {
			Out.InTH_GRP[pft] = 0.0;
			Out.SEEDLINGandGRP[pft] = 0.0;
			Out.SEEDRAINandGRP[pft] = 0.0;
			Out.DIADEATH_PFT[j][pft] = 0.0;
			Out.DIADEATH[j] = 0.0;
		}
	}

}

// -----------------------------------------------------------

/* !
 \brief     Loops over all subplots
 \details	distinguishes between modules
 looping over plots is done seperately in each subroutine:
 Establishment, mortality, light competition, growth of trees;
 included in Step()
 \return		bool true if no error occurs
 */
bool Formind::PlotLoop_modular() {
	for_seed.DoEstablishment();

#ifdef underconstruction
	if (N_Par.Liana)
		DoLiana();
#endif

	forDeath.DoMortality();

	DoLightCompetition();

#ifdef underconstruction
	if (N_Par.spatial) {
		CalculateCellVector();
	}
#endif

	forGrow.DoGrowth();

	// Environment outputs
	if (N_Par.variable_Irradiance_ON || N_Par.Temperature_ON ||
		 N_Par.Veg_period_ON || N_Par.Daylength_ON || (N_Par.CO2_dependency > 0))
	{
		if (myResultFileSwitch.env)
			Result.WriteEnvironmentOutput();
	}

	return true;

E_EXIT:
	return false;
} // PlotLoop

// -----------------------------------------------------------

/* !
 \brief     Calculates and writes output data
 \details   Calculates and writes output data of forest attributes on different
 scales, e.g. AGB, BA, SN;
 included in Step()
 \param     int where
 \return		bool true if no error occurs
 */
// 替换掉旧的 MakeOutput 函数
bool MakeOutput(int where) {

    // --- 原有的 MakeOutput 逻辑 ---
    CalculateStandAverages();
    CalculatePlotOutput();
    CalculateOutput();
	
    if (myResultFileSwitch.blender_json) {
		// vvvvvvvv 我们的 Blender JSON 输出逻辑 vvvvvvvv
		{ // 使用大括号创建一个独立的作用域
			nlohmann::json scene_data;
			scene_data["year"] = (int)T.T;
			nlohmann::json trees_array = nlohmann::json::array();
			int total_trees = 0;

			HecPointer current_hec = FirstHec; 
			while (current_hec != NULL) {
				PlotPointer current_plot = current_hec->FirstPlot;
				while (current_plot != NULL) {
					TreePointer current_tree = current_plot->FirstTree;
					while (current_tree != NULL) {
						nlohmann::json tree_json;
						if (!current_tree->IdVector.empty()) {
							tree_json["tree_id"] = std::to_string(current_tree->IdVector[0]);
						} else {
							tree_json["tree_id"] = "N/A";
						}
						tree_json["x"] = current_tree->absX();
						tree_json["y"] = current_tree->absY();
						tree_json["height"] = current_tree->H;
						tree_json["dbh"] = current_tree->D;
						const double PI = 3.14159265358979323846;
						tree_json["crown_radius"] = std::sqrt(current_tree->AC / PI);
						tree_json["species"] = std::to_string(current_tree->Grp);
						tree_json["status"] = "alive";
						trees_array.push_back(tree_json);
						total_trees++;
						current_tree = current_tree->next;
					}
					current_plot = current_plot->next;
				}
				current_hec = current_hec->next;
			}

			scene_data["total_trees"] = total_trees;
			scene_data["trees"] = trees_array;

			// 这里需要想办法获取结果目录的路径。
			// fileNames 是一个全局对象，但可能在这里不可见。
			// 先用一个相对路径来测试。
			std::string result_dir = fileNames.getResultDirAbsolutePath();
			// 在 Windows 上，路径分隔符可能是 `\`，我们需要确保它是 `/`
			for (char &c : result_dir) {
				if (c == '\\') {
					c = '/';
				}
			}
			// 确保目录路径以 `/` 结尾
			if (result_dir.back() != '/') {
				result_dir += '/';
			}

			std::string filename = result_dir + "forest_state_" + std::to_string((int)T.T) + ".json";

			std::ofstream out_file(filename);
			if (out_file.is_open()) {
				out_file << scene_data.dump(2);
				out_file.close();
			} else {
				std::cerr << "ERROR: Could not open file for writing: " << filename << std::endl;
        	}
    	}
    	// ^^^^^^^^ 我们的 Blender JSON 输出逻辑结束 ^^^^^^^^^
	}
    // --- 原有的 MakeOutput 逻辑 ---
    Result.Save(false, where);

    return true;

E_EXIT: // 标签 E_EXIT 仍然有效
    cerr << "ERROR: undefined error \tfile: " << __FILE__ <<
        "\tfunction: MakeOutput\t line:" << __LINE__ <<
        "\t undefined error." << endl;
    return false;
}

// -----------------------------------------------------------

/* !
 \brief		Loop over all timesteps
 \details 	Calls the step function iteratively.
 \return		bool true if no error occurs
 */
bool Formind::Run() {
	TimeEnd = T.End;
	while (T.T < (T.End + T.D / 2.0)) {
		bool stepOK = Step();
		if (!stepOK) {
			cerr << "ERROR: undefined error \tfile: " << __FILE__ <<
				 "\tfunction: TimeLoop\t line:" << __LINE__ <<
				 "\t TimeLoop error." << endl;
			return false;
		}
	}

	T.End = TimeEnd;
	return true;
}

// ----------------------------------------------------------------

/* !
 \brief		runs one timestep
 \return		bool true if no error occurs
 \details    executes demographic processes (e.g. growth, death, reproduction,
 competition), calculates carbon fluxes, makes output
 */
bool Formind::Step() {

	if (T.BeginOfYear(N_Par.GRASSMIND))
		InitEachYear();

	if (N_Par.variable_Irradiance_ON || N_Par.Temperature_ON ||
		 N_Par.Veg_period_ON || N_Par.Daylength_ON || (N_Par.CO2_dependency > 0))
	{
		if (!environment.calculate_reduction_factors()) {
			std::cerr << "ERROR: environment.calculate_reduction_factors() failed. "
					  << "Check Climate_File path, working directory, and climate file format (rain, temp, irradiance, day_length, PET, CO2)."
					  << std::endl;
			goto E_EXIT;
		}
	}

	if (!PlotLoop_modular())
		goto E_EXIT;

#ifdef underconstruction
	if (Logging.DoIt)
		DoLogging();

	if (Logging.Thinning)
		DoThinning();

	if (N_Par.GRASSMIND)
		Logging.isMowedrightnow = false;
	DoGrassMowing();

	if (N_Par.Century_ON) {
		if (!CalculateCarbonFluxCent())
			goto E_EXIT;
	}
	else
#endif
	{
		if (!CalculateCarbonFlux())
			goto E_EXIT;
	}

	if (!MakeOutput(0))
		goto E_EXIT;

	::Time = T.T;
	T.T += T.D;

	if (!N_Par.StoreInitialState) {
		T.FileOut += T.D;
#ifdef underconstruction
		if (Logging.Done > 0.0)
			Logging.Done += T.D;
#endif
	}

	N_Par.StoreInitialState = false;

	return true;
E_EXIT:
	cerr << "ERROR: undefined error \tfile: " << __FILE__ <<
		 "\tfunction: TimeLoop\t line:" << __LINE__ <<
		 "\t TimeLoop error." << endl;
	return false;
}

// ----------------------------------------------------------------

/* !
 \brief     Read parameter files (*.par)
 */
void Formind::ReadCommandLineParams(int argc, char*argv[], bool doNotAsk) {

	fileNames.init(argc, argv, doNotAsk);

	forio.init();
	forio.readParFile(fileNames.getParFileNameAbsolutePath().c_str());
	forio.par.ReadCommandLine(argc, argv);
	if (myCreateDirectory(fileNames.getResultDirAbsolutePath())) {
		cerr << "WARNING: Created results folder!" << endl;
	}
}

void Formind::ReadCommandLineParams(int argc, char*argv[]) {
	ReadCommandLineParams(argc, argv, false);
}

