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
// File         		for_dead.cpp
// Description  	   Functions related to tree mortality
//
///////////////////////////////////////////////////////////////////

#include "for_dead.h"
#include "random.h"
#include "for_grow.h"

#ifdef underconstruction
#include "for_fragmentation.h"
#include "for_landslide.h"
#include "grass/grass_dead.h"
#include "for_spat.h"
#include "for_liana.h"
#endif
#include <stdexcept>

#include "MMParSet/MMErrorMessage.h"
#include "for_misc.h"

formindDeath forDeath;

// -----------------------------------------------------------

/* !
 \brief       	Subroutine for Mortality
 \details		All mortality processes over all hectars and plots
 \return		 	boolean true if no error occurs.
 */

void formindDeath::DoMortality(void)

{
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

	for (int i = 0; i < MAXGRP; i++) {
		Out.DEATH_PFT[i] = 0;
		Out.TH_DEATH_PFT[i] = 0;
	}

	PlotPointer plot;
	HecPointer hec;
	TreePointer tree;

	hec = FirstHec;
	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {
			plot->NewDeadBiomass = 0;
			plot->MB = 0;
			plot->MBD = 0;
			plot->MBF = 0;
			plot->MBC = 0;
			plot->MBFIRE = 0;
			plot->MBLOG = 0;
			plot->MBLOGDAM = 0;
			plot->MBLAND = 0;
			plot = plot->next;
		}
		hec = hec->next;
	}

	hec = FirstHec;
	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {
			if (!N_Par.StoreInitialState) {
				tree = plot->FirstTree;
				while (tree != NULL) {

					if (N_Par.Spacelimitation)
						SpaceLimitation(plot, tree);

					if (tree->N > 0)
						MortalityandTreeFalling(tree, plot, hec);

					if (tree->nFall > 0) {
						if (tree->N > 0)
							DoTreeFalling(tree, plot, hec, tree->nFall);
					}

					tree = tree->next;
				}
			}

#ifdef underconstruction
			if (N_Par.Landslide) {
				plot->AccumulatedDeadMass += plot->NewDeadBiomass;
			}
#endif

			plot = plot->next;
		}
		hec = hec->next;
	}
	DeleteTreeCohorte();
}

// -----------------------------------------------------------

/* !
 \brief        Reduces stem number in relation to tree crown overlap
 \param  	   plot
 \param	   	tree
 \return       boolean true if no error occurs.
 */

bool formindDeath::SpaceLimitation(PlotPointer plot, TreePointer tree)
	// Potential crop trees (PCT, thinning) are immune against self-thinning
{
	double max_coverage = N_Par.Mort_SpaceLimitation_Factor;
	if (tree->D <= N_Par.Mort_SpaceLimitation_Diameter && tree->PCT == 0) {
		if (tree->minLRF < ((1.0 / max_coverage) - 0.01) && tree->N > 0) {
			double r = tree->N * (1.0 - (tree->minLRF * max_coverage));
			if (_Random() < (r -int(r)))
				r += 1.;
			DoTreeDeath(4, (int)r, tree, plot);
		}
	}
	if (tree->D > N_Par.Mort_SpaceLimitation_Diameter && tree->PCT == 0) {
		if (tree->minLRF < 0.99 && tree->N > 0) {
			double r = tree->N * (1.0 - (tree->minLRF));
			if (_Random() < (r -int(r)))
				r += 1.;
			DoTreeDeath(4, (int)r, tree, plot);
		}
	}

	return true;
}

// -----------------------------------------------------------

/* !
 \brief 			Mortality dependent on diameter.
 \param  		tree diameter
 \param  		tree pft
 \return     	tree mortality rate
 */

double formindDeath::MoutofD(double d, int pft) {

	double a0, a1, a2, mdia, which;

	a0 = N_Par.Mort_Dia_31[0][pft];
	a1 = N_Par.Mort_Dia_31[1][pft];
	a2 = N_Par.Mort_Dia_31[2][pft];

	which = N_Par.Mort_FUNCTION_2[0];

	if (which == 0) {
		mdia = a0 + a1 * d + a2 * SQR(d);
	}
	else if (which == 1) {
		if (d <= a1)
			mdia = a0 - a0 / a1 * d;
		else
			mdia = 0.0;
	}
	else if (which == 2) {
		mdia = a0 * pow(d, a1) + a2 * pow(d, 2);
	}
	else if (which == 3) {
		mdia = a0 * exp(-a1 * d);
	}
	else {
		MMErrorMessage("the choosen MoutofD-function doesn´t exists",
			N_Par.ErrorType);
	}
	return mdia;
}

// -----------------------------------------------------------

/* !
 \brief     	Diameter increment dependent mortality
 \param     	analysed tree diameter increment
 \return       tree mortality rate
 */

double formindDeath::MoutofDINC(double dinc, int pft, double binc, double lai)

{
	double b0, b1, b2, mdinc, which;

	b0 = N_Par.Mort_Dinc_31[0][pft];
	b1 = N_Par.Mort_Dinc_31[1][pft];
	b2 = N_Par.Mort_Dinc_31[2][pft];
	which = N_Par.Mort_FUNCTION_2[1];

	if (which == 0)
		mdinc = b0 + b1 * dinc + b2 * SQR(dinc);
	else if (which == 1)
		mdinc = b0 * exp(b1 * dinc);
	else if (which == 2)
		mdinc = 0.02 + b0 * dinc + b1 * pow(dinc, 2) + b2 * pow(dinc, 3);
	else if (which == 3) {
		if (dinc <= 0)
			mdinc = b0;
		if (dinc > 0)
			mdinc = b1;
	}
	else if (which == 4) {
		mdinc = b0 / (1 + b1 * (binc / lai));
		if ((binc / lai) < ((b0 - 1) / b1)) {
			mdinc = 1;
		}
	}
	else {
		MMErrorMessage("the choosen MoutofDINC-functions does not exist",
			N_Par.ErrorType);
	}

	return mdinc;
}

// -----------------------------------------------------------

/* !
 \brief          	Calculates higher mortality after logging.
 \details			Is activated with Logging.DamLinear
 \param				analysed tree
 \param				analysed plot
 \return         	mortality factor for tree after logging
 */

double formindDeath::MafterLogging(TreePointer tree, PlotPointer plot)

{
	double afterlogdam, yearsofhighmort, highmortality, age;

#ifdef underconstruction
	if (Logging.DamLinear > 0) {
		yearsofhighmort = floor((Logging.DamLinear - 1000000) / 1000.0);
		// 1xxxyyy: yyy percentage of mortality for xxx years
		highmortality = Logging.DamLinear - 1000000 - yearsofhighmort * 1000;
	}
	else
#endif
	{
		yearsofhighmort = 0.0;
		highmortality = 0.0;
	}

	if (tree != NULL) {
#ifdef underconstruction
		if ((Logging.Done > 0.0) && (Logging.Done <= yearsofhighmort) &&
			(tree->AGE > Logging.Done)) {
			afterlogdam = highmortality;
		}
		else
#endif
		{
			afterlogdam = 1.0;
		}
	}
	else {
		if (plot != NULL) {
#ifdef underconstruction
			if ((Logging.Done > 0.0) && (Logging.Done <= yearsofhighmort) &&
				(plot->RoadDamage <= yearsofhighmort)) {
				afterlogdam = highmortality;
			}
#endif
		}
		else {
			afterlogdam = 1.0;
		}
	}

	return afterlogdam;

}

// -----------------------------------------------------------

/* !
 \brief         tree mortality calculation
 \details
 1. basic mortality as function of PFT
 2. offset as function of diameter possible
 3. offset as function of diameter increment possible
 ms = mean + f(Dia) + f(Dinc)
 \param	     tree
 \param	     plot
 \return      void
 */

void formindDeath::CalculateTreeMortality_new(TreePointer tree,
	PlotPointer plot)

{

	double basis = 0.0;
	if (N_Par.Flag_BackgroundMortality)
		basis = N_Par.Mort_mean_19[tree->Grp];

	double mdia = 0.0;
	if (N_Par.Flag_DbhMortality)
		mdia = MoutofD(tree->D, tree->Grp);

	double mdinc = 0.0;
	// tree->DInc: diameter increment [m / yr]
	if (N_Par.Flag_DincMortality && tree->AGE > 0) {
		if (N_Par.Mort_nbinc && (N_Par.Mort_FUNCTION_2[1] == 4) &&
			(tree->PB_buffer < 0))
			mdinc = MoutofDINC(tree->DInc, tree->Grp, tree->PB_buffer, tree->L);
		else
			mdinc = MoutofDINC(tree->DInc, tree->Grp, tree->BInc, tree->L);
	}
	double mndinc = 0.0;
	if (N_Par.Mort_nbinc && (N_Par.Mort_FUNCTION_2[1] != 4)) {
		if ((N_Par.Mort_50nbinc + tree->ndinc) > 0.0)
			mndinc = ((1.0 + N_Par.Mort_50nbinc) * tree->ndinc) /
				(N_Par.Mort_50nbinc + tree->ndinc);
	}

	double afterlogmort = MafterLogging(tree, NULL);

	tree->MS = (basis + mdia + mdinc + mndinc) * afterlogmort;
#ifdef underconstruction
	if ((N_Par.Fragmentation || N_Par.Frag_highmortbigtree) &&
		(SpinupTime <= T.T)) {
		CalculateFragmentMort(plot, tree);
	}

	if (N_Par.Landslide) {
		CalculateLandslideMort(plot, tree);
	}
#endif

	if (T.D >= 1)
		tree->MS = 1 - pow(1.0 - tree->MS, T.D);
	else
		tree->MS = tree->MS * T.D;

}

// -----------------------------------------------------------

/* !
 \brief         	  Counting and summing up dead trees
 \param[in]         tote     number of dead trees in plot
 \param[in]         sortofdeath      tree death cause
 \param[in]         d                diameter size of dead tree
 \param[in]         plot             analysed plot
 \param[in]         biomass          biomass of dead tree
 \return            void.
 */

void formindDeath::CountDeath(int tote, int sortofdeath, double d, int pft,
	PlotPointer plot, double biomass)

{
	double dummy_tree_d;
	int j;

	if ((sortofdeath > 9) || (sortofdeath < 0))
		MMErrorMessage("variable sortofdeath out of range", N_Par.ErrorType);

	Out.DEATH += tote;
	Out.DEATH_PFT[pft] += tote;
	Out.DEATHCUM[0] += tote;
	Out.DEATHCUM[sortofdeath] += tote;
	Out.BiomassDEATHCUM[0] += (tote*biomass);
	Out.BiomassDEATHCUM[sortofdeath] += (tote*biomass);

	dummy_tree_d = d - 0.0000001;

	j = (int) floor(dummy_tree_d / (float)N_Par.Div_DiaClassWidth + 1.0) - 1;
	if (j > (MAXARRAY - 1))
		j = MAXARRAY - 1;
	if (j >= 0) {
		Out.DIADEATH[j] += tote;
		Out.DIADEATH_PFT[j][pft] += tote;
	}
	if (d >= Switch.Schwelle) {
		Out.TH_DEATH += tote;
		Out.TH_DEATH_PFT[pft] += tote;
		Out.TH_DEATHCUM[0] += tote;
		Out.TH_DEATHCUM[sortofdeath] += tote;
		Out.TH_BiomassDEATHCUM[0] += (tote*biomass);
		Out.TH_BiomassDEATHCUM[sortofdeath] += (tote*biomass);
	}

	if (sortofdeath == 4) {
		Out.SPACEDEATH += tote;
		if (d >= Switch.Schwelle)
			Out.TH_SPACEDEATH += tote;
	}

	if (sortofdeath == 4) {
		Out.GAPDAMAGE += tote;
		if (d >= Switch.Schwelle) {
			Out.TH_GAPDAMAGE += tote;
		}
	}

	switch (sortofdeath) {
	case 1:
		plot->MB += (tote*biomass);
		break; // normal mortality
	case 2:
		plot->MBF += (tote*biomass);
		break; // falling tree
	case 3:
		plot->MBD += (tote*biomass);
		break; // damaged trees (by a falling one)
	case 4:
		plot->MBC += (tote*biomass);
		break; // crowding mortality
#ifdef underconstruction
	case 5:
		plot->MBFIRE += (tote*biomass);
		break; // burnt trees
	case 6:
		plot->MBLAND += (tote*biomass);
		break; // trees dying due to a landslide event
	case 7:
		plot->MBLOG += (tote*biomass);
		break; // trees removed from the plot by logging (not really dying and thus, not a part of the NEE)
	case 8:
		plot->MBLOGDAM += (tote*biomass);
		break; // trees dying due to damages due to logging
#endif
	default:
		break;
	}
}

// -----------------------------------------------------------

/* !
 \brief         Random dying of individuals
 \param[in]     plot           analysed tree
 \param[in]     tree           analysed plot
 \return        sortofdeath    falling trees exit with 1, nofalling with 0
 */

int formindDeath::IndividualDying(PlotPointer plot, TreePointer tree)

{
	int sortofdeath = 0;
	double t = _Random();
	if (tree->N > 0) {

		if (_Random() < tree->MS) {
			sortofdeath = 1;

			if (N_Par.Treefall) {
				if (tree->D >= FALLTHRESHOLD) {
					if (_Random() < N_Par.Mort_FallP) {
						sortofdeath = 2;
					}
				}
			}
		}
	}

	return sortofdeath;

}

// -----------------------------------------------------------

/* !
 \brief           Calculates damage due to falling trees, fall direction and position
 \param[in]           HH       analysed hectar
 \param[in]           HP       analysed plot
 \param[in]           H        analysed tree height
 \param[in]           D        analysed tree diameter
 \param[in]           AC       gapsize
 \param[in]           crowndiameter     analysed tree crowndiameter
 \param[in]           crownlength       analysed tree crownlength
 \return          	 void.
 */

void formindDeath::DetermineDamages(HecPointer HH, PlotPointer HP, double H,
	double D, double AC, double crowndiameter, double crownlength)

{
	double rand;
	TreePointer tree;
	int i, sortofdeath;
	double damage;
	int thtotschlag, tote, totschlag, nDam;
	double xpos, ypos;

#ifdef underconstruction
	if (N_Par.spatial) {
		xpos = CalculatePos(crownlength, 0, HP);
		ypos = CalculatePos(crowndiameter, 1, HP);
	}
#endif

	totschlag = 0;
	thtotschlag = 0;
	tree = HP->FirstTree;
	while (tree != NULL) {
		nDam = 0;
		if ((tree->N > 0) && (tree->H < H)) {

#ifdef underconstruction
			if (N_Par.spatial)
				damage = CalculateDamage(tree, crownlength, crowndiameter,
				xpos, ypos);
			else
#endif
				damage = AC / (HP->Area);

			if ((tree->N < Switch.MortThreshold) || (tree->D >= INDVIDUALDEATH))
			{
				i = 0;
				while ((i < tree->N) && (tree->N > 0) && (damage > 0)) {
					i++;
					rand = _Random();

					if ((rand < damage)) {
						nDam++;
						totschlag += 1;
						Out.GAPDAMAGE += 1;
						if (tree->D >= Switch.Schwelle) {
							Out.TH_GAPDAMAGE += 1;
							thtotschlag += 1;
						}
					}
				}
			}
			else {
				nDam = floor((damage * tree->N) + 0.5);
				totschlag += nDam;
				Out.GAPDAMAGE += nDam;
				if (tree->D >= Switch.Schwelle) {
					Out.TH_GAPDAMAGE += nDam;
					thtotschlag += nDam;
				}
			}
		}

		if (nDam > 0) {
			sortofdeath = 3;
			DoTreeDeath(sortofdeath, nDam, tree, HP);
		}
		tree = tree->next;
	}
}

// -----------------------------------------------------------

/* !
 \brief          		 Calculates normal and tree falling mortality
 \param[in]           tree       analysed tree
 \param[in]           plot       analysed plot
 \param[in]           hec       analysed hectar
 */

void formindDeath::MortalityandTreeFalling(TreePointer tree, PlotPointer plot,
	HecPointer hec) {

	int i;
	int number;
	int nMort, nFall;
	int sortofdeath;
	int trees_previous;
	bool criteria;

	nMort = 0;
	tree->nFall = 0;

#ifdef underconstruction
	if (N_Par.GRASSMIND) {
		CalculateSenescence(tree, plot);
		CalculateGrassMortality(tree, plot);
	}
	else
#endif
	{
		CalculateTreeMortality_new(tree, plot);
	}

#ifdef underconstruction
	if (N_Par.GRASSMIND)
		criteria = (tree->N < Switch.MortThreshold) ||
			(tree->H >= INDVIDUALDEATH);
	else
#endif
		criteria = (tree->N < Switch.MortThreshold) ||
			(tree->D >= INDVIDUALDEATH);

	if (criteria) {
		i = 0;
		number = tree->N;

		while ((i < number) && (tree->N > 0)) {
			i++;
			sortofdeath = IndividualDying(plot, tree);
			// sortofdeath = 0 --> tree is still alive
			// sortofdeath = 1 --> tree is dying normal
			// sortofdeath = 2 --> tree is dying and falling

			if (sortofdeath == 1) {
				nMort++;
			}
			else if (sortofdeath == 2) {
				tree->nFall++;
			}
		}
	}
	else {
		nMort = floor(tree->N * tree->MS + 0.5);
		sortofdeath = 1;
	}

	if (nMort > 0) {
		sortofdeath = 1;
		DoTreeDeath(sortofdeath, nMort, tree, plot);
	}

}

// -----------------------------------------------------------

/* !
 \brief          	Executes mortality of trees for each plot
 \details			0: total, 1: basic mortality, 2: falling, 3: damage by falling,
 						4: crowding, 5: fire 6: landslide 7: logging 8: damage logging
 \param[in]       sortofdeath       tree death type
 \param[in]       numberofdeaths       number of dead trees per plot
 \param[in]       tree       analysed tree
 \param[in]       plot       analysed plot
 */

void formindDeath::DoTreeDeath(int sortofdeath, double numberofdeaths,
	TreePointer tree, PlotPointer plot) {

	if ((tree->N - numberofdeaths) < 0) {

		std::string message =
			"More dead than living trees, corrected to number of living trees. tree->N = " +
			std::to_string(tree->N) + "; number of death trees = " +
			std::to_string(numberofdeaths) + "; PFT = " + std::to_string
			(tree->Grp + 1) + "; tree->D = " + std::to_string(tree->D) +
			"; Time = " + std::to_string(T.T);
		MMErrorMessage(message, N_Par.WarningType);

		numberofdeaths = tree->N;
	}

	tree->N -= numberofdeaths;
	CountDeath(numberofdeaths, sortofdeath, tree->D, tree->Grp, plot, tree->BT);

#ifdef underconstruction
	if (N_Par.GRASSMIND) {
		CountDeathGrass(numberofdeaths, sortofdeath, tree, plot);
	}
#endif
}

// -----------------------------------------------------------

/* !
 \brief     Deletes tree cohorts
 \return		void.
 */

void formindDeath::DeleteTreeCohorte(void) {

	TreePointer acttree, nexttree, prevtree;
	PlotPointer actplot;
	HecPointer acthec;

	acthec = FirstHec;
	while (acthec != NULL) {
		actplot = acthec->FirstPlot;
		while (actplot != NULL) {
			acttree = actplot->FirstTree;
			prevtree = NULL;

			while (acttree != NULL) {
				if (acttree->N <= 0) {
#ifdef underconstruction
					if(N_Par.Liana && acttree->attachedLiana!=NULL){
						DeleteLiana(acttree);
					}
#endif
					nexttree = acttree->next;
					delete acttree;
					acttree = NULL;

					if (prevtree == NULL) {
						actplot->FirstTree = nexttree;
						acttree = actplot->FirstTree;
					}
					else {
						prevtree->next = nexttree;
						acttree = prevtree->next;
					}
				}
				else {
					prevtree = acttree;
					acttree = prevtree->next;
				}
			}
			actplot = actplot->next;
		}
		acthec = acthec->next;
	}
}
// -----------------------------------------------------------

/* !
 \brief         Deletes tree cohorts in the given Plot.
 \details		If you want to delete tree cohortes in the whole Forest use the function above (DeleteTreeCohortes).
 \param plot
 */

void formindDeath::DeleteTreeCohorte(PlotPointer actPlot) {
	TreePointer acttree, nexttree, prevtree;
	acttree = actPlot->FirstTree;
	prevtree = NULL;

	while (acttree != NULL) {
		if (acttree->N <= 0) {
			nexttree = acttree->next;
			delete acttree;
			acttree = NULL;

			if (prevtree == NULL) {
				actPlot->FirstTree = nexttree;
				acttree = actPlot->FirstTree;
			}
			else {
				prevtree->next = nexttree;
				acttree = prevtree->next;
			}
		}
		else {
			prevtree = acttree;
			acttree = prevtree->next;
		}
	}
}

// -----------------------------------------------------------

/* !
 \brief          		Executes falling mortality of trees
 \param[in]          tree       analysed tree
 \param[in]          plot       analysed plot
 \param[in]          hec       analysed hectar
 \param[in]          nFall       number of falling trees
 \return					void.
 */

void formindDeath::DoTreeFalling(TreePointer tree, PlotPointer plot,
	HecPointer hec, double nFall) {

	double x, y, xf, yf, dir, gapsize, height, diameter, crowndiameter,
		crownlength;
	HecPointer HH;
	PlotPointer HP;
	int sortofdeath, i, sameplot;

	x = tree->absX();
	y = tree->absY();

	gapsize = tree->AC;
	height = tree->H;
	diameter = tree->D;
	crowndiameter = forGrow.CDoutofD(tree->D, tree->Grp);
	crownlength = tree->H * tree->CLP;

	sortofdeath = 2;
	DoTreeDeath(sortofdeath, nFall, tree, plot);

	for (i = 0; i < nFall; i++) {
		dir = _NRandom(360);

		if (N_Par.Closed_boundary) {
			ForTools::DetermineFallLoc(dir, x, y, height, &xf, &yf);

			HH = ForTools::FindHectar(xf, yf);

			HP = ForTools::FindPlot(HH, xf, yf);

			DetermineDamages(HH, HP, height, diameter, gapsize, crowndiameter,
				crownlength);

		}
		else {
			ForTools::DetermineFallLoc_noclosedboundary(dir, x, y, height,
				&xf, &yf);

			if ((xf >= Loc.XMin) && (xf < Loc.XMax) && (yf >= Loc.YMin) && (yf <
				Loc.YMax)) {
				HH = ForTools::FindHectar(xf, yf);
				if (HH == NULL)
					MMErrorMessage(
					"can not find the hectar, in which the tree falls (doTreeFalling)",
					N_Par.ErrorType);
				HP = ForTools::FindPlot(HH, xf, yf);
				if (HP == NULL)
					MMErrorMessage(
					"can not find the plot, in which the tree falls (doTreeFalling)",
					N_Par.ErrorType);

				DetermineDamages(HH, HP, height, diameter, gapsize,
					crowndiameter, crownlength);
			}
		}
	}
}

// -----------------------------------------------------------
// ---------------- end of for_dead.cc -----------------------
// -----------------------------------------------------------
