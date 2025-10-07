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
// File						for_const.h
// Description				Description  Definition of all global constants and
//                      all preprocessor makros, which define the model
//                      performance
//
///////////////////////////////////////////////////////////////////


#ifndef  for_constH
#define  for_constH
// ----------------------------------------------------------------
// -------- MAKROS for linux portability                      -----
// ----------------------------------------------------------------
#ifndef _MAX_PATH
#define _MAX_PATH 4096
#endif
// ----------------------------------------------------------------
// -------- MAKROS DEFINITION IMPORTANT FOR WHOLE PERFORMANCE -----
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// -------- GLOBAL CONSTANTS --------------------------------------
// ----------------------------------------------------------------

const double PI = 3.14159265358979323846;
const double ODM_TO_C = 0.44; // conversion from 1 t odm in 0.44 t C
const double CO2_TO_ODM = 0.63; // conversion from 1 t CO2 in 0.63 t odm

const float GAPHEIGHT = 25.0;   // [m] max height for GAP phase
const float BUILDINGHEIGHT = 36.0; // [m] max height for BUILDING phase
const int MAXSPECIES = 469; // for singlespecies application
const int N_COMMERCIAL = 2; // dimension of arrays NEWSEEDS SEEDPOOL
const float CELLSIZE = 0.5;
const float CELLAREA = 0.25;
const float CELLHEIGHT = 0.5;
const int NGAP = 4; // gap phases distinguished, sum+1(sum)
const int PULSYEAR = 5; // Years between 2 seedpulses (in PULS)
const int MEADOWTIME = 2500; // Years of use of meadows before regrowth
const int MAXD2OTLAI = 31; // arrayborder for D2OTLAI   31: each 5cm
const int HYPERMAXGRP = 22; // absolut maximum number of groups
const int MAXLYR = 200;
// DLYR is defined in .par (usually 0.5m -> max height of trees must be <50m)
// HMAX/DLYR number of vertical layers
const int MAXSOILLAYER = 20; // maximum number of soil layers
const int MAXLAD = 20; // HMAX(50)/DLAD(5) LAD: LeafAreaDensity
const int MAXPLOT = 25; // plots in a ha
const int MAXSAVE = 20; // number of result files
const int MAXHA = 128; // 265 is absolute limit there
const int ADDMAX = 6; // arraymax for additonal output
const float HECTARE = 10000.0; // [m] area of ha
const float HECSIDE = 100.0;
// side length of one ha - needed when using non-standard hectars
const float INDVIDUALDEATH = 0.10; // [m] Threshold for stochastic death
const float FALLTHRESHOLD = 0.10; // [m] Threshold for dying tres to fall

const int MAXDDOUT = 50; // output, number of diameter classes
// only relevant for logging outputs!
// should work, as long as N_Par.Div_DiaClassWidth is <= 0.05 (i.e. 5 cm diameter classes for output)
// if smaller diameter classes are wanted, one will likely have to increase MAXDDOUT. It should be DMAX/N_Par.Div_DiaClassWidth.

const double DELTA = 1e-12;
const int FORESTTYPE = 2;  // ForestType

const int MAXB2D = 1500;
const int B2DSTEP = 1000; // translates mm in m

const char FILE_SEPARATOR = '\\';
#define  STDSIMEXT      "sim"
#define  STDINEXT       "in"
#define  STDRESEXT      "res"
#define  STDRESTHEXT      "res_th"
#define  STDRESTHBINEXT      "res_th_bin"
#define  STDCOHORTEXT      "cohort"
#define  STDCOHORTTHEXT      "cohort_th"
#define  STDTHINEXT      "thin"
#define  STDRESTARTEXT      "restart"
#define  STDRESTARTPLOTEXT      "restartplot"
#define  STDGRASSEXT    "grass"
#define  STDGRASSPLOTEXT    "grassplot"
#define  STDGRASS_MOWEXT    "grass_mow"
#define  STDGRASSCALIBEXT    "grasscalib"
#define  STDLOGEXT      "logging"
#define  STDLOGHAEXT      "logging_ha"
#define  STDLOGENDEXT   "logging_end"
#define  STDLOGNDEXT    "loging_nd"
#define  STDLOGBADEXT   "loging_bad"
#define  STDLAIEXT      "lai"
#define  STDATSEXT	"ats"
#define  STDHEIGHTEXT	"h"
#define  STDDIAEXT	"dia"
#define  STDDIAPLOTEXT	"diaplot"
#define  STDCARBONEXT    "cflux"
#define  STDCARBONPLOTEXT "cfluxplot"
#define  STDCARBONCENTEXT    "cflux_century"
#define  STDCARBONCENTPLOTEXT "cfluxplot_century"
#define  STDNITROGENCENTEXT    "nflux_century"
#define  STDNITROGENCENTPLOTEXT "nfluxplot_century"
#define  STDPINEXT      "pin"
#define  STDTABEXT      "tab"
#define  STDPAREXT      "par"
#define  STDOPEXT       "op"
#define  STDPAVEXT      "pav"
#define  STDBMPLEXT      "bmpl"
#define  STDLANDSLIDEEXT      "landslide"
#define  STDPLOTBMDYNEXT      "plotbmdyn"
#define  STDLAI_meanEXT      "lai_mean"
#define  STDLAI_plotEXT      "lai_plot"
#define  STDLAI_plot_heightEXT      "lai_plot_layer"
#define  STDFIREEXT     "fire"
#define  STDAGBPLOTEXT  "plot"
#define  STDATTRHAEXT  "ha"
#define  STDATTRHATHEXT  "ha_th"
#define  STDSPECIESPLOTEXT  "speciesplot"
#define  STDSPECIESPLOTTHEXT  "speciesplot_th"
#define  STDMORTEXT  "mort"
#define  STDMORTTHEXT  "mort_th"
#define  STDMORTPFTEXT  "mort_pft"
#define  STDMORTPFTTHEXT  "mort_pft_th"
#define  STDMORTPFTDIAEXT	"mort_pft_dia"
#define  STDPRODEXT  "prod"
#define  STDLIDARPCEXT  "lidarpc"
#define  STDVOXFOREXT  "voxfor"
#define  STDLIDARWFEXT  "lidarwf"
#define  STDENVIRONMENTEXT "env"
#define  STDWATEREXT    "water"
#define  STDWATERALLEXT "waterplot"
#define  STDWATERCENTEXT    "watercent"
#define  STDWATERCENTALLEXT "watercentplot"
#define  STDWATERCENTLAYEXT "watercentlayer"
#define  STDDYNEXT	"dyn"
#define  STDTHEXT	"dyn_th"
#define  STDDYN2EXT	"sv"
#define  STDTH2EXT	"sv_th"
#define  STDDYN3EXT	"n"
#define  STDTH3EXT	"n_th"
#define  STDDYN4EXT	"bt"
#define  STDTH4EXT	"bt_th"
#define  STDBTCEXT	"biom_chave_th"
#define  STDDYN5EXT	"ba"
#define  STDTH5EXT	"ba_th"
#define  STDDYN6EXT	"div"
#define  STDTH6EXT	"div_th"
#define  STDSEEDEXT     "seed"
#define  STDSEEDRAINEXT     "seed_rain"
#define  STDSEEDLINGEXT "seedling"
#define  STDSEEDTREEEXT "stree"
#define  STDSPECIESEXT  "spn"
#define  STDPINOUTEXT      "pin"
#define  STDBVEXT	 "bv"
#define  STDBVTHEXT	"bv_th"
#define  STDTRAITSEXT "traits"
#define 	STDSKIDEXT "skid"
#define  SQR(x)    ((x)*(x)) //defined in nrutil.h (numerical recipes)

#endif

// -----------------------------------------------------------
// ----------------- end of for_const.h -------------------
// -----------------------------------------------------------
