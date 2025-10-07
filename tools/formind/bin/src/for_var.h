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
// File						for_var.h
// Description				initialisation of all variables
//
///////////////////////////////////////////////////////////////////

#ifndef  for_varH
#define  for_varH

#include "for_global.h"
#include "for_const.h"
// Disabled missing optional module include to avoid build failure when
// external branching module is not available in this distribution.
//#ifdef underconstruction
//#include "for_branching.h"
//#endif

extern int FFruns;
extern double Time;
extern double TimeEnd;
extern double SpinupTime;
extern double TimeStep;
extern double OutputStep;
extern double OutputStart;
extern double TimeLidarOut;
extern double TimeTreeListOut;
extern double NextTimeOut;
extern int RandomInit;
extern int save_yearly_growth;
extern int MAXDD;
// number of years that growth is stored backwards
extern std::string PinFileNameX;
extern std::string resultDirectory;
extern std::string InvFileName;
extern std::string InitPoolsFileName;
extern std::string modelshort;
extern std::string individuals;
extern std::string individual;
class PLOT;
typedef PLOT*PlotPointer;

// -------------- TREE RECORD -------------------

class TREE { // Individual tree variables

public:
	TREE();
	TREE(int pft, float diameter, double seedlings,
		 PlotPointer plot, int com, int Liana);
	~TREE();

	double absX(), absY();
	// absolute coordinates for each cohort [m], taken from TPosition[1,2][0];
	double relX(int), relY(int);
	// absolute coordinates for each cohort [m], taken from TPositionRelativ[1,2];
	double absX(int), absY(int);
	// absolute coordinates for each tree [m], taken from TPosition[1,2][int _tindex];

	void absX(double), absY(double);
	// set x or y coordinates for each cohort [m], written to TPosition[1,2][0];

	void absX(int, double), absY(int, double);
	// set x or y coordinates for each tree [m], written to TPosition[1,2][int _tindex];

	void relX(int, double), relY(int, double);
	// set relativ x or y coordinates within plot for a single tree [m]

	void absXY(double, double);
	// set x,y coordinates  for each cohort [m], written to TPosition[1,2][0];

	void absXY(int, double, double);
	// set x,y coordinates  for each tree [m], written to TPosition[1,2][int _tindex];

	void relXY(int, double, double);
	// set relativ x,y coordinates for single tree [m]

	double N;
	// Number of trees in cohort, all other variable are for ONE representative tree of cohort [trees]
	short Grp; // Group number = PFT number
	short COMGrp; // commercial group (0. non, 1. commercial)
	short PCT; // potential crop tree for thinning  (0. non, 1. yes)
	float BT; // biomass per tree [t(organic dry matter) = 10^6 g (odm)]
	float RBT;
	// root biomass per tree [t(organic dry matter) = 10^6 g (odm)]
	float RAR; // root area [m^2]
	float RH; // root height[m]
	float RCON; // root cohesion
	float F; // form factor of stem [-]
	float AC; // crown area  [m^2]
	float D; // diameter at breast height (dbh) [m]
	float H; // total height [m]
	float CLP; // crown length part of total height [-]
	float IR; // incoming radiation [micromol (photon) m^-2 s^-1]
	float IRwet;
	// incoming radiation in wet season [micromol (photon) m^-2 s^-1]
	float IRdry;
	// incoming radiation in dry season [micromol (photon) m^-2 s^-1]
	float LightExtCoeff; // light extinction coefficient for photosynthesis
	float LAITREE;
	// cumulative leaf area index above tree [m^2 m^-2] including light extinction coefficient
	float LAITREE_forRes;
	// cumulative leaf area index above tree [m^2 m^-2] excluding light extinction coefficient
	// float LRFTREE; // layer reduction fraction of tree [-]
	float SV; // stem volume [m^3]
	float SVLogging; // stem volume relevant for Logging [m^3]
	float BoleVolume;
	// bole volume relevant for Logging [m^3] (alternative calculation for SVLogging)
	float minLRF; // minimum layer reduction fraction [-]
	float PB; // photoproduction [t_odm / yr]
	float PB_buffer;
	// buffer of photoproduction [t_odm / yr] if binc <0
	float ndinc; // buffer diameter if binc <0
	float PBrw; // reduced photoproduction by water [t_odm / yr]
	float PBrwday; // reduced photoproduction by water [t_odm / day]
	short nFall; // number of falling trees per cohorte
	float BInc; // biomass increment [t_odm / yr]
	float DInc; // diameter increment [m / yr]
	float L; // leaf area index of single tree [m^2 m^-2]
	float MS; // normal specific mortality rate [yr^-1]
	float OBA; // overtopping basal area [m^2 / ha]
	float AGE; // age [yr]
	float R; // total respiration [t_odm / yr]
	float RespGrowth; // growth respiration [t_odm/year]
	float RespMain; // maintanance respiration [t_odm/year]

	float FALL; // falling probability [-]

	short cellX; // cell-coordinate x-axis (in cells). Starts at zero.
	short cellY; // cell-coordinate y-axis (in cells). Starts at zero.
	short cellZ; // cell-coordinate z-axis (in cells). Starts at zero.
	float cellXL; // lower corner of cell (in m from origin)
	float cellXH; // upper corner of cell (in m from origin)
	float cellYL; // lower corner of cell (in m from origin)
	float cellYH; // upper corner of cell (in m from origin)

	std::vector<unsigned long long>IdVector;

	// an numeric ID for every tree in the cohort
	bool Burnt; // 1 for those trees who are burned and died

	// ---------- parameters only relevant for liana modelling -----------------
	int isLiana;
	TREE*attachedTree;
	TREE*attachedLiana;

	// ---------- parameters only relevant for GRASSMIND ---------------------------------
	float Lgreen; // green leaf area index of a grass individual
	float Lbrown; // senescent leaf area index of a grass individual
	float BTgreen;
	// green photosynthetic active biomass per grass shoot [g (ODM)]
	float BTbrown;
	// senescent photosynthetic inactive brown biomass per grass shoot [g (ODM)]
	float BTFracGreen; // fraction of green biomass according to total biomass
	float BTFracBrown; // fraction of brown biomass according to total biomass
	float BT_calib; // biomass above the harvest cut in the field (for calibration)
	float BT_calib_weight; //weight for the calculation of BT_calib (relevant for not uniformly distributed LAI)
	float RootLength; // total root length [m] for one individual
	float HInc;
	// height increment [m] --> needed if e.g. a shoot is in a mowed state
	float binc_yr;
	double BrInc;
	float Nshoot;
	double Nroot;
	double Nrep;
	float allocsh;
	double allocro;
	double allocrep;
	float Rep; // reproductive pool of biomass of an individual
	bool Mowed;
	// Is shoot recently mowed? So that h:w ratio is not correct or shoot cannot grow further ...
	bool Regrow;
	short Status; // status whether seedling (0) or adult (1)
	float Cost_Rhizobia;
	// costs (fraction of NPP) that is invested in rhizobia to get fixed N (only for plants in symbiosis with rhizobia)
	float RelocatedN;

	float RL; // individual reduction factor for light (production)
	float RS; // individual reduction factor for shading (irradiance absorption)
	float RW; // individual water reduction factor
	float RWFinal;
	float RN; // individual reduction factor for nitrogen
	int RootLayers; // number of soil layer an individual is rooting in
	float WaterUseCoeff; // tree-specific water use coefficient
	float NitrogenUseCoeff; // tree-specific water use coefficient
	float WDemand; // potential transpiration [mm/d] for one tree
	float NDemand; // potential NO3N demand [kg/m�*t.d] for one individual
	double NDemandGreen;
	double NDemandBrown;

	std::vector<float>perc_root;
	// percentage of accessible resources per soil layer for an individual
	std::vector<float>NUptake;
	// real nitrogen uptake of an individual (of a cohorte) per soil layer
	std::vector<float>WUptake;

	// real water uptake of an individual (of a cohorte) per soil layer
	float NTotalUptake; // real nitrogen uptake of an individual (of a cohorte)
	float WTotalUptake; // real water uptake of an individual (of a cohorte)
	// -----------------------------------------------------------------------------------
	int species; // Species Number

	int lastVecPosition;
	// In case of lookup vector inversion from AGB to D, this variable is used to remember the vector position of the tree's diameter in the last time step
	double DHprefactor; // Pre-factor for heterogeneity in the D-H-curve

#ifdef underconstruction
	// generate subclass for branching module
	Branching branches;
#endif
	// Trait distribution: first distributed parameter
	float TraitDist_Geo_AGB_2;
	TREE*next; // Pointer on next tree element

private:
	std::vector<double>TPosition[2];

	std::vector<double>TPositionRelativ[2];
	// position of each tree, index 0 is cohort position

}; // end class Tree

// ---------------------------------------------------------------------------

typedef TREE*TreePointer;

// -------------- TREE RECORD -------------------


class DEADTREE { // Individual tree variables

public:
	// int     plot;
	short N;
	// Number of trees in cohort, all other variable are for ONE representative tree of cohort [trees]
	short Grp; // Group number = PFT number
	short sortofdeath;
	// 1. normal, 2. falling, 3. smahed by falling, 4. spacelimitation/self thinning
	float BT;
	// biomass per tree [t(organic dry matter) = 10^6 g (odm)] (cd aboveground)
	float RBT; // root biomass per tree  [t(organic dry matter) = 10^6 g (odm)]
	float D; // diameter at breat height (dbh) [m]
	float AGE; // age [yr]
	float FALL; // falling probability [-]
	float X, Y; // x, y position [m]

	DEADTREE*next; // Pointer on next tree element
};

typedef DEADTREE*DeadTreePointer;


class HECTAR;
// Forward deklaration necessary to use Hdir in PlotRecordT

// -------------- PLOT  RECORD -------------------

class PLOT { // Variables within one plot (normally 20 x 20 m)

public:

	PLOT*Pdir[4]; // PlotPointer on neighboring 4 plots
	HECTAR*Hdir[4]; // HecPointer on neighboring 4 plots
	PLOT*P8dir[8]; // cd PlotPointer on neighboring 8 plots
	HECTAR*H8dir[8]; // cd HecPointer on neighboring 8 plots

	TreePointer FirstTree; // root of tree list in plot
	// TreePointer FirstDeadTree; // root of dead tree list in plot cd neu 16.06.09
	int No; // running number on plots
	float Plotmax; // height of tallest tree [m]

	double Biomass; // biomass of all trees of the plot
	double Biomass_th; // biomass of all trees with DBH>treshold of the plot
	double BiomassGrp[HYPERMAXGRP];
	double BiomassGreenGrp[HYPERMAXGRP];
	double BiomassBrownGrp[HYPERMAXGRP];
	// biomass of all trees per species of the plot
	double TH_BiomassGrp[HYPERMAXGRP];
	// biomass with DBH>treshold of all trees per species of the plot
	double RootBiomassGrp[HYPERMAXGRP];
	// biomass of all trees per species of the plot
	double BasalArea; // basal area of all trees of the plot
	double BasalAreaGrp[HYPERMAXGRP];
	// basal area of all trees per species of the plot
	double TH_BasalAreaGrp[HYPERMAXGRP];
	// basal area of all trees per species of the plot
	double StemNumber; // stem number of all trees of the plot
	double StemNumberGrp[HYPERMAXGRP];
	// stem number of all trees per species of the plot
	double TH_StemNumberGrp[HYPERMAXGRP];
	// stem number of all trees per species of the plot
	double Ncomtrees; // Number of commercial trees which are potential PCT
	double NPCT; // Number of PCT trees in the plot
	double MTree[HYPERMAXGRP]; // Number of Mother trees in plot
	double StemVolume; // stem volume of all trees in the plot
	double BasalArea_th;
	// basal area of all trees with DBH>treshold of the plot
	double StemNumber_th;
	// stem number of all trees with DBH>treshold of the plot
	double StemVolume_th;
	// stem volume of all trees with DBH>treshold in the plot
	double QuadMeanDiameter;
	// quadratic mean diameter = sqrt((D^2+D^2+...+D^2)/n), D in cm
	double SDI; // Stand Density Index
	double HorizontalIndex;
	double VerticalIndex;
	double MeanWoodDen; // mean wood density weighted by basal area
	double TH_MeanWoodDen; // mean wood density weighted by basal area
	double BiomassNewTrees;
	// biomass of trees that established in this year, needed for closing of biomass dynamics
	double DeadBiomass;
	// dead aboveground biomass [t], for summing up of dead biomass over the years + decay...
	double NewDeadBiomass; // biomass of trees that died in the actual year

	double *DiaSum; // diameter distribution
	double **DiaGrp; // diameter distribution per pft

	// --- PIN File ----
	std::vector<std::vector<float> >N0;

	// Initial stem-diameter distribution per PFT
	int landcode;

	std::string climate;
	std::string soil;
	std::string manage;

	double LocXL, LocYL, LocXH, LocYH;
	// position of plot corners [m] (lower left / upper right)
	float Area; // area [m^2]
	float MEL; // mean elevation [m] -- not used

	std::string Name;

	// ---- LANDSLIDE -----
	double Slope; // Slope of the plot, for landslide module
	double Elev; // Elevation of the plot
	int ForestType; // ForestType of this plot
	double SlideProb; // Slide probability of a plot within one year
	int SlideSize;
	// number of plots involved into the slide, just counted for the first triggering slideplot
	bool Slide; // 0 if plot did not slide in one year, 1 if it slides
	bool TriggerSlide;
	// 1 for those plots that trigger a slide, where the slide starts,
	double DeadRootbiomass; // dead root biomass [t]
	double DeadRootCohesion; // cohesion of dead roots [kPa]
	double RootCohesion; // cohesion of alive roots [kPa]
	int TimeSinceLandslide;
	// Time passed since last landslide disturbance
	double AccumulatedDeadMass;
	// since last landslide accumulated dead biomass on plot, for feedbacks in forest regeneration

	// ---- Logging ------
	int LogCut; // Plot logged yes/no
	float LogDam; // Tree with size of LogDam damaged plot [m^2]
	int LogPotential; // poteniel number of tress to log [trees]
	int PCTLogPotential; // poteniel number of PCT to log [trees]
	float SVLogPotential; // poteniel stemvolum  of tress to log [m^3]
	int PlotLogDam; // counting number of damaging trees
	float RoadDamage;
	int Skidtrail;
	// time counter for area totally destroyed for a road building [yr]
	int CountSkidtrailN;  // number of dead trees due to skidtrails [1/ha]
	float CountSkidtrailBT;  // biomass of dead trees due to skidtrails [t/ha]
	float CountSkidtrailBV;  // bole volume of dead trees due to skidtrails [m3/ha]
	int MEADOW; // switch for status (0: forest; 1:empty; 2: empty and growing)
	int BORDER;
	// distance to forest edge 0: no border, 1-5: distance in 20m steps

	// ---- FIRE -----
	bool Fire; // RF10:true if fire occurs in this plot
	PLOT*next; // Pointer on next plot element

	// ---- Recruitment ----
	int SeedPool[HYPERMAXGRP][N_COMMERCIAL];
	// Seed numbers per PFT and commercial (commercial 0: noncom; 1: com) [seeds]
	int NewSeeds[HYPERMAXGRP][N_COMMERCIAL];
	// New seeds per PFT and commercial (commercial 0: noncom; 1: com) [seeds yr^-1]
	// int N[HYPERMAXGRP]; // Stemnumber per PFT and plot for GDI [-]
	int seedlings[HYPERMAXGRP];

	std::vector<int>NewSeedsMotherTreePFT;
	std::vector<int>NewSeedsMotherTreeCOM;
	std::vector<int>NewSeedsMotherTreeSPECIES;
	std::vector<double>NewSeedsPosition[2];

	int NewSeedsWithSpeciesNumber;
	int Seeds;

	// ---- Mortality ----
	float MB, MBC, MBF, MBD;
	// normal, crowding, falling tree, damage mortality (total biomass)
	float MN, MNC;
	// normal, crowding(number of trees)
	float MFIRE; // mortality due fire = burnt trees
	float MLAND; // mortality due to landslides
	float MLOG; // trees removed by logging; not really dying of trees
	float MLOGDAM; // trees dying due to damages due to logging of trees
	float MBLOGDAM, MBFIRE, MBLAND, MBLOG;

	// --- light climate ---
	float ATSum[MAXLYR + 1]; // total leaf area in each height layer [m^2]
	float ATold[MAXLYR + 1]; // old total leaf area in each height layer [m^2]
	float LAI[MAXLYR + 1];
	// cumulative leaf area index in each height layer [m^2 m^-2] (in layer 0 is the leaf area index for the whole plot!)
	float LAIK[MAXLYR + 1];
	// as LAI but including the PFT-specific light extinction coefficient
	float LayerReductionFac[MAXLYR + 1];
	// Layer reduction factor in each height layer due to crowding [-]
	float LAD[MAXLYR + 1];
	// Sum of crown projection area of all potential crop trees in the plot

	// --- light climate without lianas---
	float ATSum_WithoutLiana[MAXLYR + 1]; // total leaf area in each height layer [m^2]
	float ATold_WithoutLiana[MAXLYR + 1]; // old total leaf area in each height layer [m^2]
	float LAI_WithoutLiana[MAXLYR + 1];
	// cumulative leaf area index in each height layer [m^2 m^-2] (in layer 0 is the leaf area index for the whole plot!)
	float LAIK_WithoutLiana[MAXLYR + 1];
	// as LAI but including the PFT-specific light extinction coefficient
	float LayerReductionFac_WithoutLiana[MAXLYR + 1];
	// Layer reduction factor in each height layer due to crowding [-]
	float LAD_WithoutLiana[MAXLYR + 1];
	// Sum of crown projection area of all potential crop trees in the plot

	float PCTCA;
	// leaf area density [m^2 m^-3] using the same height discretization as for LAI
	float IRFloorwet, IRFloordry, IRFloor;
	// irradiance at forest floor (wet,dry,average) [micromol (photon) m^-2 s^-1]

	// ---- Growth ------
	float PB; // Photoproduction of plot [t_odm/year] = GPP
	float PB2;
	float R_total; // respiration rate of the plot, (Sato, 2007)
	float R_total_biomass; // Respiration of all biomass [t_odm/T.D]

	// --- SOIL WATER ----
	float PR; // precipitation  [mm]
	float SI; // interception [mm]
	float RO; // runoff [mm]
	float RO_A; // runoff above-ground [mm]
	float RO_B; // runoff below_ground [mm]

	std::vector<float>TR; // real transpiration [mm] per soil layer
	std::vector<float>SW_now; // soil water [mm] in this timestep per soil layer

	float SW_mean; // soil water [V%] mean in one year
	float RW; // reduction factor due water stress []
	float PET; // PET [mm]
	float MSW; // MSW [mm]

	std::vector<float>PWP;
	std::vector<float>FC;

	// ---- Climate ----
	float photosynthese_reduction[HYPERMAXGRP];

	// reduction factor for every single PFG [ ]
	std::vector<double>temperature;
	std::vector<double>temp_photosynthese_reduction;

	double temp_resp_reduction;

	std::vector<double>precipitation;
	std::vector<double>irradiance;
	std::vector<double>daylength;
	std::vector<double>pet;
	std::vector<double>CO2_concentration;

	float mean_daylength; // [] reduction of respiration because of temperature

	std::vector<float>mean_light_above_canopy;

	// [mikromol(photons)/(m^2*s)] mean radation of current year during vegetation periode
	float total_light_above_canopy;
	// [mikromol (photons)/(m^2*s)] mean radiation above canopy but independent of vegetation period of species (for entire T.D)
	int length_of_vegetation_periode; // [days] length of vegeation periode
	float water_reduction;
	// [] reduction of photosynthesis because of waterstress
	float CO2_inc_GPP; // increase of GPP due to CO2 (simple version)
	float WUE_inc;
	// effects for transpiration and GPP due to CO2 (complex version)
	float WUE_GPP_inc;

	// ----- carbon -----
	float CPool_Soil_fast; // Soil carbon pool, fast decomposing (Sato, 2007)
	float CPool_Soil_slow;
	// Soil carbon pool, slow decomposing (Sato, 2007), also used for CENTURY
	float CPool_DeadWood; // Coarse woody debris biomass, (Sato, 2007)
	float Cflux_to_Soil_fast;
	// carbon flux to soil pool, fast decomposing (Sato, 2007)
	float Cflux_to_Soil_slow;
	// carbon flux to soil pool, slow decomposing (Sato, 2007)
	float Cflux_to_DeadWood; // carbon flux to dead wood Pool
	float C_flux; // total carbon flux of the plot = NEE (Sato, 2007))
	float Resp_Soil_fast;
	// Respiration of the Soil, fast decomposing (Sato, 2007)
	float Resp_Soil_slow;
	// Respiration of the Soil, slow decomposing (Sato, 2007)
	float Resp_DeadWood; // Respiration of the Dead Wood for (Sato, 2007)
	float aet; // evapotranspiration [mm/yr]
	float decomp_rate;

	// ----- CENTURY -----
	std::vector<float>ManageDate;
	std::vector<float>FertilizerAmount, MowingHeight, IrrigationAmount;

	float R_total_biomass_month, PB_month, ingrowth_month, NUptake_month,
		 RepNuptake_month;

	float WaterDemand;
	float WaterUptake;
	float EVAP; // evaporation [mm]
	float IRRIG;

	float Silt;
	float Clay;
	float Sand;

	std::vector<float>RWC;
	std::vector<float>H2OFlux;

	float SNOW;
	float SNOWLIQ;
	float BARE;

	float defac;
	float AirT;
	float SoilT;
	float CPool_Surface_litter_struc;
	float CPool_Surface_litter_met;
	float CPool_Soil_litter_struc;
	float CPool_Soil_litter_met;
	float CPool_Soil_microbes;
	float CPool_Soil_active;
	float CPool_Soil_passive;
	float CPool_soil_total;

	float Cflux_Surface_to_Soil_microbes;
	float Cflux_Surface_to_Soil_microbes_met;
	float Cflux_Surface_to_Soil_slow;
	float Cflux_Soil_to_Soil_slow;
	float Cflux_Soil_to_Soil_active;
	float Cflux_Soil_to_Soil_active_met;
	float Cflux_Microbe_to_Slow;
	float Cflux_Active_to;
	float Cflux_Active_to_Passive;
	float Cflux_Active_to_Slow;

	float Cflux_Slow_to;
	float Cflux_Slow_to_Active;
	float Cflux_Slow_to_Passive;
	float Cflux_Passive_to_Active;
	float CLeach;
	float LeachingC;

	float Resp_DecompC_surface_slow;
	float Resp_DecompC_surface_microbes;
	float Resp_DecompC_soil_slow;
	float Resp_DecompC_soil_active;
	float Resp_DecompC_surface_met;
	float Resp_DecompC_soil_met;
	float RespC_Surface_litter;
	float RespC_Soil_litter;
	float RespC_litter;
	float Resp_DecompC_microbes;
	float Resp_DecompC_active;

	float Resp_DecompC_slow;
	float Resp_DecompC_passive;

	float RespC_soilpools;

	float NPool_Surface_litter_struc;
	float NPool_Surface_litter_met;
	float NPool_Soil_litter_struc;
	float NPool_Soil_litter_met;
	float NPool_Soil_microbes;
	float NPool_Soil_active;
	float NPool_Soil_slow;
	float NPool_Soil_passive;
	float NPool_soil_total;

	float Nflux_Surface_to_Soil_slow;
	float Nflux_Surface_to_Soil_microbes;
	float Nflux_Soil_to_Soil_slow;
	float Nflux_Soil_to_Soil_active;
	float Nflux_Surface_to_Soil_microbes_met;
	float Nflux_Soil_to_Soil_active_met;
	float Nflux_Microbe_to_Slow;
	float Nflux_Active_to_Slow;
	float Nflux_Active_to_Passive;

	float Nflux_Slow_to_Active;
	float Nflux_Slow_to_Passive;
	float Nflux_Passive_to_Active;
	float NVolatilization;

	float NLeach;
	float LeachingN;

	float Nflow_Surface_slow;
	float Nflow_microbes;
	float Nflow_Soil_slow;
	float Nflow_active;
	float Nflow_microbes_met;
	float Nflow_active_met;
	float Nflow_Microbe_Slow;
	float Nflow_Active_Passive;
	float Nflow_Active_Slow;

	float Nflow_Slow_Passive;
	float Nflow_Slow_Active;
	float Nflow_Passive_Active;

	float Resp_DecompN_surface_slow;
	float Resp_DecompN_surface_microbes;
	float Resp_DecompN_soil_slow;
	float Resp_DecompN_soil_active;
	float Resp_DecompN_surface_met;
	float Resp_DecompN_soil_met;
	float Resp_DecompN_microbes;
	float Resp_DecompN_active;

	float Resp_DecompN_slow;
	float Resp_DecompN_passive;

	float CN_surface_active_new;
	float CN_surface_slow_new;
	float CN_soil_active_new;
	float CN_soil_slow_new;
	float CN_surface_active_met;
	float CN_soil_active_met;
	float CN_microbes;
	float CN_active;
	float CN_active_passive;
	float CN_slow;
	float CN_slow_passive;
	float CN_passive;

	float miner_Surface_slow;
	float miner_Soil_slow;
	float miner_microbes;
	float miner_active;
	float miner_microbes_met;
	float miner_active_met;
	float miner_microbe_slow;
	float miner_active_passive;
	float miner_active_slow;

	float miner_slow_passive;
	float miner_slow_active;
	float miner_passive_active;

	float immob_Surface_slow;
	float immob_Soil_slow;
	float immob_microbes;
	float immob_active;
	float immob_microbes_met;
	float immob_active_met;
	float immob_microbe_slow;
	float immob_active_passive;
	float immob_active_slow;

	float immob_slow_passive;
	float immob_slow_active;
	float immob_passive_active;

	float NMineralization_gross;
	float NMineralization_net;
	float NFixation;
	float NFertilization;
	float Nflux;
	float Nflux_to_Dead_Fresh;
	float Nflux_to_Dead_Brown;
	float Nflux_to_DeadWood;
	float Nflux_to_Dead_Root;
	float MNGreen, MNGreen_C;
	float MNBrown, MNBrown_C, MNBrown_S;
	float MNR, MNR_C, MNR_S;

	float strlig_SRFC, strlig_SOIL;

	// ----- GRASSMIND ---
	float MBGreen, MBBrown, MBR;
	// normal mortality --> green and brown and root biomass
	float MBGreen_C, MBBrown_C, MBR_C;
	// crowding mortality --> green and brown and root biomass
	float MBGreen_S, MBBrown_S, MBR_S;
	// senescence mortality --> green and brown and root biomass
	float Cflux_to_Dead_Fresh, Cflux_to_Dead_Brown, Cflux_to_Dead_Root;
	float HarvestBg, HarvestBb;
	// harvest of fresh (Bg) or senescent (Bb) biomassvector<float>PWPLayer; // permanent wilting point [mm] per soil layer of plot
	float Biomass_calib; // Biomass over a certain hight (harvest height)

	std::vector<float>WDemand;
	std::vector<float>RootLength;
	// total root length [m] per soil layer of vegetation on plot
	std::vector<float>NDemand;
	// total N demand [kg] per soil layer of vegetation on plot
	std::vector<float>NO3N; // nitrogen content in soil [kg/ha]
	std::vector<float>NUptake;

	// real nitrogen uptake [kg/ha] per plot of all individuals per soil layer
	double Nrep;
	double RepNuptake;

	PLOT();
};

// -------------- HEC  RECORD -------------------

class HECTAR { // Variables within one hectare (100 x 100 m = 5 x 5 plots)

public:
	PlotPointer FirstPlot; // root of plot list in ha
	int HecNo;// running number
	float LocXL, LocYL, LocXH, LocYH;
	// position of ha corners (lower left / upper right) [m]
	float HSVLogged; // stem volume logged in ha [m^3]
	int HLogPotential; // poteniel number of tress to log [trees]
	int PCTHLogPotential; // poteniel number of PCT to log [trees]
	int PCTtolog_ha; // number of PCT to log [trees]
	int HLogN; // stem number logged in ha [trees]
	bool HecLog; // switch ha already logged ?
	int Treestolog_ha; // stem number to be logged in ha [trees]
	int MEADOW; // switch for status (0: forest; 1:empty; 2: empty and growing)
	int BORDER; // if 1, then at border with fragmentation effects
	int globalseed[HYPERMAXGRP];
	HECTAR*next; // Pointer on next hec element

	std::vector<double>LidarWF; // lidar waveform
};

typedef HECTAR*HecPointer;

// -------------- PIN  RECORD -------------------

class PIN { // Plot initalization, only used for data read-in of pin file

public:
	std::string name[MAXPLOT * MAXHA]; // name

	long position[MAXPLOT * MAXHA][4];
	// position of corners [m] (lower left / upper right)
	long mel[MAXPLOT * MAXHA]; // mean elevation [m] -- not used
	long landcode[MAXPLOT * MAXHA];
	// specific landcode: 35 - forest, 0 - non-forest
	long seeds[MAXPLOT * MAXHA][HYPERMAXGRP];
	// seeds per PFT in initial seed pool [seeds]

	std::vector<std::vector<std::vector<std::string> > >n0;
	// Initial stem-diameter distribution per PFT
};

typedef PIN*PinPointer;

// -------------- SWITCH RECORD -------------------

class SWITCH { // Some read-in infos for scenario definition

public:
	bool Files[MAXSAVE]; // old, additional out files
	float DLYR; // vertikal diskretisation [m]
	float Schwelle; // D-Schwelle fuer Ausgabe in m
	int MortThreshold; // thresholh for stochastic mortality
	int Ha; // Size of simlation area [ha]
	float Hectare;
	float Hecside;
	int Maxplot;
};

// -------------- TIME RECORD -------------------

class TIME { // Simulation time

public:
	double Start; // start [yr]
	double End; // end [yr]
	double D; // stepsize [yr]
	double Out; // outstepsize [yr]
	double T; // current time [yr]
	double FileOut; // counter for additional output [yr]
	double OutputStart; // edna 2015, start for saviing [yr]
	int S; // number of timesteps within one day [-]

	bool BeginOfYear(bool isgrassmind) {
		if (isgrassmind || (D < 1.0))
			return true;
		else
			return ((T - D) - floor((T - D) + D / 2.0)) < (D / 2.0);
	}
};

// -------------- LOG  RECORD -------------------

class LOG { // Everything concerning the LOGGING of tree - harvest time !!!

public:
	double*DamDia; // upper diameter of damage classes [m]
	double*Dam1; // damage1 per diameter class [-]
	double*Dam2; // damage2 per diameter class [-]
	double Deathpc[4]; // for output, finally damaged trees per diameter[%]

	double N_good_pft_d[HYPERMAXGRP + 1][MAXDDOUT + 1];
	// tree number per PFT and dbh, alive   [trees ha^-1]
	double N_dam_pft_d[HYPERMAXGRP + 1][MAXDDOUT + 1];
	// tree number per PFT and dbh, damaged [trees ha^-1]
	double N_yield_pft_d[HYPERMAXGRP + 1][MAXDDOUT + 1];
	// tree number per PFT and dbh, yielded [trees ha^-1]
	double BA_good_pft_d[HYPERMAXGRP + 1][MAXDDOUT + 1];
	// basal area per PFT and dbh, alive   [m^2 ha^-1]
	double BA_dam_pft_d[HYPERMAXGRP + 1][MAXDDOUT + 1];
	// basal area per PFT and dbh, damaged [m^2 ha^-1]
	double BA_yield_pft_d[HYPERMAXGRP + 1][MAXDDOUT + 1];
	// basal area per PFT and dbh, yielded [m^2 ha^-1]

	int DoIt; // Logging?, 0: no; 1: yes
	bool AreaLogging;     // log all trees in a few plots  ; no - 0; yes - 1
	int Thinning; // Thinning? 0:no, 1: yes
	int ThinningNPCT;
	// Number of desired potential crop trees (PCT) per 20m x 20m patch
	double ThinningMinDBH; // minimum DBH for PCT
	int MaxRemove;
	// Maximal number of trees that are removable per plot(thinning)
	int StrategyN; // Strategy 1: CON, 2: RIL, 3: CON-AH, 4: RIL-AH
	// 5/6: no damage to harv. trees
	// 7/8: area damage (whole plots->0) see for_log.cc for more
	double Time; // Start of Logging [yr]
	double FirstThinning; // Year of first Thinning event [yr]
	double ThinTimeAfterLog;
	// Years [yr] after a logging event for a thinning event
	double*Diameter; // Logging diameter Threshold [m]
	double Cycle;
	// Logging Cycle, alternativ input for meadowfraction, if < 1.0 [yr]
	double ThinningCycle; // Thinning Cycle
	double Age; // used in old version
	int AreaN; // used in old version
	int Rest; // min left harvestable trees [trees ha^-1]
	long DamLinear; 	// additional Strategy information
							// 0: nothing
							// 1xxxyyy: yyy: percentage of mortality for xxx years
	int No_HarvestDamage; // no damage for potential harvest trees
	int SkidTrailDamage; // Calculates skidtrail damages in plots
	int SkidTrailDamage_Time; // x years after logging calculate skid trails
	double SkidTrailDamage_Area; // relative size of skid trails (to total area)
	int SkidTrailEstablishNewTrails;
	// establish new skit trails or use old ones
	int Falling_DamagedPlots; // looks for already damaged plots
	int Min; // min harvest per hectare and cycle [trees ha^-1 cycle^-1]
	// additional information
	// if Min > 1000: 1xxx: xxx: volume to yield
	int Max; // max harvest per hectare and cycle [trees ha^-1 cycle^-1]
	int CutPlotN; // Plots already logged
	double Done; // Have I just logged? Counts years after logging
	double NextTime; // = t + cycle [yr]
	double ThinningNextTime; // = t + cycle [yr]
	double Harvestable; // harvestable trees [trees]
	double HarvestableSV; // harvestable volume [m^3]
	int LogCount; // count logging operation
	double Log; // trees to be logged [trees]
	double VolumeSum; // out, total [m3/ha]
	double NumberSum; // out, total [1/ha]
	double DamSVSum; // out, total [m3/ha]
	double DamNSum; // out, total [1/ha]
	double InvNSum; // out, total [1/ha]
	double Number; // out, one logging [1/ha]
	double NumberHa[1000]; //out, �ne logging in every hectar [1]
	double Volume; // out, one logging [m3/ha]
	double VolumeHa[1000]; // out, one logging in every hectar [m3]
	double BoleVolume; // out, one logging [m3/ha]
	double BoleVolumeHa[1000]; // out, one logging in every hectar [m3]
	double DamSV; // out, one logging [m3/ha]
	double DamSVHa[1000]; // out, one logging in every hectar [m3]
	double DamN; // out, one logging [1/ha]
	double InvN; // out, one logging  [1/ha]
	int CycleCount; // count how many cycle were performed
	double t_avDiameter;
	// variable to calculate average cutting diameter, total [m]
	double t_RestSV;
	// variable to calculate rest standing volume, total [m3/ha]
	double t_PriorSV;
	// variable to calculate standing volume prior to logging, total [m3/ha]
	double avDiameter; // variable to calculate average cutting diameter [m]
	double RestSV; // variable to calculate rest standing volume [m3/ha]
	double PriorSV;
	// variable to calculate standing volume prior to logging [m3/ha]

	double HarvestBg[HYPERMAXGRP];
	// harvest of fresh green biomass for all plots and hectares summed up
	double HarvestBb[HYPERMAXGRP];
	bool isMowedrightnow;
	// harvest of senescent brown biomass for all plots and hectares summed up
	double emission_factor;
	// Portion of Carbon directly emitted to atmosphere due to logging

};

// -------------- LOGADD  RECORD -------------------
class LOGADD {
public:
	double VolumeSum[ADDMAX]; // out, total [m3/ha]
	double NumberSum[ADDMAX]; // out, total [1/ha]
	double DamSVSum[ADDMAX]; // out, total [m3/ha]
	double DamN1[ADDMAX]; // unused
	double DamN2[ADDMAX]; // unused
	double DamN3[ADDMAX]; // unused
	double DamN4[ADDMAX]; // unused
	double t_avDiameter[ADDMAX];
	// variable to calculate average cutting diameter, total [m]
	double t_RestSV[ADDMAX];
	// variable to calculate rest standing volume, total [m3/ha]
	double t_PriorSV[ADDMAX];
	// variable to calculate standing volume prior to logging, total [m3/ha]
};

// -------------- LOC  RECORD -------------------
class LOC { // location record

public:
	double XMin; // x val of lower left  corner [m]
	double XMax; // x val of upper right corner [m]
	double YMin; // y val of lower left  corner [m]
	double YMax; // y val of upper right corner [m]
	double XDiff; // x diff upper right - lower left  [m]
	double YDiff; // y diff upper right - lower left  [m]
};

// -------------- RESULT  RECORD -------------------

class RESULT
{

public:
	char FileName[_MAX_PATH * 2];
	char GRASSFileName[_MAX_PATH * 2];
	char GRASSPLOTFileName[_MAX_PATH * 2];
	char GRASS_MOWFileName[_MAX_PATH * 2];
	char GRASSCALIBFileName[_MAX_PATH * 2];
	char RESFileName[_MAX_PATH * 2];
	char RESTHFileName[_MAX_PATH * 2];
	char RESTHBINFileName[_MAX_PATH * 2];
	char COHORTFileName[_MAX_PATH * 2];
	char COHORTTHFileName[_MAX_PATH * 2];
	char THINFileName[_MAX_PATH * 2];
	char RESTARTFileName[_MAX_PATH * 2];
	char RESTARTPLOTFileName[_MAX_PATH * 2];
	char LogFileName[_MAX_PATH * 2];
	char LogHAFileName[_MAX_PATH * 2];
	char LOGNDFileName[_MAX_PATH * 2];
	char LOGBADFileName[_MAX_PATH * 2];
	char LAIFileName[_MAX_PATH * 2];
	char ATSFileName[_MAX_PATH * 2];
	char DYNFileName[_MAX_PATH * 2];
	char DIAFileName[_MAX_PATH * 2];
	char CARBONFileName[_MAX_PATH * 2];
	char CARBONPlotFileName[_MAX_PATH * 2];
	char CARBONCENTFileName[_MAX_PATH * 2];
	char CARBONCENTPlotFileName[_MAX_PATH * 2];
	char NITROGENCENTFileName[_MAX_PATH * 2];
	char NITROGENCENTPlotFileName[_MAX_PATH * 2];
	char BMPLFileName[_MAX_PATH * 2];
	char LANDSLIDEFileName[_MAX_PATH * 2];
	char PLOTBMDYNFileName[_MAX_PATH * 2];
	char LAI_meanFileName[_MAX_PATH * 2];
	char LAI_plotFileName[_MAX_PATH * 2];
	char LAI_plot_heightFileName[_MAX_PATH * 2];
	char FIREFileName[_MAX_PATH * 2];
	char AGBPlotFileName[_MAX_PATH * 2];
	char DIAPLOTFileName[_MAX_PATH * 2];
	char AttrHaFileName[_MAX_PATH * 2];
	char AttrHaThFileName[_MAX_PATH * 2];
	char HEIGHTFileName[_MAX_PATH * 2];
	char SPECIESPlotFileName[_MAX_PATH * 2];
	char SPECIESPlotTHFileName[_MAX_PATH * 2];
	char MORTFileName[_MAX_PATH * 2];
	char MORTTHFileName[_MAX_PATH * 2];
	char MORTPFTFileName[_MAX_PATH * 2];
	char MORTPFTTHFileName[_MAX_PATH * 2];
	char MORTPFTDIAFileName[_MAX_PATH * 2];
	char PRODFileName[_MAX_PATH * 2];
	char SKIDTRAILFileName[_MAX_PATH * 2];
	char LIDARPCFileName[_MAX_PATH * 2];
	char VOXFORFileName[_MAX_PATH * 2];
	char LIDARWFFileName[_MAX_PATH * 2];
	char ENVIRONMENTFileName[_MAX_PATH * 2];
	char WATERFileName[_MAX_PATH * 2];
	char WATERPLOTFileName[_MAX_PATH * 2];
	char WATERCENTFileName[_MAX_PATH * 2];
	char WATERCENTPLOTFileName[_MAX_PATH * 2];
	char WATERCENTPLOTLAYERFileName[_MAX_PATH * 2];
	char DYNTHFileName[_MAX_PATH * 2];
	char DYN2FileName[_MAX_PATH * 2];
	char DYN3FileName[_MAX_PATH * 2];
	char DYNTH2FileName[_MAX_PATH * 2];
	char DYNTH3FileName[_MAX_PATH * 2];
	char DYN4FileName[_MAX_PATH * 2];
	char DYN5FileName[_MAX_PATH * 2];
	char DYNTH4FileName[_MAX_PATH * 2];
	char DYNTH5FileName[_MAX_PATH * 2];
	char DYN6FileName[_MAX_PATH * 2];
	char DYNTH6FileName[_MAX_PATH * 2];
	char FALFileName[_MAX_PATH * 2];
	char INFileName[_MAX_PATH * 2];
	char GLOBFileName[_MAX_PATH * 2];
	char SEEDFileName[_MAX_PATH * 2];
	char SEEDRAINFileName[_MAX_PATH * 2];
	char SEEDLINGFileName[_MAX_PATH * 2];
	char SEEDTREEFileName[_MAX_PATH * 2];
	char PINOUTFileName[_MAX_PATH * 2];
	char BVFileName[_MAX_PATH * 2];
	char BVTHFileName[_MAX_PATH * 2];
	char TRAITSFileName[_MAX_PATH * 2];
	char BTCFileName[_MAX_PATH * 2];

	char ParameterHeader[81], VersionName[81], RegionHeader[81];
	char GrpName[81][HYPERMAXGRP];

	bool InitFile(void);
	void GetFileNames(void);
	bool OpenFiles(void);
	bool WriteHeaderLines(void);
	bool Save(bool, int);
	void SaveF11(void);
	void SaveF12(void);
	void Close(void);
	void Save_parameters(void);
	bool SaveLogging(void);
	bool LastLogInfo(double, double, double);
	bool LastLogInit(void);
	void WriteLoggingOutput(int);
	void WriteFireOutput();
	bool WriteEnvironmentOutput();
	void WriteWaterOutput();
	void WriteWaterOutputCent();
	void WriteSkidtrail();
	void WriteThinTreeRecord(TreePointer, PlotPointer, HecPointer);

	FILE*LIDARPCFile;
	FILE*VOXFORFile;
	FILE*LIDARWFFile;
	FILE*SKIDTRAILFile;
//	FILE*ThinFile;

private:
	FILE*File;
	FILE*ResThFile;
	FILE*ResThBinFile;
	FILE*CohortFile;
	FILE*CohortThFile;
	FILE*ThinFile;
	FILE*RestartFile;
	FILE*RestartPlotFile;
	FILE*LogFile;
	FILE*LogHAFile;
	FILE*LOGNDFile;
	FILE*LOGBADFile;
	FILE*LAIFile;
	FILE*ATSFile;
	FILE*DYNFile;
	FILE*DIAFile;
	FILE*ATS0File;
	FILE*ATS100File;
	FILE*LAI0File;
	FILE*LAI100File;
	FILE*DIA0File;
	FILE*DIA100File;
	FILE*P250File;
	FILE*P25100File;
	FILE*CARBONFile;
	FILE*CARBONPlotFile;
	FILE*CARBONCENTFile;
	FILE*CARBONCENTPlotFile;
	FILE*NITROGENCENTFile;
	FILE*NITROGENCENTPlotFile;
	FILE*BMPLFile;
	FILE*LANDSLIDEFile;
	FILE*PLOTBMDYNFile;
	FILE*LAI_meanFile;
	FILE*LAI_plotFile;
	FILE*LAI_plot_heightFile;
	FILE*ENVIRONMENTFile;
	FILE*HEIGHTFile;
	FILE*SPECIESPlotFile;
	FILE*SPECIESPlotTHFile;
	FILE*GRASSFile;
	FILE*GRASSPLOTFile;
	FILE*GRASS_MOWFile;
	FILE*GRASSCALIBFile;
	FILE*BTCFile; // AGB after chave
	// File for environmental reduction factors Climate
	FILE*FIREFile; // File for Fire data.
	FILE*AGBPlotFile; // File for AGB per plot.
	FILE*DIAPLOTFile;
	FILE*AttrHaFile; // File for AGB per ha.
	FILE*AttrHaThFile; // File for AGB per ha with treshold.
	FILE*MORTFile; // File for mortality.
	FILE*MORTTHFile; // File for mortality th
	FILE*MORTPFTFile; // File for mortality per pft
	FILE*MORTPFTTHFile; // File for mortality th per pft
	FILE*MORTPFTDIAFile; // File for mortality per pft and diameter
	FILE*PRODFile; // File for production.
	FILE*WATERFile;
	FILE*WATERPLOTFile;
	FILE*WATERCENTFile;
	FILE*WATERCENTPLOTFile;
	FILE*WATERCENTPLOTLAYERFile;
	FILE*DYNTHFile;
	FILE*DYN2File;
	FILE*DYNTH2File;
	FILE*DYN3File;
	FILE*DYNTH3File;
	FILE*DYN4File;
	FILE*DYNTH4File;
	FILE*DYN5File;
	FILE*DYNTH5File;
	FILE*DYN6File;
	FILE*DYNTH6File;
	FILE*FALFile;
	FILE*GLOBFile;
	FILE*INFile;
	FILE*SEEDFile;
	FILE*SEEDRAINFile;
	FILE*SEEDLINGFile;
	FILE*SEEDTREEFile;
	FILE*PINOUTFile;
	FILE*BVFile;
	FILE*BVTHFile;
	FILE*TRAITSFile;

};

// -------------- RESULTFILES  RECORD -------------------
class resultFileSwitch { // which

public:
	resultFileSwitch();

	bool plot; // old agbplot
	bool diaplot;
	bool speciesplot;
	bool speciesplot_th;
	bool grass;
	bool grassplot;
	bool grass_mow;
	bool grasscalib;
	bool ats;
	bool ba;
	bool ba_th;
	bool bmpl;
	bool bt;
	bool bt_th;
 	bool biom_chave_th;
	bool cflux;
	bool cfluxplot;
	bool dia;
	bool div;
	bool div_th;
	bool dyn;
	bool dyn_th;
	bool fal;
	bool fire;
	bool ha;
	bool ha_th;
	bool h;
	bool in;
	bool lai;
	bool lai_mean;
	bool lai_plot;
	bool lai_plot_heightlayer;
	bool log;
	bool log_ha;
	bool log_bad;
	bool log_nd;
	bool logg_end;
	bool mort;
	bool mort_th;
	bool mort_pft;
	bool mort_pft_th;
	bool mort_pft_dia;
	bool n;
	bool n_th;
	bool prod;
	bool lidarpc;
	bool voxfor;
	bool lidarwf;
	bool res;
	bool res_th;
	bool res_th_bin;
	bool cohort;
	bool cohort_th;
	bool thin;
	bool restart;
	bool restartplot;
	bool seed;
	bool seed_rain;
	bool seedling;
	bool stree;
	bool sv;
	bool sv_th;
	bool bv;
	bool bv_th;
	bool branch;
	bool water;
	bool water_plot;
	bool env;
	bool landslide;
	bool plotbmdyn;
	bool spn;
	bool save_parameter_files;
	bool result_time_stamp;
	bool spatial;
	bool pin;
	bool water_century_plot;
	bool water_century_plot_layer;
	bool cflux_century_plot;
	bool nflux_century_plot;
	bool traits;
	bool skid;
	bool blender_json;
};

// -------------- PAR  RECORD -------------------

class N_PAR { // Parameter record, see parameter file for description

public:

	std::string Formind_Version;

	double Div_DiaClassWidth;
	bool OldVersionTest;
	bool StoreInitialState;
	bool ExpNotation;


	std::string InvFileName;
	std::string InitPoolsFileName;

	// Switches
	bool Closed_boundary;
	bool Globalseeds;
	bool Spacelimitation;
	bool Treefall;
	bool Nogrow;
	bool Seedtree;
	bool Patch_Seed;
	bool Densityreg;
	bool Predation;
	bool Predmort;
	bool Fragmentation;
	bool Frag_highmortbigtree;
	bool Frag_lowseed;
	bool Frag_TemperatureEffects;
	bool Flag_SpeciesNumber;
	bool Century_ON; // Century soil model
	bool Candy_ON; // Candy soil model
	bool GRASSMIND;
	bool TraitDist_ON; // frag for trait distribution
	bool Cohort; // activate Cohort approach (default)
	bool random_seed_ON; // flag for rancom seed
	bool Landslide;
	bool spatial; // spatially explicit or not
	bool Fire;
	bool Liana;
	bool Flag_BackgroundMortality, Flag_DbhMortality, Flag_DincMortality;
	bool Flag_DD_Seed_Mortality, Flag_DD_Seedling_Mortality;
	int CrownDensityCurve;
	bool Mort_nbinc;
	bool Patch_Heterogeneity;

	// species properties

	std::vector<double>Div_COMMERCIAL_A;
	std::vector<int>Div_Liana;
	std::vector<double>Div_Animaldispersed;
	std::vector<int>SpeciesNumber;

	long Div_MAXGRP;
	long Div_MAXTREE;

	// Treelists
	float TreeListOutputStep;
	float TreeListOutputStart;
	int TreeListInput;
	int InitPools;

	// ---- Geometry -----
	double**Geo_Bio2Dbh; // parameterset for BoutofD-powerlaw and DoutofB;
	double**Geo_TR_21, *Geo_HMmean_5, **Geo_HD_35, **Geo_HD_39, **Geo_LD_31,
		 **Geo_LAIT_21, **Geo_CD_31, **Geo_FD_31, **Geo_CLFH_31;
	long*Geo_FUNCTION_7;

	double Geo_RB, Geo_RH, Pro_RD, Pro_RCON;
	double StandardDeviation_Diameter_Height;
	double Min_Diameter_Inc_Step;

	// --- Growth ---
	double**Pro_MLoss_33;
	double*Pro_Limit_3;
	int Pro_FUNCTION_2;
	double*Pro_Pmax_3, *Pro_Alpha_3, *Pro_dbh_growth_max, *Pro_dbh_growth_start,
		 *Pro_dbh_growth_end, *Pro_dbh_growth_maxpoint, *Pro_Rho_3;
	double**Pro_dbh_growth;

	std::string Growth_Function;

	double*Div_K;
	double Pro_GLoss, Pro_M;
	bool Flag_PBbuffer;

	// --- Recruitment ---
	double*Est_DSTree_5, *Est_ISeed_3, *Est_Dist_3, *Est_SeedMort_3,
		 *Est_SeedSurvival_3, *MeanSeedDistance, *SeedPredatorNumber,
		 *SeedPredatorEfficiency, *SeedPredatorHandlingTime;
	long*Est_NS_3;
	double*Est_SeedTree_3;
	double*Max_Den;
	int*DispersalKernel;
	double Est_DS, HGRP_EXP, Lin_Dis;
	int TimeInvasion;
	int*Invasion;

	// --- Mortality ----
	long*Mort_FUNCTION_2;
	double**Mort_mean_35, **Mort_Dia_31, **Mort_DD_Seed_31,
		 **Mort_DD_Seedling_31, **Mort_Dinc_31, *Mort_mean_19;
	double Mort_50nbinc;
	double Mort_SpaceLimitation_Factor;
	double Mort_SpaceLimitation_Diameter;
	double Mort_FallP;

	// --- Environment/Climate -----
	long*Climate;

	std::vector<std::string>Climate_File;

	double*Env_IS_2, *Env_DayL_2, *Env_SeaL_2, *Env_VegPeriod;
	double*Temperature_month_cold;
	double*Temperature_month_hot;
	double*ref_environment_reduction;
	double*ref_length_of_vegetation_periode;
	double*ref_Irradiance;
	bool variable_Irradiance_ON;
	bool Temperature_ON;
	bool Water_ON;
	bool Veg_period_ON;
	bool Daylength_ON;
	bool Puls;
	int CO2_dependency;

	std::string Temperature_Reduction_Algorithm;

	// [�C] optimal temperature for photosyntheses (Horn algorithm)
	float Temperature_k; // [1/�C] rate of Change (Horn algorithm)
	float Temperature_min;
	// [�C] Minimum mean temperature for photosynthesis all day with higher temperature are definend as vegetation period
	float Temperature_Q10;
	// [�C] Short-term respiration response to temperature
	float Temperature_reference; // [�C] reference temperature
	double temp_co2low;
	// [�C] Minimum temperature for photosynthesis
	double temp_co2high;
	// [�C] Minimum temperature for photosynthesis
	// Parameter for temperature reduction curve normal distributed:
	float Temperature_sig;
	float Temperature_opt; // optimal temperature condition

	// CO2 dependency parameters
	float CO2_reference_temperature;
	float CO2_reference_concentration;
	float CO2_stomata_reference_concentration;
	float WUE_Transpiration_Assimilation_ratio;
	double*CO2_dependency_function_simple;

	// ---- FIRE ----
	long*Fire_Tolerance;
	double Fire_Lambda;
	double Fire_Beta;
	double Fire_Sev;
	double Fire_minFuelLoad;
	double Fire_exmoisture;
	double Fire_FirstTime;

	// --- FRAGMENTATION ----
	double Edge_Distance;

	// --- LIGHT COMPETITION ---
	float Light_Comp_Factor;  // Scaling factor for light competition (GRASSMIND, reziprok is used)

	// ---- WATER ----
	float Water_k; // [?] rate of change (Horn algorithm)
	float Water_inflection; // [?] inflection point (Horn algorithm)
	double*Water_WUE;
	float Water_KL;
	float Water_POR;
	float Water_KS;
	float Water_L;
	float Water_SW_RES;
	float Water_RainfallDuration; // duration of daily rainfall [h]

	float Water_SD; // soil depth (m)
	int Water_SoilLayer; // number of soil layers (default: 1)
	double*Water_LayerDepth; // height of each soil layer (default: Water_SD)
	double*Water_FC; // field capacity
	double*Water_PWP; // permanent wilting point

	// -------- CENTURY -------------
	std::vector<std::string>Soil_File;
	std::vector<std::string>Manage_File;

	// ---- spatially explicit version ----
	bool spatial_area;
	bool spatial_ZOI20;

	// ---- LANDSLIDE ----
	int Timespan_reduced_rates;
	double SlideFreqHaYear;
	int Landslide_reduced_growth;
	int Landslide_reduced_seeds;
	int Landslide_increased_mortality;
	long*Slide_Timelag_extern_file;
	double**SlideSizeFrequency;

	// ---- CARBON ----
	double Cflux_Dead_to_Athmo;
	double Cflux_Dead_to_Soil;
	double Cflux_Soil_to_Athmo;
	float Cflux_aet;
	double CPool_DeadWood;
	// [tC/ha] used to initialize dead wood pool
	double CPool_Soil_fast;
	// [tc/ha] used to initialize soil pools
	double CPool_Soil_slow;
	// [tc/ha] used to initialize soil pools

	// ---- Grassmind ---
	double Pro_Mresp; // maintenance respiration []
	double*Geo_RL; // specific root length
	double*Div_CN_Green;
	double*Div_CN_Brown;
	double**Geo_AGB_2; // shoot-root relationship
	double**Geo_RD_2; // shoot-rooting depth relationship
	double**Geo_alloc_4; // allocation fractions
	double*Geo_LLS; // leaf life span
	double*Geo_RLS; // root life span
	double*Geo_OF; // overlapping factor
	double*Est_Germ; // germination rate
	double*Est_Em; // days to emergence of seedlings/germination
	double*Div_Nfixing;
	double*Div_Lifespan;
	double*Sow_Date; // sowing date of seeds for the grassland
	bool InitialMowed;
	float InitialMowHeight;
	float InitialGreenFraction;
	float Cost_Rhiz; // symbiosis with rhizobia, percentage of NPP is energy
	//investment in rhizobia to get full N
	float Calib_Biomass; // height of the harvest cut in field (for calibration)

	// ---- Sidar parameters -----
	float LidarOutputStep;
	float LidarOutputStart;
	float PulseDistance;
	float LargeFootprintDiameter;
	float LargeFootprintEnergySD;
	float VegetationSurfaceReturnProbability;
	float VegetationExtinctionCoef;
	float GroundSurfaceReturnProbability;
	float GroundExtinctionCoef;
	int*CrownGeometry;
	bool LidarLAIBased;
	bool CrownOverlapSum;
	float CrownOverlapMaxLAD;
	bool LidarTreeTrunks;
	bool LidarPostProcessing;
	float ThinningCellSize;

	std::string RScriptPath;
	std::string RPath;

	// Error Handling
	int WarningType;
	int ErrorType;
};

// -------------- OUT  RECORD -------------------
class OUT_FORMIND { // output record, the name stand for the contents

public:
	long SPECIESandGAPSUM[NGAP];
	double SVandCOM[2], BVandCOM[2], BAandCOM[2], BTandCOM[2], NCOM[2];
	double MeanWoodDen, TH_MeanWoodDen;
	double TH_SVandCOM[2], TH_BVandCOM[2], TH_BAandCOM[2], TH_BTandCOM[2],
		 TH_NCOM[2];
	double SEEDandCOM[2], SEEDRAINandCOM[2], SEEDTREEandCOM[2];
	double*SVandGRP, *BVandGRP, *BTandGRP, *BAandGRP, *NGRP;
	double*TH_SVandGRP, *TH_BVandGRP, *TH_BTandGRP, *TH_BAandGRP, *TH_NGRP;
	// number of dead trees and rates due to:
	// 0: total, 1: basic mortality, 2: falling, 3: damage by falling, 4: crowding, 5: fire 6: landslide 7: logging 8: damage logging
	double DEATHCUM[9], TH_DEATHCUM[9];
	double DEATHCUMrate[9], TH_DEATHCUMrate[9];
	// biomass of dead trees and rate due to:
	// 0: total, 1: basic mortality, 2: falling, 3: damage by falling, 4: crowding, 5: fire 6: landslide 7: logging 8: damage logging
	double BiomassDEATHCUM[9], TH_BiomassDEATHCUM[9];
	double BiomassDEATHCUMrate[9], TH_BiomassDEATHCUMrate[9];

	std::vector<std::vector<double> >GROWTHandGRP, TH_GROWTHandGRP;
	std::vector<std::vector<double> >GROWTH_COUNTER_andGRP,
		 TH_GROWTH_COUNTER_andGRP;
	std::vector<double>GROWTHSUM, TH_GROWTHSUM;
	std::vector<double>GROWTH_COUNTER_SUM, TH_GROWTH_COUNTER_SUM;

	double AvATSUM[MAXLYR + 1], AvLAI[MAXLYR + 1];
	double AvATSF[MAXLYR + 1][3];
	double AvLAD[MAXLAD + 1];

	std::vector<double>DIAFALL;
	std::vector<double>SVDIASUM;

	// double BADIASUM[MAXDDOUT+1], TH_BADIASUM[MAXDDOUT+1];
	double BINCPlot[MAXHA][MAXPLOT]; //biomass increase for each plot
	double BMPlot[MAXHA][MAXPLOT];
	// alive aboveground biomass per plot
	double BMofNewTrees[MAXHA][MAXPLOT];
	// biomass of new established trees to close biomass dynamics
	double RBMPlot[MAXHA][MAXPLOT]; //alive root biomass per plot
	double RCONPlot[MAXHA][MAXPLOT];
	// root conductivity of alive roots per plot
	double DBMPlot[MAXHA][MAXPLOT];
	// dead aboveground biomass per plot
	double NEWDBMPlot[MAXHA][MAXPLOT];
	// dead aboveground biomass per plot that died in the actual year

	double DRMPlot[MAXHA][MAXPLOT]; //dead root biomass per plot
	double DRCONPlot[MAXHA][MAXPLOT];
	//root conductivity of dead roots per plot
	double SLOPE[MAXHA][MAXPLOT]; //slope for each plot
	double ELEV[MAXHA][MAXPLOT]; //elevation for each plot
	int ForType[MAXHA][MAXPLOT]; //forest type for each plot
	int TimeSinceSlide[MAXHA][MAXPLOT];
	//time passed since last slide event in this plot
	double SlideProb[MAXHA][MAXPLOT]; //sliding probability for each plot
	int PlotCorner[MAXHA][MAXPLOT][4];
	//coordinates of plotcorners for graphical output, used in .bmpl
	int SlideSize[MAXHA][MAXPLOT];
	// size of the slide; each slide occurs only once
	int firenum; // number of fire per annum. used in *.fire
	int firesize; //size of fire. used in *.fire
	double firesev; //  severity of fire. used in *.fire
	float water_pr; //  precipitation *.water
	float water_ev; //  precipitation *.water
	float water_si; //  interception *.water
	float water_ro; //  run-off *.water
	float water_ro_a; //  run-off above-gruond *.water
	float water_ro_b; //  run-off below-ground *.water
	float water_tr; //  transpiration *.water
	float water_soil; //  soil-water *.water
	float water_rw; //  reduction factor due to lack of water *.water
	float water_count; //  auxiliary variable for output time-step *.water
	float water_pwp; //  permanent wilting point *.water
	float water_msw; //  minimum water for potential photosynthese *.water
	float water_pet; //  PET *.water
	double LAI[MAXHA][MAXPLOT];

	double LAI_Layer[MAXHA][MAXPLOT][MAXLYR + 1];
	double LAD_Layer[MAXHA][MAXPLOT][MAXLYR + 1];

	double LAI_mean; // LAI averaged over all plots for output LAI_mean
	double InTH_GRP[HYPERMAXGRP];
	double SEEDandGRP[HYPERMAXGRP];
	double SEEDRAINandGRP[HYPERMAXGRP];
	double SEEDLINGandGRP[HYPERMAXGRP];
	double SEEDTREEandGRP[HYPERMAXGRP];
	double DEATH_PFT[HYPERMAXGRP], TH_DEATH_PFT[HYPERMAXGRP];

	double**DIAandGRP_new;
	double**DIADEATH_PFT;
	double**TH_DIAandGRP_new;
	double**BADIAandGRP_new;
	double**TH_BADIAandGRP_new;

	double*DIASUM;
	double*DIADEATH;
	double*TH_DIASUM;
	double*BADIASUM_new;
	double*TH_BADIASUM_new;

	double**DIAandLGRP_new;
	double**TH_DIAandLGRP_new;

	double**DIAandHGRP_new;
	double**TH_DIAandHGRP_new;

	double BTSUM_2, SVSUM, BVSUM, BTSUM, BASALSUM, NTOTAL, GAP, MATURE, BUILDING;
	double TH_BTSUM_2, TH_SVSUM, TH_BVSUM, TH_BTSUM, TH_BASALSUM, TH_NTOTAL, TH_BTCSUM, TH_BTCHSUM;
	double DEATH, TH_DEATH;
	long GAPNO, GAPDAMAGE, TH_GAPDAMAGE;
	double SPACEDEATH, TH_SPACEDEATH;
	double DEADMEAN, DEADDERI;
	double AvIRFloor;
	double AvIRFloorPC;
	double GAPSUM;
	double InTH;
	double SWI_N, SWI_BT, TH_SWI_N, TH_SWI_BT;
	double SEEDSUM, SEEDRAINSUM, SEEDTREESUM, SEEDLINGSUM;
	long SPECIESSUM, TH_SPECIESSUM;

	// relevant for Grassmind
	double MeanHeight[HYPERMAXGRP], MaxHeight[HYPERMAXGRP];
	double BTGreen[HYPERMAXGRP], LAIGrass[HYPERMAXGRP], LAIGreen[HYPERMAXGRP];
	double Biomass_calib[HYPERMAXGRP];
	double RBTandGRP[HYPERMAXGRP], ShootBiomassMean[HYPERMAXGRP];
	double RootDepthMean[HYPERMAXGRP], LeafNAreaMean[HYPERMAXGRP],
		 WMHC[HYPERMAXGRP], BiomassDensity[HYPERMAXGRP];

	//cflux results   Carbon_circle_ON
	//(Sato, 2007)
	float CPool_Soil_fast, CPool_Soil_slow, CPool_DeadWood; // [tC / ha]
	float Cflux_to_Soil_fast, Cflux_to_Soil_slow; // [tC / ha]
	float Cflux_to_DeadWood; // [tC / ha]
	float CPool_alive_biomass; // [tC / ha]
	float C_flux, PB; // [tC / ha]
	float Resp_DeadWood, R_total, R_total_biomass; // [tC / ha]
	float Resp_Soil_fast, Resp_Soil_slow; // [tC / ha]
	float aet;

};

extern char SimDir[_MAX_PATH * 2];
extern double TestTEnd;
extern char PinFileName[_MAX_PATH * 2];
extern FILE*PinFile;
extern int FileN;
extern RESULT Result;
extern LOC Loc;
extern LOG Logging;
extern LOGADD Logadd;
extern TIME T;
extern SWITCH Switch;
extern OUT_FORMIND Out;
extern double HMAX;
extern double DMAX;
extern int HIGHESTLAYERNUMBER;
extern double AREASUM;
extern double AREASUM_MEADOW;
extern int HELPSTEP, MAXARRAY;
extern int SAEMLINGSKRONE;
extern int MaxSeed, MinSeed;
extern double MAXLAI;
extern int MAXGRP;
extern HecPointer FirstHec;
extern int HGRP_BIG;
extern int WRITE_BEGIN;
extern double GROWTH_COEFF[MAXSPECIES][4];
extern N_PAR N_Par;
extern resultFileSwitch myResultFileSwitch;


extern std::vector<std::string>DCLASS;

extern double DM[MAXSPECIES];
extern double BTM[MAXSPECIES];
extern double A4[MAXSPECIES];
extern double B2D[MAXB2D][MAXSPECIES];
extern int BINC[2];
extern double D2OTLAI[MAXD2OTLAI];
extern std::vector<HecPointer>vHec;
extern double CumSpeciesNumber[HYPERMAXGRP];
extern bool sown;
extern std::vector<std::vector<double> >vDiameterLookup;
extern std::vector<std::vector<double> >vBiomassLookup;

extern double*diadd, **diagrp;

namespace ForTools {
	PlotPointer FindPlot(HecPointer, double, double);
	HecPointer FindHectar(double, double);
	void DetermineFallLoc(int Dir, double x, double y, double H, double*xf,
		 double*yf);
	void DetermineFallLoc_noclosedboundary(int Dir, double x, double y, double H,
		 double*xf, double*yf);
	bool FindNeighbours(PlotPointer plot);
	bool FindEightNeighbours(PlotPointer plot);
	bool FractionalDice(double val);
	bool isnotseeded(int val, std::vector<int>&arr, int size);
};

#endif  // for_var.hh
