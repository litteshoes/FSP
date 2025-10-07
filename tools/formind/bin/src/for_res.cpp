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
// File						for_res.cpp
// Description				Calculates results
//
///////////////////////////////////////////////////////////////////

#include "for_res.h"
#ifdef underconstruction
#include "grass/grass_grow.h"
#include "for_sidar.h"
#endif
#include <math.h>

using namespace std;

// -----------------------------------------------------------

/* !
 \brief          Calculate average ha-normalized values of global variables like LAI
 \param	        void
 \return	        void
 */

void CalculateStandAverages(void)

{
	int zz, plotzahl, i, pz;
	double hirfl, height;
	double hats[MAXLYR + 1], hlai[MAXLYR + 1];
	double hatsf[MAXLYR + 1][3];
	double hlad[MAXLAD + 1];
	PlotPointer plot;
	HecPointer hec;

	hirfl = 0.0;
	for (zz = 0; zz <= MAXLYR; zz++) {
		hats[zz] = 0.0;
		hlai[zz] = 0.0;
		Out.AvATSUM[zz] = 0.0;

		for (i = 0; i < 3; i++)
		{ // for the 3 types of plots: gap, building state or mature state
			hatsf[zz][i] = 0.0;
			Out.AvATSF[zz][i] = 0.0;
		}
		height = zz * Switch.DLYR;
	}

	for (zz = 0; zz <= MAXLAD; zz++) {
		hlad[zz] = 0.0;
	}

	plotzahl = 0;
	pz = 0;
	Out.GAP = 0.0;
	Out.BUILDING = 0.0;
	Out.MATURE = 0.0;

	hec = FirstHec;
	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {

			hirfl += plot->IRFloor;
			for (zz = 0; zz <= MAXLYR; zz++) {
				if (plot->Plotmax <= GAPHEIGHT)
					hatsf[zz][0] += plot->ATSum[zz];
				else if (plot->Plotmax <= BUILDINGHEIGHT)
					hatsf[zz][1] += plot->ATSum[zz];
				else
					hatsf[zz][2] += plot->ATSum[zz];

				hats[zz] += plot->ATSum[zz];
				hlai[zz] += plot->LAI[zz];

				height = zz * Switch.DLYR;
			}
			for (zz = 0; zz <= MAXLAD; zz++) {
				hlad[zz] += plot->LAD[zz];
			}

			plotzahl++;

			if (plot->Plotmax <= GAPHEIGHT)
				Out.GAP += 1.0;
			else if (plot->Plotmax <= BUILDINGHEIGHT)
				Out.BUILDING += 1.0;
			else
				Out.MATURE += 1.0;
			pz++;
			plot = plot->next;
		}
		hec = hec->next;
	}

	Out.AvIRFloor = hirfl / plotzahl;
	for (zz = 0; zz <= MAXLYR; zz++) {
		if (Out.GAP < 1.0)
			Out.AvATSF[zz][0] = 0.0;
		else
			Out.AvATSF[zz][0] = hatsf[zz][0] / Out.GAP;
		if (Out.BUILDING < 1.0)
			Out.AvATSF[zz][1] = 0.0;
		else
			Out.AvATSF[zz][1] = hatsf[zz][1] / Out.BUILDING;
		if (Out.MATURE < 1.0)
			Out.AvATSF[zz][2] = 0.0;
		else
			Out.AvATSF[zz][2] = hatsf[zz][2] / Out.MATURE;

		Out.AvATSUM[zz] = hats[zz] / plotzahl;
		Out.AvLAI[zz] = hlai[zz] / plotzahl;
		height = zz * Switch.DLYR;
	}
	for (zz = 0; zz <= MAXLAD; zz++) {
		Out.AvLAD[zz] = hlad[zz] / plotzahl;
	}
	Out.GAP /= pz;
	Out.BUILDING /= pz;
	Out.MATURE /= pz;

}

// -----------------------------------------------------------

/* !
 \brief          Calculates Output for N, BA, SV, DIA, BT, DEATH
 \param	        void
 \return	        void
 */

void CalculateOutput(void) {
	int lastj, j, kk, pft, lgrp, hgrp, com, species[NGAP], stems[NGAP], gap,
		plotno, th_species;
	double f_n, f_bt;
	double dummy_tree_d;
	TreePointer tree;
	PlotPointer plot;
	HecPointer hec;
	double threshold_zero, isfull;
	double DDOUT = N_Par.Div_DiaClassWidth;

	threshold_zero = 0.0;

	// init--------------------------------------------

	Out.SWI_N = 0.0;
	Out.SWI_BT = 0.0;
	Out.TH_SWI_N = 0.0;
	Out.TH_SWI_BT = 0.0;
	Out.LAI_mean = 0.0;

	// carbon_cycle ---------------------------------------------------------------
	Out.C_flux = 0.0;
	Out.CPool_DeadWood = 0.0;
	Out.CPool_alive_biomass = 0.0;
	Out.PB = 0.0;
	Out.R_total = 0.0;
	Out.Cflux_to_DeadWood = 0.0;
	Out.Resp_DeadWood = 0.0;
	Out.R_total_biomass = 0.0;
	Out.CPool_Soil_fast = 0.0;
	Out.CPool_Soil_slow = 0.0;
	Out.Resp_Soil_fast = 0.0;
	Out.Resp_Soil_slow = 0.0;
	Out.aet = 0.0;
	Out.Cflux_to_Soil_fast = 0.0;
	Out.Cflux_to_Soil_slow = 0.0;

	for (com = 0; com < 2; com++) {
		Out.BAandCOM[com] = 0.0;
		Out.SVandCOM[com] = 0.0;
		Out.BVandCOM[com] = 0.0;
		Out.NCOM[com] = 0.0;
		Out.BTandCOM[com] = 0.0;
		Out.TH_BAandCOM[com] = 0.0;
		Out.TH_SVandCOM[com] = 0.0;
		Out.TH_BVandCOM[com] = 0.0;
		Out.TH_NCOM[com] = 0.0;
		Out.TH_BTandCOM[com] = 0.0;
		Out.SEEDandCOM[com] = 0.0;
		Out.SEEDTREEandCOM[com] = 0.0;
	}

	MAXARRAY = (int) floor(DMAX / (float)N_Par.Div_DiaClassWidth + 1.0);
	for (kk = 0; kk < MAXGRP; kk++) {
		for (j = 0; j < MAXARRAY; j++) {
			Out.DIAandGRP_new[j][kk] = 0.0;
			Out.DIASUM[j] = 0.0;
			Out.BADIASUM_new[j] = 0.0;
			Out.SVDIASUM[j] = 0.0;
			Out.GROWTH_COUNTER_SUM[j] = 0.0;
			Out.TH_GROWTH_COUNTER_SUM[j] = 0.0;
			Out.GROWTHSUM[j] = 0.0;
			Out.TH_GROWTHSUM[j] = 0.0;
			Out.TH_DIAandGRP_new[j][kk] = 0.0;
			Out.TH_DIASUM[j] = 0.0;
			Out.TH_BADIASUM_new[j] = 0.0;
			Out.BADIAandGRP_new[j][kk] = 0.0;
			Out.TH_BADIAandGRP_new[j][kk] = 0.0;
			Out.GROWTH_COUNTER_andGRP[j][kk] = 0.0;
			Out.TH_GROWTH_COUNTER_andGRP[j][kk] = 0.0;
			Out.GROWTHandGRP[j][kk] = 0.0;
			Out.TH_GROWTHandGRP[j][kk] = 0.0;
		}
		Out.BAandGRP[kk] = 0.0;
		Out.SVandGRP[kk] = 0.0;
		Out.BVandGRP[kk] = 0.0;
		Out.NGRP[kk] = 0.0;
		Out.BTandGRP[kk] = 0.0;
		Out.TH_BAandGRP[kk] = 0.0;
		Out.TH_SVandGRP[kk] = 0.0;
		Out.TH_BVandGRP[kk] = 0.0;
		Out.TH_NGRP[kk] = 0.0;
		Out.TH_BTandGRP[kk] = 0.0;
		Out.SEEDandGRP[kk] = 0.0;
		Out.SEEDTREEandGRP[kk] = 0.0;

#ifdef underconstruction
		if (N_Par.GRASSMIND) {
			Out.MeanHeight[kk] = 0.0;
			Out.MaxHeight[kk] = 0.0;
			Out.LAIGrass[kk] = 0.0;
			Out.LAIGreen[kk] = 0.0;
			Out.BTGreen[kk] = 0.0;
			Out.Biomass_calib[kk] = 0.0;

			Out.RBTandGRP[kk] = 0.0;
			Out.ShootBiomassMean[kk] = 0.0;
			Out.RootDepthMean[kk] = 0.0;
			Out.LeafNAreaMean[kk] = 0.0;
			Out.WMHC[kk] = 0.0;
			Out.BiomassDensity[kk] = 0.0;
		}
#endif
	}
	Out.NTOTAL = 0.0;
	Out.BASALSUM = 0.0;
	Out.SVSUM = 0.0;
	Out.BVSUM = 0.0;
	Out.BTSUM = 0.0;
	Out.BTSUM_2 = 0.0;
	Out.TH_NTOTAL = 0.0;
	Out.TH_BASALSUM = 0.0;
	Out.TH_SVSUM = 0.0;
	Out.TH_BVSUM = 0.0;
	Out.TH_BTSUM = 0.0;
	Out.TH_BTCSUM = 0.0;
	Out.TH_BTCHSUM = 0.0;
	Out.TH_BTSUM_2 = 0.0;
	Out.SEEDSUM = 0.0;
	Out.SEEDTREESUM = 0.0;
	Out.MeanWoodDen = 0.0;
	Out.TH_MeanWoodDen = 0.0;

	hec = FirstHec;
	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {

			// all in [tC / ha] or in [tC / ha/ yr]
			Out.C_flux += plot->C_flux;
			Out.CPool_Soil_fast += plot->CPool_Soil_fast;
			Out.CPool_Soil_slow += plot->CPool_Soil_slow;
			Out.CPool_DeadWood += plot->CPool_DeadWood;
			Out.CPool_alive_biomass += ODM_TO_C * plot->Biomass;
			Out.PB += ODM_TO_C * plot->PB;
			Out.R_total += plot->R_total;
			Out.Cflux_to_DeadWood += plot->Cflux_to_DeadWood;
			Out.Cflux_to_Soil_fast += plot->Cflux_to_Soil_fast;
			Out.Cflux_to_Soil_slow += plot->Cflux_to_Soil_slow;
			Out.Resp_DeadWood += plot->Resp_DeadWood;
			Out.Resp_Soil_fast += plot->Resp_Soil_fast;
			Out.Resp_Soil_slow += plot->Resp_Soil_slow;
			Out.R_total_biomass += ODM_TO_C * plot->R_total_biomass;
			Out.aet += plot->aet / (Switch.Maxplot); // [mm/yr]

			tree = plot->FirstTree;
			while (tree != NULL) {
				if (tree->N < 0)
					cout << "WARNING: variable warning \tfile: " << __FILE__ <<
						"\tfunction: CalculateOutput\t line:" << __LINE__ <<
						"\t tree->N smaller than zero." << endl;

#ifdef underconstruction
				if (N_Par.GRASSMIND)
					dummy_tree_d = tree->H - 0.00000000000001;
				else
#endif
					dummy_tree_d = tree->D - 0.0000001;

				j = (int) floor(dummy_tree_d / (float)N_Par.Div_DiaClassWidth +
					1.0) - 1;
				if (j > (MAXARRAY - 1))
					j = MAXARRAY - 1;

				if (tree->D >= threshold_zero) {

					if (j >= 0) {
						Out.DIAandGRP_new[j][tree->Grp] += tree->N;
						if (tree->DInc > 0) {
							Out.GROWTHandGRP[j][tree->Grp] +=
								tree->DInc * tree->N;
							Out.GROWTHSUM[j] += tree->DInc * tree->N;
							Out.GROWTH_COUNTER_andGRP[j][tree->Grp] += tree->N;
							Out.GROWTH_COUNTER_SUM[j] += tree->N;
						}
						Out.BADIAandGRP_new[j][tree->Grp] +=
							tree->N * SQR(tree->D) / 4.0 * PI;
						Out.DIASUM[j] += tree->N;
						Out.BADIASUM_new[j] +=
							tree->N * SQR(tree->D) / 4.0 * PI;
						Out.SVDIASUM[j] += tree->N * tree->SV;
					}
					Out.NTOTAL += tree->N;
					Out.NGRP[tree->Grp] += tree->N;
					Out.NCOM[tree->COMGrp] += tree->N;

#ifdef underconstruction
					if (N_Par.GRASSMIND) {
						Out.MeanHeight[tree->Grp] += tree->N * tree->H;
						if (tree->H > Out.MaxHeight[tree->Grp]) {
							Out.MaxHeight[tree->Grp] = tree->H;
						}
						Out.BTGreen[tree->Grp] += (tree->BTgreen*tree->N);
						Out.Biomass_calib[tree->Grp] += (tree->BT_calib*tree->N);
						Out.LAIGrass[tree->Grp] +=
							(tree->L * tree->AC * tree->N) / plot->Area;
						Out.LAIGreen[tree->Grp] +=
							(tree->Lgreen * tree->AC * tree->N) / plot->Area;

						Out.RBTandGRP[tree->Grp] += (tree->RBT*tree->N);
						Out.ShootBiomassMean[tree->Grp] += (tree->BT*tree->N);
						Out.RootDepthMean[tree->Grp] += (tree->RH*tree->N);
						Out.LeafNAreaMean[tree->Grp] +=
							(tree->N*(tree->Nshoot / (tree->L*tree->AC)));
						Out.WMHC[tree->Grp] +=
							CalculateStrataBiomass(hec, plot, tree);
						Out.BiomassDensity[tree->Grp] += (tree->BT*tree->N);
					}
#endif
				}
				if (tree->D >= Switch.Schwelle) {
					if (j >= 0) {
						Out.TH_DIAandGRP_new[j][tree->Grp] += tree->N;
						if (tree->DInc > 0) {
							Out.TH_GROWTHandGRP[j][tree->Grp] +=
								tree->DInc * tree->N;
							Out.TH_GROWTHSUM[j] += tree->DInc * tree->N;
							Out.TH_GROWTH_COUNTER_andGRP[j][tree->Grp] +=
								tree->N;
							Out.TH_GROWTH_COUNTER_SUM[j] += tree->N;
						}
						Out.TH_BADIAandGRP_new[j][tree->Grp] +=
							tree->N * SQR(tree->D) / 4.0 * PI;

						Out.TH_DIASUM[j] += tree->N;
						Out.TH_BADIASUM_new[j] +=
							tree->N * SQR(tree->D) / 4.0 * PI;
					}
					Out.TH_NTOTAL += tree->N;
					Out.TH_NGRP[tree->Grp] += tree->N;
					Out.TH_NCOM[tree->COMGrp] += tree->N;
				}

				if (tree->D >= threshold_zero) {
					Out.BAandGRP[tree->Grp] +=
						tree->N * SQR(tree->D) / 4.0 * PI;
					Out.BAandCOM[tree->COMGrp] +=
						tree->N * SQR(tree->D) / 4.0 * PI;
					Out.BASALSUM += tree->N * SQR(tree->D) / 4.0 * PI;
				}

				if (tree->D >= Switch.Schwelle) {

					Out.TH_BAandGRP[tree->Grp] +=
						tree->N * SQR(tree->D) / 4.0 * PI;
					Out.TH_BAandCOM[tree->COMGrp] +=
						tree->N * SQR(tree->D) / 4.0 * PI;
					Out.TH_BASALSUM += tree->N * SQR(tree->D) / 4.0 * PI;
				}

				if (tree->D >= threshold_zero)
					Out.BTSUM_2 += tree->N * tree->BT;

				if (tree->D >= Switch.Schwelle)
					Out.TH_BTSUM_2 += tree->N * tree->BT;

				if (tree->D >= threshold_zero) {
					Out.SVandGRP[tree->Grp] += tree->N * tree->SV;
					Out.SVandCOM[tree->COMGrp] += tree->N * tree->SV;
					Out.SVSUM += tree->N * tree->SV;

					Out.BVandGRP[tree->Grp] += tree->N * tree->BoleVolume;
					Out.BVandCOM[tree->COMGrp] += tree->N * tree->BoleVolume;
					Out.BVSUM += tree->N * tree->BoleVolume;
				}

				if (tree->D >= Switch.Schwelle) {
					Out.TH_SVandGRP[tree->Grp] += tree->N * tree->SV;
					Out.TH_SVandCOM[tree->COMGrp] += tree->N * tree->SV;
					Out.TH_SVSUM += tree->N * tree->SV;

					Out.TH_BVandGRP[tree->Grp] += tree->N * tree->BoleVolume;
					Out.TH_BVandCOM[tree->COMGrp] += tree->N * tree->BoleVolume;
					Out.TH_BVSUM += tree->N * tree->BoleVolume;
				}

				if (tree->D >= threshold_zero) {
					Out.BTandGRP[tree->Grp] += tree->N * tree->BT;
					Out.BTandCOM[tree->COMGrp] += tree->N * tree->BT;
					if (tree->N > 0)
						Out.BTSUM += tree->N * tree->BT;
					Out.MeanWoodDen +=
						(tree->N*(SQR(tree->D) / 4.0*PI)
						*N_Par.Pro_Rho_3[tree->Grp]);
				}

				if (tree->D >= Switch.Schwelle) {

					Out.TH_BTandGRP[tree->Grp] += tree->N * tree->BT;
					Out.TH_MeanWoodDen +=
						(tree->N*(SQR(tree->D) / 4.0*PI)
						*N_Par.Pro_Rho_3[tree->Grp]);
					Out.TH_BTandCOM[tree->COMGrp] += tree->N * tree->BT;
					Out.TH_BTSUM += tree->N * tree->BT;
					Out.TH_BTCHSUM +=
						tree->N *
						((0.0509 * N_Par.Pro_Rho_3[tree->Grp] * pow
						(tree->D * 100, 2) * tree->H) * 0.001);
					Out.TH_BTCSUM +=
						tree->N *
						((N_Par.Pro_Rho_3[tree->Grp] * exp(-1.562 +
						2.148 * log(tree->D * 100) +
						0.207 * pow(log(tree->D * 100),
						2) - 0.0281 * pow(log(tree->D * 100), 3))) * 0.001);

				}

				if (N_Par.Seedtree) {
					if (tree->D >= N_Par.Est_DSTree_5[tree->Grp]) {
						Out.SEEDTREESUM += tree->N;
						Out.SEEDTREEandGRP[tree->Grp] += tree->N;
						Out.SEEDTREEandCOM[tree->COMGrp] += tree->N;
					}
				}
				tree = tree->next;
			}

			for (pft = 0; pft < MAXGRP; pft++) {
				for (com = 0; com < N_COMMERCIAL; com++) {
					Out.SEEDandGRP[pft] += plot->SeedPool[pft][com];
					Out.SEEDSUM += plot->SeedPool[pft][com];
					lgrp = pft;
					Out.SEEDandCOM[com] += plot->SeedPool[pft][com];
				}
			}

			Out.LAI[hec->HecNo - 1][plot->No - 1] = plot->LAI[0];
			Out.LAI_mean += Out.LAI[hec->HecNo - 1][plot->No - 1];
			for (int layer = 0; layer < MAXLYR; layer++) {
				Out.LAI_Layer[hec->HecNo - 1][plot->No - 1][layer] =
					plot->LAI[layer];
				Out.LAD_Layer[hec->HecNo - 1][plot->No - 1][layer] =
					plot->LAD[layer];
			}

			plot = plot->next;
		}
		hec = hec->next;
	}
	if (Out.TH_BASALSUM == 0.0)
		Out.TH_MeanWoodDen = 0.0;
	else
		Out.TH_MeanWoodDen /= Out.TH_BASALSUM;

	if (Out.BASALSUM == 0.0)
		Out.MeanWoodDen = 0.0;
	else
		Out.MeanWoodDen /= Out.BASALSUM;

	Out.LAI_mean = Out.LAI_mean / (Switch.Ha * Switch.Ha * Switch.Maxplot);

	for (j = 0; j < MAXARRAY; j++) {
		for (kk = 0; kk < MAXGRP; kk++) {
			if (Out.GROWTH_COUNTER_andGRP[j][kk] > 0) {
				Out.GROWTHandGRP[j][kk] =
					Out.GROWTHandGRP[j][kk] / Out.GROWTH_COUNTER_andGRP[j][kk];
			}
			else {
				Out.GROWTHandGRP[j][kk] = -9999;
			}
			if (Out.TH_GROWTH_COUNTER_andGRP[j][kk] > 0) {
				Out.TH_GROWTHandGRP[j][kk] =
					Out.TH_GROWTHandGRP[j][kk]
					/ Out.TH_GROWTH_COUNTER_andGRP[j][kk];
			}
			else {
				Out.TH_GROWTHandGRP[j][kk] = -9999;
			}
		}
		if (Out.GROWTH_COUNTER_SUM[j] > 0) {
			Out.GROWTHSUM[j] = Out.GROWTHSUM[j] / Out.GROWTH_COUNTER_SUM[j];
		}
		else {
			Out.GROWTHSUM[j] = -9999;
		}
		if (Out.TH_GROWTH_COUNTER_SUM[j] > 0) {
			Out.TH_GROWTHSUM[j] =
				Out.TH_GROWTHSUM[j] / Out.TH_GROWTH_COUNTER_SUM[j];
		}
		else {
			Out.TH_GROWTHSUM[j] = -9999;
		}
	}

	// Shannon-Wiener-Diversity-Index
	for (kk = 0; kk < MAXGRP; kk++) {
		if (Out.BTSUM_2 > 0)
			f_bt = Out.BTandGRP[kk] / Out.BTSUM_2;
		else
			f_bt = 0.0;
		if (Out.NTOTAL > 0)
			f_n = Out.NGRP[kk] / Out.NTOTAL;
		else
			f_n = 0.0;
		if (f_n > 0)
			Out.SWI_N -= f_n * log(f_n);
		if (f_bt > 0) {
			Out.SWI_BT -= f_bt * log(f_bt);
		}

		if (Out.TH_BTSUM_2 > 0)
			f_bt = Out.TH_BTandGRP[kk] / Out.TH_BTSUM_2;
		else
			f_bt = 0.0;
		if (Out.TH_NTOTAL > 0)
			f_n = Out.TH_NGRP[kk] / Out.TH_NTOTAL;
		else
			f_n = 0.0;
		if (f_n > 0)
			Out.TH_SWI_N -= f_n * log(f_n);
		if (f_bt > 0)
			Out.TH_SWI_BT -= f_bt * log(f_bt);
	}

	// Calculates values per ha

	for (j = 0; j < MAXARRAY; j++) {
		Out.DIASUM[j] *= Switch.Hectare / AREASUM;
		Out.DIADEATH[j] *= Switch.Hectare / AREASUM;
		Out.BADIASUM_new[j] *= Switch.Hectare / AREASUM;
		Out.SVDIASUM[j] *= Switch.Hectare / AREASUM;
		Out.TH_DIASUM[j] *= Switch.Hectare / AREASUM;
		Out.TH_BADIASUM_new[j] *= Switch.Hectare / AREASUM;
		for (kk = 0; kk < MAXGRP; kk++) {
			Out.DIAandGRP_new[j][kk] *= Switch.Hectare / AREASUM;
			Out.DIADEATH_PFT[j][kk] *= Switch.Hectare / AREASUM;
			Out.TH_DIAandGRP_new[j][kk] *= Switch.Hectare / AREASUM;
			Out.BADIAandGRP_new[j][kk] *= Switch.Hectare / AREASUM;
			Out.TH_BADIAandGRP_new[j][kk] *= Switch.Hectare / AREASUM;
		}

	}

	for (kk = 0; kk < MAXGRP; kk++) {
		Out.BAandGRP[kk] *= Switch.Hectare / AREASUM;
		Out.SVandGRP[kk] *= Switch.Hectare / AREASUM;
		Out.BVandGRP[kk] *= Switch.Hectare / AREASUM;
		Out.NGRP[kk] *= Switch.Hectare / AREASUM;
		Out.BTandGRP[kk] *= Switch.Hectare / AREASUM;
		Out.TH_BAandGRP[kk] *= Switch.Hectare / AREASUM;
		Out.TH_SVandGRP[kk] *= Switch.Hectare / AREASUM;
		Out.TH_BVandGRP[kk] *= Switch.Hectare / AREASUM;
		Out.TH_NGRP[kk] *= Switch.Hectare / AREASUM;
		Out.TH_BTandGRP[kk] *= Switch.Hectare / AREASUM;
		Out.InTH_GRP[kk] *= Switch.Hectare / AREASUM;
		Out.SEEDandGRP[kk] *= Switch.Hectare / AREASUM;
		Out.SEEDLINGandGRP[kk] *= Switch.Hectare / AREASUM;
		Out.SEEDTREEandGRP[kk] *= Switch.Hectare / AREASUM;
		Out.SEEDRAINandGRP[kk] *= Switch.Hectare / AREASUM;
		Out.DEATH_PFT[kk] *= Switch.Hectare / AREASUM;
		Out.TH_DEATH_PFT[kk] *= Switch.Hectare / AREASUM;

#ifdef underconstruction
		if (N_Par.GRASSMIND) {
			if (Out.NGRP[kk] > 0) {
				Out.MeanHeight[kk] /= Out.NGRP[kk];

				Out.BTGreen[kk] *= Switch.Hectare / AREASUM;
				Out.Biomass_calib[kk] *= Switch.Hectare / AREASUM;
				Out.LAIGreen[kk] *= Switch.Hectare / AREASUM;
				Out.LAIGrass[kk] *= Switch.Hectare / AREASUM;

				Out.RBTandGRP[kk] *= Switch.Hectare / AREASUM;
				Out.ShootBiomassMean[kk] /= Out.NGRP[kk];
				Out.RootDepthMean[kk] /= Out.NGRP[kk];
				Out.LeafNAreaMean[kk] /= Out.NGRP[kk];
				Out.WMHC[kk] *= Switch.Hectare / AREASUM;
				Out.BiomassDensity[kk] /= Out.MaxHeight[kk];
			}
		}
#endif
	}

	for (kk = 0; kk < 2; kk++) {
		Out.BAandCOM[kk] *= Switch.Hectare / AREASUM;
		Out.SVandCOM[kk] *= Switch.Hectare / AREASUM;
		Out.BVandCOM[kk] *= Switch.Hectare / AREASUM;
		Out.NCOM[kk] *= Switch.Hectare / AREASUM;
		Out.BTandCOM[kk] *= Switch.Hectare / AREASUM;
		Out.TH_BAandCOM[kk] *= Switch.Hectare / AREASUM;
		Out.TH_SVandCOM[kk] *= Switch.Hectare / AREASUM;
		Out.TH_BVandCOM[kk] *= Switch.Hectare / AREASUM;
		Out.TH_NCOM[kk] *= Switch.Hectare / AREASUM;
		Out.TH_BTandCOM[kk] *= Switch.Hectare / AREASUM;
		Out.SEEDandCOM[kk] *= Switch.Hectare / AREASUM;
		Out.SEEDTREEandCOM[kk] *= Switch.Hectare / AREASUM;
		Out.SEEDRAINandCOM[kk] *= Switch.Hectare / AREASUM;
	}
	Out.NTOTAL *= Switch.Hectare / AREASUM;
	Out.BASALSUM *= Switch.Hectare / AREASUM;
	Out.SVSUM *= Switch.Hectare / AREASUM;
	Out.BVSUM *= Switch.Hectare / AREASUM;
	Out.BTSUM *= Switch.Hectare / AREASUM;
	Out.TH_NTOTAL *= Switch.Hectare / AREASUM;
	Out.TH_BASALSUM *= Switch.Hectare / AREASUM;
	Out.TH_SVSUM *= Switch.Hectare / AREASUM;
	Out.TH_BVSUM *= Switch.Hectare / AREASUM;
	Out.TH_BTSUM *= Switch.Hectare / AREASUM;
	Out.TH_BTCSUM *= Switch.Hectare / AREASUM;

	Out.DEATH *= Switch.Hectare / AREASUM;
	Out.TH_DEATH *= Switch.Hectare / AREASUM;
	Out.SPACEDEATH *= Switch.Hectare / AREASUM;
	Out.TH_SPACEDEATH *= Switch.Hectare / AREASUM;

	Out.InTH *= Switch.Hectare / AREASUM;
	Out.SEEDSUM *= Switch.Hectare / AREASUM;
	Out.SEEDRAINSUM *= Switch.Hectare / AREASUM;
	Out.SEEDLINGSUM *= Switch.Hectare / AREASUM;
	Out.SEEDTREESUM *= Switch.Hectare / AREASUM;

	Out.C_flux *= Switch.Hectare / AREASUM;
	Out.CPool_DeadWood *= Switch.Hectare / AREASUM;
	Out.CPool_alive_biomass *= Switch.Hectare / AREASUM;
	Out.CPool_Soil_fast *= Switch.Hectare / AREASUM;
	Out.CPool_Soil_slow *= Switch.Hectare / AREASUM;
	Out.Cflux_to_Soil_fast *= Switch.Hectare / AREASUM;
	Out.Cflux_to_Soil_slow *= Switch.Hectare / AREASUM;
	Out.Cflux_to_DeadWood *= Switch.Hectare / AREASUM;
	Out.Resp_DeadWood *= Switch.Hectare / AREASUM;
	Out.Resp_Soil_slow *= Switch.Hectare / AREASUM;
	Out.Resp_Soil_fast *= Switch.Hectare / AREASUM;
	Out.R_total_biomass *= Switch.Hectare / AREASUM;
	Out.PB *= Switch.Hectare / AREASUM;
	Out.R_total *= Switch.Hectare / AREASUM;
	Out.aet *= Switch.Hectare / AREASUM;

	if (Out.TH_NTOTAL > 0)
		Out.InTH /= Out.TH_NTOTAL;
	else
		Out.InTH = 1;
	for (kk = 0; kk < MAXGRP; kk++)
		if (Out.TH_NTOTAL > 0)
			Out.InTH_GRP[kk] /= Out.TH_NTOTAL;
		else
			Out.InTH_GRP[kk] = 1;

}

// -----------------------------------------------------------------------------

/* !
 \brief          Calculation of basal area, stem numbers, above ground alive
 and dead biomass and root biomass for each plot.
 \param	        void
 \return         void
 \details        units: per plot. not averaged over several time steps!
 */

void CalculatePlotOutput(void) {

	TreePointer tree;
	PlotPointer plot;
	HecPointer hec;
	double bm, bm_th; // aboveground biomass
	double ba, ba_th; // basal area
	double sn, sn_th; // stem numbers
	double sv, sv_th; // stem volume
	double den_th; // wood density
	double bmGrp[HYPERMAXGRP], baGrp[HYPERMAXGRP], snGrp[HYPERMAXGRP],
		rbmGrp[HYPERMAXGRP];
	// biomass, basal area, stem number, root biomass per species and plot
	double bmGrpth[HYPERMAXGRP], baGrpth[HYPERMAXGRP], snGrpth[HYPERMAXGRP];
	// biomass, basal area, stem number per species and plot threshold th
	double bmgreenGrp[HYPERMAXGRP], bmbrownGrp[HYPERMAXGRP];

	int maxclass = (int) floor(DMAX / (float)N_Par.Div_DiaClassWidth + 1.0);
	hec = FirstHec;
	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {

			bm = 0;
			ba = 0;
			sn = 0;
			sv = 0;
			bm_th = 0;
			ba_th = 0;
			sn_th = 0;
			sv_th = 0;
			den_th = 0;

			for (int j = 0; j < maxclass; j++)
				diadd[j] = 0.0;

			for (int id = 0; id < MAXGRP; id++) {
				rbmGrp[id] = 0;
				bmGrp[id] = 0;
				bmgreenGrp[id] = 0;
				bmbrownGrp[id] = 0;
				baGrp[id] = 0;
				snGrp[id] = 0;

				bmGrpth[id] = 0.0;
				baGrpth[id] = 0.0;
				snGrpth[id] = 0.0;
				for (int j = 0; j < maxclass; j++)
					diagrp[j][id] = 0.0;
			}

			tree = plot->FirstTree;
			while (tree != NULL) {
				if (tree->N > 0) {
					bm += tree->N * tree->BT;
					ba += tree->N * SQR(tree->D) / 4.0 * PI;
					sn += tree->N;
					sv += tree->N * tree->SV;

					if (tree->D >= Switch.Schwelle) {
						bm_th += tree->N * tree->BT;
						ba_th += tree->N * SQR(tree->D) / 4.0 * PI;
						sn_th += tree->N;
						sv_th += tree->N * tree->SV;
						den_th +=
							(tree->N*(SQR(tree->D) / 4.0*PI)
							*N_Par.Pro_Rho_3[tree->Grp]);

					}

					/////////////////////////////////////////////
					// diameter class
					double dummy_tree_d;
#ifdef underconstruction
					if (N_Par.GRASSMIND)
						dummy_tree_d = tree->H - 0.00000000000001;
					else
#endif
						dummy_tree_d = tree->D - 0.0000001;
					int ll =
						(int) floor(dummy_tree_d / N_Par.Div_DiaClassWidth);
					if (ll >= maxclass)
						ll = maxclass - 1;
					if (ll >= 0) {
						diadd[ll] += tree->N;
						diagrp[ll][tree->Grp] += tree->N;
					}

					rbmGrp[tree->Grp] += tree->N * tree->RBT;
					bmGrp[tree->Grp] += tree->N * tree->BT;
					snGrp[tree->Grp] += tree->N;

#ifdef underconstruction
					if (N_Par.GRASSMIND) {
						baGrp[tree->Grp] += tree->N * tree->AC;
						bmgreenGrp[tree->Grp] +=
							tree->N * tree->BT * tree->BTFracGreen;
						bmbrownGrp[tree->Grp] +=
							tree->N * tree->BT * tree->BTFracBrown;
					}
					else
#endif
					{
						baGrp[tree->Grp] += tree->N * SQR(tree->D) / 4.0 * PI;
						double tr = N_Par.Geo_TR_21[0][tree->Grp];
						bmgreenGrp[tree->Grp] +=
							tree->N * tree->BT * (1.0 - tr);
						bmbrownGrp[tree->Grp] += tree->N * tree->BT * tr;
					}

					if (tree->D >= Switch.Schwelle) {
						bmGrpth[tree->Grp] += tree->N * tree->BT;
#ifdef underconstruction
						if (N_Par.GRASSMIND)
							baGrpth[tree->Grp] += tree->N * tree->AC;
						else
#endif
							baGrpth[tree->Grp] +=
								tree->N * SQR(tree->D) / 4.0 * PI;
						snGrpth[tree->Grp] += tree->N;
					}

				}
				tree = tree->next;
			}
			Out.BMPlot[hec->HecNo - 1][plot->No - 1] = bm;
			Out.RBMPlot[hec->HecNo - 1][plot->No - 1] = 0; // rbm;
			Out.RCONPlot[hec->HecNo - 1][plot->No - 1] = 0; // rcon/area;
			Out.BMofNewTrees[hec->HecNo - 1][plot->No - 1] =
				plot->BiomassNewTrees;
			Out.NEWDBMPlot[hec->HecNo - 1][plot->No - 1] = plot->NewDeadBiomass;

			plot->Biomass = bm;
			plot->BasalArea = ba;
			plot->StemNumber = sn;
			plot->StemVolume = sv;
			plot->Biomass_th = bm_th;
			plot->BasalArea_th = ba_th;
			plot->StemNumber_th = sn_th;
			plot->StemVolume_th = sv_th;
			if (ba_th == 0.0)
				plot->MeanWoodDen = 0.0;
			else
				plot->MeanWoodDen = den_th / ba_th;

			for (int j = 0; j < maxclass; j++) {
				plot->DiaSum[j] = diadd[j];
				for (int k = 0; k < MAXGRP; k++)
					plot->DiaGrp[j][k] = diagrp[j][k];
			}
			for (int id = 0; id < MAXGRP; id++) {
				plot->RootBiomassGrp[id] = rbmGrp[id];
				plot->BiomassGrp[id] = bmGrp[id];
				plot->BiomassGreenGrp[id] = bmgreenGrp[id];
				plot->BiomassBrownGrp[id] = bmbrownGrp[id];
				plot->BasalAreaGrp[id] = baGrp[id];
				plot->StemNumberGrp[id] = snGrp[id];

				plot->TH_BiomassGrp[id] = bmGrpth[id];
				plot->TH_BasalAreaGrp[id] = baGrpth[id];
				plot->TH_StemNumberGrp[id] = snGrpth[id];
			}
#ifdef underconstruction
			CalculateStructureIndex(plot);
#endif
			plot = plot->next;
		}
		hec = hec->next;
	}

}
