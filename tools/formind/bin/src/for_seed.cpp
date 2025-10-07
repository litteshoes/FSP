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
// File         			 for_seed.cpp
// Description  	       In this file are all functions related to seed dispersal,
// establishment and mortality.
//
///////////////////////////////////////////////////////////////////

#include "for_grow.h"
#include "for_seed.h"
#ifdef underconstruction
#include "for_landslide.h"
#include "grass/grass_seed.h"
#include "for_log.h"
#include "for_fragmentation.h"
#include "for_predate.h"
#include "for_species.h"
#endif
#include "for_misc.h"
#include "MMParSet/MMErrorMessage.h"

ForSeed for_seed;

ForSeed::ForSeed() : PULSYEAR(5)

{

}

// -----------------------------------------------------------

/* !
 \brief			Adds one Tree element to the list
 \param	      plot
 \param	      tree
 \param		  	pft - specie
 \param		  	seedlings - number of trees
 \param		  	com - commercial use
 \return       newtree
 */
TreePointer ForSeed::ExpandTreeRecord(PlotPointer plot, TreePointer tree,
	 int pft, int seedlings, int com) {
	if (plot->StemNumber >= N_Par.Div_MAXTREE) {
			std::string message =
							" can not expand Tree List, as maximum List length of Plot is reached. T.T = " +
							std::to_string(T.T) + "; Plot = "
							+ std::to_string(plot->No)+ "; TreeNo: " +std::to_string(plot->StemNumber);
						MMErrorMessage(message, N_Par.ErrorType);
	}
	else {
		float ingrowDBH;
		if(N_Par.Div_Liana[pft]==false){
			ingrowDBH = N_Par.Est_DS;
		}else{
			ingrowDBH = 0.01;
			//lianas grow in with 1 cm dbh
			//could be different to trees
			//maybe later construct an input parameter for that
		}

		TreePointer newtree = new TREE(pft, ingrowDBH, seedlings, plot, com, N_Par.Div_Liana[pft]);

		if (newtree == NULL) {
			MMErrorMessage("can't detect enough memory for tree", N_Par.ErrorType);
		}
		newtree->next = NULL;
		if (tree != NULL) {
			while (tree->next != NULL)
				tree = tree->next;
			tree->next = newtree;
		}
		else
			plot->FirstTree = newtree;
		return newtree;
	}
	return NULL;
}

/* !
 \brief			initialisation of dispersal kernel function
 \return       distance_from_distribution[pft]
 */
void ForSeed::Init() {
	PULSCOUNT = 0;
	PULS = true;
	distance_from_distribution.resize(N_Par.Div_MAXGRP, NULL);

	for (int pft = 0; pft < N_Par.Div_MAXGRP; pft++) {
		switch (N_Par.DispersalKernel[pft]) {
		case 1:
			distance_from_distribution[pft] =
				 new DistancefromDistribution_Weibull();
			break;
		case 2:
			distance_from_distribution[pft] =
				 new DistancefromDistribution_Linear();
			break;
		case 3:
			distance_from_distribution[pft] =
				 new DistancefromDistribution_Uniform();
			break;
		case 4:
			distance_from_distribution[pft] =
				 new DistancefromDistribution_Exponential
				 (N_Par.MeanSeedDistance[pft]);
			break;
		case 5:
			distance_from_distribution[pft] =
				 new DistancefromDistribution_Gaussian(N_Par.MeanSeedDistance[pft]);
			break;
		default:
			std::string message = "unknown dispersal mode for PFT" +
				std::to_string(pft + 1);
			MMErrorMessage(message, N_Par.ErrorType);
		}
	}
}

/* !
 \brief			deconstructor of dispersal kernel function
*/
ForSeed::~ForSeed() {
	for (int pft = 0; pft < distance_from_distribution.size(); pft++)
		if (distance_from_distribution[pft] != NULL)
			delete distance_from_distribution[pft];
}

/* !
 \brief	Reads global seed number from parameter data
 \param	hec
 \return	void
 */
void ForSeed::DetermineGlobalSeedNumber(HecPointer hec) {
	for (int pft = 0; pft < MAXGRP; pft++) {
		double seedinput = N_Par.Est_NS_3[pft];

#ifdef underconstruction
		if (N_Par.Invasion[pft] && T.T < N_Par.TimeInvasion)
			seedinput = 0;
		if (N_Par.Predation)
			seedinput = CalculatePredateSeed(seedinput, pft);
#endif

		if (T.D >= 1)
			hec->globalseed[pft] = (int) floor(seedinput * T.D + 0.5);
		else
			hec->globalseed[pft] = (int) floor(seedinput + 0.5);

		if (N_Par.Puls)
			hec->globalseed[pft] *= PULSYEAR;
	}
}

/* !
 \brief	calls DetermineSeedGermination_standard(plot)
 \param	plot
*/
bool ForSeed::DetermineSeedGermination(PlotPointer plot) {

#ifdef underconstruction
	if (N_Par.spatial) {
		DetermineSeedGermination_spatial(plot);
	}
	else
#endif
	{
		DetermineSeedGermination_standard(plot);
		// new trees from seeds (global seeds + seed dispersal)

#ifdef underconstruction
		if (N_Par.Flag_SpeciesNumber)
			DetermineSeedGermination_speciesNumber(plot);
#endif
	}

	return true;
}
/* !
 \brief	main function of establishment
*/
void ForSeed::DoEstablishment(void) {

	DeterminePulsedEstablishment();

	HecPointer hec = FirstHec;
	while (hec != NULL) {
		if (N_Par.Globalseeds) {
			DetermineGlobalSeedNumber(hec);
		}
		PlotPointer plot = hec->FirstPlot;
		while (plot != NULL) {

#ifdef underconstruction
			if (N_Par.Landslide) {
				ReduceSeeds(plot, hec);
			}

			if ((N_Par.GRASSMIND && N_Par.Century_ON) || N_Par.Candy_ON)
				CalculateNRecruit(plot);
#endif

			if ((!N_Par.StoreInitialState)) {
				if (T.BeginOfYear(N_Par.GRASSMIND) && PULS) {

					RearangeSeedPool(plot);
					DetermineSeedGermination(plot);

					if (N_Par.Seedtree) {
#ifdef underconstruction
						if (N_Par.Flag_SpeciesNumber)
							InitSeedTreePlot(plot);
#endif
						ThrowSeeds(plot);
					}

					if (N_Par.Globalseeds) {
						for (int pft = 0; pft < MAXGRP; pft++) {
							hec->globalseed[pft] -=
								 ThrowGlobalSeeds2(hec, plot, pft,
								 hec->globalseed[pft]);
						}
					}

					if (!N_Par.GRASSMIND)
						SeedPoolMortality(plot);
				}
			}

			plot = plot->next;
		}
		hec = hec->next;
	}
}

// -----------------------------------------------------------


/* !
 \brief	Initializes vectors for seedtree if species number is activated
 \param	plot
*/
void ForSeed::InitSeedTreePlot(PlotPointer plot) {
	plot->NewSeedsMotherTreePFT.clear();
	plot->NewSeedsMotherTreeCOM.clear();
	plot->NewSeedsMotherTreeSPECIES.clear();
	plot->NewSeedsPosition[0].clear();
	plot->NewSeedsPosition[1].clear();
	plot->NewSeedsWithSpeciesNumber = 0;
}

/* !
 \brief	Establishes seedlings according to pulse year.
 \return	Puls - Pulsed ingrowth of seeds in this year
*/
void ForSeed::DeterminePulsedEstablishment() {
	if (N_Par.Puls) {
		PULSCOUNT++;
		if (PULSCOUNT >= PULSYEAR) {
			PULS = true;
			PULSCOUNT = 0;
		}
		else
			PULS = false;
	}
	else
		PULS = true;
}

/* !
 \brief	Distributes seeds to target plots clumped if seedtree is activated
 \details comparable to function below - only for saving computational time
 \param	TreePointer mothertree
 \param	treex - x position of tree
 \param	treey - y position of tree
 \param	maxdistance - maximum distance of seed dispersal
 \param	meandistance - mean distance of seed dispersal
 \param	seed_i - number of seeds
 \param	crowndiameter
*/
void ForSeed::DistributeSeeds_clumped(TreePointer mothertree, double treex,
	 double treey, double maxdistance, double meandistance, int seed_i,
	 double crowndiameter) {

	int pft = mothertree->Grp;
	int com = mothertree->COMGrp;

	HecPointer hec;
	PlotPointer plot;

	double plotside = Switch.Hecside / sqrt((double)Switch.Maxplot);
	int max_patchdistance = floor(maxdistance / plotside);
	int length = (int)(max_patchdistance);
	std::vector<int>seeded_patches;
	int veclength = 8 * ((length * (length + 1)) / 2);
	seeded_patches.assign(veclength, 0);
	int count = 0;
	int n = 0;
	int sumtot = 0;
	int tempseed = 1;
	int restseed;

	while (tempseed > 0) {
		int direction = _NRandom(360);
		// using the 8-neighborhood
		for (int i = 0; i <= (8 * n); i++) {
			double xf, yf;
			double distance = n * (Switch.Hecside / sqrt((double)Switch.Maxplot));

			if (N_Par.Closed_boundary)
				ForTools::DetermineFallLoc(direction, treex, treey, distance,
				 &xf, &yf);
			else
				ForTools::DetermineFallLoc_noclosedboundary(direction, treex, treey,
				 distance, &xf, &yf);

			if ((xf >= Loc.XMin) && (xf < Loc.XMax) && (yf >= Loc.YMin) && (yf <
				 Loc.YMax)) {
				hec = ForTools::FindHectar(xf, yf);
				plot = ForTools::FindPlot(hec, xf, yf);

#ifdef underconstruction
				if (N_Par.Predation) {
					hec = ForTools::FindHectar(treex, treey);
					plot = DoAnimalDispersion(hec, plot, mothertree, treex, treey);
				}
#endif
			}

			if (ForTools::isnotseeded(plot->No, seeded_patches, count)) {
				if (n == 0) {
					tempseed =
						 floor(seed_i * exp((-1 / meandistance) * 20 * n) -
						 seed_i * exp((-1 / meandistance) * 20 * (n + 1)));
					plot->NewSeeds[pft][com] += tempseed;
					sumtot += tempseed;

				}
				else {
					tempseed =
						 floor((seed_i * exp((-1 / meandistance) * 20 * n) -
						 seed_i * exp((-1 / meandistance) * 20 * (n + 1))) / (8 * n));
					if (tempseed > 0) {
						plot->NewSeeds[pft][com] += tempseed;
						sumtot += tempseed;
					}
				}
				seeded_patches[count] = plot->No;
				count++;
			}

			if (tempseed == 0)
				break;

			if (n > 0)
				direction += 360 / (8*n);
		}
		n++;
		if (n >= max_patchdistance)
			break;
	}

	restseed = seed_i - sumtot;

	if (restseed > 0) {
		for (int i = 0; i < restseed; i++) {
			double xf, yf;

			int direction = _NRandom(360);
			double distance = distance_from_distribution[pft]->Get(maxdistance,
				 crowndiameter);

			if (N_Par.Closed_boundary)
				ForTools::DetermineFallLoc(direction, treex, treey, distance,
				 &xf, &yf);
			else
				ForTools::DetermineFallLoc_noclosedboundary(direction, treex, treey,
				 distance, &xf, &yf);

			if ((xf >= Loc.XMin) && (xf < Loc.XMax) && (yf >= Loc.YMin) && (yf <
				 Loc.YMax)) {
				hec = ForTools::FindHectar(xf, yf);
				plot = ForTools::FindPlot(hec, xf, yf);

#ifdef underconstruction
				if (N_Par.Predation) {
					plot = DoAnimalDispersion(hec, plot, mothertree, treex, treey);
				}
#endif
				plot->NewSeeds[pft][com] += 1;

#ifdef underconstruction
				if (N_Par.Flag_SpeciesNumber) {
					CalculateNewSeeds(plot, mothertree, pft, com, xf, yf);
				}
#endif
			}
		}
	}
}

/* !
 \brief	Distributes seeds to target plots if seedtree is activated
 \param	TreePointer mothertree
 \param	treex - x position of tree
 \param	treey - y position of tree
 \param	maxdistance - maximum distance of seed dispersal
 \param	meandistance - mean distance of seed dispersal
 \param	seed_i - number of seeds
 \param	crowndiameter
 \return void
*/
void ForSeed::DistributeSeeds_single(TreePointer mothertree, double treex,
	 double treey, double maxdistance, double meandistance, int seed_i,
	 double crowndiameter) {

	int pft = mothertree->Grp;
	int com = mothertree->COMGrp;

	HecPointer hec;
	PlotPointer plot;

	for (int i = 0; i < seed_i; i++) {
		double xf, yf;

		int direction = _NRandom(360);
		double distance = distance_from_distribution[pft]->Get(maxdistance,
			 crowndiameter);

		if (N_Par.Closed_boundary)
			ForTools::DetermineFallLoc(direction, treex, treey, distance,
			 &xf, &yf);
		else
			ForTools::DetermineFallLoc_noclosedboundary(direction, treex, treey,
			 distance, &xf, &yf);

		if ((xf >= Loc.XMin) && (xf < Loc.XMax) && (yf >= Loc.YMin) && (yf <
			 Loc.YMax)) {
			hec = ForTools::FindHectar(xf, yf);
			plot = ForTools::FindPlot(hec, xf, yf);

#ifdef underconstruction
			if (N_Par.Predation) {
				hec = ForTools::FindHectar(treex, treey);
				plot = DoAnimalDispersion(hec, plot, mothertree, treex, treey);
			}
#endif

			plot->NewSeeds[pft][com] += 1;

#ifdef underconstruction
			if (N_Par.Flag_SpeciesNumber) {
				CalculateNewSeeds(plot, mothertree, pft, com, xf, yf);
			}
#endif
		}
	}
}

/* !
 \brief	Distributes seeds to target plots if seedtree is activated
 \param	TreePointer mothertree
 \param	seeds - number of seeds
 */

void ForSeed::DistributeSeeds2TargetPlots_randomly(TreePointer mothertree,
	 double seeds) {

	const double maxdistance = N_Par.Est_Dist_3[mothertree->Grp];
	const double meandistance = N_Par.MeanSeedDistance[mothertree->Grp];
	const double crowndiameter = forGrow.CDoutofD(mothertree->D,
		 mothertree->Grp);
	double treex = mothertree->absX();
	double treey = mothertree->absY();

	int pft = mothertree->Grp;
	int com = mothertree->COMGrp;

	HecPointer hec;
	PlotPointer plot;

	int seed_i = mothertree->N * (int)floor(seeds);

#ifdef underconstruction
	if (N_Par.Patch_Seed) { // block dispersal of seeds in patches
		DistributeSeeds_clumped(mothertree, treex, treey, maxdistance,
			 meandistance, seed_i, crowndiameter);
	}
	else
#endif
	{
		DistributeSeeds_single(mothertree, treex, treey, maxdistance,
			 meandistance, seed_i, crowndiameter);

	}
}

// -----------------------------------------------------------

/* !
 \brief	Calculates the actual number of thrown seeds from parameter SeedTree
 \param	seedpertree - number of seeds
 */

int ForSeed::GetSeedNumber(double seedpertree) {
	if (N_Par.Puls)
		seedpertree *= PULSYEAR;

#ifdef underconstruction
	if (N_Par.Frag_lowseed)
		seedpertree *= CalculateReducedSeeds();
#endif

	if (!N_Par.GRASSMIND && ForTools::FractionalDice(seedpertree))
		seedpertree += 1.0;

	return (std::max)((int)(seedpertree + 0.5), 0);
}

// -----------------------------------------------------------

/* !
 \brief	Transfers newly dispersed seeds into seedpool
 \param	PlotPointer plot
 \return void
 */

void ForSeed::RearangeSeedPool(PlotPointer plot) {

	for (int pft = 0; pft < MAXGRP; pft++) {
		for (int com = 0; com < N_COMMERCIAL; com++) {

			Out.SEEDRAINandGRP[pft] += plot->NewSeeds[pft][com];
			Out.SEEDRAINandCOM[com] += plot->NewSeeds[pft][com];
			Out.SEEDRAINSUM += plot->NewSeeds[pft][com];

			double addseeds = plot->NewSeeds[pft][com];

#ifdef underconstruction
			if (N_Par.Predation) {
				double predation = DoPostPredationSeedRain(plot, pft, com);
				addseeds *= (1.0 - predation);
			}
#endif

			plot->SeedPool[pft][com] += addseeds;
			plot->Seeds += addseeds;

#ifdef underconstruction
			if (N_Par.Predation) {
				DoPostPredationSeedPool(plot, pft, com);
			}
#endif

			plot->NewSeeds[pft][com] = 0;
		}
	}
}

// -----------------------------------------------------------

/* !
 \brief	Calculates seed mortality
 \param	PlotPointer plot
 \return plot->SeedPool - number of seeds per pft in seedPool
 */
void ForSeed::SeedPoolMortality(PlotPointer plot) {
	double mortality;

	for (int pft = 0; pft < MAXGRP; pft++) {

#ifdef underconstruction
		if (N_Par.GRASSMIND)
			mortality = 0.0;
		else
#endif
			mortality = N_Par.Est_SeedMort_3[pft];

		if (T.D >= 1.)
			mortality = 1. - pow(1. - mortality, T.D);
		else
			mortality *= T.D;
		if (mortality > 1.0)
			mortality = 1.0;

		if (mortality > 0.)
			for (int com = 0; com < N_COMMERCIAL; com++) {
				if (plot->SeedPool[pft][com] > 0) {

					int dying = (int)(plot->SeedPool[pft][com] * mortality);
					double rest = (plot->SeedPool[pft][com] * mortality) - dying;

					if (rest > DELTA && _Random() < rest)
						dying++;
					if (plot->SeedPool[pft][com] >= dying) {
						if (plot->Seeds >= dying)
							plot->Seeds -= dying;
						else {
							std::string message =
								" more seeds dying than in Seedpool. Corrected to total number of seeds in Seedpool. dying =" +
								std::to_string(dying) + "; PFT" +
								std::to_string(pft + 1) +
								"; SeedPool[pft][com]" + std::to_string
								(plot->SeedPool[pft][com]) + "; Time = " +
								std::to_string(T.T);
							MMErrorMessage(message, N_Par.WarningType);
							plot->Seeds = 0;
						}

						plot->SeedPool[pft][com] -= dying;
					}
					else {
						if (plot->Seeds >= plot->SeedPool[pft][com])
							plot->Seeds -= plot->SeedPool[pft][com];
						else {
							std::string message =
								" more seeds dying than in Seedpool. Corrected to total number of seeds in Seedpool. dying =" +
								std::to_string(dying) + "; PFT" +
								std::to_string(pft + 1) +
								"; SeedPool[pft][com]" + std::to_string
								(plot->SeedPool[pft][com]) + "; Time = " +
								std::to_string(T.T);
							MMErrorMessage(message, N_Par.WarningType);
							plot->Seeds = 0;
						}
						std::string message =
								" more seeds dying than in Seedpool. Corrected to total number of seeds in Seedpool. dying =" +
								std::to_string(dying) + "; PFT" +
								std::to_string(pft + 1) +
								"; SeedPool[pft][com]" + std::to_string
								(plot->SeedPool[pft][com]) + "; Time = " +
								std::to_string(T.T);
							MMErrorMessage(message, N_Par.WarningType);
						plot->SeedPool[pft][com] = 0;
					}
				}
			}
	}
}

/* !
 \brief	Distributes seeds to target plots if Globalseeds is activated
 \param	   HecPointer hec
 \param		PlotPointer plot
 \param	   pft
 \param	   globalseed - number of seeds
 \return    seed - number of seeds
 */

int ForSeed::ThrowGlobalSeeds2(HecPointer hec, PlotPointer plot, int pft,
	 int globalseed) {

	int seed = 0, dice, rest, hecseeds;
	int dummy, count;
	double ddice;
	float seedinput;

#ifdef underconstruction
	if (N_Par.Predation)
		seedinput = CalculatePredateSeed2(pft);
	else
#endif
		seedinput = N_Par.Est_NS_3[pft];

#ifdef underconstruction
	if (N_Par.Invasion[pft] && T.T < N_Par.TimeInvasion)
		seedinput = 0;
#endif

	if (T.D >= 1)
		hecseeds = (int) floor(seedinput * T.D + 0.5);
	else
		hecseeds = (int) floor(seedinput + 0.5); ;

#ifdef underconstruction
	if (N_Par.Landslide) {
		hecseeds = CalculateLandslideSeed(plot, pft, hecseeds);
	}
#endif

	if (plot->No == 25) {
		seed = globalseed;
	}
	else {
		rest = hecseeds - hecseeds / Switch.Maxplot * Switch.Maxplot;
		// division is done first.
		// afterwards (hecseeds/Switch.Maxplot) is rounded to an integer value
		seed = hecseeds / Switch.Maxplot;
		for (int i = 0; i < rest; i++) {
			dice = _NRandom(Switch.Maxplot);
			// rest seeds will be dispersed with a 1/plot number probability
			if ((dice < 1.0) && (seed < globalseed))
				seed++;
		}
	}
	if (seed < 0)
		seed = 0;
	dummy = seed;
	count = 0;

#ifdef underconstruction
	if (N_Par.Landslide) {
		dummy = CalculateLandslideRecruit(plot, pft, dummy);
	}
#endif

	// Commerical groups (0: not commerical; 0-1: partly commercial; 1: commercial)
	for (int c = 0; c < dummy; c++) {
		ddice = _Random();
		if (ddice < N_Par.Div_COMMERCIAL_A[pft])
			count++; // commercial (1)
	}
	dummy -= count; // noncommercial (0)
	plot->NewSeeds[pft][0] += dummy;
	plot->NewSeeds[pft][1] += count;

#ifdef underconstruction
	if (N_Par.Landslide) {
		seed = CalculateLandslideRecruit(plot, pft, seed);
	}
#endif

	return seed;
}

// -----------------------------------------------------------

/* !
 \brief		Calculates dispersal of seeds from mother trees to seedpool
 \param		PlotPointer plot
 \return 	void
 */

void ForSeed::ThrowSeeds(PlotPointer plot) {

	TreePointer tree = plot->FirstTree;
#ifdef underconstruction
	if (N_Par.GRASSMIND) {
		CalculateGrassSeeds(plot, tree);
	}
	else
#endif
	{
		while (tree != NULL) {
			if (tree->D >= N_Par.Est_DSTree_5[tree->Grp] && tree->N > 0) {

				double seedspertree = N_Par.Est_SeedTree_3[tree->Grp];

#ifdef underconstruction
				if (N_Par.Predation)
					seedspertree = SeedsPerMotherTree(plot, tree, seedspertree);
#endif

				double nseeds = GetSeedNumber(seedspertree) * pow
					 ((N_Par.Lin_Dis * ((tree->Grp) + 1)), N_Par.HGRP_EXP);

				DistributeSeeds2TargetPlots_randomly(tree, nseeds);
			}
			tree = tree->next;
		}
	}
}

// -----------------------------------------------------------

/* !
 \brief	Calculates Seedling input depending on season, light and others
 \param	      irel_wet - irradiance for wet season
 \param	      irel_dry - irradiance for dry season
 \param	      imin - minimum required irradiance
 \param	      vpw - length of wet season
 \param	      vpd - length of dry season
 \param	      seedinput - number of seeds
 \return       in - realised number of seeds
 */

long ForSeed::CalculateSeedlingInput(double irel_wet, double irel_dry,
	 double imin, double vpw, double vpd, double seedinput) {
	int in;
	double total, part1, part2;

	if (irel_wet >= imin)
		part1 = vpw;
	else
		part1 = 0.0;

	if (irel_dry >= imin)
		part2 = vpd;
	else
		part2 = 0.0;

	total = part1 + part2;

	if (total > 1.0) {
		std::string message = "Seedling input larger than 100%; T.T = " +
			std::to_string(T.T);
		MMErrorMessage(message, N_Par.ErrorType);
	}

	in = (int) floor(total * seedinput + 0.5);

	return in;
}

// -----------------------------------------------------------

/* !
 \brief	Checks if in that plot and pft seeds are available
 \param	     PlotPointer plot
 \param		  pft
 \param		  com
 \return      plot->SeedPool
 */

int ForSeed::CheckSeedPoolForSeeds(PlotPointer plot, int pft, int com) {

	if (plot->SeedPool[pft][com] >= 1)
		return plot->SeedPool[pft][com];
	else
		return 0;
}

// -----------------------------------------------------------

/* !
 \brief	Removes the germinated seeds out of the seedpool
 \param       PlotPointer plot
 \param		  pft
 \param		  com
 \param		  output - number of seeds
 */

void ForSeed::ReduceSeedPool(PlotPointer plot, int pft, int com, int output) {

	plot->SeedPool[pft][com] -= output;
	plot->Seeds -= output;
}

/* !
 \brief	Calculates output variables
 \param	TreePointer tree
 \param	PlotPointer plot
 */

void ForSeed::ProtocolSeedlingIngrowth(TreePointer tree, PlotPointer plot) {

#ifdef underconstruction
	if (N_Par.Century_ON)
		plot->ingrowth_month += tree->BT * tree->N;
#endif
	plot->BiomassNewTrees += tree->BT * tree->N;

	Out.SEEDLINGSUM += tree->N;
	Out.SEEDLINGandGRP[tree->Grp] += tree->N;
}

// -----------------------------------------------------------

/* !
 \brief	Main function of seed germination: Calculation of establishment rates from a principle approach
 \param	      PlotPointer plot
 */

void ForSeed::DetermineSeedGermination_standard(PlotPointer plot) {
	TreePointer tree;
	int seedlings, seedlings_gone;
	double imin, irel_wet, irel_dry, vpw, vpd;
	double max_density;
	bool criteria;

	plot->BiomassNewTrees = 0;

	for (int pft = 0; pft < MAXGRP; pft++) {
		for (int com = 0; com < N_COMMERCIAL; com++) {

			imin = N_Par.Est_ISeed_3[pft];

#ifdef underconstruction
			if (Logging.DoIt)
				ChangeSeedlingsForRoadDamage(plot, pft);
#endif
			seedlings = CheckSeedPoolForSeeds(plot, pft, com);

#ifdef underconstruction
			if (N_Par.GRASSMIND) {
				seedlings_gone = DetermineGerminatedSeedlings(pft, seedlings, plot);
			}
			else
#endif
			{
#ifdef underconstruction
				if (N_Par.variable_Irradiance_ON) {
					if (plot->mean_light_above_canopy[pft] > 0) {
						irel_wet = plot->IRFloor / plot->mean_light_above_canopy[pft];
						irel_dry = 0;
					}
					else {
						irel_wet = 0;
						irel_dry = 0;
					}
				}
				else
#endif
				{
					if (N_Par.Env_IS_2[0] > 0) {
						irel_wet = plot->IRFloor / N_Par.Env_IS_2[0];
						irel_dry = 0;
					}
					else {
						irel_wet = 0;
						irel_dry = 0;
					}
				}

#ifdef underconstruction
				if (N_Par.Veg_period_ON) {
					if (N_Par.ref_length_of_vegetation_periode[pft] < 1.0) {
						vpw = plot->length_of_vegetation_periode / 365.0;
					}
					else {
						vpw = 1;
					}
					vpd = 0;
				}
				else
#endif
				{
					vpw = N_Par.Env_SeaL_2[0]; // length of season - wet
					vpd = N_Par.Env_SeaL_2[1]; // length of season - dry
				}

				plot->seedlings[pft] = CalculateSeedlingInput(irel_wet, irel_dry,
					 imin, vpw, vpd, seedlings);
			}

#ifdef underconstruction
			if (N_Par.Landslide) {
				ReduceRecruitmentLandslide(plot, pft);
			}

			if (N_Par.GRASSMIND)
				ReduceSeedPool(plot, pft, com, seedlings_gone);
			else
#endif
				ReduceSeedPool(plot, pft, com, plot->seedlings[pft]);

			if (N_Par.Densityreg) {
				max_density = N_Par.Max_Den[pft] * T.D;
				if (plot->seedlings[pft] > max_density)
					plot->seedlings[pft] = max_density;
			}

#ifdef underconstruction
			if (N_Par.Flag_DD_Seedling_Mortality) {
				CalculateDDSeedMort(plot, pft);
			}

			if (plot->landcode == 0) {
				plot->seedlings[pft] = 0;
			}

			if (N_Par.GRASSMIND)
				criteria = (plot->seedlings[pft] >= 1);
			else
#endif
				criteria = (plot->ATSum[SAEMLINGSKRONE] < 1.0) &&
					 (plot->seedlings[pft] >= 1);

			if (criteria) {
				if (!N_Par.TraitDist_ON && N_Par.Cohort) {
					tree = ExpandTreeRecord(plot, plot->FirstTree, pft,
						 plot->seedlings[pft], com);
				}
				else {
#ifdef underconstruction

					for (int i = 0; i < plot->seedlings[pft]; i++) {
						tree = ExpandTreeRecord(plot, plot->FirstTree, pft, 1, com);
					}
#endif
				}
				ProtocolSeedlingIngrowth(tree, plot);
			}
		}
	}
}

