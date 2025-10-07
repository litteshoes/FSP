////////////////////////////////////////////////////////////////////
//
// FORMIND � the forest model
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
// File						for_comp.cpp
// Description				light competition
//
///////////////////////////////////////////////////////////////////

#include "for_comp.h"
#ifdef underconstruction
#include "for_log.h"
#include "grass/grass_grow.h"
#include "grass/grass_environment.h"
#include "for_liana.h"
#include "for_spat.h"
#endif

#include "MMParSet/MMErrorMessage.h"
#include "for_misc.h"
using namespace std;

// ----------------------- Deklarations --------------

void ExtractCrownHeightParallelTrees(PlotPointer, double, TreePointer, int);

// ----------------------- subroutines ---------------

/* !
 \brief Subroutine for all light competition processes
 \return    boolean true if no error occures.
 */
void DoLightCompetition() {
	PlotPointer plot;
	HecPointer hec;

	hec = FirstHec;
	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {
			// light competition --------------------------------------------------
			CalculateLAI(hec, plot);

			CalculateLightAttenuation_new(hec, plot);

			// trigger for crowding -----------------------------------------------
			CalculateSpaceConditions(hec, plot);
			plot = plot->next;
		}
		hec = hec->next;
	}
}

// ----------------------------------------------------------------

/* !
 \brief   Calculate factor for triggering space limitation
 \param    HecPointer hec
 \param    PlotPointer  plot
 \return   void;
 */
void CalculateSpaceConditions(HecPointer hec, PlotPointer plot) {
	int up, down, count;
	TreePointer tree;

	tree = plot->FirstTree;
	while (tree != NULL) {

#ifdef underconstruction
		if (N_Par.spatial) {
			tree->minLRF = CalculateSpatialminLRF(tree);
		}
		else
#endif
		{
			up = (int) floor(tree->H / Switch.DLYR + DELTA);
			down = (int) floor((tree->H * (1 - tree->CLP)) / Switch.DLYR + DELTA);
			if ((down < 0) || (down > up) || (up < 0) || (up > MAXLYR)) {
				std::string message =
					 "inconsistent calculation of top and down layer of treecrown. top = " +
					 std::to_string(up) + "; down = " + std::to_string(down) +
					 "; MAXLYR = " + std::to_string(MAXLYR) + ";PFT " +
					 std::to_string(tree->Grp + 1) + "; Plot = " +
					 std::to_string(plot->No) + "; Hec = " + std::to_string
					 (hec->HecNo) + "; Time = " + std::to_string(T.T);
				MMErrorMessage(message, N_Par.ErrorType);
			}

			tree->minLRF = 1.0;
			count = down;
			while (count <= up) {
				if ((1.0 / plot->ATSum[count]) < tree->minLRF) {
					tree->minLRF = (1.0 / plot->ATSum[count]);
				}
				count++;
			}
		}

		tree = tree->next;
	}
}

// ----------------------------------------------------------------
/* !
 \brief   Calculate factor for triggering space limitation with extended width in all layers
 \param    HecPointer hec
 \param    PlotPointer  plot
 \param    vector<float>atSumMax
 */

void CalculateSpaceConditions(HecPointer hec, PlotPointer plot,
	 vector<float>atSumMax) {

	int up, down, count;
	TreePointer tree;

	tree = plot->FirstTree;
	while (tree != NULL) {
#ifdef underconstruction
		if (N_Par.spatial) {
			tree->minLRF = CalculateSpatialminLRF(tree);
		}
		else
#endif
		{
			up = (int) floor(tree->H / Switch.DLYR + DELTA);
			down = (int) floor((tree->H * (1 - tree->CLP)) / Switch.DLYR + DELTA);
			if ((down < 0) || (down > up) || (up < 0) || (up > MAXLYR)) {
				std::string message =
					 "inconsistent calculation of top and down layer of treecrown. top = " +
					 std::to_string(up) + "; down = " + std::to_string(down) +
					 "; MAXLYR = " + std::to_string(MAXLYR) + ";PFT " +
					 std::to_string(tree->Grp + 1) + "; Plot = " +
					 std::to_string(plot->No) + "; Hec = " + std::to_string
					 (hec->HecNo) + "; Time = " + std::to_string(T.T);
				MMErrorMessage(message, N_Par.ErrorType);
			}

			tree->minLRF = 1.0;
			count = down;
			while (count <= up) {

				if ((atSumMax[count] / plot->ATSum[count]) < tree->minLRF) {
					tree->minLRF = (1.0 / plot->ATSum[count]);
				}
				count++;
			}
		}

		tree = tree->next;
	}

}

/* !
 \brief Calculates LAI for whole canopy
 \param   hecPointer  hec
 \param   plotPointer  plot
 \return  void
 */
void CalculateLAI(HecPointer hec, PlotPointer plot) {
	int c, count, up, down, cc;
	TreePointer tree;
	float weight[MAXLYR + 1];
	float weightsum = 1;

	// MAXLYR  = max. amount of hight layers
	for (c = 0; c <= MAXLYR; c++) {
		plot->ATSum[c] = 0.0;
		plot->ATold[c] = 0.0;
		plot->LAI[c] = 0.0;
		plot->LAIK[c] = 0.0;
		weight[c] = 1.0;
#ifdef underconstruction
		if (N_Par.Liana) {
			plot->ATSum_WithoutLiana[c] = 0.0;
			plot->ATold_WithoutLiana[c] = 0.0;
			plot->LAI_WithoutLiana[c] = 0.0;
			plot->LAIK_WithoutLiana[c] = 0.0;
		}
#endif
	}

	// MAXLAD = max. leaf area density (20)
	for (cc = 0; cc <= MAXLAD; cc++) {
		plot->LAD[cc] = 0.0;
#ifdef underconstruction
		if (N_Par.Liana)
			plot->LAD_WithoutLiana[cc] = 0.0;
#endif
	}

	// determine total leaf area (ATSum) and LAI per layer of the plot-------------
	tree = plot->FirstTree;
	while (tree != NULL) {
		if (tree->N > 0) {
			up = (int) floor(tree->H / Switch.DLYR + DELTA);
			down = (int) floor((tree->H * (1 - tree->CLP)) / Switch.DLYR + DELTA);
			if ((down < 0) || (down > up) || (up < 0) || (up > MAXLYR)) {
				std::string message =
					 "inconsistent calculation of top and down layer of treecrown. top = " +
					 std::to_string(up) + "; down = " + std::to_string(down) +
					 "; MAXLYR = " + std::to_string(MAXLYR) + ";PFT " +
					 std::to_string(tree->Grp + 1) + "; Plot = " +
					 std::to_string(plot->No) + "; Hec = " + std::to_string
					 (hec->HecNo) + "; Time = " + std::to_string(T.T);
				MMErrorMessage(message, N_Par.ErrorType);
			}

			count = down;

			if (N_Par.CrownDensityCurve == 2 && up - down > 0)
			{ // vertical leaf layers are weighted by normal distribution
				weightsum = 0;
				double mue = (up - down) / 2.0;
				double sig = 0.51 * mue;
				// 95% of all values are within the plant height (integrated from 0m up to Hmax)
				double countlayers = 0;
				for (int ii = down; ii <= up; ii++) {
					weightsum += exp(-(ii - mue) * (ii - mue) / (4 * sig * sig));
					weight[ii] = exp(-(ii - mue) * (ii - mue) / (4 * sig * sig));
					countlayers++;
				}
				weightsum = weightsum / countlayers;

			}

			while (count <= up) {
				plot->ATSum[count] +=
					 tree->N * tree->AC * (weight[count] / weightsum) / plot->Area;
				plot->LAI[count] +=
					 (tree->N * tree->AC * (weight[count] / weightsum) / plot->Area)
					 * tree->L * Switch.DLYR / (tree->CLP * tree->H);
				plot->LAIK[count] +=
					 ((tree->N * tree->AC * (weight[count] / weightsum))
					 / plot->Area) * tree->L * tree->LightExtCoeff * Switch.DLYR /
					 (tree->CLP * tree->H);
#ifdef underconstruction
				if (N_Par.Liana && tree->isLiana == 0)
					CalculateLeafDistributionWithoutLianas(plot, tree, count, weight,
					 weightsum);
#endif

#ifdef underconstruction
				if (N_Par.GRASSMIND)
					CalculateGrassCover(plot, tree, count);
#endif
				count++;
			}
#ifdef underconstruction
			if (N_Par.GRASSMIND) {
				if (N_Par.CrownDensityCurve == 2) {
					tree->BT_calib_weight = 0;
					for (int i = down; i <= (int)(N_Par.Calib_Biomass * 100.0); i++)
					{ // N_Par.Calib_Biomass in m, Switch.DLYR = 0.01
						tree->BT_calib_weight += weight[i] / weightsum;
						// weight for area under N_Par.Calib_Biomass
					}
				}
				else
					tree->BT_calib_weight = 1;
			}
#endif
		}
		tree = tree->next;
	}

	c = (int) ceil(HMAX / Switch.DLYR);
	if (c > MAXLYR)
		c = MAXLYR;
	do {
		plot->ATold[c] = plot->ATSum[c];

		if (plot->ATSum[c] > 1.0) {
			plot->LayerReductionFac[c] = 1 / plot->ATSum[c];
		}
		else {
			plot->LayerReductionFac[c] = 1.0;
		}
		plot->LAI[c] = plot->LAI[c] * plot->LayerReductionFac[c];
		plot->LAIK[c] = plot->LAIK[c] * plot->LayerReductionFac[c];

		plot->LAD[c] = plot->LAI[c] / Switch.DLYR;

		if (((c + 1) * Switch.DLYR) <= HMAX) {
			plot->LAI[c] += plot->LAI[c + 1];
			plot->LAIK[c] += plot->LAIK[c + 1];
		}

#ifdef underconstruction
		if (N_Par.Liana)
			CalculateLayerReductionWithoutLianas(plot, c);
#endif

		c--;
	}
	while (c >= 0);
}

// ----------------------------------------------------------------

/* !
 \brief 		Calculate available irradiance of a tree for wet and dry
 seasons.
 \param    	double lai
 \param 		int season
 \return       double
 \details  	If changing extinction formula, then here. irradiance within
 the canopy is calculated with normal extinction
 */
double CalculateI(double lai, int season) {
	double irradiance;
	irradiance = N_Par.Env_IS_2[season] * exp(-lai);
	return irradiance;
}

// ----------------------------------------------------------------

/* !
 \brief 		Calculate available irradiance of a tree for whole year.
 \param        double lai (accumulated leaf area above tree crown)
 \param		double irr (irradiance above canopy)
 \return       double (irradiance at top of the tree)
 \details      irradiance within the canopy is calculated with normal extinction
 like in upper formula and NOT like in photosynthesis (below).
 */
double CalculateIR(double lai, double irr) {
	double irr_top_of_tree;

	irr_top_of_tree = exp(-lai) * irr;

	return irr_top_of_tree;
}

// ----------------------------------------------------------------

/* !
 \brief 			Calculate light for both wet and dry seasons.
 \param   		HecPointer  hec
 \param 			PlotPointer  plot
 \details		Determines light profile within the crown according to shading
 */
void CalculateLightAttenuation_new(HecPointer hec, PlotPointer plot) {
	int top, pft;
	double max;
	TreePointer tree;
	plot->Plotmax = 0.0;
	tree = plot->FirstTree;
	while (tree != NULL) {
		if (tree->N > 0) {
			top = (int) ceil(tree->H / Switch.DLYR);
			if (top > MAXLYR)
				top = MAXLYR;
			max = (tree->H > plot->Plotmax) ? tree->H : plot->Plotmax;
			// if-else-Syntax
			plot->Plotmax = max;

#ifdef underconstruction
			if (N_Par.spatial) {
				tree->LAITREE = CalculateSpatialLAIK(tree);
			}
			else if (N_Par.GRASSMIND) {
				CalculateGrassLAIK(plot, tree, top);
			}
			else
#endif
			{
				// classical approach without thinning and PCT
				if (Logging.Thinning == 0) {
#ifdef underconstruction
					if (N_Par.Liana && tree->attachedLiana == NULL) {
						// lianas are activated but tree is not infested by lianas
						tree->LAITREE_forRes = plot->LAI_WithoutLiana[top];
						tree->LAITREE = plot->LAIK_WithoutLiana[top];
					}
					else
#endif
					{ // if lianas are deactivated or the tree is infested by lianas. standard case.
						tree->LAITREE_forRes = plot->LAI[top];
						tree->LAITREE = plot->LAIK[top];
					}
				}
#ifdef underconstruction
				else {
					CalculatePCTLAIK(plot, tree, top);
				}
#endif
			}

            if (N_Par.variable_Irradiance_ON) {

				pft = tree->Grp;
				tree->IR = CalculateIR(tree->LAITREE,
					 plot->mean_light_above_canopy[pft]);

            } else {
				tree->IRwet = CalculateI(tree->LAITREE, 0);
				// incoming radiation in wet season [micromol (photon) m^-2 s^-1]
				tree->IRdry = CalculateI(tree->LAITREE, 1);
				// incoming radiation in dry season [micromol (photon) m^-2 s^-1]
				tree->IR = tree->IRwet * N_Par.Env_SeaL_2[0] +
					 tree->IRdry * N_Par.Env_SeaL_2[1];
				// incoming radiation [micromol (photon) m^-2 s^-1]
            }
		}
		tree = tree->next;
	}

    if (N_Par.variable_Irradiance_ON) {
        plot->IRFloor = CalculateIR(plot->LAIK[0], plot->total_light_above_canopy);
    } else {
		plot->IRFloorwet = CalculateI(plot->LAIK[0], 0);
		plot->IRFloordry = CalculateI(plot->LAIK[0], 1);
		plot->IRFloor = plot->IRFloorwet * N_Par.Env_SeaL_2[0] +
			 plot->IRFloordry * N_Par.Env_SeaL_2[1];
    }
}

// -----------------------------------------------------------
// ----------------- end of for_comp.cc ----------------------
// -----------------------------------------------------------
