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
// File						for_grow.cpp
// Description				calculates tree growth
//
///////////////////////////////////////////////////////////////////

#include "for_grow.h"
#ifdef underconstruction
#include "century/century_water.h"
#include "for_water.h"
#include "for_environment.h"
#endif
#include <stdexcept>

#include "MMParSet/MMErrorMessage.h"
#include "for_misc.h"
using namespace std;

// ----------------------- Deklarations --------------
void ProtocolIngrowth(TreePointer);
static void DetermineOBA(PlotPointer);
static double CalculateMaintResp(TreePointer, PlotPointer);

formindGrow forGrow;

/* !
 \brief   initialization of Biomass-Diameter-Relationships
 \details DoutofBFunc and BoutofDFunc

 */
void formindGrow::InitDB() {
	int whichB, whichH, whichF;
	whichB = N_Par.Geo_FUNCTION_7[7]; // Biomass-Diameter-Relationship
	whichH = N_Par.Geo_FUNCTION_7[0]; // Height-Diameter-Relationship
	whichF = N_Par.Geo_FUNCTION_7[3]; // Formfactor-Diameter-Relationship

	if (whichB == 0) {
#ifdef underconstruction
		if (N_Par.GRASSMIND) {
			DoutofBFunc = new DoutofB_grass(N_Par.Geo_FD_31, N_Par.Geo_HD_39,
				 N_Par.Pro_Rho_3);
		}
		else
#endif
		{
			if (whichH == 2 && whichF == 1)
				DoutofBFunc = new DoutofB_standard(N_Par.Geo_FD_31, N_Par.Geo_HD_39,
				 N_Par.Pro_Rho_3);
			else if ((whichH == 3 || whichH == 0) && whichF == 1)
				DoutofBFunc = new DoutofB_lookup(N_Par.Geo_FD_31, N_Par.Geo_HD_39,
				 N_Par.Pro_Rho_3);
			else
				DoutofBFunc = new DoutofB_discrete(N_Par.Geo_FD_31, N_Par.Geo_HD_39,
				 N_Par.Pro_Rho_3);
		}
		BoutofDFunc = new BoutofD_standard(N_Par.Pro_Rho_3);
	}
	else {
		DoutofBFunc = new DoutofB_friedrich(N_Par.Geo_Bio2Dbh, whichB);
		BoutofDFunc = new BoutofD_friedrich(N_Par.Geo_Bio2Dbh, whichB);
	}
}

// ----------------------------------------------------------------

/* !
 \brief   initialization of Height-Diameter-Relationships DoutofHFunc and HoutofDFunc
 */
void formindGrow::InitDH() {
	int which = N_Par.Geo_FUNCTION_7[0];
	if (which == 0) {
		DoutofHFunc = new DoutofH_0(N_Par.Geo_HD_39);
		HoutofDFunc = new HoutofD_0(N_Par.Geo_HD_39);
	}
	else if (which == 1) {
		DoutofHFunc = new DoutofH_1(N_Par.Geo_HD_39);
		HoutofDFunc = new HoutofD_1(N_Par.Geo_HD_39);
	}
	else if (which == 2) {
		DoutofHFunc = new DoutofH_2(N_Par.Geo_HD_39);
		HoutofDFunc = new HoutofD_2(N_Par.Geo_HD_39);
	}
	else if (which == 3) {
		DoutofHFunc = new DoutofH_3(N_Par.Geo_HD_39);
		HoutofDFunc = new HoutofD_3(N_Par.Geo_HD_39);
	}
	else {
		MMErrorMessage("the choosen DoutofH/HoutofD-functions do not exist", N_Par.ErrorType);

	}
}

// ----------------------------------------------------------------

/* !
 \brief   initialization of calculation for maintenance respiration
 \details
 */
void formindGrow::InitMaintenanceRespiration() {
	int which = N_Par.Pro_FUNCTION_2;

	if (which == 0) {
		InitGrow();
	}
	else if (which == 1) {
		MainResp = new LossFunction_1();
	}
	else if (which == 2) {
		MainResp = new LossFunction_2();
	}
	else if (which == 3) {
		MainResp = new LossFunction_3();
	}
	else if (which == 4) {
		MainResp = new LossFunction_4();
	}
	else if (which == 5) {
		MainResp = new LossFunction_5();
	}
	else {
	MMErrorMessage("the choosen maintenance-respiration-functions do not exist", N_Par.ErrorType);
	}
}

// ----------------------------------------------------------------

/* !
 \brief   initialization of diameter growth curves for the calculation of
 maintainance
 \details included in InitMaintenanceRespiration()
 respiration DiamGrow
 */
void formindGrow::InitGrow() {
	if ((N_Par.Growth_Function == "polynom") ||
		 (N_Par.Growth_Function == "chanter_maxPos"))
		CalculateGrowthCurves();

	if (N_Par.Growth_Function == "polynom")
		DiamGrow = new GrowPoly(GROWTH_COEFF);
	else if (N_Par.Growth_Function == "polynom_coefficient")
		DiamGrow = new GrowPolyCoefficient(N_Par.Pro_dbh_growth);
	else if (N_Par.Growth_Function == "weibull")
		DiamGrow = new GrowWeibull(N_Par.Pro_dbh_growth);
	else if (N_Par.Growth_Function == "richards")
		DiamGrow = new GrowRichards(N_Par.Pro_dbh_growth);
	else if (N_Par.Growth_Function == "chanter")
		DiamGrow = new GrowChanter(N_Par.Pro_dbh_growth, DM);
	else if (N_Par.Growth_Function == "chanter_maxPos")
		DiamGrow = new GrowChanterMaxPos(GROWTH_COEFF, DM);
	else if (N_Par.Growth_Function == "maxHaefner")
		DiamGrow = DiamGrow = new Grow_maxHaefner(N_Par.Pro_dbh_growth);
	else
	MMErrorMessage("the choosen potential growth function doesn�t exists", N_Par.ErrorType);

	TestNegativeGrowth() ;
}

// ----------------------------------------------------------------

/* !
 \brief   Calculates biomass of one cohort
 */
double formindGrow::CalculateBiomass(TREE*tree) {
	const double bt = tree->N * BoutofDFunc->Calculate(tree->D,
		 tree->DHprefactor * HoutofDFunc->Calculate(tree->D, tree->Grp),
		 FoutofD(tree->D, N_Par.Pro_Rho_3[tree->Grp], tree->Grp), tree->Grp);
	return bt;
}

// ----------------------------------------------------------------

/* !
 \brief     Loop over all hectars, plots, trees which includes all subroutines
 of all growth processes
 \details	included in formind.cpp
 \return    void
 */
void formindGrow::DoGrowth() {
	TreePointer tree;
	PlotPointer plot;
	HecPointer hec;
	double diameter_previous, biomass_previous, height_previous, root_previous;
	double IRRw, IRRd, daylw, dayld, vpd, vpw, temp_red;

	hec = FirstHec;
	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {
			tree = plot->FirstTree;
			Out.BINCPlot[hec->HecNo - 1][plot->No - 1] = 0;
			plot->R_total_biomass = 0; // respiration of all biomass in plot
			plot->PB = 0; // total photosynthesis of all trees in plot
			plot->Biomass = 0; // total biomass in plot
			plot->Biomass_calib = 0; // GRASSMIND, total biomass over a certain height
			plot->PCTCA = 0.0; // total crown projection area of PCT in plot
			while (tree != NULL) {

				if (tree->N > 0) {
					if (N_Par.variable_Irradiance_ON) {
						IRRw = tree->IR;
						// irradiance reaching tree in wet season
						IRRd = 0; // irradiance reaching tree in dry season
					}
					else
					{
						IRRw = tree->IRwet;
						IRRd = tree->IRdry;
					}

					if (N_Par.Daylength_ON) {
						daylw = plot->mean_daylength;
						dayld = 0;
					}
					else
					{
						daylw = N_Par.Env_DayL_2[0]; // daylength wetseason
						dayld = N_Par.Env_DayL_2[1]; // daylength dryseason
					}

					if (N_Par.Temperature_ON)
						temp_red = plot->temp_photosynthese_reduction[tree->Grp];
					else
						temp_red = 1;

					if (N_Par.Veg_period_ON) {
						if (N_Par.ref_length_of_vegetation_periode[tree->Grp] < 1.0)
							vpw = plot->length_of_vegetation_periode / T.D;
						else
							vpw = 365;
						vpd = 0;
					}
					else
					{
						vpw = N_Par.Env_SeaL_2[0] * 365;
						// length of season - wet
						vpd = N_Par.Env_SeaL_2[1] * 365;
						// length of season - dry
					}
					
                    tree->PB = CalculatePhotoProduction_new(tree, IRRw, IRRd, daylw,
                         dayld, vpw, vpd, temp_red);
                    // apply simple water limitation based on annual P/PET if enabled
                    if (N_Par.Water_ON && plot->water_reduction > 0.0f)
                        tree->PB *= plot->water_reduction;

					if (N_Par.CO2_dependency == 2)
						tree->PB *= plot->CO2_inc_GPP;

				}
				tree = tree->next;
			}

			int test = plot->No;
			if (!N_Par.StoreInitialState) {
#ifdef underconstruction
				if (N_Par.Century_ON)
					DoWaterCentury(plot);
				else if (N_Par.Candy_ON) {
					plot->SI = 0;
					DoSoilWaterCompetition(plot);
				}
				else {
					if (N_Par.Water_ON)
						DoWater(plot);
				}
#endif
			}

			tree = plot->FirstTree;
			while (tree != NULL) {
#ifdef underconstruction
				if (N_Par.TraitDist_ON) {
					overwrite_trait_parameter(tree);
				}
#endif
				if (tree->N > 0) {
					if (!N_Par.StoreInitialState) {
#ifdef underconstruction
						if (N_Par.GRASSMIND) {
							height_previous = tree->H;
							root_previous = tree->RBT;
						}
#endif
						diameter_previous = tree->D;
						biomass_previous = tree->BT;

#ifdef underconstruction
						if (N_Par.GRASSMIND)
							CalculateGrassGeometry(tree, plot);
						else
#endif
						{
							CalculateTreeGeometry_new(tree, plot);
							if ((tree->D + 0.00003) < diameter_previous) {
								tree->D = diameter_previous;
							}
							if ((tree->BT + 0.00005) < biomass_previous) {
								tree->BT = biomass_previous;
							}
						}

#ifdef underconstruction
						if (N_Par.GRASSMIND) {
							tree->HInc = (tree->H - height_previous);
							tree->BrInc = (tree->RBT - root_previous);
						}
#endif
						tree->BInc = (tree->BT - biomass_previous);
						tree->DInc = (tree->D - diameter_previous);

						Out.BINCPlot[hec->HecNo - 1][plot->No - 1] +=
							 tree->BInc * tree->N;

#ifdef underconstruction
						if (N_Par.GRASSMIND) {
							plot->Biomass_calib += tree->BT_calib * tree->N;
							if ((height_previous < Switch.Schwelle) && (tree->H >=
								 Switch.Schwelle)) {
								ProtocolIngrowth(tree);
							}
						}
						else
#endif
						{
							if ((diameter_previous < Switch.Schwelle) && (tree->D >=
								 Switch.Schwelle)) {
								ProtocolIngrowth(tree);
							}
						}
					}
				}

				plot->Biomass += tree->BT * tree->N;
				plot->R_total_biomass += tree->R * tree->N;
				plot->PB += tree->PB * tree->N;
				plot->R_total_biomass_month += tree->R * tree->N;

				if (tree->PCT == 1) {
					plot->PCTCA += tree->AC * tree->N;
				}

				plot->PB_month += tree->PB * tree->N;

				tree = tree->next;
			}
			DetermineOBA(plot);
			plot = plot->next;
		}
		hec = hec->next;
	}
}

// ----------------------------------------------------------------

/* !
 \brief       Calculates number of trees that reach the ingrowth threshold
 \param       TreePointer tree
 */
void ProtocolIngrowth(TreePointer tree) {
	Out.InTH += tree->N;
	Out.InTH_GRP[tree->Grp] += tree->N;
}

// ----------------------------------------------------------------

/* !
 \brief       Calculates for each PFT maximum size
 \details	  maximum diameter DM, maximum biomass per PFT
 and maximum biomass of tree BTM for either
 a defined maximum diameter if N_Par.Geo_FUNCTION_7[6]==0
 or a defined maximum height if N_Par.Geo_FUNCTION_7[6]==1
 */
void formindGrow::InitMaximumDiameter() {
	double max_HMmean = 0;

	for (int pft = 0; pft < MAXGRP; pft++) {
		if (N_Par.Geo_FUNCTION_7[6] == 0) {
			DM[pft] = DoutofHFunc->Calculate(N_Par.Geo_HMmean_5[pft], pft);
			BTM[pft] = BoutofDFunc->Calculate(DM[pft], N_Par.Geo_HMmean_5[pft],
				 FoutofD(DM[pft], N_Par.Pro_Rho_3[pft], pft), pft);
		}
		else {
			DM[pft] = N_Par.Geo_HMmean_5[pft];
			BTM[pft] = BoutofDFunc->Calculate(DM[pft],
				 HoutofDFunc->Calculate(DM[pft], pft),
				 FoutofD(DM[pft], N_Par.Pro_Rho_3[pft], pft), pft);
		}
	}
}

// ----------------------------------------------------------------

/* !
 \brief     In case of a non-power law D-H-curve (e.g. Michaelis-Menten),
 \details   this function initializes the required lookup vectors for diameter
 and corresponding biomass.
 */
void formindGrow::InitLookupVectors() {
	// Scaling the first dimension of the diameter and biomass lookup vectors
	// according to the number of PFTs
	vDiameterLookup.resize(MAXGRP);
	vBiomassLookup.resize(MAXGRP);
	// Scaling the second dimension of the diameter and biomass lookup vectors
	// according to the minimal diameter increment steps. The maximal possible
	// tree diameter is hardcoded to 10 m
	int maxPosition = (int)(10 / N_Par.Min_Diameter_Inc_Step);
	// Fill the lookup vectors with values
	double myDbh, myForm, myHeight, myBiomass, minDbh;
	for (int pft = 0; pft < MAXGRP; pft++) {
		for (int it_pos = 0; it_pos < maxPosition; it_pos++) {
			// Start with the ingrowth diameter as minimal possible diameter and
			// increase at each step
			minDbh = 0.01; //N_Par.Est_DS
			myDbh = minDbh + it_pos * N_Par.Min_Diameter_Inc_Step;
			// Calculate the geometries based on each diameter
			myForm = FoutofD(myDbh, N_Par.Pro_Rho_3[pft], pft);
			myHeight = HoutofDFunc->Calculate(myDbh, pft);
			myBiomass = BoutofDFunc->Calculate(myDbh, myHeight, myForm, pft);
			// Append the DBH and biomass values to the lookup vectors
			vDiameterLookup[pft].push_back(myDbh);
			vBiomassLookup[pft].push_back(myBiomass);
		}
	}
	cout << "Initialize lookup vectors" << endl;
}

// ----------------------------------------------------------------

/* !
 \brief     Calculates the coefficients of maximal growth curves
 \details 	if N_Par.Growth_Function == "chanter_maxPos"
 or N_Par.Growth_Function == "polynom"
 included in InitGrow()
 \return    void
 */
void formindGrow::CalculateGrowthCurves() {

	double dbh_small, growth_small; // smallest tress
	double dbh_peak, growth_peak; // maximum of growth curve
	double dbh_big, growth_big; // biggest trees

	double help, i;
	double a_3 = 0;
	double a_2 = 0;
	double a_1 = 0;
	double a_0 = 0;
	double h1, h2, h3, h4, h5;

	for (int pft = 0; pft < MAXGRP; pft++) {

		growth_peak = N_Par.Pro_dbh_growth_max[pft];
		growth_big = N_Par.Pro_dbh_growth_end[pft] * growth_peak;
		growth_small = N_Par.Pro_dbh_growth_start[pft] * growth_peak;

		dbh_small = N_Par.Est_DS; // diameter of seedlings at establishment
		dbh_big = DM[pft]; // maximum diameter [m]
		dbh_peak = dbh_big * N_Par.Pro_dbh_growth_maxpoint[pft];

		if (N_Par.Growth_Function == "polynom") {
			h1 = pow(dbh_peak, 4) * (-dbh_small + dbh_big);
			h2 = pow(dbh_peak, 3) * (pow(dbh_small, 2) * 2 - pow(dbh_big, 2) * 2);
			h3 = pow(dbh_peak, 2) * (pow(dbh_small, 3) * 5 + pow(dbh_big,
				 2) * dbh_small * 3 - dbh_big * pow(dbh_small, 2) * 3 +
				 pow(dbh_big, 3));
			h4 = dbh_peak * (dbh_big * pow(dbh_small, 3) * 2 - pow(dbh_big,
				 3) * dbh_small * 2);
			h5 = -pow(dbh_big, 2) * pow(dbh_small, 3) + pow(dbh_big, 3) * pow
				 (dbh_small, 2) + pow(dbh_small, 4) - pow(dbh_small, 5);
			help = h1 + h2 + h3 + h4 + h5;
			a_3 = ((-growth_small + growth_peak) * (2 * dbh_peak * (dbh_small -
				 dbh_big) - pow(dbh_small, 2) + pow(dbh_big, 2)) +
				 (growth_small - growth_big) * (2 * dbh_peak * (dbh_small -
				 dbh_peak) - pow(dbh_small, 2) + pow(dbh_peak, 2))) / help;
			a_2 = (-growth_small + growth_peak -
				 a_3 * (3 * dbh_small * pow(dbh_peak, 2) - pow(dbh_peak,
				 3) * 2 - pow(dbh_small, 3))) / (dbh_small * 2 * dbh_peak -
				 pow(dbh_peak, 2) - pow(dbh_small, 2));
			a_1 = -3 * pow(dbh_peak, 2) * a_3 - 2 * dbh_peak * a_2;
			a_0 = growth_small - pow(dbh_small, 3) * a_3 - pow(dbh_small, 2)
				 * a_2 - dbh_small * a_1;
		}

		if (N_Par.Growth_Function == "chanter_maxPos") {
			a_0 = (exp((dbh_big - 2 * dbh_peak) / (dbh_big - dbh_peak))
				 * dbh_big * growth_peak) / ((dbh_big - dbh_peak) * dbh_peak);
			a_1 = (dbh_big - 2 * dbh_peak) /
				 (dbh_big * dbh_peak - pow(dbh_peak, 2));
			a_2 = 0.;
			a_3 = 0.;
		}

		GROWTH_COEFF[pft][0] = a_0;
		GROWTH_COEFF[pft][1] = a_1;
		GROWTH_COEFF[pft][2] = a_2;
		GROWTH_COEFF[pft][3] = a_3;
	}
}

// ----------------------------------------------------------------
/* !
 \brief       tests negative growth with warning
 \return      boolean true if no error occures.
 */
bool formindGrow::TestNegativeGrowth() {
	for (int pft = 0; pft < MAXGRP; pft++) {
		double max_dbh_cm = floor(DM[pft] * 100.0 + 0.5);
		double min_dbh_cm = floor(N_Par.Est_DS * 100);
		for (int i = min_dbh_cm; i < max_dbh_cm; i++)
			if (DiamGrow->CalculateMaxGrowth(pft, i / double(100)) < 0) {
				std::string message = "negative growth in maximum growth curve. PFT:" + std::to_string(pft+1);
				MMErrorMessage(message,N_Par.ErrorType);
				return true;
			}
	}
	return false;
}

// ----------------------------------------------------------------

/* !
 \brief     	calculate maintanance respiration of a tree as a fraction of tree biomass
 either backcalculated from the maximum growth of the PFT at maximum light (standard)
 or calculated dbh-dependend if N_Par.Pro_FUNCTION_2 > 0 (only used in old projects)
 maintanance respiration is independent of light
 \param[in]   TreePointer tree
 \param[in]   PlotPointer plot
 \param[out]  mresp	maintanance respiration as fraction of tree biomass [-]
 \return      double
 */
double formindGrow::CalculateMaintResp(TreePointer tree, PlotPointer plot) {
	double mresp; // maintanance respiration as fraction of tree biomass [-]
	double photo_potential;
	// potential photosynthesis from reference climate
	double growth_potential; // potential growth from growth curve
	double biomass_potential; // potential biomass after potential growth
	double h_potential; // potential tree height after potential growth
	double f_potential; // potential tree form factor after potential growth
	double IRRw, IRRd, daylw, dayld, vpd, vpw, ref_temp_red;
	// reference climate

	const double ra = N_Par.Pro_GLoss; // growth respiration (is a constant)
	const double rho = N_Par.Pro_Rho_3[tree->Grp];

	// calculation of the maintainance respiration backcalculated from the
	// maximum growth
	if (N_Par.Pro_FUNCTION_2 == 0) {
		// if a climate module is on, a reference climate is used as defined in
		// *.par

#ifdef underconstruction
		if (N_Par.variable_Irradiance_ON) {
			IRRw = N_Par.ref_Irradiance[tree->Grp];
			IRRd = 0;
		}
		else
#endif
		{
			IRRw = N_Par.Env_IS_2[0];
			IRRd = N_Par.Env_IS_2[1];
		}

#ifdef underconstruction
		if (N_Par.Temperature_ON)
			ref_temp_red = N_Par.ref_environment_reduction[tree->Grp];
		else
#endif
			ref_temp_red = 1;

		daylw = N_Par.Env_DayL_2[0]; // daylength in hours - wet
		dayld = N_Par.Env_DayL_2[1]; // daylength in hours - dry

#ifdef underconstruction
		if (N_Par.Veg_period_ON) {
			vpw = N_Par.ref_length_of_vegetation_periode[tree->Grp] * 365;
			vpd = 0;
		}
		else
#endif
		{
			vpw = N_Par.Env_SeaL_2[0] * 365; // length of season - wet
			vpd = N_Par.Env_SeaL_2[1] * 365; // length of season - dry
		}

		// calculate potential photsysnthesis under reference climate conditions
		photo_potential = CalculatePhotoProduction_new(tree, IRRw, IRRd, daylw,
			 dayld, vpw, vpd, ref_temp_red);

		// calculate potential growth from growth curve
		growth_potential = DiamGrow->CalculateMaxGrowth(tree->Grp, tree->D);

		// restrict potential dbh to maximum dbh, if case, reduce growth
		if (tree->D + growth_potential > DM[tree->Grp])
			growth_potential = DM[tree->Grp] - tree->D;
		if (growth_potential < 0)
			growth_potential = 0;

		// calculate potential height after potential growth
		h_potential = tree->DHprefactor * HoutofDFunc->Calculate
			 (tree->D + growth_potential, tree->Grp);

		// calculate potential form factor after potential growth
		f_potential = FoutofD(tree->D + growth_potential, rho, tree->Grp);

		// calculate potential biomass after potential growth
		biomass_potential = BoutofDFunc->Calculate(tree->D + growth_potential,
			 h_potential, f_potential, tree->Grp);

		// calculate maintainance respiration
		if (tree->BT <= 0)
			mresp = 0;
		else
			mresp = (photo_potential - (biomass_potential - tree->BT) / (1 - ra))
				 / tree->BT;
		if (mresp < 0)
			mresp = 0.0;
	}
	else
		mresp = MainResp->CalculateMaintResp(tree, plot);

	return mresp;
}

// ----------------------------------------------------------------

/* !
 \brief     calculates group depending parameter a4
 (only relevant for test cases of old projects)
 */
void formindGrow::InitA4() {
	int grp;
	double IRRw, IRRd, daylw, dayld, vpd, vpw, temp_red;
	double pmax, ra, totre, dummya4, re;
	TreePointer maxtree;
	HecPointer hec;
	hec = FirstHec;

	for (grp = 0; grp < MAXGRP; grp++) {

		if (N_Par.OldVersionTest) {
			ra = N_Par.Pro_GLoss;

			IRRw = N_Par.Env_IS_2[0];
			IRRd = N_Par.Env_IS_2[1];
			daylw = N_Par.Env_DayL_2[0];
			dayld = N_Par.Env_DayL_2[1];
			temp_red = 1;
			vpw = N_Par.Env_SeaL_2[0] * 365;
			vpd = N_Par.Env_SeaL_2[1] * 365;

			float ingrowDBH;
			if(N_Par.Div_Liana[grp]==false){
				ingrowDBH = N_Par.Est_DS;
			}else{
				ingrowDBH = 0.01;
			}
			maxtree = new TREE(grp, ingrowDBH, 1, hec->FirstPlot, 0, N_Par.Div_Liana[grp]);
			maxtree->D = DM[grp];
			maxtree->BT = BTM[grp];
			maxtree->AC = SQR(CDoutofD(DM[grp], grp)) * PI / 4.0;
			maxtree->L = LAIoutofLandCD(DM[grp], maxtree->AC, grp);

			pmax = CalculatePhotoProduction_new(maxtree, IRRw, IRRd, daylw, dayld,
				 vpw, vpd, temp_red);
			re = MainResp->CalculateMaintResp(maxtree, hec->FirstPlot);
			totre = maxtree->BT * (1 - ra) * re;

			dummya4 = totre / (pmax * (1 - ra));
			if (dummya4 > 1.0)
				A4[grp] = 1.0;
			else
				A4[grp] = dummya4;
		}
		else {
			A4[grp] = 1.0;
		}
	}
}

// ----------------------------------------------------------------

/* !
 \brief       calculates biomass increase, maintainance and growth respiration
 [t ha-1 yr-1]
 \param[in]   TreePointer tree
 \param[in]   PlotPointer plot
 \param[out]  binc	biomass increament [t ha-1 yr-1]
 \return      double
 */
double formindGrow::GetBInc(TreePointer tree, PlotPointer plot) {
	// limit dbh to maximum dbh defines in parameter file
	if (!N_Par.GRASSMIND) {
		if (tree->D > DM[tree->Grp])
			tree->D = DM[tree->Grp];
	}

	// ==== calculation of maintainance respiration ====
	double mresp;
#ifdef underconstruction
	if (N_Par.GRASSMIND) {
		CalculateMaintRespGrass(plot, tree);
	}
	else
#endif
	{
		mresp = CalculateMaintResp(tree, plot);

		tree->RespMain = mresp * tree->BT;
		if (N_Par.Temperature_ON)
			tree->RespMain *= plot->temp_resp_reduction; // [t ha-1 yr-1]
	}

	// ==== calculation of growth and total respiration ====
	double rl = PBeffectOldProjects(tree);
	tree->RespGrowth = N_Par.Pro_GLoss * (tree->PB * rl - tree->RespMain);
	// growth respiration [t ha-1 yr-1]
	if (tree->RespGrowth < 0)
		tree->RespGrowth = 0;
	tree->R = tree->RespMain + tree->RespGrowth; // total respiration

	// ==== calculation of biomass increament ====
	// calculate biomass increament binc = (1-ra)*(tree->PB - tree->RespMain);
	double binc = tree->PB * rl - tree->R + tree->PB_buffer;

	// ==== reset buffer for next year ====
	// tree->PB_buffer is negative if before year's respiration exceeded photosynthesis
	tree->PB_buffer = 0.0; // reset for current year
	if ((T.D > (1. / 365 + 0.0001) && binc < 0) || (binc < 0 && N_Par.GRASSMIND))
	{
		// buffer only active for timesteps>1day
		// if T.D=1day=1./365, tree biomass can decrease
		tree->PB_buffer = binc; // buffer  of photosynthesis
		tree->RespMain = tree->PB;
		// all photosynthesis is used for respiration
		tree->RespGrowth = N_Par.Pro_GLoss * (tree->PB - tree->RespMain);
		tree->R = tree->RespMain + tree->RespGrowth;
		binc = tree->PB - tree->R;
		if (N_Par.Mort_nbinc) {
			// potential negative diameter increament, needed for mortality
			// mortality due to buffer mndinc(tree->ndinc) is increased
			tree->ndinc = (tree->D - DoutofBFunc->Calculate(tree->BT += binc,
				 tree->Grp, tree)) / tree->D;
		}
	}

	// ==== reduce biomass increament after landslide (only N_Par.Landslide) ====
#ifdef underconstruction
	if (N_Par.Landslide)
		binc *= CalculateLandslideReduceGrowth(plot, tree);
#endif
	// ==== only flag N_Par.Nogrow ====
	if (N_Par.Nogrow)
		binc = 0.0;

	return binc;
}

// ----------------------------------------------------------------

/* !
 \brief       calculates effect on photosynthesis (only needed for old projects)
 \param[in]   TreePointer tree
 \param[out]  r1	factor effect on photosynthesis
 \return      double
 */
double formindGrow::PBeffectOldProjects(TreePointer tree) {
	double rl = 1 - (pow((tree->D / DM[tree->Grp]), N_Par.Pro_Limit_3[tree->Grp])
		 * (1 - A4[tree->Grp]));
	if (rl > 1.0)
		rl = 1.0;
	return rl;
}

/* !
 \brief       captured C02 per second for photosynthesis calculation
 \details included in CalculatePhotoProduction_new()
 \param[in]   double pm		maximum leaf photosynthesis
 \param[in]   double k 		light extinction
 \param[in]   double l 		LAI  [m2 m-2]
 \param[in]   double alpha 	slope of light response curve
 \param[in]   double m 		transmission coefficient of leaves
 \param[out]  double r1		factor effect on photosynthesis (old projects)
 \return      double
 */
double formindGrow::C02persecond(double pm, double k, double l, double alpha,
	 double m, double ir) {
	const double t1 = alpha * k * ir;
	const double t2 = pm * (1 - m);

	return ((pm / k) * log((t1 + t2) / (t1 * exp(-k * l) + t2)));
}

// ----------------------------------------------------------------

/* !
 \brief  	  		  calculation photosynthesis of tree
 \param[in]  	  	  int pft
 \param[in] 		  double k
 \param[in] 		  double irwet		irradiance wet season [mumol m-2 s-1]
 \param[in] 		  double irdry 	irradiance dry season [mumol m-2 s-1]
 \param[in] 		  double daylw		day length wet [h]
 \param[in] 		  double daylw		day length dry [h]
 \param[in] 		  double vpd		vegetation period dry [d]
 \param[in] 		  double vpw		vegetation period wet [d]
 \param[in] 		  double temp_red	temperature reduction [-]
 \return[out] 		  double PB  		photosynthesis [tODM a-1]
 */
double formindGrow::CalculatePhotoProduction_new(TREE*tree, double irwet,
	 double irdry, double daylw, double dayld, double vpw, double vpd,
	 double temp_red) {

	// length of photosynthesis time [s]
	double sec2yearwet = daylw * 60 * 60 * vpw;
	double sec2yeardry = dayld * 60 * 60 * vpd;

	// calculate photosynthesis [mumolCO2 s-1 m-2]  for wet and dry season
	double ptwet = C02persecond(N_Par.Pro_Pmax_3[tree->Grp], tree->LightExtCoeff,
		 tree->L, N_Par.Pro_Alpha_3[tree->Grp], N_Par.Pro_M, irwet);
	double ptdry = C02persecond(N_Par.Pro_Pmax_3[tree->Grp], tree->LightExtCoeff,
		 tree->L, N_Par.Pro_Alpha_3[tree->Grp], N_Par.Pro_M, irdry);

	// calculate yearly photosynthesis dependend on length of photosynthesis time
	// and convert to [tODM yr-1]
	const double ps = (ptwet * sec2yearwet + ptdry * sec2yeardry)
		 * CO2_TO_ODM * 44E-12 * temp_red;

	// scale to crown area of tree
	return tree->AC * ps;
}

// ----------------------------------------------------------------

/* !
 \brief  	  		  Calculates stem volume out of diameter
 \param[in] 		  double d  	dbh [m]
 \param[in] 		  double h 		tree height [m]
 \param[in] 		  double f     form factor  [-]
 \return[out] 		  double sv    stem volume [m3]
 */
double formindGrow::SVoutofD(double d, double h, double f) {
	double sv = f * (PI / 4.0) * SQR(d) * h; // stem volume sv
	return sv;
}

// ----------------------------------------------------------------

/* !
 \brief  	  		  Calculates bole volume for logging, part of total stem
 volume
 \param[in]  	  	  double sv    stem volume [m3]
 \param[in] 		  double clp  	crown length proportion [-]
 \param[in] 		  double f     form factor  [-]
 \return[out] 		  double bv    bole volume [m3]
 */
double formindGrow::SVLoggingoutofD(double sv, double clp, double f) {
	double rad = (3 * f) - (3.0 / 4.0); // radicant of model
	if (rad < 0)
		rad = 0;

	double c = 1 - ((1 - clp) * ((3.0 / 2.0) - sqrt(rad)));
	double bv = (1 / (3 * f)) * (1 + c + SQR(c)) * (1 - clp) * sv;
	// bole volume

	return bv;
}

// ----------------------------------------------------------------

/* !
 \brief  	  		  Calculates commercial bole volume for logging: part of trunk
 from bottom to crown base height. BV = ba * (1-clp) * h * f
 = sv * (1-clp)
 \param[in]  	  	  double sv    stem volume [m3]
 \param[in] 		  double clp  	crown length proportion [-]
 \param[in] 		  double f     form factor  [-]
 \return[out] 		  double bv    bole volume [m3]
 */
double formindGrow::CommercialBoleVolumeOutofD(double sv, double clp, double f)
{
	double bv = sv * (1 - clp);

	return bv;
}

// ----------------------------------------------------------------

/* !
 \brief  	  		  Calculates crown diameter out of stem diamter
 \param[in]  	  	  int pft
 \param[in] 		  double d  	dbh [m]
 \return[out] 		  double cd    crown diameter [m]
 */
double formindGrow::CDoutofD(double d, int pft) {
	double cd;
	double cd0 = N_Par.Geo_CD_31[0][pft];
	double cd1 = N_Par.Geo_CD_31[1][pft];
	double cd2 = N_Par.Geo_CD_31[2][pft];
	int which = N_Par.Geo_FUNCTION_7[2];

	// calculate crown diameter with different functions which
	if (which == 0)
		cd = cd0 * d;
	else if (which == 1)
		cd = cd0 * d + cd1 * SQR(d);
	else if (which == 2)
		cd = d / (1 / cd0 + d / cd1);
	else if (which == 3)
		cd = cd0 * d + cd1 * SQR(d) + cd2 * pow(d, 3);
	else if (which == 4)
		cd = (cd0 + cd1 * exp(-cd2 * d)) * d;
	else if (which == 5)
		cd = cd0 * d + cd1 * exp(-cd2 * d);
	else if (which == 6)
		cd = cd0 * d + cd1;
	else if (which == 7)
		cd = cd0 * d / (1 + cd0 * cd1 * d); // holling type II function
	else if (which == 8)
		cd = cd0 * pow(d, cd1) - cd2; // powerlaw function
	else {
			MMErrorMessage("the choosen CDoutofD-function doesn�t exists", N_Par.ErrorType);
	}
	return cd;
}

// ----------------------------------------------------------------

/* !
 \brief  	  		  Calculates leaf area index out of stem diamter
 \param[in]  	  	  int pft
 \param[in] 		  double d  	dbh [m]
 \param[in] 		  double ac 	crown area [m2]
 \return[out] 		  double lai   leaf area index [m2m-2]
 */
double formindGrow::LAIoutofLandCD(double d, double ac, int pft) {
	double lai;
	int which = N_Par.Geo_FUNCTION_7[8];
	double l0 = N_Par.Geo_LAIT_21[0][pft];
	double l1 = N_Par.Geo_LAIT_21[1][pft];
	double l2 = N_Par.Geo_LAIT_21[2][pft];

	// calculate lai with different functions which
	if (which == 0)
		lai = l0 * pow(d, l1); // powerlaw
	else if (which == 1)
		lai = l0 + l1 * d; // linear
	else if (which == 2) {
		lai = (l0 + l1 * d + l2 * SQR(d)) / ac;
		if (lai > MAXLAI)
			lai = MAXLAI;
		// bounded by MAXLAI, which is set to 3 (InitConst)
	}
	else if (which == 3) {
		lai = (l0 * d + l1 * SQR(d) + l2 * pow(d, 3)) / ac;
		if (lai > MAXLAI)
			lai = MAXLAI;
		// bounded by MAXLAI, which is set to 3 (InitConst)
	}
	return lai;
}

// ----------------------------------------------------------------

/* !
 \brief  	  		  Calculates form factor out of stem diamter
 \param[in]  	  	  int pft
 \param[in] 		  double d  	dbh [m]
 \param[in] 		  double rho 	wood density []
 \return[out] 		  double ff   	form factor [-]
 */
double formindGrow::FoutofD(double d, double rho, int pft) {
	double ff;
	int which = N_Par.Geo_FUNCTION_7[3];
	double f0 = N_Par.Geo_FD_31[0][pft];
	double f1 = N_Par.Geo_FD_31[1][pft];
	double f2 = N_Par.Geo_FD_31[2][pft];

	// calculate ff with different functions which
	if (which == 0)
		ff = f0 * exp(f1 * pow(d, f2));
	else if (which == 1)
		ff = f0 * pow(d, f1);
	else {
			MMErrorMessage("the choosen FoutofD-function doesn�t exists", N_Par.ErrorType);
	}
	return ff;
}

// ----------------------------------------------------------------

/* !
 \brief  	  		  Calculates crown length proportion out of height
 \param[in]  	  	  int 			pft
 \param[in] 		  double h  	tree height [m]
 \return[out] 		  double clp   crown length proportion [-]
 */
double formindGrow::CLPoutofH(double h, int pft) {
	double clp;
	int which = N_Par.Geo_FUNCTION_7[4];
	double c0 = N_Par.Geo_CLFH_31[0][pft];
	double c1 = N_Par.Geo_CLFH_31[1][pft];
	double c2 = N_Par.Geo_CLFH_31[2][pft];

	// calculate clp with different functions which
	if (which == 0)
		clp = -c0 * c1 * h / (c0 * h + c1) + c2;
	else if (which == 1)
		clp = c0 + c1 * h + c2 * SQR(h);
	else if (which == 2)
		clp = c0;
	else if (which == 3)
		clp = c0 * pow(h, c1);
	else {
		MMErrorMessage("the choosen CLPoutofD-function doesn�t exists", N_Par.ErrorType);
	}

	return clp;
}




// ----------------------------------------------------------------

/* !
 \brief	initialize lookup table biomass to diameter
 (only needed for old projects)
 */
void formindGrow::InitB2D(void) {
	for (int grp = 0; grp < MAXGRP; grp++) {
		B2D[0][grp] = 0.0;
		for (int i = 1; i < MAXB2D; i++) {
			double d = i / (double)B2DSTEP;
			double h = forGrow.HoutofDFunc->Calculate(d, grp);
			double f = forGrow.FoutofD(d, N_Par.Pro_Rho_3[grp], grp);
			B2D[i][grp] = forGrow.BoutofDFunc->Calculate(d, h, f, grp);
		}
	}
}

// ----------------------------------------------------------------

/* !
 \brief  	  		  Calculates tree geometry
 \param[in]  	  	  pointer 			tree
 \param[in]  	  	  pointer 			tree
 */
void formindGrow::CalculateTreeGeometry_new(TreePointer tree, PlotPointer plot)
{
	tree->AGE += T.D;

	double binc_yr = GetBInc(tree, plot); // biomass increament [a-1]
	if ((tree->BT + (binc_yr * T.D)) > BTM[tree->Grp]) {
		binc_yr = (BTM[tree->Grp] - tree->BT) / T.D;
		tree->PB = binc_yr / (1 - N_Par.Pro_GLoss) + tree->RespMain;
		tree->RespGrowth = N_Par.Pro_GLoss * (tree->PB - tree->RespMain);
		tree->R = tree->RespGrowth + tree->RespMain;
	}

	tree->BT += binc_yr * T.D;
	tree->RBT = N_Par.Geo_RB * tree->BT * N_Par.Geo_TR_21[0][tree->Grp];
	// root biomass [t ha-1]
	// root biomass as fraction of stem biomass (N_Par.Geo_RB)
	// stem biomass as a fraction of total biomass (N_Par.Geo_TR_21[0][tree->Grp])

	double d = DoutofBFunc->Calculate(tree->BT, tree->Grp, tree);
	if (d < DM[tree->Grp])
		tree->D = d;
	else
		tree->D = DM[tree->Grp];

	tree->H = tree->DHprefactor * HoutofDFunc->Calculate(tree->D, tree->Grp);
	tree->F = FoutofD(tree->D, N_Par.Pro_Rho_3[tree->Grp], tree->Grp);
	tree->CLP = CLPoutofH(tree->H, tree->Grp);
	tree->AC = PI * SQR(CDoutofD(tree->D, tree->Grp)) / 4.0;

#ifdef underconstruction
	//for the case tree is a liana
	if (tree->isLiana && tree->attachedTree!=NULL) {
		UpdateLianaGeometry(tree);
	}
#endif

	if ((tree->CLP > 1) || (tree->CLP < 0)) {
		std::string message =
			"CLP out of range (0-1), but corrected to the closer edge of the range. CLP = " +
			std::to_string(tree->CLP) + "; PFT" + std::to_string(tree->Grp + 1)
			+ "; Plot = " + std::to_string(plot->No) +
			"; Time = " + std::to_string(T.T);
		MMErrorMessage(message, N_Par.WarningType);

		if (tree->CLP < 0)
			tree->CLP = 0.0;
		if (tree->CLP > 1)
			tree->CLP = 1.0;
	}


	tree->L = LAIoutofLandCD(tree->D, tree->AC, tree->Grp);

	tree->SV = SVoutofD(tree->D, tree->H, tree->F);
	if ((tree->SV < 0.0) || (tree->SV > 100.0)) {
		std::string message =
			"SV (stem volume) out of range (0-100), NOT corrected to the closer edge of the range. SV = " +
			std::to_string(tree->SV) + "; PFT = " + std::to_string
			(tree->Grp + 1) + "; Plot = " + std::to_string(plot->No) +
			"; Time = " + std::to_string(T.T);
		MMErrorMessage(message, N_Par.WarningType);
	}

	tree->SVLogging = SVLoggingoutofD(tree->SV, tree->CLP, tree->F);
	if ((tree->SVLogging < 0.0) || (tree->SVLogging > 100.0)) {
		std::string message =
			"Bole volume (logging) out of range (0-100), NOT corrected to the closer edge of the range. SV = " +
			std::to_string(tree->SVLogging) + "; PFT" + std::to_string
			(tree->Grp + 1) + "; Plot = " + std::to_string(plot->No) +
			"; Time = " + std::to_string(T.T);
	}

	tree->BoleVolume = CommercialBoleVolumeOutofD(tree->SV, tree->CLP, tree->F);
	if ((tree->BoleVolume < 0.0) || (tree->BoleVolume > 100.0)) {
		std::string message =
			"Bole volume out of range (0-100), NOT corrected to the closer edge of the range. BV = " +
			std::to_string(tree->BoleVolume) + "; PFT" + std::to_string
			(tree->Grp + 1) + "; Plot = " + std::to_string(plot->No) +
			"; Time = " + std::to_string(T.T);
		MMErrorMessage(message, N_Par.WarningType);
	}
	}

// ----------------------------------------------------------------

/*!
 \brief 		Calculate Overtopping Basal Area of own plot
 \param		PlotPointer plot
 \details  	Attention this function takes 25% of total cpu time
 1. OBA is indicator for concurrence situation
 2. OBA is ha-normed, and leads to very high values ~18,
 */
void formindGrow::DetermineOBA(PlotPointer plot) {
	TreePointer tree, xtree;

	if (myResultFileSwitch.res || myResultFileSwitch.res_th || myResultFileSwitch.res_th_bin) {
		tree = plot->FirstTree;
		while (tree != NULL) {
			tree->OBA = 0.0;
			xtree = plot->FirstTree;
			while (xtree != NULL) {
				if ((xtree->H) > tree->H) { // shading if tree is bigger
					if ((xtree->H - tree->H) > (xtree->CLP*xtree->H))
						tree->OBA += (xtree->L*xtree->AC / plot->Area);
					else
						tree->OBA += (xtree->L*xtree->AC / plot->Area)*
							 (xtree->H - tree->H) / (xtree->CLP*xtree->H);
				}
				xtree = xtree->next;
			}
			tree = tree->next;
		}
	}
}

// -----------------------------------------------------------
// ------------ end of for_grow.cc ---------------------------
// -----------------------------------------------------------
