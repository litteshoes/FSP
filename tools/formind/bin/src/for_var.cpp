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
// File						for_var.cpp
// Description				initialisation of all variables
//
///////////////////////////////////////////////////////////////////

#include "for_var.h"
#include "for_grow.h"
#ifdef underconstruction
#include "grass/grass_grow.h"
#endif
#include "random.h"
#include <cstring>

#include "MMParSet/MMErrorMessage.h"
#include "for_misc.h"

using namespace std;
int FFruns = 0;
double Time;
double TimeEnd;
double SpinupTime;
double TimeStep;
double OutputStep;
double OutputStart;
double TimeLidarOut;
double TimeTreeListOut;
double NextTimeOut;
int RandomInit;
int save_yearly_growth;
std::string PinFileNameX;
std::string resultDirectory;
std::string InvFileName;
std::string InitPoolsFileName;
std::string modelshort;
std::string individuals;
std::string individual;
int MAXDD;

char SimDir[_MAX_PATH * 2];
double TestTEnd;
char PinFileName[_MAX_PATH * 2];
FILE*PinFile;
int FileN;
RESULT Result;
LOC Loc;
LOG Logging;
LOGADD Logadd;
N_PAR N_Par;
TIME T;
SWITCH Switch;
OUT_FORMIND Out;
double HMAX;
double DMAX;
int HIGHESTLAYERNUMBER;
double AREASUM;
double AREASUM_MEADOW;
int HELPSTEP, MAXARRAY;
int SAEMLINGSKRONE;
int MaxSeed, MinSeed;
double MAXLAI;
int MAXGRP;
HecPointer FirstHec;
bool PULS;
int HGRP_BIG;
double GROWTH_COEFF[MAXSPECIES][4];
resultFileSwitch myResultFileSwitch;

vector<string>DCLASS;
double DM[MAXSPECIES];
double BTM[MAXSPECIES];
double A4[MAXSPECIES];
double B2D[MAXB2D][MAXSPECIES];
int BINC[2];
double D2OTLAI[MAXD2OTLAI];
vector<HecPointer>vHec;
double CumSpeciesNumber[HYPERMAXGRP];
bool sown;
vector<vector<double> >vDiameterLookup;
vector<vector<double> >vBiomassLookup;

double*diadd, **diagrp;

/* !
 \brief			counts the number of digits befor comma
 \param			double coord
 \return			number of digits
 \details		returns an integer with has counted all digits in front of the comma
 */
int number_of_digits(double coord) {

	int return_val = 0;
	unsigned int i_coord = (unsigned int)coord;
	do {
		++return_val;
		i_coord /= 10;
	}
	while (i_coord);
	return return_val;
}
// ------------------------------------------------------------------------

double TREE::absX(int _tindex) {
	return TPosition[0][_tindex];
}

double TREE::absY(int _tindex) {
	return TPosition[1][_tindex];
}

double TREE::relX(int _tindex) {
	return TPositionRelativ[0][_tindex];
}

double TREE::relY(int _tindex) {
	return TPositionRelativ[1][_tindex];
}

double TREE::absX() {
	return absX(0);
}

double TREE::absY() {
	return absY(0);
}

void TREE::relX(int _tindex, double _relX) {
	TPositionRelativ[0][_tindex] = _relX;
}

void TREE::relY(int _tindex, double _relY) {
	TPositionRelativ[1][_tindex] = _relY;
}

void TREE::absX(int _tindex, double _absX) {
	TPosition[0][_tindex] = _absX;
}

void TREE::absY(int _tindex, double _absY) {
	TPosition[1][_tindex] = _absY;
}

void TREE::absX(double _absX) {
	absX(0, _absX);
}

void TREE::absY(double _absY) {
	absY(0, _absY);
}

void TREE::absXY(int _tindex, double _absX, double _absY) {
	TPosition[0][_tindex] = _absX;
	TPosition[1][_tindex] = _absY;
}

void TREE::relXY(int _tindex, double _relX, double _relY) {
	TPosition[0][_tindex] = _relX;
	TPosition[1][_tindex] = _relY;
}

void TREE::absXY(double _absX, double _absY) {
	absXY(0, _absX, _absY);
}

// ------------------------------------------------------------------------

TREE::TREE() {

}

TREE::TREE(int pft, float diameter, double seedlings,
	 PlotPointer plot, int com, int Liana)

	 /*!
	  \brief          Tree constructor. Initializes all tree - variables
	  \author
	  \date
	  \param	      int pft
	  \param	      int diameter
	  \param	      int seedlings
	  \param	      PlotPointer plot
	  \param	      int com
	  \param			int isLiana
	  \result         TREE
	  \details        tree = new TREE(grp,dd,seedling,plot,com,isLiana);
	  Generates existing tree with PFT=grp, Diameter=dd, tree->N=seedling with position in plot (important for Pin-Files)
	  tree = new TREE(grp,dd,seedlings,plot,com,isLiana);
	  Generates new tree with PFT=grp, minimal diameter, tree->N=seedlings with position in plot (important for new trees)
	  */

{
	double cd, rho;
	next = NULL;
	Grp = pft;
	N = seedlings;

	isLiana = Liana;
	attachedTree = NULL;
	attachedLiana = NULL;

	if ((N_Par.spatial) && (N > 1))
		cout << "WARNING: N>1 despite that SPATIAL version is activated." << endl;

	struct coordinate {
		unsigned long x;
		unsigned long y;
	};

	union coordWithId {
		coordinate v;
		unsigned long long id;
	};

	coordWithId cWithId;

	DHprefactor = 1.0;
#ifdef underconstruction
	DHprefactor = CalculatePreFactor();
#endif

#ifdef underconstruction
	if (N_Par.TraitDist_ON) {
		CalculateTrait(pft);
	}

	if (N_Par.GRASSMIND) { //for grass diameter is interpreted as height
		
		H = diameter;
		D = forGrow.DoutofHFunc->Calculate(H, pft);
		/*else { // needs to be modified
			D = diameter;
			if (N_Par.InitialMowed)
				H = N_Par.InitialMowHeight;
			else
				H = forGrow.HoutofDFunc->Calculate(D, pft);
		*/
		LightExtCoeff = N_Par.Div_K[pft];
		WaterUseCoeff = N_Par.Water_WUE[pft];
		NitrogenUseCoeff = 0;
	}
	else
#endif
	{
		D = diameter;
		LightExtCoeff = N_Par.Div_K[pft];
		H = DHprefactor * forGrow.HoutofDFunc->Calculate(D, pft);
	}

	rho = N_Par.Pro_Rho_3[pft];
	cd = forGrow.CDoutofD(D, pft);
	CLP = forGrow.CLPoutofH(H, pft);
	AC = PI * SQR(cd) / 4.0;
	F = forGrow.FoutofD(D, rho, pft);
	BT = forGrow.BoutofDFunc->Calculate(D, H, F, pft);

#ifdef underconstruction
	if (N_Par.GRASSMIND) {

			BTgreen = BT;
			BTbrown = 0.0;
			BTFracGreen = 1.0;
			BTFracBrown = 0.0;
			BT_calib = 0.0;
			BT_calib_weight = 0.0;
		/*
			BTFracGreen = N_Par.InitialGreenFraction;
			BTFracBrown = 1.0 - N_Par.InitialGreenFraction;
			BTgreen = BT * BTFracGreen;
			BTbrown = BT * BTFracBrown;
		}*/
		Lgreen = LoutofB(BTgreen, AC, pft);
		Lbrown = 0.0;
		L = Lgreen + Lbrown;
		RBT = pow(BT / N_Par.Geo_AGB_2[0][pft], 1.0 / N_Par.Geo_AGB_2[1][pft]);
		RH = N_Par.Geo_RD_2[0][pft] * pow((N_Par.Geo_AGB_2[0][pft] / F), N_Par.Geo_RD_2[1][pft]) * pow((double)RBT, (double)N_Par.Geo_RD_2[1][pft]);
		RootLength = RBT * N_Par.Geo_RL[pft];
		if (N_Par.InitialMowed)
			Mowed = true;
		else
			Mowed = false;
		Regrow = false;
		Rep = 0;
		Cost_Rhizobia = 0.0;
		RelocatedN = 0;
		HInc = 0.0;
		BrInc = 0.0;
		allocsh = N_Par.Geo_alloc_4[0][pft];
		allocro = N_Par.Geo_alloc_4[0][pft] / N_Par.Geo_AGB_2[0][pft];
		allocrep = (1 - allocsh - allocro);

		NTotalUptake = 0;
		NDemand = 0;
		NDemandGreen = 0;
		NDemandBrown = 0;
		Nshoot = (1000 * BT * ODM_TO_C) / N_Par.Div_CN_Green[pft];
		Nroot = (1000 * RBT * ODM_TO_C) / N_Par.Div_CN_Brown[pft];
		Nrep = 0;

		/*if (diameter>=0) {
			AGE = -0.0795+(5.3*std::atof(DCLASS[(int)diameter].c_str()));
			Status = 0;
			if (AGE >= N_Par.Est_DSTree_5[pft])
				Status = 1;
		}
		else {*/
			AGE = 0;
			Status = 0;
		

		RS = 1;
		RW = 1;
		RN = 1;
	}
	else
#endif
	{
		SV = forGrow.SVoutofD(D, H, F);
		SVLogging = forGrow.SVLoggingoutofD(SV, CLP, F);
		BoleVolume = forGrow.CommercialBoleVolumeOutofD(SV, CLP, F);
		L = forGrow.LAIoutofLandCD(D, AC, pft);
		RBT = 0;
		RCON = 0;
		RH = N_Par.Geo_RH;
		RAR = 0;
		AGE = 0.0;
	}

	PB = 0.0;
	PB_buffer = 0.0;
	ndinc = 0.0;
	PBrw = 0.0;
	PBrwday = 0.0;
	R = 0.0;
	RespGrowth = 0.0;
	RespMain = 0.0;
	DInc = 0.0;
	MS = 0.0;
	OBA = 0.0;
	LAITREE = 0.0;
	minLRF = 1.0;
	IR = 0.0;
	IRwet = 0.0;
	IRdry = 0.0;
	nFall = 0;
	COMGrp = com; // commercial group (0. non, 1. commercial)
	PCT = 0;

	BInc = 0.0;

	if (N_Par.Fire)
		Burnt = false;

	int myCount;
	myCount = (int)(floor(N) + 1);
	TPosition[0].resize(myCount);
	TPosition[1].resize(myCount);
	TPositionRelativ[0].resize(myCount);
	TPositionRelativ[1].resize(myCount);
	IdVector.resize(myCount);

	TPositionRelativ[0][0] = _Random() * (plot->LocXH - plot->LocXL);
	TPositionRelativ[1][0] = _Random() * (plot->LocYH - plot->LocYL);
	TPosition[0][0] = TPositionRelativ[0][0] + plot->LocXL;
	TPosition[1][0] = TPositionRelativ[1][0] + plot->LocYL;

	cWithId.v.x = (unsigned long)(TPosition[0][0] * pow(10.0, (double)(10 - (number_of_digits(TPosition[0][0])))));
	cWithId.v.y = (unsigned long)(TPosition[1][0] * pow(10.0, (double)(10 - (number_of_digits(TPosition[1][0])))));
	IdVector[0] = cWithId.id;

	for (int i = 1; i < myCount; i++) {
		TPositionRelativ[0][i] = myRand.drnd() * (plot->LocXH - plot->LocXL);
		TPositionRelativ[1][i] = myRand.drnd() * (plot->LocYH - plot->LocYL);
		TPosition[0][i] = TPositionRelativ[0][i] + plot->LocXL;
		TPosition[1][i] = TPositionRelativ[1][i] + plot->LocYL;
		cWithId.v.x = (unsigned long)(TPosition[0][i] * pow(10.0, (double)(10 - (number_of_digits(TPosition[0][i])))));
		cWithId.v.y = (unsigned long)(TPosition[1][i] * pow(10.0, (double)(10 - (number_of_digits(TPosition[1][i])))));
		IdVector[i] = cWithId.id;
	}

	if (N_Par.spatial) {
		cellXL = floor(absX() * (1 / CELLSIZE));
		cellYL = floor(absY() * (1 / CELLSIZE));

		cellXH = ceil(absX() * (1 / CELLSIZE));
		cellYH = ceil(absY() * (1 / CELLSIZE));

		cellX = int(cellXL);
		cellY = int(cellYL);
	}

	lastVecPosition = 0;

	species = 0;
	if (N_Par.Flag_SpeciesNumber) {
		species = (int)(myRand.drnd() * N_Par.SpeciesNumber[pft] + CumSpeciesNumber[pft]) + 1;
	}

	if (myResultFileSwitch.branch) {
#ifdef underconstruction
		branches = Branching(this);
#endif
	}
}

// ------------------------------------------------------------------------
/* !
 \brief			deconstructor of tree
 */
TREE::~TREE() {

}

// ------------------------------------------------------------------------
/* !
 \brief			constructor of plot
 \details		initialyse arrays with 0. So that they do not contain -NAN which throws an error when copied. float LAD[MAXLYR + 1]
 */
PLOT::PLOT() {
	memset(LAD, 0, (MAXLYR + 1)*sizeof(float));
}

resultFileSwitch::resultFileSwitch() {
}

/* !
 \brief			Formind tools
 \details     FractionalDice, FindPlot, FindHectar, DetermineFallLoc, DetermineFallLoc_noclosedboundary,
 FindNeighbours, FindEightNeighbours, isnotseeded
 */
namespace ForTools {

	bool FractionalDice(double val) {
		return (_Random() < (val - (int)val));
	}

	/* !
	 \brief			search plot in hectar with given coordinates
	 \param			hh - hecpointer
	 \param			xf - xCoordinates
	 \param			yf - yCoordinates
	 \return			plot or NULL
	*/
	PlotPointer FindPlot(HecPointer hh, double xf, double yf) {
		PlotPointer hp;
		hp = hh->FirstPlot;
		while (hp != NULL) {
			if ((hp->LocXL <= xf) && (hp->LocXH > xf) && (hp->LocYL <= yf) && (hp->LocYH > yf))
				return hp;
			hp = hp->next;
		}
		MMErrorMessage(
			"Cannot find plot",
			N_Par.ErrorType);
		return NULL;
	}

	/* !
	 \brief			search hectar with given coordinates
	 \param			xf - xCoordinates
	 \param			yf - yCoordinates
	 \return			hectar or throw error if no hectare is found.
	*/
	HecPointer FindHectar(double xf, double yf) {
		HecPointer hh;
		hh = FirstHec;
		while (hh != NULL) {
			if ((hh->LocXL <= xf) && (hh->LocXH > xf) && (hh->LocYL <= yf) && (hh->LocYH > yf))
				return hh;
			hh = hh->next;
		}
		MMErrorMessage(
				"Cannot find hectar",
				N_Par.ErrorType);
		return NULL;
	}

	/* !
	 \brief			calculates coordinates of a fallen tree
	 \param			dir - angle
	 \param			x - x coordinate of living tree
	 \param			y - y coordinate of living tree
	 \param			H - height of tree
	 \return			xf -  x coordinate of fallen treeCrown
	 \return			yf -  y coordinate of fallen treeCrown
	*/
	void DetermineFallLoc(int Dir, double x, double y, double H, double*xf, double*yf) {
		*xf = x + H * sin(Dir / 360.0 * 2 * PI);
		*yf = y + H * cos(Dir / 360.0 * 2 * PI);
		while (*xf < Loc.XMin)
			* xf += Loc.XDiff;
		while (*xf >= Loc.XMax)
			* xf -= Loc.XDiff;
		while (*yf < Loc.YMin)
			* yf += Loc.YDiff;
		while (*yf >= Loc.YMax)
			* yf -= Loc.YDiff;

	}

	/* !
	 \brief			calculates coordinates of a fallen tree without closed boundaries
	 \param			dir - angle
	 \param			x - x coordinate of living tree
	 \param			y - y coordinate of living tree
	 \param			H - height of tree
	 \return			xf -  x coordinate of fallen treeCrown
	 \return			yf -  y coordinate of fallen treeCrown
	*/
	void DetermineFallLoc_noclosedboundary(int Dir, double x, double y, double H, double*xf, double*yf) {
		*xf = x + H * sin(Dir / 360.0 * 2 * PI);
		*yf = y + H * cos(Dir / 360.0 * 2 * PI);

	}

	/* !
	 \brief			search neighbourPlots (4) of a plot
	 \param			plot
	 \return			plot->Pdir - array of neighbourPlots
	*/
	bool FindNeighbours(PlotPointer plot) {
		double xf, yf, xx, yy, dplot;
		int direction = 0;
		int i;

		xx = plot->LocXL + (plot->LocXH - plot->LocXL) / 2.0;
		yy = plot->LocYL + (plot->LocYH - plot->LocYL) / 2.0;
		dplot = plot->LocYH - plot->LocYL;

		for (i = 0; i < 4; i++) {
			if (N_Par.Closed_boundary) {
				ForTools::DetermineFallLoc(direction, xx, yy, dplot, &xf, &yf);
				plot->Hdir[i] = ForTools::FindHectar(xf, yf);
				if (plot->Hdir[i] == NULL)
					goto E_EXIT;
				plot->Pdir[i] = ForTools::FindPlot(plot->Hdir[i], xf, yf);
				if (plot->Pdir[i] == NULL)
					goto E_EXIT;
			}
			else {
				ForTools::DetermineFallLoc_noclosedboundary(direction, xx, yy, dplot, &xf, &yf);
				if ((xf >= Loc.XMin) && (xf < Loc.XMax) && (yf >= Loc.YMin) && (yf < Loc.YMax)) {
					plot->Hdir[i] = ForTools::FindHectar(xf, yf);
					if (plot->Hdir[i] == NULL)
						goto E_EXIT;
					plot->Pdir[i] = ForTools::FindPlot(plot->Hdir[i], xf, yf);
					if (plot->Pdir[i] == NULL)
						goto E_EXIT;
				}
				else {
					plot->Hdir[i] = NULL;
					plot->Pdir[i] = NULL;
				}
			}

			direction += 90;
		}
		return true;
	E_EXIT:
		cerr << "ERROR: undefined error \tfile: " << __FILE__ << "\tfunction: FindNeighbours\t line:" << __LINE__ << "\t Neighbours not found." << endl;
		return false;
	}

	/* !
	 \brief			search neighbourPlots (8) of a plot
	 \param			plot
	 \return			plot->Pdir - array of neighbourPlots
	*/
	bool FindEightNeighbours(PlotPointer plot) {
		double xf, yf, xx, yy, dplot;
		int direction = 0;
		int i;

		xx = plot->LocXL + (plot->LocXH - plot->LocXL) / 2.0;
		yy = plot->LocYL + (plot->LocYH - plot->LocYL) / 2.0;
		dplot = plot->LocYH - plot->LocYL;

		for (i = 0; i < 8; i++) {

			if (N_Par.Closed_boundary) {
				ForTools::DetermineFallLoc(direction, xx, yy, dplot, &xf, &yf);
				plot->H8dir[i] = ForTools::FindHectar(xf, yf);
				if (plot->H8dir[i] == NULL)
					goto E_EXIT;
				plot->P8dir[i] = ForTools::FindPlot(plot->H8dir[i], xf, yf);
				if (plot->P8dir[i] == NULL)
					goto E_EXIT;
			}
			else {
				ForTools::DetermineFallLoc_noclosedboundary(direction, xx, yy, dplot, &xf, &yf);
				if ((xf >= Loc.XMin) && (xf < Loc.XMax) && (yf >= Loc.YMin) && (yf < Loc.YMax)) {
					plot->H8dir[i] = ForTools::FindHectar(xf, yf);
					if (plot->H8dir[i] == NULL)
						goto E_EXIT;
					plot->P8dir[i] = ForTools::FindPlot(plot->H8dir[i], xf, yf);
					if (plot->P8dir[i] == NULL)
						goto E_EXIT;
				}
				else {
					plot->H8dir[i] = NULL;
					plot->P8dir[i] = NULL;
				}
			}

			direction += 45;
		}
		return true;
	E_EXIT:
		cerr << "ERROR: undefined error \tfile: " << __FILE__ << "\tfunction: FindEightNeighbours\t line:" << __LINE__ << "\t Neighbours not found." << endl;
		return false;

	}

	/* !
	 \brief			check if plot is seeded
	 \param			val
	 \param			arr
	 \param			size
	*/
	bool isnotseeded(int val, std::vector<int>&arr, int size) {
		int k;
		if (arr.size() > 0) {
			for (k = 0; k < size; k++) {
				if (arr[k] == val)
					return false;
			}
		}
		return true;
	}

}

// -----------------------------------------------------------
// ----------------- end of for_var.cc -----------------------
// -----------------------------------------------------------

