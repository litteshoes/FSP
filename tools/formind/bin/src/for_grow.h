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
// File						for_grow.h
// Description				calculates tree growth
//
///////////////////////////////////////////////////////////////////

#ifndef for_growH
#define for_growH

#include "for_global.h"
#include "for_var.h"
#ifdef underconstruction
#include "for_trait.h"
#endif
#include <stdexcept>

/* !
 \brief       Calculation of diameter growth
 */
class DGrow {
public:

/* !
 \brief       Calculation of maximal diameter growth
 \param[in]   grp     Species group (PFT)
 \param[in]   dbh     DBH
 */
	virtual double CalculateMaxGrowth(int grp, double dbh) = 0;
	virtual ~DGrow() {
	};
};

/* !
 \brief       Calculation of diameter growth using polynomial model
 */
class GrowPoly : public DGrow {

	/* !
	 \brief       Calculation of diameter growth using polynomial model which is fitted to given point coordinates
	 */
public:
	GrowPoly(double(*_coef)[4]) : coef(_coef) {
	};

	
	double CalculateMaxGrowth(int grp, double dbh) {
		const double*grpcoef = &coef[grp][0];
		return (grpcoef[3] * pow(dbh, 3) + grpcoef[2] * SQR(dbh) +
			 grpcoef[1] * dbh + grpcoef[0]);
	};

protected:
	double(*coef)[4];
};

/* !
 \brief       Calculation of diameter growth using polynomial model which is defined by coefficients
 */
class GrowPolyCoefficient : public DGrow {
public:
	GrowPolyCoefficient(double**_coef) : coef(_coef) {
	};

	double CalculateMaxGrowth(int grp, double dbh) {
		double g0 = coef[0][grp];
		double g1 = coef[1][grp];
		double g2 = coef[2][grp];
		double g3 = coef[3][grp];
		return (g3 * pow(dbh, 3) + g2 * pow(dbh, 2) + g1 * dbh + g0);
	}

protected:
	double**coef, *max_dbh_m;
};

/* !
 \brief       Calculation of diameter growth using Weibull model which is
 defined by coefficients
 */
class GrowWeibull : public DGrow {
public:
	GrowWeibull(double**_coef) : coef(_coef) {
	};

	double CalculateMaxGrowth(int grp, double dbh) {
		double g0 = coef[0][grp];
		double g1 = coef[1][grp];
		double g2 = coef[2][grp];
		return (g0 * g1 * g2 * pow((dbh * g1), (g2 - 1)) * exp
			 (-pow((dbh * g1), g2)));
	}

protected:
	double**coef;
};

/* !
 \brief       Calculation of diameter growth using Richards model which is
 defined by coefficients
 */
class GrowRichards : public DGrow {
public:
	GrowRichards(double**_coef) : coef(_coef) {
	};

	double CalculateMaxGrowth(int grp, double dbh) {
		double g0 = coef[0][grp];
		double g1 = coef[1][grp];
		double g2 = coef[2][grp];
		return (g0 * g1 * g2 * exp(-g1 * dbh) * pow((1 - exp(-g1 * dbh)),
			 (g2 - 1)));
		// Zeide et al 1993
	}

protected:
	double**coef;
};

/* !
 \brief       Calculation of diameter growth using Chanter model which is
 defined by coefficients
 */
class GrowChanter : public DGrow {
public:
	GrowChanter(double**_coef, double*_max_dbh_m)
		 : coef(_coef), max_dbh_m(_max_dbh_m) {
	};

	double CalculateMaxGrowth(int grp, double dbh) {
		double g0 = coef[0][grp];
		double g1 = coef[1][grp];
		double g2 = coef[2][grp];
		return (g0 * dbh * (1 - dbh / max_dbh_m[grp]) * exp(-g1 * dbh));
	}

protected:
	double**coef, *max_dbh_m;
};

/* !
 \brief       Calculation of diameter growth using Chanter model which is fitted to given point coordinates
 */
class GrowChanterMaxPos : public DGrow {
public:
	GrowChanterMaxPos(double(*_coef)[4], double*_max_dbh_m)
		 : coef(_coef), max_dbh_m(_max_dbh_m) {
	};

	double CalculateMaxGrowth(int grp, double dbh) {
		const double g0 = coef[grp][0]; // intern ausgerechnete koeffizienten
		const double g1 = coef[grp][1];
		return (g0 * dbh * (1 - dbh / max_dbh_m[grp]) * exp(-g1 * dbh));
	}

protected:
	double(*coef)[4], *max_dbh_m;
};

/* !
 \brief       Calculation of diameter growth using Haefner model which is defined by coefficients
 */
class Grow_maxHaefner : public DGrow {
public:
	Grow_maxHaefner(double**_coef) : coef(_coef) {
	};

	double CalculateMaxGrowth(int grp, double dbh) {
		double g0 = coef[0][grp];
		double g1 = coef[1][grp];
		double g2 = coef[2][grp];
		return (g0 * pow(dbh, g1) * exp(g2 * dbh)) / 100;
	} // Haefner (1996): Modelling Biological Systems, S. 91ff, J

protected:
	double**coef;
};

/* !
 \brief       Calculation of diameter from biomass
 */
class cDoutofB {
public:
	// TreePointer as third argument is needed for DoutofB_lookup.
	// Other versions of DoutofB don't make use of it.
	// old version: virtual double Calculate(double biomass, int pft) = 0;
	virtual double Calculate(double biomass, int pft, TreePointer tree) = 0;

	virtual ~cDoutofB() {
	};
};

/* !
 \brief       Calculation of diameter from biomass
 */
class DoutofB_friedrich : public cDoutofB {
public:
	DoutofB_friedrich(double**_coef_d, int _which)
		 : coef_d(_coef_d), which(_which) {

	};

	double Calculate(double biomass, int pft, TreePointer tree) {
		double b1 = N_Par.Geo_Bio2Dbh[0][pft];
		double b2 = N_Par.Geo_Bio2Dbh[1][pft];
		double b3 = N_Par.Geo_Bio2Dbh[2][pft];
		double d = 0.;
		if (which == 1) { // short powerlaw version 10.2011 Friedrich
			d = pow(biomass / b1, 1 / b2);
			// Ter-Mikaelian and Korzukhin 1997; Chave etal
			// d [m]; b [todm]
		}
		else if (which == 2) {
			double log_biomass = log(biomass);
			double log_diameter =
				 b3 + (log_biomass - 2 * b1 * b2 + pow((4 * pow(b1, 2) * pow(b2,
				 2) + pow(log_biomass, 2)), 0.5)) / (2 * b1);
			// siehe kommentar BoutofD()
			d = exp(log_diameter) / 100;
		}
		return (d);
	};

	int which;
	double**coef_d;
};

/* !
 \brief       Calculation of diameter from biomass
 */
class DoutofB_standard : public cDoutofB {
public:
	DoutofB_standard(double**_coef_fd, double**_coef_hd, double*_coef_rho)
		 : coef_fd(_coef_fd), coef_hd(_coef_hd), coef_rho(_coef_rho) {

	};

	double Calculate(double biomass, int pft, TreePointer tree) {
		double h0 = coef_hd[0][pft];
		double h1 = coef_hd[1][pft];
		double f0 = N_Par.Geo_FD_31[0][pft];
		double f1 = N_Par.Geo_FD_31[1][pft];
		double rho = N_Par.Pro_Rho_3[pft];
		double diameter = 0.0;
		// Here, power-laws are assumed for d-h-relation and d-f-relation
		if (N_Par.Geo_FUNCTION_7[0] == 2 && N_Par.Geo_FUNCTION_7[3] == 1) {
			diameter = pow(biomass * N_Par.Geo_TR_21[0][pft] / ((PI / 4.) * h0 * f0 * rho),
				 1 / (2 + h1 + f1));
		}
		else {
			std::cerr << "ERROR: Calculate DoutofB not available for the choosen combination of tree geometry."
			<< "\tFile: " << __FILE__ << ".\t Line:" << std::endl;
			throw std::runtime_error("ERROR in Biomass calculation.");
		}
		return diameter;
	};

	double**coef_fd, **coef_hd, *coef_rho;
};

/* !
 \brief  Calculation of diameter from biomass using lookup vectors for D and B, because of
 a non-invertible relationship, e.g. when using a Michaelis-Menten D-H-function.
 */
class DoutofB_lookup : public cDoutofB {
public:
	DoutofB_lookup(double**_coef_fd, double**_coef_hd, double*_coef_rho)
		 : coef_fd(_coef_fd), coef_hd(_coef_hd), coef_rho(_coef_rho) {

	};

	double Calculate(double biomass, int pft, TreePointer tree) {

		double diameter = vDiameterLookup[pft][tree->lastVecPosition];
		// Move up the biomass lookup vector starting at the last known vector position of the tree
		// and going until the newly calculated biomass of the tree is reached. In case of heterogeneity
		// in the D-H-relation, the reference biomass is not the actual biomass of the tree, but
		// the one corrected for the heterogeneity pre-factor.
		while (vBiomassLookup[pft][tree->lastVecPosition] <=
			 (biomass / tree->DHprefactor)) {
			// The diameter corresponding to the current biomass is taken from the lookup vector and
			// set as the tree's new diameter.
			diameter = vDiameterLookup[pft][tree->lastVecPosition];
			// The vector position is incremented and memorized
			tree->lastVecPosition += 1;
		}
		return diameter;
	};

	double**coef_fd, **coef_hd, *coef_rho;
};

/* !
 \brief       Calculation of diameter from biomass
 */
class DoutofB_discrete : public cDoutofB {
public:
	DoutofB_discrete(double**_coef_fd, double**_coef_hd, double*_coef_rho)
		 : coef_fd(_coef_fd), coef_hd(_coef_hd), coef_rho(_coef_rho) {

	};

	double Calculate(double biomass, int pft, TreePointer tree) {

		double diameter, blow, bhigh;
		int found, dindex, dummy, olddummy, dlow, dhigh, call, which;

		dindex = (int) ceil(MAXB2D / 2.0);
		dummy = (int) ceil(MAXB2D / 2.0);
		found = 0;
		call = 0;

		while ((found != 1) && (dindex < MAXB2D)) {
			call++;
			olddummy = dummy;
			dummy = (int) ceil(dummy / 2.0);

			if ((dummy == 1) && (olddummy == 1))
				found = 1;

			if (B2D[dindex][pft] > biomass) {
				if (dindex >= dummy)
					dindex -= dummy;
				else
					dindex = 0;
				which = 0;
			}
			else {
				dindex += dummy;
				which = 1;
			}
		}

		if (which == 0)
			dhigh = dindex + 1;
		else
			dhigh = dindex;
		if (dhigh >= MAXB2D)
			dhigh = MAXB2D - 1;
		dlow = dhigh - 1;
		blow = B2D[dlow][pft];
		bhigh = B2D[dhigh][pft];

		diameter = dlow + (biomass - blow) * ((dhigh - dlow) / (bhigh - blow));
		// 2pointsequation

		diameter = diameter / B2DSTEP; // d in [m]

		return diameter;
	};

	double**coef_fd, **coef_hd, *coef_rho;
};

/* !
 \brief       Calculation of diameter from biomass for grass
 */
class DoutofB_grass : public cDoutofB {
public:
	DoutofB_grass(double**_coef_fd, double**_coef_hd, double*_coef_rho)
		 : coef_fd(_coef_fd), coef_hd(_coef_hd), coef_rho(_coef_rho) {

	};

	double Calculate(double biomass, int pft, TreePointer tree) {
		double f0, f1, rho;
		double h1 = coef_hd[0][pft];
		double h2 = coef_hd[1][pft];

		f0 = N_Par.Geo_FD_31[0][pft];
		f1 = N_Par.Geo_FD_31[1][pft];
		rho = N_Par.Pro_Rho_3[pft];

		double diameter = pow(biomass * N_Par.Geo_TR_21[0][pft] / ((PI / 4.) * h1 * f0 * rho),
			 1 / (2 + h2 + f1));

		return diameter;
	};

	bool fmulti;
	double**coef_fd, **coef_hd, *coef_rho;
};

/* !
 \brief       Calculation of biomass from diameter
 param[in] 	d		DBH
 param[in] 	h		Height
 param[in] 	f		Form factor
 param[in] 	lgrp	Species group (PFT)
 */
class cBoutofD {
public:
	virtual double Calculate(double d, double h, double f, int lgrp) = 0;

	virtual ~cBoutofD() {
	};
};

/* !
 \brief       Calculation of biomass from diameter
 param[in] 	d		DBH
 param[in] 	h		Height
 param[in] 	f		Form factor
 param[in] 	lgrp	Species group (PFT)
 */
class BoutofD_standard : public cBoutofD {
public:
	BoutofD_standard(double*coef_rho) : coef_rho(coef_rho) {

	};

	double Calculate(double d, double h, double f, int lgrp) {
		return (SQR(d) * h * (PI / 4.0) * f * (coef_rho[lgrp] / N_Par.Geo_TR_21[0][lgrp]));
	}

	double*coef_rho;
};

/* !
 \brief       Calculation of biomass from diameter
 param[in] 	d		DBH
 param[in] 	h		Height
 param[in] 	f		Form factor
 param[in] 	lgrp	Species group (PFT)
 */
class BoutofD_friedrich : public cBoutofD {
public:
	BoutofD_friedrich(double**_coef_d, int _which)
		 : coef_d(_coef_d), which(_which) {

	};

	double Calculate(double d, double h, double f, int lgrp) {
		double b1 = coef_d[0][lgrp];
		double b2 = coef_d[1][lgrp];
		double b3 = coef_d[2][lgrp];
		double biomass;
		if (which == 1) { // new Version 10.2011 Friedrich
			biomass = b1 * pow(d, b2);
			// Ter-Mikaelian and Korzukhin 1997; Chave etal
			// d [m]; b [todm]
		}
		else if (which == 2) { // new Version 10.2011 Friedrich
			double d_log = log(d * 100); // converting to cm and logarithm
			biomass = b1 * (d_log - b3) * (2 * b2 + (d_log - b3)) /
				 (b2 + (d_log - b3));
			// because of overestimation of biomass for small trees by using the powerlaw this formula is developed to produce a better fit. It is a power law corrected by a negative michaelis-menten function
			biomass = exp(biomass); // converting back
			// d [m]; b [todm]
		}
		return biomass;
	}

	int which;
	double**coef_d;
};

/* !
 \brief       Calculation of height from diameter
 param[in] 	d		DBH
 param[in] 	grp	Species group (PFT)
 */
class cHoutofD {
public:
	virtual double Calculate(double d, int grp) = 0;

	virtual ~cHoutofD() {
	};
};

/* !
 \brief       Calculation of diameter from height
 param[in] 	h		Height
 param[in] 	grp	Species group (PFT)
 */
class cDoutofH {
public:
	virtual double Calculate(double h, int grp) = 0;

	virtual ~cDoutofH() {
	};
};

/* !
 \brief     Calculation of diameter from height
 */
class DoutofH_0 : public cDoutofH {
public:
	DoutofH_0(double**_coef) : coef(_coef) {
	};

	double Calculate(double h, int grp) {
		// 2nd Order Polynom Function: inverse
		double h0 = coef[0][grp];
		double h1 = coef[1][grp];
		double h2 = coef[2][grp];
		double term_under_root = SQR(h1 / (2 * h2)) - (h0 - h) / h2;
		double d1;
		double d2;
		double d;
		if (term_under_root >= 0.0) {
			// Calculate the two possible solutions
			d1 = -h1 / (2 * h2) - sqrt(term_under_root);
			d2 = -h1 / (2 * h2) + sqrt(term_under_root);
			if (d1 > 0.0 && d2 > 0.0) {
				if (d1 < d2) {
					d = d1;
				}
				else {
					d = d2;
				}
			}
			else if (d2 > 0.0) {
				d = d2;
			}
			else if (d1 > 0.0) {
				d = d1;
			}
		}
		else {
			std::cerr << "ERROR: corrupt diameter-height-function for PFT " <<
				 grp + 1 << "\tfile: " __FILE__ <<
				 "\tfunction: DoutofH_0\t line:" << __LINE__ <<
				 "\t Choose different parameters h0, h1, h2 to get increasing diameter-height-function and make sure that the curve reaches the maximal height of the PFT, otherwise lower the maximal height parameter." <<
				 std::endl;
			throw std::runtime_error("ERROR corrupt diameter-height-function");
		}

		return d;
	}

	double**coef;
};

class HoutofD_0 : public cHoutofD {
public:
	HoutofD_0(double**_coef) : coef(_coef) {
	};

	double Calculate(double d, int grp) {
		double h0 = coef[0][grp];
		double h1 = coef[1][grp];
		double h2 = coef[2][grp];
		return (h0 + h1 * d + h2 * SQR(d));
	}

	double**coef;
};

class DoutofH_1 : public cDoutofH {
public:
	DoutofH_1(double**_coef) : coef(_coef) {
	};

	double Calculate(double h, int grp) {
		double h0 = coef[0][grp];
		double h1 = coef[1][grp];
		double h2 = coef[2][grp];
		return h / (h0 - h0 * h / h1);
	}

	double**coef;
};

class HoutofD_1 : public cHoutofD {
public:
	HoutofD_1(double**_coef) : coef(_coef) {
	};

	double Calculate(double d, int grp) {
		double h0 = coef[0][grp];
		double h1 = coef[1][grp];
		double h2 = coef[2][grp];
		return (d / (1 / h0 + d / h1));
	}

	double**coef;
};

class DoutofH_2 : public cDoutofH {
public:
	DoutofH_2(double**_coef) : coef(_coef) {
	};

	double Calculate(double h, int grp) {
		double h0 = coef[0][grp];
		double h1 = coef[1][grp];
		double h2 = coef[2][grp];
		if (N_Par.GRASSMIND)
			return (exp(1 / h1 * log(h / h0)));
		else
			return exp(1 / h1 * log(h / h0));
	}

	double**coef;
};

class HoutofD_2 : public cHoutofD {
public:
	HoutofD_2(double**_coef) : coef(_coef) {
	};

	double Calculate(double d, int grp) {
		double h0 = coef[0][grp];
		double h1 = coef[1][grp];
		double h2 = coef[2][grp];
		return (h0 * pow(d, h1));
	}

	double**coef;
};

class DoutofH_3 : public cDoutofH {
public:
	DoutofH_3(double**_coef) : coef(_coef) {
	};

	double Calculate(double h, int grp) {
		// Michaelis-Menten Function: inverse
		double h0 = coef[0][grp];
		double h1 = coef[1][grp];
		return (-h * h1) / (h - h0);
	}

	double**coef;
};

class HoutofD_3 : public cHoutofD {
public:
	HoutofD_3(double**_coef) : coef(_coef) {
	};

	double Calculate(double d, int grp) {
		// Michaelis-Menten Function
		double h0 = coef[0][grp];
		double h1 = coef[1][grp];
		return ((h0 * d) / (h1 + d));
	}

	double**coef;
};

/* !
 \brief       Calculation of maintenance respiration
 */
class MLoss {
public:
	virtual double CalculateMaintResp(TreePointer tree, PlotPointer plot) = 0;

	virtual ~MLoss() {
	};
};

/* !
 \brief       Calculation of maintenance respiration using approach 1 (either a
 power law or a linear function)
 */
class LossFunction_1 : public MLoss {
public:
	LossFunction_1() {
	};

	double CalculateMaintResp(TreePointer tree, PlotPointer plot) {

		double mresp, ma2, x1, y1;
		double a0, a1, a2, ra;

		a0 = N_Par.Pro_MLoss_33[0][tree->Grp];
		a1 = N_Par.Pro_MLoss_33[1][tree->Grp];
		a2 = N_Par.Pro_MLoss_33[2][tree->Grp];
		ra = N_Par.Pro_GLoss;

		if (tree->D > DM[tree->Grp])
			tree->D = DM[tree->Grp];

		if (tree->D >= a2) { // a2 is lower diameter threshold for function
			mresp = a0 * pow((double)tree->D, a1);

		}
		else {
			ma2 = a0 * a1 * pow((double)a2, (a1 - 1));
			x1 = a2;
			y1 = a0 * pow((double)a2, a1);
			mresp = (ma2 * (tree->D - x1) + y1);
		}

		mresp /= (1 - ra);

		return mresp;
	};

};

/* !
 \brief       Calculation of maintenance respiration using approach 2
 (exponential function)
 */
class LossFunction_2 : public MLoss {
public:
	LossFunction_2() {
	};

	double CalculateMaintResp(TreePointer tree, PlotPointer plot) {

		double mresp, ma2, x1, y1;
		double a0, a1, a2, ra;

		a0 = N_Par.Pro_MLoss_33[0][tree->Grp];
		a1 = N_Par.Pro_MLoss_33[1][tree->Grp];
		a2 = N_Par.Pro_MLoss_33[2][tree->Grp];
		ra = N_Par.Pro_GLoss;

		if (tree->D > DM[tree->Grp])
			tree->D = DM[tree->Grp];

		mresp = a0 * exp(a1 * pow((double)tree->D, a2));
		mresp /= (1 - ra);

		return mresp;
	};

};

/* !
 \brief       Calculation of maintenance respiration using approach 3
 (exponential function)
 */
class LossFunction_3 : public MLoss {
public:
	LossFunction_3() {
	};

	double CalculateMaintResp(TreePointer tree, PlotPointer plot) {

		double mresp, ma2, x1, y1;
		double a0, a1, a2, ra;

		a0 = N_Par.Pro_MLoss_33[0][tree->Grp];
		a1 = N_Par.Pro_MLoss_33[1][tree->Grp];
		a2 = N_Par.Pro_MLoss_33[2][tree->Grp];
		ra = N_Par.Pro_GLoss;

		if (tree->D > DM[tree->Grp])
			tree->D = DM[tree->Grp];

		mresp = pow(1 - tree->D / DM[tree->Grp], a0) * (1 - a1) + a1;
		mresp /= (1 - ra);

		return mresp;
	};

};

/* !
 \brief       Calculation of maintenance respiration using approach 4
 (polynomial)
 */
class LossFunction_4 : public MLoss {
public:
	LossFunction_4() {
	};

	double CalculateMaintResp(TreePointer tree, PlotPointer plot) {

		double mresp, ma2, x1, y1, totre;
		double a0, a1, a2, ra;

		a0 = N_Par.Pro_MLoss_33[0][tree->Grp];
		a1 = N_Par.Pro_MLoss_33[1][tree->Grp];
		a2 = N_Par.Pro_MLoss_33[2][tree->Grp];
		ra = N_Par.Pro_GLoss;

		if (tree->D > DM[tree->Grp])
			tree->D = DM[tree->Grp];

		if (tree->D < 0.2)
			totre = 0.16 * tree->BT;
		else
			totre = a0 * tree->D + a1 * pow(tree->D, 2) + a2 * pow(tree->D, 3);

		mresp = totre / tree->BT;
		mresp /= (1 - ra);

		return mresp;
	};

};

/* !
 \brief       Calculation of maintenance respiration using approach 5
 */
class LossFunction_5 : public MLoss {
public:
	LossFunction_5() {
	};

	double CalculateMaintResp(TreePointer tree, PlotPointer plot) {

		double mresp, ma2, x1, y1, totre;
		double a0, a1, a2, ra;

		a0 = N_Par.Pro_MLoss_33[0][tree->Grp];
		a1 = N_Par.Pro_MLoss_33[1][tree->Grp];
		a2 = N_Par.Pro_MLoss_33[2][tree->Grp];
		ra = N_Par.Pro_GLoss;

		if (tree->D > DM[tree->Grp])
			tree->D = DM[tree->Grp];

		totre = a0 * pow((double)tree->BT, 2.0 / 3.0) + a1 * tree->BT +
			 a2 * SQR(tree->BT);
		mresp = totre / tree->BT;

		mresp /= (1 - ra);

		return mresp;
	};

};

class formindGrow {
public:
	formindGrow() : DoutofBFunc(NULL), BoutofDFunc(NULL), DoutofHFunc(NULL),
		 HoutofDFunc(NULL), DiamGrow(NULL), MainResp(NULL) {

	}

	~formindGrow() {
		if (DoutofBFunc)
			delete DoutofBFunc;
		if (BoutofDFunc)
			delete BoutofDFunc;
		if (DoutofHFunc)
			delete DoutofHFunc;
		if (HoutofDFunc)
			delete HoutofDFunc;
		if (DiamGrow)
			delete DiamGrow;
		if (MainResp)
			delete MainResp;
	}
	void InitDB();
	void InitDH();
	void InitMaintenanceRespiration();
	void InitGrow();
	void DoGrowth(void);
	bool TestNegativeGrowth();
	double CalculateBiomass(TREE * tree);
	double CalculateMaintResp(TreePointer tree, PlotPointer plot);
	void CalculatePlotBiomass(void);
	void InitMaximumDiameter(void);
	void InitLookupVectors(void);
	void InitDoutofB_approximation(void);
	void InitMresp2B(void);
	void CalculateTreeGeometry_new(TreePointer, PlotPointer);
	double FoutofD(double, double, int);
	double CDoutofD(double, int);
	double CalculatePhotoProduction_new(TREE * tree, double irwet, double irdry,
		 double daylw, double dayld, double vpw, double vpd, double temp_red);
	void CalculateGrowthCurves(void);
	double CLPoutofH(double, int);
	double SVoutofD(double, double, double);
	double SVLoggingoutofD(double, double, double);
	double CommercialBoleVolumeOutofD(double, double, double);
	double LAIoutofLandCD(double, double, int);
	double GetBInc(TreePointer, PlotPointer);
	void InitA4(void);
	double DoutofBTest(double b, int grp);
	void InitB2D(void);
	void DetermineOBA(PlotPointer plot);

	cDoutofB*DoutofBFunc;
	cBoutofD*BoutofDFunc;
	cDoutofH*DoutofHFunc;
	cHoutofD*HoutofDFunc;
	DGrow*DiamGrow;
	MLoss*MainResp;

private:
	double C02persecond(double pm, double k, double l, double alpha, double m,
		 double ir);
	double PBeffectOldProjects(TreePointer tree);
};

extern formindGrow forGrow;

#endif //__FOR_GROW_HH
