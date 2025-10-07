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
// File         				  for_iommparset.h
// Description                    parameter handling
//
///////////////////////////////////////////////////////////////////

#ifndef _FORIOMMPARSET_H
#define _FORIOMMPARSET_H

#include <MMParSet/MMParSet.h>
#include <MMParSet/MMPar.h>
#include <string>
#include <fstream>
#include <for_misc.h>
#include "for_var.h"   // for global N_Par

/*
 HOWTO USE:
 1. init(); // this adds all parameters to parset and initialyses with default values
 2. readParFile(somefilename); // this reads parameters from parfile

 OPTIONAL:



 HOWTO add a new parameter, eg myNewParameter:

 Short version:
 1. remember which parameter was the last before the newly defined parameter
 in	the file you defined it.
 2. Search it inside this file. Thenn below add one line:
 par.addPar(myNewParameter);

 Long verion:
 1. define it in formind - usually done in for_var. Make sure to export
 it in for_var.h (as before)
 2. in this file add one line (NEW):
 par.addPar(myNewParameter);
 The best way to find a good place where to add this line is to search
 for an existing parameter which comes from the same group/section of
 parameters the new parameter myNewParameter comes from.
 Even better if you keep the same order here as in for_var.

 ------------

 I did sort the parameters into different groups. So each group is initilized
 by its own function:
 init_N_Par();
 init_Log();
 init_Switch();
 init_myResultFileSwitch();

 If there is a parameter in a parameter file which has not been added
 here MMParSet will throw an exception. So you will know.

 */

// main simulation file variables

extern double OutputStep;
extern double Time;
extern double OutputStart;
extern double TimeEnd;
extern double SpinupTime;
extern double TimeStep;
extern int RandomInit;

class forIOMMParSet {
public:
	MMParSet par;

	// variables just used inside the .sisi file and nowhere inside formind (changed from String to string) :
	std::string SimulationName;
	std::string ModelName;
	std::string ProjectName;
	std::vector<std::string>IncludeFiles;
	std::string ts;

	int ti;

	std::string Workaround_PinFileNameX;

	//
	forIOMMParSet() {
		par.options.setPrecision(6);
		// default, it can be changed for each parameter
	}

	// Complete initialisation using default values. no file is read yet
	void init() {
		setDefaultValues();
		setGrassmindDefaultValues();
		init_par();
	}



// -----------------------------------------------------------

/* !
 \brief          Sets default values of the model
 \param	       		void
 \return	        void
 \description 		if the parameter values are not given by the *.par file, this values are used for the simulation
 */
	void setDefaultValues() {
		// no Defaults for myResultFileSwitch

		ProjectName = "";
		modelshort = "Formind";
		individuals = "trees";
		individual = "tree";
		T.Start = 0.0;
		T.End = 100.0;
		TimeEnd = 100.0;
		T.D = 1.0;
		TimeStep = 1.0;
		T.Out = 1.0;
		T.OutputStart = T.Start;
		OutputStep = 1.0;
		RandomInit = 0;

		N_Par.Cohort = 1;
		// Treelists
		N_Par.TreeListOutputStep = 1.0;
		N_Par.TreeListOutputStart = 0.0;
		N_Par.TreeListInput = 0;
		N_Par.InitPools = 0;

		// experimental
		N_Par.Patch_Seed = false;
		N_Par.Flag_SpeciesNumber = false;
		N_Par.OldVersionTest = false;
		N_Par.Patch_Heterogeneity = false;

		N_Par.ExpNotation = false;
		N_Par.Puls = false;
		N_Par.Treefall = true;
		N_Par.Nogrow = false;
		N_Par.Spacelimitation = true;
		N_Par.Globalseeds = true;
		N_Par.Closed_boundary = true;
		N_Par.Seedtree = false;
		N_Par.Densityreg = false;

		// Predation / Fragmentation
		SpinupTime = 0.0;
		N_Par.Predation = false;
		N_Par.Predmort = false;
		N_Par.Fragmentation = false;
		N_Par.Frag_highmortbigtree = false;
		N_Par.Frag_lowseed = false;
		N_Par.Edge_Distance = 0.0;
		N_Par.Frag_TemperatureEffects = false;

		// Grassmind
		N_Par.GRASSMIND = false;
		N_Par.Century_ON = false;
		N_Par.Candy_ON = false;

		// Trait Dist.
		N_Par.TraitDist_ON = false;

		// spatial
		N_Par.spatial = false;
		N_Par.spatial_area = false;
		N_Par.spatial_ZOI20 = false;

		N_Par.Mort_nbinc = false;
		N_Par.Flag_PBbuffer = true;
		N_Par.Div_MAXGRP = HYPERMAXGRP;
		N_Par.Div_MAXTREE = 10000;
		N_Par.Div_DiaClassWidth = 0.10;
		N_Par.Est_DS = 0.02;
		N_Par.HGRP_EXP = 0.0;
		N_Par.Lin_Dis = 1.0;
		N_Par.Mort_50nbinc = 0.1;
		N_Par.Mort_SpaceLimitation_Factor = 1.0;
		N_Par.Mort_SpaceLimitation_Diameter = 0.0;
		N_Par.Flag_BackgroundMortality = true;
		N_Par.Flag_DbhMortality = false;
		N_Par.Flag_DincMortality = false;
		N_Par.Flag_DD_Seed_Mortality = false;
		N_Par.Flag_DD_Seedling_Mortality = false;

		N_Par.DispersalKernel = new int[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.DispersalKernel[i] = 1;

		N_Par.Div_K = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Div_K[i] = 0.7;

		N_Par.Div_COMMERCIAL_A.assign(N_Par.Div_MAXGRP, 0);
		N_Par.Liana = 0;
		N_Par.Div_Liana.assign(N_Par.Div_MAXGRP, 0);
		// does resize and initialyze
		N_Par.Div_Animaldispersed.assign(N_Par.Div_MAXGRP, 0);
		N_Par.SpeciesNumber.assign(N_Par.Div_MAXGRP, 1);

		N_Par.Est_NS_3 = new long[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Est_NS_3[i] = 10;

		N_Par.Est_ISeed_3 = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Est_ISeed_3[i] = 0.1;

		N_Par.Est_Dist_3 = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Est_Dist_3[i] = 0;

		N_Par.Est_SeedTree_3 = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Est_SeedTree_3[i] = 0;

		N_Par.MeanSeedDistance = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.MeanSeedDistance[i] = 0.0;

		N_Par.SeedPredatorNumber = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.SeedPredatorNumber[i] = 0.0;

		N_Par.SeedPredatorEfficiency = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.SeedPredatorEfficiency[i] = 0.0;

		N_Par.SeedPredatorHandlingTime = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.SeedPredatorHandlingTime[i] = 0.0;

		N_Par.Est_SeedMort_3 = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Est_SeedMort_3[i] = 0;

		N_Par.Est_SeedSurvival_3 = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Est_SeedSurvival_3[i] = 1;

		N_Par.Est_DSTree_5 = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Est_DSTree_5[i] = 0.05;

		N_Par.Invasion = new int[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Invasion[i] = 0;

		N_Par.TimeInvasion = 0;

		N_Par.Mort_FUNCTION_2 = new long[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Mort_FUNCTION_2[i] = 1;

		N_Par.Mort_mean_19 = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Mort_mean_19[i] = 0.02;

		unsigned int RowsOfN_Par_Mort_Dia_31 = 3; // is static value
		unsigned int ColumnsOfN_Par_Mort_Dia_31 = N_Par.Div_MAXGRP;
		N_Par.Mort_Dia_31 = new double*[RowsOfN_Par_Mort_Dia_31];
		for (unsigned i = 0; i < RowsOfN_Par_Mort_Dia_31; i++)
			N_Par.Mort_Dia_31[i] = new double[ColumnsOfN_Par_Mort_Dia_31];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++) {
			N_Par.Mort_Dia_31[0][i] = 0.0;
			N_Par.Mort_Dia_31[1][i] = 0.0;
			N_Par.Mort_Dia_31[2][i] = 0.0;
		}

		unsigned int RowsOfN_Par_Mort_DD_Seed_31 = 3;
		unsigned int ColumnsOfN_Par_Mort_DD_Seed_31 = N_Par.Div_MAXGRP;
		N_Par.Mort_DD_Seed_31 = new double*[RowsOfN_Par_Mort_DD_Seed_31];
		for (unsigned i = 0; i < RowsOfN_Par_Mort_DD_Seed_31; i++)
			N_Par.Mort_DD_Seed_31[i] = new double[ColumnsOfN_Par_Mort_DD_Seed_31];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++) {
			N_Par.Mort_DD_Seed_31[0][i] = 0.0;
			N_Par.Mort_DD_Seed_31[1][i] = 0.0;
			N_Par.Mort_DD_Seed_31[2][i] = 2.0;
		}

		unsigned int RowsOfN_Par_Mort_DD_Seedling_31 = 3;
		unsigned int ColumnsOfN_Par_Mort_DD_Seedling_31 = N_Par.Div_MAXGRP;
		N_Par.Mort_DD_Seedling_31 = new double*[RowsOfN_Par_Mort_DD_Seedling_31];
		for (unsigned i = 0; i < RowsOfN_Par_Mort_DD_Seedling_31; i++)
			N_Par.Mort_DD_Seedling_31[i] =
				 new double[ColumnsOfN_Par_Mort_DD_Seedling_31];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++) {
			N_Par.Mort_DD_Seedling_31[0][i] = 0.0;
			N_Par.Mort_DD_Seedling_31[1][i] = 0.0;
			N_Par.Mort_DD_Seedling_31[2][i] = 2.0;
		}

		unsigned int RowsOfN_Par_Mort_Dinc_31 = 3; // is static value
		unsigned int ColumnsOfN_Par_Mort_Dinc_31 = N_Par.Div_MAXGRP;
		N_Par.Mort_Dinc_31 = new double*[RowsOfN_Par_Mort_Dinc_31];
		for (unsigned i = 0; i < RowsOfN_Par_Mort_Dinc_31; i++)
			N_Par.Mort_Dinc_31[i] = new double[ColumnsOfN_Par_Mort_Dinc_31];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++) {
			N_Par.Mort_Dinc_31[0][i] = 0.0;
			N_Par.Mort_Dinc_31[1][i] = 0.0;
			N_Par.Mort_Dinc_31[2][i] = 0.0;
		}

		N_Par.Mort_FallP = 0.4;

		N_Par.Geo_FUNCTION_7 = new long[9]; // is static value
		N_Par.Geo_FUNCTION_7[0] = 2;
		N_Par.Geo_FUNCTION_7[1] = 1;
		N_Par.Geo_FUNCTION_7[2] = 0;
		N_Par.Geo_FUNCTION_7[3] = 1;
		N_Par.Geo_FUNCTION_7[4] = 2;
		N_Par.Geo_FUNCTION_7[5] = 1;
		N_Par.Geo_FUNCTION_7[6] = 0;
		N_Par.Geo_FUNCTION_7[7] = 0;
		N_Par.Geo_FUNCTION_7[8] = 0;

		N_Par.Geo_HMmean_5 = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Geo_HMmean_5[i] = 30.0;



		unsigned int RowsOfN_Par_Geo_Bio2Dbh = 3; // is static value
		unsigned int ColumnsOfN_Par_Geo_Bio2Dbh = N_Par.Div_MAXGRP;
		N_Par.Geo_Bio2Dbh = new double*[RowsOfN_Par_Geo_Bio2Dbh];
		for (unsigned i = 0; i < RowsOfN_Par_Geo_Bio2Dbh; i++)
			N_Par.Geo_Bio2Dbh[i] = new double[ColumnsOfN_Par_Geo_Bio2Dbh];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++) {
			N_Par.Geo_Bio2Dbh[0][i] = 1.029;
			N_Par.Geo_Bio2Dbh[1][i] = 3.204;
			N_Par.Geo_Bio2Dbh[2][i] = 3.717;
		}

		unsigned int RowsOfN_Par_Geo_HD_39 = 3; // is static value
		unsigned int ColumnsOfN_Par_Geo_HD_39 = N_Par.Div_MAXGRP;
		N_Par.Geo_HD_39 = new double*[RowsOfN_Par_Geo_HD_39];
		for (unsigned i = 0; i < RowsOfN_Par_Geo_HD_39; i++)
			N_Par.Geo_HD_39[i] = new double[ColumnsOfN_Par_Geo_HD_39];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++) {
			N_Par.Geo_HD_39[0][i] = 3.0;
			N_Par.Geo_HD_39[1][i] = 0.6;
			N_Par.Geo_HD_39[2][i] = 0.0;
		}

		unsigned int RowsOfN_Par_Geo_LAIT_21 = 3; // is static value
		unsigned int ColumnsOfN_Par_Geo_LAIT_21 = N_Par.Div_MAXGRP;
		N_Par.Geo_LAIT_21 = new double*[RowsOfN_Par_Geo_LAIT_21];
		for (unsigned i = 0; i < RowsOfN_Par_Geo_LAIT_21; i++)
			N_Par.Geo_LAIT_21[i] = new double[ColumnsOfN_Par_Geo_LAIT_21];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++) {
			N_Par.Geo_LAIT_21[0][i] = 6.0;
			N_Par.Geo_LAIT_21[1][i] = 0.0;
			N_Par.Geo_LAIT_21[2][i] = 0.0;
		}

		unsigned int RowsOfN_Par_Geo_CD_31 = 3; // is static value
		unsigned int ColumnsOfN_Par_Geo_CD_31 = N_Par.Div_MAXGRP;
		N_Par.Geo_CD_31 = new double*[RowsOfN_Par_Geo_CD_31];
		for (unsigned i = 0; i < RowsOfN_Par_Geo_CD_31; i++)
			N_Par.Geo_CD_31[i] = new double[ColumnsOfN_Par_Geo_CD_31];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++) {
			N_Par.Geo_CD_31[0][i] = 0.6;
			N_Par.Geo_CD_31[1][i] = 0.6;
			N_Par.Geo_CD_31[2][i] = 0.0;
		}

		unsigned int RowsOfN_Par_Geo_FD_31 = 3; // is static value
		unsigned int ColumnsOfN_Par_Geo_FD_31 = N_Par.Div_MAXGRP;
		N_Par.Geo_FD_31 = new double*[RowsOfN_Par_Geo_FD_31];
		for (unsigned i = 0; i < RowsOfN_Par_Geo_FD_31; i++)
			N_Par.Geo_FD_31[i] = new double[ColumnsOfN_Par_Geo_FD_31];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++) {
			N_Par.Geo_FD_31[0][i] = 0.01;
			N_Par.Geo_FD_31[1][i] = 0.01;
			N_Par.Geo_CD_31[2][i] = 0.0;
		}

		unsigned int RowsOfN_Par_Geo_CLFH_31 = 3; // is static value
		unsigned int ColumnsOfN_Par_Geo_CLFH_31 = N_Par.Div_MAXGRP;
		N_Par.Geo_CLFH_31 = new double*[RowsOfN_Par_Geo_CLFH_31];
		for (unsigned i = 0; i < RowsOfN_Par_Geo_CLFH_31; i++)
			N_Par.Geo_CLFH_31[i] = new double[ColumnsOfN_Par_Geo_CLFH_31];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++) {
			N_Par.Geo_CLFH_31[0][i] = 0.01;
			N_Par.Geo_CLFH_31[1][i] = 0.0;
			N_Par.Geo_CLFH_31[2][i] = 0.0;
		}

		unsigned int RowsOfN_Par_Geo_TR_21 = 2; // is static value
		unsigned int ColumnsOfN_Par_Geo_TR_21 = N_Par.Div_MAXGRP;
		N_Par.Geo_TR_21 = new double*[RowsOfN_Par_Geo_TR_21];
		for (unsigned i = 0; i < RowsOfN_Par_Geo_TR_21; i++)
			N_Par.Geo_TR_21[i] = new double[ColumnsOfN_Par_Geo_TR_21];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++) {
			N_Par.Geo_TR_21[0][i] = 0.7;
			N_Par.Geo_TR_21[1][i] = 0.0;
		}

		N_Par.Pro_Pmax_3 = new double[N_Par.Div_MAXGRP]; // is static value
		for (int i = 0; i < 10; i++)
			N_Par.Pro_Pmax_3[i] = 8;

		N_Par.Pro_Alpha_3 = new double[N_Par.Div_MAXGRP]; // is static value
		for (int i = 0; i < 10; i++)
			N_Par.Pro_Alpha_3[i] = 0.3;

		N_Par.Pro_GLoss = 0.25;

		unsigned int RowsOfN_Par_Pro_MLoss_33 = 3; // is static value
		unsigned int ColumnsOfN_Par_Pro_MLoss_33 = N_Par.Div_MAXGRP;
		N_Par.Pro_MLoss_33 = new double*[RowsOfN_Par_Pro_MLoss_33];
		for (unsigned i = 0; i < RowsOfN_Par_Pro_MLoss_33; i++)
			N_Par.Pro_MLoss_33[i] = new double[ColumnsOfN_Par_Pro_MLoss_33];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++) {
			N_Par.Pro_MLoss_33[0][i] = 0.0;
			N_Par.Pro_MLoss_33[1][i] = 0.0;
			N_Par.Pro_MLoss_33[2][i] = 0.0;
		}

		N_Par.Pro_Limit_3 = new double[N_Par.Div_MAXGRP]; // is static value
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Pro_Limit_3[i] = 1.0;

		N_Par.Pro_FUNCTION_2 = 0;

		unsigned int RowsOfN_Par_Pro_dbh_growth = 4; // is static value
		unsigned int ColumnsOfN_Par_Pro_dbh_growth = N_Par.Div_MAXGRP;
		N_Par.Pro_dbh_growth = new double*[RowsOfN_Par_Pro_dbh_growth];
		for (unsigned i = 0; i < RowsOfN_Par_Pro_dbh_growth; i++)
			N_Par.Pro_dbh_growth[i] = new double[ColumnsOfN_Par_Pro_dbh_growth];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++) {
			N_Par.Pro_dbh_growth[0][i] = 0.0;
			N_Par.Pro_dbh_growth[1][i] = 0.0;
			N_Par.Pro_dbh_growth[2][i] = 0.0;
			N_Par.Pro_dbh_growth[3][i] = 0.0;
		}

		N_Par.Growth_Function = "polynom";

		N_Par.Pro_dbh_growth_max = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Pro_dbh_growth_max[i] = 12.0;

		N_Par.Pro_dbh_growth_start = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Pro_dbh_growth_start[i] = 0.0;

		N_Par.Pro_dbh_growth_end = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Pro_dbh_growth_end[i] = 0.0;

		N_Par.Pro_dbh_growth_maxpoint = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Pro_dbh_growth_maxpoint[i] = 0.33;

		N_Par.Fire = false;
		N_Par.Fire_FirstTime = 0.0;
		N_Par.Fire_Lambda = 50;
		N_Par.Fire_exmoisture = 2.0;
		N_Par.Fire_minFuelLoad = 0.0;
		N_Par.Fire_Beta = 0.30;
		N_Par.Fire_Sev = 0.50;

		N_Par.Fire_Tolerance = new long[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Fire_Tolerance[i] = 2;

		N_Par.Env_IS_2 = new double[2];
		for (int i = 0; i < 2; i++)
			N_Par.Env_IS_2[i] = 500;

		N_Par.Env_DayL_2 = new double[2];
		for (int i = 0; i < 2; i++)
			N_Par.Env_DayL_2[i] = 12;

		N_Par.Env_SeaL_2 = new double[2];
		N_Par.Env_SeaL_2[0] = 1.0;
		N_Par.Env_SeaL_2[0] = 0.0;

		N_Par.Climate = new long[6];
		N_Par.Climate[0] = 0;
		N_Par.Climate[1] = 0;
		N_Par.Climate[2] = 0;
		N_Par.Climate[3] = 0;
		N_Par.Climate[4] = 0;
		N_Par.Climate[5] = 0;
		N_Par.Water_ON = 0;
		N_Par.Temperature_ON = 0;
		N_Par.variable_Irradiance_ON = 0;
		N_Par.Daylength_ON = 0;
		N_Par.Veg_period_ON = 0;
		N_Par.CO2_dependency = 0;
		N_Par.CO2_reference_temperature = 24;
		N_Par.CO2_reference_concentration = 400;
		N_Par.CO2_stomata_reference_concentration = 200;
		N_Par.WUE_Transpiration_Assimilation_ratio = 0.5;
		N_Par.CO2_dependency_function_simple = new double[4];
		for (int i = 0; i < 4; i++)
			N_Par.CO2_dependency_function_simple[i] = 0.0;

		N_Par.Temperature_Reduction_Algorithm = "norm_4";
		N_Par.ref_length_of_vegetation_periode = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.ref_length_of_vegetation_periode[i] = 1.0;

		N_Par.ref_Irradiance = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.ref_Irradiance[i] = 500.0;

		N_Par.ref_environment_reduction = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.ref_environment_reduction[i] = 1.0;

		N_Par.temp_co2low = -10.0;
		N_Par.temp_co2high = 37.0;
		N_Par.Temperature_min = 10;
		N_Par.Temperature_opt = 15;
		N_Par.Temperature_sig = 15;
		N_Par.Temperature_Q10 = 2.0;
		N_Par.Temperature_reference = 25.0;

		N_Par.Temperature_month_cold = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Temperature_month_cold[i] = 0.0;

		N_Par.Temperature_month_hot = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Temperature_month_hot[i] = 20.0;

		N_Par.Climate_File.resize(MAXPLOT * MAXHA);
		std::fill(N_Par.Climate_File.begin(), N_Par.Climate_File.end(), "None");

		N_Par.Light_Comp_Factor = 9.0;

		N_Par.Water_WUE = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Water_WUE[i] = 6.0;

		N_Par.Water_SD = 2.5;
		N_Par.Water_SoilLayer = 1;

		N_Par.Water_FC = new double[MAXSOILLAYER];
		N_Par.Water_PWP = new double[MAXSOILLAYER];
		N_Par.Water_LayerDepth = new double[MAXSOILLAYER];
		for (int i = 0; i < MAXSOILLAYER; i++) {
			if (i == 0)
				N_Par.Water_LayerDepth[i] = N_Par.Water_SD;
			else
				N_Par.Water_LayerDepth[i] = 0;
			N_Par.Water_FC[i] = 31.0;
			N_Par.Water_PWP[i] = 19.4;
		}

		N_Par.Soil_File.resize(MAXPLOT * MAXHA);
		std::fill(N_Par.Soil_File.begin(), N_Par.Soil_File.end(), "None");

		N_Par.Water_KL = 0.2;
		N_Par.Water_POR = 45.3;
		N_Par.Water_RainfallDuration = 24.0;
		N_Par.Water_KS = 0.00000606;
		N_Par.Water_L = 0.378;
		N_Par.Water_SW_RES = 4.1;
		N_Par.Geo_RB = 0.4;
		N_Par.Geo_RH = N_Par.Water_SD;
		N_Par.Cflux_aet = 1350;
		N_Par.random_seed_ON = 0;
		N_Par.CPool_DeadWood = 0.0;
		N_Par.CPool_Soil_fast = 0.0;
		N_Par.CPool_Soil_slow = 0.0;
		N_Par.Landslide = false;
		N_Par.Landslide_reduced_growth = 0;
		N_Par.Landslide_reduced_seeds = 0;
		N_Par.Landslide_increased_mortality = 0;
		N_Par.Slide_Timelag_extern_file = new long[4];
		for (int i = 0; i < 4; i++)
			N_Par.Slide_Timelag_extern_file[i] = 0;
		N_Par.Timespan_reduced_rates = 0;
		N_Par.SlideFreqHaYear = 0;

		unsigned int RowsOfN_Par_SlideSizeFrequency = 2; // is static value
		unsigned int ColumnsOfN_Par_SlideSizeFrequency = 25;
		N_Par.SlideSizeFrequency = new double*[RowsOfN_Par_SlideSizeFrequency];
		for (unsigned i = 0; i < RowsOfN_Par_SlideSizeFrequency; i++)
			N_Par.SlideSizeFrequency[i] =
				 new double[ColumnsOfN_Par_SlideSizeFrequency];
		for (int i = 0; i < 25; i++) {
			N_Par.SlideSizeFrequency[0][i] = 0.0;
			N_Par.SlideSizeFrequency[1][i] = 0.0;
		}

		N_Par.Pro_Rho_3 = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Pro_Rho_3[i] = 0.55;

		N_Par.Pro_RD = 0.4;
		N_Par.Pro_RCON = 1000;
		N_Par.StandardDeviation_Diameter_Height = 0.0;
		N_Par.Min_Diameter_Inc_Step = 0.0001;
		N_Par.CrownDensityCurve = 1;
		N_Par.Pro_M = 0.1;
		Switch.DLYR = 0.5;
		Switch.Schwelle = 0.10;
		Switch.MortThreshold = 100;
		Switch.Ha = 1;
		Switch.Maxplot = 25;
		Switch.Hectare = 10000.0;
		Switch.Hecside = 100.0;
		N_Par.StoreInitialState = 1;
		N_Par.WarningType = 11;
		N_Par.ErrorType = 4;
		PinFileNameX =
			 "../../formind-projects/standard1/formind_parameters/standard3.pin";
		resultDirectory = "../results";
		N_Par.InvFileName =
			 "../../formind-projects/standard1/formind_parameters/standard3.inv";
		N_Par.InitPoolsFileName =
			 "../../formind-projects/standard1/formind_parameters/standard3.initpools";
		Logging.DoIt = 0;
		Logging.Time = 50;
		Logging.AreaLogging = 0;
		Logging.Thinning = 0;
		Logging.ThinTimeAfterLog = 5;
		Logging.ThinningNPCT = 0;
		Logging.ThinningMinDBH = 0.0;
		Logging.FirstThinning = 0.0;
		Logging.ThinningCycle = 10;
		Logging.MaxRemove = 400;

		Logging.Diameter = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			Logging.Diameter[i] = 0.6;

		Logging.Cycle = 20;
		Logging.Min = 1;
		Logging.Max = 30;
		Logging.Rest = 0;
		Logging.DamLinear = 0;
		Logging.No_HarvestDamage = 0;
		Logging.Falling_DamagedPlots = 0;
		Logging.SkidTrailDamage = 0;
		Logging.SkidTrailDamage_Time = 0;
		Logging.SkidTrailDamage_Area = 0.0;
		Logging.SkidTrailEstablishNewTrails = 0;

		Logging.DamDia = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			Logging.DamDia[i] = 0.5;

		Logging.Dam1 = new double[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			Logging.Dam1[i] = 0.55;
		Logging.emission_factor = 0.0;

		// Sidar parameters
		N_Par.LidarOutputStep = 500.0;
		N_Par.LidarOutputStart = 0.0;
		N_Par.PulseDistance = 1.0;
		N_Par.LargeFootprintDiameter = 22.0;
		N_Par.LargeFootprintEnergySD = 5.5;
		N_Par.VegetationSurfaceReturnProbability = 0.2;
		N_Par.VegetationExtinctionCoef = 0.2;
		N_Par.GroundSurfaceReturnProbability = 0.2;
		N_Par.GroundExtinctionCoef = 0.2;

		N_Par.CrownGeometry = new int[N_Par.Div_MAXGRP];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.CrownGeometry[i] = 1;

		N_Par.LidarLAIBased = true;
		N_Par.CrownOverlapSum = true;
		N_Par.CrownOverlapMaxLAD = 2.0;
		N_Par.LidarTreeTrunks = true;
		N_Par.LidarPostProcessing = false;
		N_Par.ThinningCellSize = 1.0;
		N_Par.RScriptPath =
			 "..\\..\\..\\..\\..\\formind-analysis\\LidarPostProcessing\\FormindLidarPostprocessing.R";
		N_Par.RPath =
			 "..\\..\\..\\..\\..\\formind-analysis\\R-Portable\\App\\R-Portable\\bin\\RScript.exe";


	}


 // -----------------------------------------------------------

/* !
 \brief          Sets default values of the grassmind model
 \param	       		void
 \return	        void
 \description 		if the parameter values are not given by the *.par file, this values are used for the simulation
 */
	void setGrassmindDefaultValues() {

		N_Par.InitialMowed = false;
		N_Par.InitialMowHeight = 0;
		N_Par.InitialGreenFraction = 1;

		N_Par.Div_CN_Green = new double[N_Par.Div_MAXGRP];
		for (int j = 0; j < N_Par.Div_MAXGRP; j++)
			N_Par.Div_CN_Green[j] = 23.0;

		N_Par.Div_CN_Brown = new double[N_Par.Div_MAXGRP];
		for (int j = 0; j < N_Par.Div_MAXGRP; j++)
			N_Par.Div_CN_Brown[j] = 23.0;

		N_Par.Pro_Mresp = 0.02;
		N_Par.Cost_Rhiz = 0.2;
		N_Par.Calib_Biomass = 0.0;

		N_Par.Geo_LLS = new double[N_Par.Div_MAXGRP]; // is static value
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Geo_LLS[i] = 100.0;

		N_Par.Geo_RLS = new double[N_Par.Div_MAXGRP]; // is static value
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Geo_RLS[i] = 100.0;

		N_Par.Div_Lifespan = new double[N_Par.Div_MAXGRP]; // is static value
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Div_Lifespan[i] = 20.0;

		N_Par.Geo_OF = new double[N_Par.Div_MAXGRP]; // is static value
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Geo_OF[i] = 1.0;

		N_Par.Div_Nfixing = new double[N_Par.Div_MAXGRP]; // is static value
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Div_Nfixing[i] = 0.0;

		N_Par.Est_Germ = new double[N_Par.Div_MAXGRP]; // is static value
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Est_Germ[i] = 1.0;

		N_Par.Est_Em = new double[N_Par.Div_MAXGRP]; // is static value
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Est_Em[i] = 0;

		N_Par.Sow_Date = new double[N_Par.Div_MAXGRP]; // is static value
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Sow_Date[i] = 0;

		unsigned int RowsOfN_Par_Geo_AGB_2 = 2; // is static value
		unsigned int ColumnsOfN_Par_Geo_AGB_2 = N_Par.Div_MAXGRP;
		N_Par.Geo_AGB_2 = new double*[RowsOfN_Par_Geo_AGB_2];
		for (unsigned i = 0; i < RowsOfN_Par_Geo_AGB_2; i++)
			N_Par.Geo_AGB_2[i] = new double[ColumnsOfN_Par_Geo_AGB_2];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++) {
			N_Par.Geo_AGB_2[0][i] = 1.0;
			N_Par.Geo_AGB_2[1][i] = 1.0;
		}

		unsigned int RowsOfN_Par_Geo_RD_2 = 2; // is static value
		unsigned int ColumnsOfN_Par_Geo_RD_2 = N_Par.Div_MAXGRP;
		N_Par.Geo_RD_2 = new double*[RowsOfN_Par_Geo_RD_2];
		for (unsigned i = 0; i < RowsOfN_Par_Geo_RD_2; i++)
			N_Par.Geo_RD_2[i] = new double[ColumnsOfN_Par_Geo_RD_2];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++) {
			N_Par.Geo_RD_2[0][i] = 1.0;
			N_Par.Geo_RD_2[1][i] = 1.0;
		}

		unsigned int RowsOfN_Par_Geo_alloc_4 = 6; // is static value
		unsigned int ColumnsOfN_Par_Geo_alloc_4 = N_Par.Div_MAXGRP;
		N_Par.Geo_alloc_4 = new double*[RowsOfN_Par_Geo_alloc_4];
		for (unsigned i = 0; i < RowsOfN_Par_Geo_alloc_4; i++)
			N_Par.Geo_alloc_4[i] = new double[ColumnsOfN_Par_Geo_alloc_4];
		for (int i = 0; i < N_Par.Div_MAXGRP; i++) {
			N_Par.Geo_alloc_4[0][i] = 1;
			N_Par.Geo_alloc_4[1][i] = 0;
			N_Par.Geo_alloc_4[2][i] = 0;
			N_Par.Geo_alloc_4[3][i] = 0;
			N_Par.Geo_alloc_4[4][i] = 0;
			N_Par.Geo_alloc_4[5][i] = 0;
		}

		N_Par.Geo_RL = new double[N_Par.Div_MAXGRP]; // is static value
		for (int i = 0; i < N_Par.Div_MAXGRP; i++)
			N_Par.Geo_RL[i] = 370000000;

		N_Par.Manage_File.resize(MAXPLOT * MAXHA);
		std::fill(N_Par.Manage_File.begin(), N_Par.Manage_File.end(), "None");
	}

 // -----------------------------------------------------------

/* !
 \brief          	Initialize the *.par file
 \param	       		void
 \return	        void
 \description       addes the most important parameters of the *.par file
 */

	void init_par() {
		par.addPar(IncludeFiles);
		par.addPar(SimulationName);
		par.addPar(OutputStep);
		par.addPar(ModelName);
		par.addPar(ProjectName);
		par.addByReference(::Time, "Time");
		par.addPar(TimeEnd);
		par.addPar(SpinupTime);
		par.addPar(TimeStep);
		par.addPar(OutputStart);
		par.addPar(RandomInit);
		par.addPar(modelshort);
		par.addPar(individuals);
		par.addPar(individual);
		par.setOutputStyle(styleSISI);
		//
		par.addByReference(ts, "Result.FileNameX");
		// Result.FileNameX is deprecated - but exists in the files
		par.addByReference(ti, "N_Par.InitC_ON");
		par.addByReference(Workaround_PinFileNameX, "PinFileNameX");
		par.addByReference(resultDirectory, "resultDirectory");
		//
		init_N_Par();
		init_N_Par_Grass();
		init_Log();
		init_Switch();
		init_myResultFileSwitch();
	}

  // -----------------------------------------------------------

/* !
 \brief          	Error handling and modules
 \param	       		void
 \return	        void
 \description 		test if FORMIND modules like climate, management,... are included
 */
	// is called afterparfile was read by readParFile()
	void applyDefaultParameterLogic() {
		// I impemented only the ones which showed Warnings in
		if (!par.findByName("Switch.Maxplot")) {
			Switch.Maxplot = 25;
			std::cerr <<
				 "INFO: Default value of the number of simulated plots per hectars is used (Switch.Maxplot = 25)." <<
				 std::endl;
		}
		if (Switch.Maxplot > MAXPLOT) {
			std::cerr <<
				 "ERROR: The number of plots exceeds MAXPLOT. Please increase the constant MAXPLOT in for_const.h";
			// should throw exception here? mimu todo.
		}
		//
		if (!par.findByName("Switch.Hectare")) {
			Switch.Hectare = 10000.0;
			std::cerr <<
				 "INFO: Default value for the hectar size is used (Switch.Hectare = 10000.0)." <<
				 std::endl;
		}
		//
		if (!par.findByName("Switch.Hecside")) {
			Switch.Hecside = 100.0;
			std::cerr <<
				 "INFO: Default value for the side length of a hectare is used (Switch.Hecside = 100.0)." <<
				 std::endl;
		}
		//
		N_Par.Water_ON = N_Par.Climate[0];
		N_Par.Temperature_ON = N_Par.Climate[1];
		N_Par.variable_Irradiance_ON = N_Par.Climate[2];
		N_Par.Daylength_ON = N_Par.Climate[3];
		N_Par.Veg_period_ON = N_Par.Climate[4];
		N_Par.CO2_dependency = N_Par.Climate[5];
		if ((N_Par.CO2_dependency == 1) && (N_Par.Water_ON == 0)) {
			N_Par.CO2_dependency = 2;
			std::cerr <<
				 "WARNING: CO2 Dependency complex version is ON but water module is off. Set CO2 Dependency to simple version." <<
				 std::endl;
		}

		T.End = TimeEnd;
		T.D = TimeStep;
		T.Out = OutputStep;
		T.OutputStart = OutputStart;
		MAXGRP = N_Par.Div_MAXGRP;
	}

  // -----------------------------------------------------------

/* !
 \brief          	Initialize result switches
 \param	       		void
 \return	        void
 \description       Initialize which outputfile should be written after simulation
 */

	void init_myResultFileSwitch() {
		par.addPar(myResultFileSwitch.ats);
		par.addPar(myResultFileSwitch.ba);
		par.addPar(myResultFileSwitch.ba_th);
		par.addPar(myResultFileSwitch.bmpl);
		par.addPar(myResultFileSwitch.bt);
		par.addPar(myResultFileSwitch.bt_th);
 		par.addPar(myResultFileSwitch.biom_chave_th);
		par.addPar(myResultFileSwitch.bv);
		par.addPar(myResultFileSwitch.bv_th);
		par.addPar(myResultFileSwitch.cflux);
		par.addPar(myResultFileSwitch.cfluxplot);
		par.addPar(myResultFileSwitch.cflux_century_plot);
		par.addPar(myResultFileSwitch.nflux_century_plot);
		par.addPar(myResultFileSwitch.dia);
		par.addPar(myResultFileSwitch.div);
		par.addPar(myResultFileSwitch.div_th);
		par.addPar(myResultFileSwitch.dyn);
		par.addPar(myResultFileSwitch.dyn_th);
		par.addPar(myResultFileSwitch.env);
		par.addPar(myResultFileSwitch.fal);
		par.addPar(myResultFileSwitch.fire);
		par.addPar(myResultFileSwitch.h);
		par.addPar(myResultFileSwitch.speciesplot);
		par.addPar(myResultFileSwitch.speciesplot_th);
		par.addPar(myResultFileSwitch.grass);
		par.addPar(myResultFileSwitch.grassplot);
		par.addPar(myResultFileSwitch.grass_mow);
		par.addPar(myResultFileSwitch.grasscalib);
		par.addPar(myResultFileSwitch.in);
		par.addPar(myResultFileSwitch.lai);
		par.addPar(myResultFileSwitch.lai_mean);
		par.addPar(myResultFileSwitch.lai_plot);
		par.addPar(myResultFileSwitch.lai_plot_heightlayer);
		par.addPar(myResultFileSwitch.landslide);
		par.addPar(myResultFileSwitch.lidarpc);
		par.addPar(myResultFileSwitch.lidarwf);
		par.addPar(myResultFileSwitch.log);
		par.addPar(myResultFileSwitch.log_ha);
		par.addPar(myResultFileSwitch.log_bad);
		par.addPar(myResultFileSwitch.log_nd);
		par.addPar(myResultFileSwitch.logg_end);
		par.addPar(myResultFileSwitch.mort);
		par.addPar(myResultFileSwitch.mort_th);
		par.addPar(myResultFileSwitch.mort_pft);
		par.addPar(myResultFileSwitch.mort_pft_th);
		par.addPar(myResultFileSwitch.mort_pft_dia);
		par.addPar(myResultFileSwitch.n);
		par.addPar(myResultFileSwitch.n_th);
		par.addPar(myResultFileSwitch.plot);
		par.addPar(myResultFileSwitch.diaplot);
		par.addPar(myResultFileSwitch.ha);
		par.addPar(myResultFileSwitch.ha_th);
		par.addPar(myResultFileSwitch.prod);
		par.addPar(myResultFileSwitch.res);
		par.addPar(myResultFileSwitch.res_th);
		par.addPar(myResultFileSwitch.res_th_bin);
		par.addPar(myResultFileSwitch.cohort);
		par.addPar(myResultFileSwitch.cohort_th);
		par.addPar(myResultFileSwitch.thin);
		par.addPar(myResultFileSwitch.restart);
		par.addPar(myResultFileSwitch.restartplot);
		par.addPar(myResultFileSwitch.seed);
		par.addPar(myResultFileSwitch.seed_rain);
		par.addPar(myResultFileSwitch.seedling);
		par.addPar(myResultFileSwitch.skid);
		par.addPar(myResultFileSwitch.stree);
		par.addPar(myResultFileSwitch.sv);
		par.addPar(myResultFileSwitch.sv_th);
		par.addPar(myResultFileSwitch.voxfor);
		par.addPar(myResultFileSwitch.water);
		par.addPar(myResultFileSwitch.water_plot);
		par.addPar(myResultFileSwitch.water_century_plot);
		par.addPar(myResultFileSwitch.water_century_plot_layer);
		par.addPar(myResultFileSwitch.save_parameter_files);
		par.addPar(myResultFileSwitch.result_time_stamp);
		par.addPar(myResultFileSwitch.pin);
		par.addPar(myResultFileSwitch.branch);
		par.addPar(myResultFileSwitch.traits);
		par.addPar(myResultFileSwitch.blender_json);

	}

 // -----------------------------------------------------------

/* !
 \brief          	Initialize size parameters
 \param	       		void
 \return	        void
 */
	void init_Switch() {
		par.addPar(Switch.DLYR);
		par.addPar(Switch.Schwelle);
		par.addPar(Switch.MortThreshold);
		par.addPar(Switch.Ha);
		par.addPar(Switch.Maxplot);
		par.addPar(Switch.Hectare);
		par.addPar(Switch.Hecside);
	}

 // -----------------------------------------------------------

/* !
 \brief          	Initialize logging parameters
 \param	       		void
 \return	        void
 */
	void init_Log() {
		// Log renamed to Logging in C++, but not in parameter files.
		// Thus here the addByReference method is used
		par.addByReference(Logging.DoIt, "Log.DoIt");
		par.addByReference(Logging.AreaLogging, "Log.AreaLogging");
		par.addByReference(Logging.Time, "Log.Time");
		par.addByReference(Logging.No_HarvestDamage, "Log.No_HarvestDamage");
		par.addByReference(Logging.SkidTrailDamage, "Log.SkidTrailDamage");
		par.addByReference(Logging.SkidTrailDamage_Time,
			 "Log.SkidTrailDamage_Time");
		par.addByReference(Logging.SkidTrailDamage_Area,
			 "Log.SkidTrailDamage_Area");
		par.addByReference(Logging.SkidTrailEstablishNewTrails,
			 "Log.SkidTrailEstablishNewTrails");
		par.addByReference(Logging.Falling_DamagedPlots,
			 "Log.Falling_DamagedPlots");
		par.addByReference(Logging.Diameter, "Log.Diameter");
		par.addByReference(Logging.Cycle, "Log.Cycle");
		par.addByReference(Logging.Min, "Log.Min");
		par.addByReference(Logging.Max, "Log.Max");
		par.addByReference(Logging.Rest, "Log.Rest");
		par.addByReference(Logging.DamLinear, "Log.DamLinear");
		par.addByReference(Logging.DamDia, "Log.DamDia");
		par.addByReference(Logging.Dam1, "Log.Dam1");
		par.addByReference(Logging.Dam2, "Log.Dam2");
		par.addByReference(Logging.emission_factor, "Log.emission_factor");

		par.addByReference(Logging.Thinning, "Log.Thinning");
		par.addByReference(Logging.ThinningNPCT, "Log.ThinningNPCT");
		par.addByReference(Logging.ThinningMinDBH, "Log.ThinningMinDBH");
		par.addByReference(Logging.FirstThinning, "Log.FirstThinning");
		par.addByReference(Logging.ThinTimeAfterLog, "Log.ThinTimeAfterLog");
		par.addByReference(Logging.ThinningCycle, "Log.ThinningCycle");
		par.addByReference(Logging.MaxRemove, "Log.MaxRemove");
	}


 // -----------------------------------------------------------

/* !
 \brief          	Initialize further parameters
 \param	       		void
 \return	        void
 */
	void init_N_Par() {
		// maybe not all used in parameter files:
		par.addPar(N_Par.Div_COMMERCIAL_A);
		par.addPar(N_Par.Div_Liana);
		par.addPar(N_Par.Liana);
		par.addPar(N_Par.Div_MAXTREE);
		par.addPar(N_Par.Div_Animaldispersed);
		par.addPar(N_Par.Geo_Bio2Dbh);
		par.addPar(N_Par.Patch_Heterogeneity);

		// Treelists
		par.addPar(N_Par.TreeListOutputStep);
		par.addPar(N_Par.TreeListOutputStart);
		par.addPar(N_Par.TreeListInput);
		par.addPar(N_Par.InitPools);

		// parameterset for BoutofD-powerlaw and DoutofB;
		par.addPar(N_Par.Geo_TR_21);
		par.addPar(N_Par.Geo_HMmean_5);
		par.addPar(N_Par.Geo_HD_35);
		par.addPar(N_Par.Geo_HD_39);
		par.addPar(N_Par.Geo_LD_31);
		par.addPar(N_Par.Geo_LAIT_21);
		par.addPar(N_Par.Geo_CD_31);
		par.addPar(N_Par.Geo_FD_31);
		par.addPar(N_Par.Geo_CLFH_31);
		par.addPar(N_Par.Pro_MLoss_33);
		par.addPar(N_Par.Pro_Limit_3);
		par.addPar(N_Par.Pro_FUNCTION_2);
		par.addPar(N_Par.Pro_Pmax_3);
		par.addPar(N_Par.Pro_Alpha_3);
		par.addPar(N_Par.Pro_dbh_growth_max);
		par.addPar(N_Par.Pro_dbh_growth_start);
		par.addPar(N_Par.Pro_dbh_growth_end);
		par.addPar(N_Par.Pro_dbh_growth_maxpoint);
		par.addPar(N_Par.Pro_Rho_3);
		par.addPar(N_Par.Est_DSTree_5);
		par.addPar(N_Par.Est_ISeed_3);
		par.addPar(N_Par.Est_Dist_3);
		par.addPar(N_Par.Est_SeedMort_3);
		par.addPar(N_Par.Est_SeedSurvival_3);
		par.addPar(N_Par.Env_IS_2);
		par.addPar(N_Par.Env_DayL_2);
		par.addPar(N_Par.Env_SeaL_2);
		par.addPar(N_Par.Env_VegPeriod);
		par.addPar(N_Par.Geo_FUNCTION_7);
		par.addPar(N_Par.Mort_FUNCTION_2);
		par.addPar(N_Par.Climate);
		par.addPar(N_Par.Est_NS_3);
		par.addPar(N_Par.Est_SeedTree_3);
		par.addPar(N_Par.Invasion);
		par.addPar(N_Par.TimeInvasion);
		par.addPar(N_Par.MeanSeedDistance);
		par.addPar(N_Par.Predation);
		par.addPar(N_Par.SeedPredatorNumber);
		par.addPar(N_Par.SeedPredatorEfficiency);
		par.addPar(N_Par.SeedPredatorHandlingTime);
		par.addPar(N_Par.Mort_mean_35);
		par.addPar(N_Par.Mort_Dia_31);
		par.addPar(N_Par.Mort_DD_Seed_31);
		par.addPar(N_Par.Mort_DD_Seedling_31);
		par.addPar(N_Par.Mort_Dinc_31);
		par.addPar(N_Par.Mort_mean_19);
		par.addPar(N_Par.Mort_50nbinc);
		par.addPar(N_Par.Mort_SpaceLimitation_Factor);
		par.addPar(N_Par.Mort_SpaceLimitation_Diameter);
		par.addPar(N_Par.Pro_dbh_growth);
		par.addPar(N_Par.Fire_Tolerance);
		par.addPar(N_Par.InvFileName);
		par.addPar(N_Par.InitPoolsFileName);
		par.addPar(N_Par.Temperature_month_cold);
		par.addPar(N_Par.Temperature_month_hot);
		par.addPar(N_Par.ref_environment_reduction);
		par.addPar(N_Par.ref_length_of_vegetation_periode);
		par.addPar(N_Par.ref_Irradiance);
		// par.addPar(N_Par.FileName);
		// FILE File;

		par.addPar(N_Par.Pro_GLoss);
		par.addPar(N_Par.Pro_M);
		par.addPar(N_Par.Est_DS);
		par.addPar(N_Par.HGRP_EXP);
		par.addPar(N_Par.Lin_Dis);
		par.addPar(N_Par.Max_Den);
		par.addPar(N_Par.DispersalKernel);
		par.addPar(N_Par.Div_K);
		par.addPar(N_Par.Mort_FallP);
		MMParNodeBase* pNode = par.addPar(N_Par.Div_MAXGRP);
		pNode->isConstant = true;
		par.addPar(N_Par.Geo_RB);
		par.addPar(N_Par.Geo_RH);
		par.addPar(N_Par.Pro_RD);
		par.addPar(N_Par.Pro_RCON);
		par.addPar(N_Par.StandardDeviation_Diameter_Height);
		par.addPar(N_Par.Min_Diameter_Inc_Step);
		par.addPar(N_Par.CrownDensityCurve);
		par.addByReference(N_Par.Growth_Function, "N_Par.Growth_Function_Switch");
		par.addPar(N_Par.Div_DiaClassWidth);
		par.addPar(N_Par.Fire);

		par.addPar(N_Par.Fire_FirstTime);
		par.addPar(N_Par.Fire_Lambda);
		par.addPar(N_Par.Fire_Beta);
		par.addPar(N_Par.Fire_Sev);
		par.addPar(N_Par.Fire_minFuelLoad);
		par.addPar(N_Par.Fire_exmoisture);
		par.addPar(N_Par.variable_Irradiance_ON);
		par.addPar(N_Par.Temperature_ON);
		par.addPar(N_Par.Water_ON);
		par.addPar(N_Par.Veg_period_ON);
		par.addPar(N_Par.Daylength_ON);
		par.addPar(N_Par.OldVersionTest);
		// file location of the climate input file
		par.addPar(N_Par.Temperature_Reduction_Algorithm);
		// lag for algorithm of temperature reduction function
		par.addPar(N_Par.StoreInitialState);
		// [�C] optimal temperature for photosyntheses neede for Horn algorithm
		par.addPar(N_Par.Temperature_k);
		// [1/�C] rate of Change  neede for Horn algorithm
		par.addPar(N_Par.Temperature_min);
		// [�C] Minimum mean temperature for photosynthesis all day with higher temperature are definend as vegetation periode
		par.addPar(N_Par.Temperature_Q10);
		// [�C] Short-term respiration response to temperature
		par.addPar(N_Par.Temperature_reference); // [�C] reference temperature
		par.addPar(N_Par.Water_k);
		// [?] rate of change neede for Horn algorithm
		par.addPar(N_Par.Water_inflection);
		// [?] inflection point neede for Horn algorithm
		par.addPar(N_Par.temp_co2low);
		// [�C] Minimum temperature for photosynthesis
		par.addPar(N_Par.temp_co2high);
		// [�C] Minimum temperature for photosynthesis
		par.addPar(N_Par.Temperature_sig);
		// Parameter for temperature reduction curve normal distributed
		par.addPar(N_Par.Temperature_opt); // optimal temperature condition
		par.addPar(N_Par.Climate_File);

		// Light competition factor in GRASSMIND
		par.addPar(N_Par.Light_Comp_Factor);

		// Parameter Soil Water Module
		par.addPar(N_Par.Water_WUE);
		par.addPar(N_Par.Water_PWP);
		par.addPar(N_Par.Water_KL);
		par.addPar(N_Par.Water_POR);
		par.addPar(N_Par.Water_KS);
		par.addPar(N_Par.Water_L);
		par.addPar(N_Par.Water_SW_RES);
		par.addPar(N_Par.Water_FC);
		par.addPar(N_Par.Water_SD);
		par.addPar(N_Par.Water_SoilLayer);
		// number of soil layers (default: 1)
		par.addPar(N_Par.Water_LayerDepth);
		// height of each soil layer (default: Water_SD)
		par.addPar(N_Par.Water_RainfallDuration);
		// duration of daily rainfall [h]

		par.addPar(N_Par.CO2_reference_concentration);
		par.addPar(N_Par.CO2_reference_temperature);
		par.addPar(N_Par.CO2_stomata_reference_concentration);
		par.addPar(N_Par.WUE_Transpiration_Assimilation_ratio);
		par.addPar(N_Par.CO2_dependency_function_simple);

		par.addPar(N_Par.Soil_File);

		// Replacement flags for preprocessor flags
		par.addPar(N_Par.Closed_boundary);
		par.addPar(N_Par.Globalseeds);
		par.addPar(N_Par.Spacelimitation);
		par.addPar(N_Par.Puls);
		par.addPar(N_Par.Treefall);
		par.addPar(N_Par.Nogrow);
		par.addPar(N_Par.Seedtree);
		par.addPar(N_Par.Patch_Seed);
		par.addPar(N_Par.Densityreg);
		par.addPar(N_Par.Predmort);
		par.addPar(N_Par.Flag_SpeciesNumber);
		par.addPar(N_Par.SpeciesNumber);
		par.addPar(N_Par.Fragmentation);
		par.addPar(N_Par.Frag_highmortbigtree);
		par.addPar(N_Par.Frag_lowseed);
		par.addPar(N_Par.Edge_Distance);
		par.addPar(N_Par.Frag_TemperatureEffects);

		par.addPar(N_Par.Flag_BackgroundMortality);
		par.addPar(N_Par.Flag_DbhMortality);
		par.addPar(N_Par.Flag_DincMortality);
		par.addPar(N_Par.Flag_DD_Seed_Mortality);
		par.addPar(N_Par.Flag_DD_Seedling_Mortality);




		// spatially explicit version
		par.addPar(N_Par.spatial); // general flag - spatially explicit or not
		par.addPar(N_Par.spatial_area);
		par.addPar(N_Par.spatial_ZOI20);

		par.addPar(N_Par.Landslide);
		par.addPar(N_Par.Slide_Timelag_extern_file);
		// landslide-module parameter
		par.addPar(N_Par.Timespan_reduced_rates);
		par.addPar(N_Par.SlideFreqHaYear);
		par.addPar(N_Par.SlideSizeFrequency);
		par.addPar(N_Par.Landslide_reduced_growth);
		par.addPar(N_Par.Landslide_reduced_seeds);
		par.addPar(N_Par.Landslide_increased_mortality);

		par.addPar(N_Par.Cflux_Dead_to_Athmo);
		par.addPar(N_Par.Cflux_Dead_to_Soil);
		par.addPar(N_Par.Cflux_Soil_to_Athmo);
		par.addPar(N_Par.Cflux_aet);

		par.addPar(N_Par.CPool_DeadWood);
		// [tC/ha] used to initialize dead wood pool in xpar
		par.addPar(N_Par.CPool_Soil_fast);
		// [tc/ha] used to initialize soil pools in xpar
		par.addPar(N_Par.CPool_Soil_slow);
		// [tc/ha] used to initialize soil pools in xpar
		par.addPar(N_Par.random_seed_ON);
		// flag for rancom seed

		par.addPar(N_Par.ExpNotation);
		par.addPar(N_Par.Mort_nbinc);
		par.addPar(N_Par.Flag_PBbuffer);

		// Sidar parameters
		par.addPar(N_Par.LidarOutputStep);
		par.addPar(N_Par.LidarOutputStart);
		par.addPar(N_Par.PulseDistance);
		par.addPar(N_Par.LargeFootprintDiameter);
		par.addPar(N_Par.LargeFootprintEnergySD);
		par.addPar(N_Par.VegetationSurfaceReturnProbability);
		par.addPar(N_Par.VegetationExtinctionCoef);
		par.addPar(N_Par.GroundSurfaceReturnProbability);
		par.addPar(N_Par.GroundExtinctionCoef);
		par.addPar(N_Par.CrownGeometry);
		par.addPar(N_Par.LidarLAIBased);
		par.addPar(N_Par.CrownOverlapSum);
		par.addPar(N_Par.CrownOverlapMaxLAD);
		par.addPar(N_Par.LidarTreeTrunks);
		par.addPar(N_Par.LidarPostProcessing);
		par.addPar(N_Par.ThinningCellSize);
		par.addPar(N_Par.RScriptPath);
		par.addPar(N_Par.RPath);

		par.addPar(N_Par.Cohort);

		// Trait Distr.
		par.addPar(N_Par.TraitDist_ON);

		// Error handling.
		par.addPar(N_Par.WarningType);
		par.addPar(N_Par.ErrorType);

	}

// -----------------------------------------------------------

/* !
 \brief          	Initialize grass parameters  (GRASSMIND)
 \param	       		void
 \return	        void
 */
	void init_N_Par_Grass() {
		// -------------- relevant for Grassmind --------------------------------
		par.addPar(N_Par.InitialMowed);
		par.addPar(N_Par.InitialMowHeight);
		par.addPar(N_Par.InitialGreenFraction);
		par.addPar(N_Par.Pro_Mresp); // maintenance respiration []
		par.addPar(N_Par.Cost_Rhiz); // percentage of NPP for symbiosis with rhizobia
		par.addPar(N_Par.Calib_Biomass); // height of the cut biomass in field for calibration
		par.addPar(N_Par.Geo_RL); // specific root length
		par.addPar(N_Par.Div_CN_Green);
		par.addPar(N_Par.Div_CN_Brown);
		par.addPar(N_Par.Geo_AGB_2); // shoot-root relationship
		par.addPar(N_Par.Geo_RD_2); // shoot-rooting depth relationship
		par.addPar(N_Par.Geo_alloc_4); // allocation fractions
		par.addPar(N_Par.Geo_LLS); // leaf life span
		par.addPar(N_Par.Geo_RLS); // root life span
		par.addPar(N_Par.Geo_OF); // overlapping factor
		par.addPar(N_Par.Est_Germ); // germination rate
		par.addPar(N_Par.Est_Em); // days to emergence of seedlings/germination
		par.addPar(N_Par.Div_Nfixing);
		par.addPar(N_Par.Div_Lifespan);
		par.addPar(N_Par.Sow_Date); // sowing date of seeds for the grassland
		par.addPar(N_Par.Century_ON);
		par.addPar(N_Par.Candy_ON);
		par.addPar(N_Par.GRASSMIND);
		par.addPar(N_Par.Manage_File);

	}

 // -----------------------------------------------------------

/* !
 \brief          	read and  write of *.par file and error handling
 \param	       		void
 \return	        void
 */
	void readParFile(const char*fn) {
		std::ifstream sisifile(fn, std::ios::binary);
		if (!sisifile.is_open()) {
			std::string ts = std::string("Cannot open file for reading: ") +
				 std::string(fn);
			MMErrorMessage(ts, 4); // 4=exception
		}

		sisifile >> par;
		PinFileNameX = Workaround_PinFileNameX.c_str();
		fileNames.setPinFileName(PinFileNameX);
		applyDefaultParameterLogic();
	}


	void writeParFile(const char*fn) {
		std::ofstream sisifile(fn, std::ios::binary);
		if (!sisifile.is_open()) {
			std::string ts = std::string("Cannot open file for writing: ") +
				 std::string(fn);
			MMErrorMessage(ts, 4); // 4=exception
		}

		// write to file
		MMParSetOutputStyle tOutputStyle = par.getOutputStyle();
		par.setOutputStyle(styleSISI);
		sisifile << par;
		par.setOutputStyle(tOutputStyle);

		sisifile.close();
	}


};

extern forIOMMParSet forio;

# endif
