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
// File         						for_io.cpp
// Description  	              File input and output functions of FORMIND.
//
///////////////////////////////////////////////////////////////////

#include "for_io.h"
#include "for_misc.h"
#include "random.h"
#include "for_var.h"
#include "MMParSet/MMErrorMessage.h"

#ifdef underconstruction
#include "for_sidar.h"
#include "for_trait.h"
#include "century/century_carbon.h"
#endif

#include <time.h>
#include <cstring>
#include <sstream>
#include <stdexcept>
using namespace std;

// --------------------------------------
void Add_Ext(char, const char);
bool WriteHeaderATS(FILE*DestFile);
bool WriteHeaderLAI(FILE*DestFile);
void WritePFTHeader(FILE, int, char, char, char);
bool WriteHeaderDIA(FILE);
bool WriteHeaderKAI(FILE);
bool WriteHeaderCARBON(FILE);
bool WriteHeaderCARBONPlot(FILE);
bool WriteHeaderLUDWIG(FILE);
bool WriteHeaderDIAMORT(FILE);
bool WriteHeaderVAR(FILE, char, char, char);
char myFileName[1024];
std::string time_now, myDir;
std::string mydrive, mydir, myfile, myext;
void WriteTreeRecord(FILE, TreePointer, PlotPointer, HecPointer);
void WriteTreeRecordDINC(FILE, TreePointer);
void WriteTreeRecord3D(FILE, TreePointer, PlotPointer, HecPointer);
void WriteCARBON(FILE);
void WriteCARBONPlot(FILE);
void WriteLUDWIG_DIA(FILE);
void WriteDIAMORT(FILE);
void WriteBMofPLOTS(FILE, int, int, int, int);
void WriteAGBPlot(FILE);
void WriteDIAPLOT(FILE);
void WriteAttrHa(FILE);
void WriteAttrHaTh(FILE);
void WriteRESTARTPLOT(FILE);
void WriteHEIGHT(FILE);
void WriteSPECIESPlot(FILE);
void WriteMORT(FILE);
void WriteMORTTH(FILE);
void WritePROD(FILE);
#ifdef underconstruction
void WriteFIRE(FILE);
void WriteLANDSLIDE(FILE, int, int, int, int);
#endif
void WritePLOTBMDYN(FILE, int, int, int, int);
void WritePLOTLAI(FILE, int, int, int, int);
void CalculatePlotNumbers2(PlotPointer, std::vector<std::vector<int> >&);

// -----------------------------------------------------------

/* !
 \brief          Closes all files
 \param	        void
 \return	        void
 */

void RESULT::Close(void)

{
	if (myResultFileSwitch.plot) {
		fflush(AGBPlotFile);
		fclose(AGBPlotFile);
	}

	if (myResultFileSwitch.diaplot) {
		fflush(DIAPLOTFile);
		fclose(DIAPLOTFile);
	}

	if (myResultFileSwitch.ha) {
		fflush(AttrHaFile);
		fclose(AttrHaFile);
	}

	if (myResultFileSwitch.ha_th) {
		fflush(AttrHaThFile);
		fclose(AttrHaThFile);
	}

	if (myResultFileSwitch.speciesplot) {
		fflush(SPECIESPlotFile);
		fclose(SPECIESPlotFile);
	}

	if (myResultFileSwitch.speciesplot_th) {
		fflush(SPECIESPlotTHFile);
		fclose(SPECIESPlotTHFile);
	}

	if (myResultFileSwitch.res) {
		fflush(File);
		fclose(File);
	}

	if (myResultFileSwitch.res_th) {
		fflush(ResThFile);
		fclose(ResThFile);
	}

	if (myResultFileSwitch.res_th_bin) {
		fflush(ResThBinFile);
		fclose(ResThBinFile);
	}


	if (myResultFileSwitch.cohort) {
		fflush(CohortFile);
		fclose(CohortFile);
	}

	if (myResultFileSwitch.cohort_th) {
		fflush(CohortThFile);
		fclose(CohortThFile);
	}

	if (myResultFileSwitch.thin) {
		fflush(ThinFile);
		fclose(ThinFile);
	}

	if (myResultFileSwitch.restart) {
		fflush(RestartFile);
		fclose(RestartFile);
	}

	if (myResultFileSwitch.restartplot) {
		fflush(RestartPlotFile);
		fclose(RestartPlotFile);
	}

	if (myResultFileSwitch.grass) {
		fflush(GRASSFile);
		fclose(GRASSFile);
	}

	if (myResultFileSwitch.grassplot) {
		fflush(GRASSPLOTFile);
		fclose(GRASSPLOTFile);
	}

	if (myResultFileSwitch.grass_mow) {
		fflush(GRASS_MOWFile);
		fclose(GRASS_MOWFile);
	}

	if (myResultFileSwitch.grasscalib) {
		fflush(GRASSCALIBFile);
		fclose(GRASSCALIBFile);
	}

	if (myResultFileSwitch.ats) {
		fflush(ATSFile);
		fclose(ATSFile);
	}
	if (myResultFileSwitch.lai) {
		fflush(LAIFile);
		fclose(LAIFile);
	}
	if (myResultFileSwitch.dia) {
		fflush(DIAFile);
		fclose(DIAFile);
	}
	if (myResultFileSwitch.sv) {
		fflush(DYN2File);
		fclose(DYN2File);
	}
	if (myResultFileSwitch.bv) {
		fflush(BVFile);
		fclose(BVFile);
	}
	if (myResultFileSwitch.n) {
		fflush(DYN3File);
		fclose(DYN3File);
	}
	if (myResultFileSwitch.h) {
		fflush(HEIGHTFile);
		fclose(HEIGHTFile);
	}
	if (myResultFileSwitch.bt) {
		fflush(DYN4File);
		fclose(DYN4File);
	}
	if (myResultFileSwitch.ba) {
		fflush(DYN5File);
		fclose(DYN5File);
	}
	if (myResultFileSwitch.div) {
		fflush(DYN6File);
		fclose(DYN6File);
	}
	if (myResultFileSwitch.in) {
		fflush(INFile);
		fclose(INFile);
	}
	if (myResultFileSwitch.seed) {
		fflush(SEEDFile);
		fclose(SEEDFile);
	}
	if (myResultFileSwitch.seed_rain) {
		fflush(SEEDRAINFile);
		fclose(SEEDRAINFile);
	}
	if (myResultFileSwitch.seedling) {
		fflush(SEEDLINGFile);
		fclose(SEEDLINGFile);
	}
	if (myResultFileSwitch.stree) {
		fflush(SEEDTREEFile);
		fclose(SEEDTREEFile);
	}
	if (myResultFileSwitch.sv_th) {
		fflush(DYNTH2File);
		fclose(DYNTH2File);
	}
	if (myResultFileSwitch.bv_th) {
		fflush(BVTHFile);
		fclose(BVTHFile);
	}
	if (myResultFileSwitch.n_th) {
		fflush(DYNTH3File);
		fclose(DYNTH3File);
	}
	if (myResultFileSwitch.bt_th) {
		fflush(DYNTH4File);
		fclose(DYNTH4File);
	}
	if (myResultFileSwitch.biom_chave_th) {
		fflush(BTCFile);
		fclose(BTCFile);
	}
	if (myResultFileSwitch.ba_th) {
		fflush(DYNTH5File);
		fclose(DYNTH5File);
	}
	if (myResultFileSwitch.div_th) {
		fflush(DYNTH6File);
		fclose(DYNTH6File);
	}

	if (myResultFileSwitch.bmpl) {
		fflush(BMPLFile);
		fclose(BMPLFile);
	}
	if (myResultFileSwitch.plotbmdyn) {
		fflush(PLOTBMDYNFile);
		fclose(PLOTBMDYNFile);
	}
	if (myResultFileSwitch.landslide) {
		fflush(LANDSLIDEFile);
		fclose(LANDSLIDEFile);
	}
	if (myResultFileSwitch.fire) {
		fflush(FIREFile);
		fclose(FIREFile);
	}
	if (myResultFileSwitch.mort) {
		fflush(MORTFile);
		fclose(MORTFile);
	}
	if (myResultFileSwitch.mort_th) {
		fflush(MORTTHFile);
		fclose(MORTTHFile);
	}
	if (myResultFileSwitch.mort_pft) {
		fflush(MORTPFTFile);
		fclose(MORTPFTFile);
	}
	if (myResultFileSwitch.mort_pft_th) {
		fflush(MORTPFTTHFile);
		fclose(MORTPFTTHFile);
	}
	if (myResultFileSwitch.mort_pft_dia) {
		fflush(MORTPFTDIAFile);
		fclose(MORTPFTDIAFile);
	}
	if (myResultFileSwitch.prod) {
		fflush(PRODFile);
		fclose(PRODFile);
	}
	if (myResultFileSwitch.water) {
		fflush(WATERFile);
		fclose(WATERFile);
	}
	if (myResultFileSwitch.water_plot) {
		fflush(WATERPLOTFile);
		fclose(WATERPLOTFile);
	}
	if (myResultFileSwitch.water_century_plot) {
		fflush(WATERCENTPLOTFile);
		fclose(WATERCENTPLOTFile);
	}
	if (myResultFileSwitch.water_century_plot_layer) {
		fflush(WATERCENTPLOTLAYERFile);
		fclose(WATERCENTPLOTLAYERFile);
	}
	if (myResultFileSwitch.env) {
		fflush(ENVIRONMENTFile);
		fclose(ENVIRONMENTFile);
	}
	if (myResultFileSwitch.cflux) {
		fflush(CARBONFile);
		fclose(CARBONFile);
	}
	if (myResultFileSwitch.cfluxplot) {
		fflush(CARBONPlotFile);
		fclose(CARBONPlotFile);
	}

	if (myResultFileSwitch.cflux_century_plot) {
		fflush(CARBONCENTPlotFile);
		fclose(CARBONCENTPlotFile);
	}

	if (myResultFileSwitch.nflux_century_plot) {
		fflush(NITROGENCENTPlotFile);
		fclose(NITROGENCENTPlotFile);
	}
	if (myResultFileSwitch.lai_mean) {
		fflush(LAI_meanFile);
		fclose(LAI_meanFile);
	}
	if (myResultFileSwitch.lai_plot) {
		fflush(LAI_plotFile);
		fclose(LAI_plotFile);
	}
	if (myResultFileSwitch.lai_plot_heightlayer) {
		fflush(LAI_plot_heightFile);
		fclose(LAI_plot_heightFile);
	}
	if (myResultFileSwitch.pin) {
		fflush(PINOUTFile);
		fclose(PINOUTFile);
	}
	if (myResultFileSwitch.log) {
		fflush(LogFile);
		fclose(LogFile);
	}
	if (myResultFileSwitch.log_ha) {
		fflush(LogHAFile);
		fclose(LogHAFile);
	}
	if (myResultFileSwitch.log_nd) {
		fflush(LOGNDFile);
		fclose(LOGNDFile);
	}
	if (myResultFileSwitch.log_bad) {
		fflush(LOGBADFile);
		fclose(LOGBADFile);
	}
	if (myResultFileSwitch.lidarwf) {
		fflush(LIDARWFFile);
		fclose(LIDARWFFile);
	}

	if (myResultFileSwitch.traits) {
		fflush(TRAITSFile);
		fclose(TRAITSFile);
	}

	if (myResultFileSwitch.skid) {
		fflush(SKIDTRAILFile);
		fclose(SKIDTRAILFile);
	}

}

// -----------------------------------------------------------

/* !
 \brief          Creates a copy of parameter file and pin file in result folder
 and creates stout and sterr
 \param	        void
 \return	        void
 */

void RESULT::Save_parameters(void) {
	ifstream source;
	ofstream dest;

	string pardir = for_dirname(fileNames.getParFileNameAbsolutePath());
	// directory of .par file  (relative path usually)
	string parfn = for_basename(fileNames.getParFileNameAbsolutePath());
	// filename of .parfile, e.g. virtual.par
	string resdir = pardir + directorySeparator + "..\\results\\"; // resultdir
	resdir = makeNicePathForYourOS(resdir);

	// Defining relative file names of stout.txt and sterr.txt
	string stoutfile = pardir + directorySeparator + "..\\stout.txt";
	string sterrfile = pardir + directorySeparator + "..\\sterr.txt";
	stoutfile = makeNicePathForYourOS(stoutfile);
	sterrfile = makeNicePathForYourOS(sterrfile);

	source.open(fileNames.getParFileNameAbsolutePath().c_str(), ios::binary);
	// open .par file for reading
	std::string destfile = resdir + time_now + "_" + parfn;
	dest.open(destfile.c_str(), ios::binary);
	if (source.is_open() && dest.is_open()) {
		dest << source.rdbuf();
		source.close();
		dest.close();
	}
	source.open(PinFileName, ios::binary);
	std::string destPinFileName = resdir + time_now + "_" + removeExtension
		 (parfn) + ".pin";
	dest.open(destPinFileName.c_str(), ios::binary);
	if (source.is_open() && dest.is_open()) {
		dest << source.rdbuf();

		source.close();
		dest.close();
	}
	source.open(stoutfile.c_str(), ios::binary);
	std::string destTxtFileName = resdir + time_now + "_" + removeExtension
		 (parfn) + ".txt";
	dest.open(destTxtFileName.c_str(), ios::binary);
	if (source.is_open() && dest.is_open()) {
		dest << source.rdbuf();

		source.close();
		dest.close();
	}
	source.open(sterrfile.c_str(), ios::binary);
	std::string destStderrTxtFileName = resdir + time_now + "_sterr.txt";
	dest.open(destStderrTxtFileName.c_str(), ios::binary);
	if (source.is_open() && dest.is_open()) {
		dest << source.rdbuf();

		source.close();
		dest.close();
	}

}

// ---------------------------------------------------------------

/* !
 \brief          Initializes output files
 \param	        void
 \return         boolean true if no error occurs.
 */

bool RESULT::InitFile(void)

{
	Result.GetFileNames();
	if (!Result.OpenFiles())
		goto E_EXIT;
	if (!Result.WriteHeaderLines())
		goto E_EXIT;
	return true;

E_EXIT:
	cerr << "ERROR: variable error \tfile: " << __FILE__ <<
		 "\tfunction: InitFile\t line:" << __LINE__ <<
		 "\t FORMIND InitFile error detected" << endl;
	return false;

}

// ---------------------------------------------------------------

/* !
 \brief          Creates file names
 \param	        void
 \return         void
 */

void RESULT::GetFileNames(void)

{
	time_now = currentDateTime();
	if (myResultFileSwitch.result_time_stamp) {
		fileNames.setResultFilePrefix(time_now + std::string("_"));
	}

	strcpy(RESFileName, fileNames.getResultFileNameAbsolutePath(STDRESEXT).c_str());
	strcpy(RESTHFileName, fileNames.getResultFileNameAbsolutePath(STDRESTHEXT).c_str());
	strcpy(RESTHBINFileName, fileNames.getResultFileNameAbsolutePath(STDRESTHBINEXT).c_str());
	strcpy(COHORTFileName, fileNames.getResultFileNameAbsolutePath(STDCOHORTEXT).c_str());
	strcpy(COHORTTHFileName, fileNames.getResultFileNameAbsolutePath(STDCOHORTTHEXT).c_str());
	strcpy(THINFileName, fileNames.getResultFileNameAbsolutePath(STDTHINEXT).c_str());
	strcpy(RESTARTFileName, fileNames.getResultFileNameAbsolutePath(STDRESTARTEXT).c_str());
	strcpy(RESTARTPLOTFileName, fileNames.getResultFileNameAbsolutePath(STDRESTARTPLOTEXT).c_str());
	strcpy(GRASSFileName, fileNames.getResultFileNameAbsolutePath(STDGRASSEXT).c_str());
	strcpy(GRASSPLOTFileName, fileNames.getResultFileNameAbsolutePath(STDGRASSPLOTEXT).c_str());
	strcpy(GRASS_MOWFileName, fileNames.getResultFileNameAbsolutePath(STDGRASS_MOWEXT).c_str());
	strcpy(GRASSCALIBFileName, fileNames.getResultFileNameAbsolutePath(STDGRASSCALIBEXT).c_str());
	strcpy(SPECIESPlotFileName, fileNames.getResultFileNameAbsolutePath(STDSPECIESPLOTEXT).c_str());
	strcpy(SPECIESPlotTHFileName, fileNames.getResultFileNameAbsolutePath(STDSPECIESPLOTTHEXT).c_str());
	strcpy(LogFileName, fileNames.getResultFileNameAbsolutePath(STDLOGEXT).c_str());
	strcpy(LogHAFileName, fileNames.getResultFileNameAbsolutePath(STDLOGHAEXT).c_str());
	strcpy(LAIFileName, fileNames.getResultFileNameAbsolutePath(STDLAIEXT).c_str());
	strcpy(ATSFileName, fileNames.getResultFileNameAbsolutePath(STDATSEXT).c_str());
	strcpy(HEIGHTFileName, fileNames.getResultFileNameAbsolutePath(STDHEIGHTEXT).c_str());
	strcpy(DYNFileName, fileNames.getResultFileNameAbsolutePath(STDDYNEXT).c_str());
	strcpy(DYNTHFileName, fileNames.getResultFileNameAbsolutePath(STDTHEXT).c_str());
	strcpy(DIAFileName, fileNames.getResultFileNameAbsolutePath(STDDIAEXT).c_str());
	strcpy(DIAPLOTFileName, fileNames.getResultFileNameAbsolutePath(STDDIAPLOTEXT).c_str());
	strcpy(BMPLFileName, fileNames.getResultFileNameAbsolutePath(STDBMPLEXT).c_str());
	strcpy(LANDSLIDEFileName, fileNames.getResultFileNameAbsolutePath(STDLANDSLIDEEXT).c_str());
	strcpy(PLOTBMDYNFileName, fileNames.getResultFileNameAbsolutePath(STDPLOTBMDYNEXT).c_str());
	strcpy(LAI_meanFileName, fileNames.getResultFileNameAbsolutePath(STDLAI_meanEXT).c_str());
	strcpy(LAI_plotFileName, fileNames.getResultFileNameAbsolutePath(STDLAI_plotEXT).c_str());
	strcpy(LAI_plot_heightFileName, fileNames.getResultFileNameAbsolutePath(STDLAI_plot_heightEXT).c_str());
	strcpy(FIREFileName, fileNames.getResultFileNameAbsolutePath(STDFIREEXT).c_str());
	strcpy(AGBPlotFileName, fileNames.getResultFileNameAbsolutePath(STDAGBPLOTEXT).c_str());
	strcpy(AttrHaFileName, fileNames.getResultFileNameAbsolutePath(STDATTRHAEXT).c_str());
	strcpy(AttrHaThFileName, fileNames.getResultFileNameAbsolutePath(STDATTRHATHEXT).c_str());
	strcpy(MORTFileName, fileNames.getResultFileNameAbsolutePath(STDMORTEXT).c_str());
	strcpy(MORTTHFileName, fileNames.getResultFileNameAbsolutePath(STDMORTTHEXT).c_str());
	strcpy(MORTPFTFileName, fileNames.getResultFileNameAbsolutePath(STDMORTPFTEXT).c_str());
	strcpy(MORTPFTTHFileName, fileNames.getResultFileNameAbsolutePath(STDMORTPFTTHEXT).c_str());
	strcpy(MORTPFTDIAFileName, fileNames.getResultFileNameAbsolutePath(STDMORTPFTDIAEXT).c_str());
	strcpy(PRODFileName, fileNames.getResultFileNameAbsolutePath(STDPRODEXT).c_str());
	strcpy(LIDARPCFileName, fileNames.getResultFileNameAbsolutePath(STDLIDARPCEXT).c_str());
	strcpy(VOXFORFileName, fileNames.getResultFileNameAbsolutePath(STDVOXFOREXT).c_str());
	strcpy(LIDARWFFileName, fileNames.getResultFileNameAbsolutePath(STDLIDARWFEXT).c_str());
	strcpy(WATERFileName, fileNames.getResultFileNameAbsolutePath(STDWATEREXT).c_str());
	strcpy(WATERPLOTFileName, fileNames.getResultFileNameAbsolutePath(STDWATERALLEXT).c_str());
	strcpy(WATERCENTFileName, fileNames.getResultFileNameAbsolutePath(STDWATERCENTEXT).c_str());
	strcpy(WATERCENTPLOTFileName, fileNames.getResultFileNameAbsolutePath(STDWATERCENTALLEXT).c_str());
	strcpy(WATERCENTPLOTLAYERFileName, fileNames.getResultFileNameAbsolutePath(STDWATERCENTLAYEXT).c_str());
	strcpy(DYN2FileName, fileNames.getResultFileNameAbsolutePath(STDDYN2EXT).c_str());
	strcpy(DYNTH2FileName, fileNames.getResultFileNameAbsolutePath(STDTH2EXT).c_str());
	strcpy(DYN3FileName, fileNames.getResultFileNameAbsolutePath(STDDYN3EXT).c_str());
	strcpy(DYNTH3FileName, fileNames.getResultFileNameAbsolutePath(STDTH3EXT).c_str());
	strcpy(DYN4FileName, fileNames.getResultFileNameAbsolutePath(STDDYN4EXT).c_str());
	strcpy(DYNTH4FileName, fileNames.getResultFileNameAbsolutePath(STDTH4EXT).c_str());
	strcpy(DYN5FileName, fileNames.getResultFileNameAbsolutePath(STDDYN5EXT).c_str());
	strcpy(DYNTH5FileName, fileNames.getResultFileNameAbsolutePath(STDTH5EXT).c_str());
	strcpy(DYN6FileName, fileNames.getResultFileNameAbsolutePath(STDDYN6EXT).c_str());
	strcpy(DYNTH6FileName, fileNames.getResultFileNameAbsolutePath(STDTH6EXT).c_str());
	strcpy(BVFileName, fileNames.getResultFileNameAbsolutePath(STDBVEXT).c_str());
	strcpy(BVTHFileName, fileNames.getResultFileNameAbsolutePath(STDBVTHEXT).c_str());
	strcpy(INFileName, fileNames.getResultFileNameAbsolutePath(STDINEXT).c_str());
	strcpy(GLOBFileName, fileNames.getResultFileNameAbsolutePath(STDLOGENDEXT).c_str());
	strcpy(SEEDFileName, fileNames.getResultFileNameAbsolutePath(STDSEEDEXT).c_str());
	strcpy(SEEDRAINFileName, fileNames.getResultFileNameAbsolutePath(STDSEEDRAINEXT).c_str());
	strcpy(SEEDLINGFileName, fileNames.getResultFileNameAbsolutePath(STDSEEDLINGEXT).c_str());
	strcpy(SEEDTREEFileName, fileNames.getResultFileNameAbsolutePath(STDSEEDTREEEXT).c_str());
	strcpy(LOGNDFileName, fileNames.getResultFileNameAbsolutePath(STDLOGNDEXT).c_str());
	strcpy(LOGBADFileName, fileNames.getResultFileNameAbsolutePath(STDLOGBADEXT).c_str());
	strcpy(ENVIRONMENTFileName, fileNames.getResultFileNameAbsolutePath(STDENVIRONMENTEXT).c_str());
	strcpy(CARBONFileName, fileNames.getResultFileNameAbsolutePath(STDCARBONEXT).c_str());
	strcpy(CARBONPlotFileName, fileNames.getResultFileNameAbsolutePath(STDCARBONPLOTEXT).c_str());
	strcpy(CARBONCENTPlotFileName, fileNames.getResultFileNameAbsolutePath(STDCARBONCENTPLOTEXT).c_str());
	strcpy(NITROGENCENTPlotFileName, fileNames.getResultFileNameAbsolutePath(STDNITROGENCENTPLOTEXT).c_str());
	strcpy(PINOUTFileName, fileNames.getResultFileNameAbsolutePath(STDPINOUTEXT).c_str());
	strcpy(TRAITSFileName, fileNames.getResultFileNameAbsolutePath(STDTRAITSEXT).c_str());
	strcpy(SKIDTRAILFileName, fileNames.getResultFileNameAbsolutePath(STDSKIDEXT).c_str());
	strcpy(BTCFileName, fileNames.getResultFileNameAbsolutePath(STDBTCEXT).c_str());
}


// ---------------------------------------------------------------

/* !
 \brief           Executes the open file function
 \param			   destfile     File name
 \param  			FileName     File name
 \return   			void
 */

void doFileOpen(FILE**destfile, char*FileName) {
	if ((*destfile = fopen(FileName, "wt")) == NULL) {
		cerr << "ERROR : Can't create  " << FileName << endl;
	}
	else {
		setvbuf(*destfile, NULL, _IOFBF, 10485760);
	}
}

// for res_th_bin only:
void doFileOpenBinary(FILE**destfile, char*FileName) {
	if ((*destfile = fopen(FileName, "wb")) == NULL) {
		cerr << "ERROR : Can't create  " << FileName << endl;
	}
	else {
		setvbuf(*destfile, NULL, _IOFBF, 10485760);
	}
}


// ---------------------------------------------------------------

/* !
 \brief          Opens all necessary files
 \param  		  void
 \return         boolean true if no error occurs.
 */

bool RESULT::OpenFiles(void)

{
	if (myCreateDirectory(fileNames.getResultDirAbsolutePath())) {
		cerr << "INFO: Created results folder!" << endl;
	}

	if (myResultFileSwitch.res)
		doFileOpen(&File, RESFileName);
	if (myResultFileSwitch.res_th)
		doFileOpen(&ResThFile, RESTHFileName);
	if (myResultFileSwitch.res_th_bin)
		doFileOpenBinary(&ResThBinFile, RESTHBINFileName);
	if (myResultFileSwitch.cohort)
		doFileOpen(&CohortFile, COHORTFileName);
	if (myResultFileSwitch.cohort_th)
		doFileOpen(&CohortThFile, COHORTTHFileName);
	if (myResultFileSwitch.thin)
		doFileOpen(&ThinFile, THINFileName);
	if (myResultFileSwitch.restart)
		doFileOpen(&RestartFile, RESTARTFileName);
	if (myResultFileSwitch.restartplot)
		doFileOpen(&RestartPlotFile, RESTARTPLOTFileName);
	if (myResultFileSwitch.grass)
		doFileOpen(&GRASSFile, GRASSFileName);
	if (myResultFileSwitch.grassplot)
		doFileOpen(&GRASSPLOTFile, GRASSPLOTFileName);
	if (myResultFileSwitch.grass_mow)
		doFileOpen(&GRASS_MOWFile, GRASS_MOWFileName);
	if (myResultFileSwitch.grasscalib)
		doFileOpen(&GRASSCALIBFile, GRASSCALIBFileName);
	if (myResultFileSwitch.speciesplot)
		doFileOpen(&SPECIESPlotFile, SPECIESPlotFileName);
	if (myResultFileSwitch.speciesplot_th)
		doFileOpen(&SPECIESPlotTHFile, SPECIESPlotTHFileName);
	if (myResultFileSwitch.log)
		doFileOpen(&LogFile, LogFileName);
	if (myResultFileSwitch.log_ha)
		doFileOpen(&LogHAFile, LogHAFileName);
	if (myResultFileSwitch.log_nd)
		doFileOpen(&LOGNDFile, LOGNDFileName);
	if (myResultFileSwitch.log_bad)
		doFileOpen(&LOGBADFile, LOGBADFileName);
	if (myResultFileSwitch.ats)
		doFileOpen(&ATSFile, ATSFileName);
	if (myResultFileSwitch.lai)
		doFileOpen(&LAIFile, LAIFileName);
	if (myResultFileSwitch.dia)
		doFileOpen(&DIAFile, DIAFileName);
	if (myResultFileSwitch.sv)
		doFileOpen(&DYN2File, DYN2FileName);
	if (myResultFileSwitch.bv)
		doFileOpen(&BVFile, BVFileName);
	if (myResultFileSwitch.n)
		doFileOpen(&DYN3File, DYN3FileName);
	if (myResultFileSwitch.h)
		doFileOpen(&HEIGHTFile, HEIGHTFileName);
	if (myResultFileSwitch.bt)
		doFileOpen(&DYN4File, DYN4FileName);
	if (myResultFileSwitch.ba)
		doFileOpen(&DYN5File, DYN5FileName);
	if (myResultFileSwitch.div)
		doFileOpen(&DYN6File, DYN6FileName);
	if (myResultFileSwitch.in)
		doFileOpen(&INFile, INFileName);
	if (myResultFileSwitch.seed)
		doFileOpen(&SEEDFile, SEEDFileName);
	if (myResultFileSwitch.seed_rain)
		doFileOpen(&SEEDRAINFile, SEEDRAINFileName);
	if (myResultFileSwitch.stree)
		doFileOpen(&SEEDTREEFile, SEEDTREEFileName);
	if (myResultFileSwitch.seedling)
		doFileOpen(&SEEDLINGFile, SEEDLINGFileName);
	if (myResultFileSwitch.sv_th)
		doFileOpen(&DYNTH2File, DYNTH2FileName);
	if (myResultFileSwitch.bv_th)
		doFileOpen(&BVTHFile, BVTHFileName);
	if (myResultFileSwitch.n_th)
		doFileOpen(&DYNTH3File, DYNTH3FileName);
	if (myResultFileSwitch.bt_th)
		doFileOpen(&DYNTH4File, DYNTH4FileName);
	if (myResultFileSwitch.biom_chave_th)
		doFileOpen(&BTCFile, BTCFileName);
	if (myResultFileSwitch.ba_th)
		doFileOpen(&DYNTH5File, DYNTH5FileName);
	if (myResultFileSwitch.div_th)
		doFileOpen(&DYNTH6File, DYNTH6FileName);
	if (myResultFileSwitch.plot)
		doFileOpen(&AGBPlotFile, AGBPlotFileName);
	if (myResultFileSwitch.diaplot)
		doFileOpen(&DIAPLOTFile, DIAPLOTFileName);
	if (myResultFileSwitch.ha)
		doFileOpen(&AttrHaFile, AttrHaFileName);
	if (myResultFileSwitch.ha_th)
		doFileOpen(&AttrHaThFile, AttrHaThFileName);
	if (myResultFileSwitch.mort)
		doFileOpen(&MORTFile, MORTFileName);
	if (myResultFileSwitch.mort_th)
		doFileOpen(&MORTTHFile, MORTTHFileName);
	if (myResultFileSwitch.mort_pft)
		doFileOpen(&MORTPFTFile, MORTPFTFileName);
	if (myResultFileSwitch.mort_pft_th)
		doFileOpen(&MORTPFTTHFile, MORTPFTTHFileName);
	if (myResultFileSwitch.mort_pft_dia)
		doFileOpen(&MORTPFTDIAFile, MORTPFTDIAFileName);
	if (myResultFileSwitch.prod)
		doFileOpen(&PRODFile, PRODFileName);
	if (myResultFileSwitch.bmpl)
		doFileOpen(&BMPLFile, BMPLFileName);
	if (myResultFileSwitch.landslide)
		doFileOpen(&LANDSLIDEFile, LANDSLIDEFileName);
	if (myResultFileSwitch.plotbmdyn)
		doFileOpen(&PLOTBMDYNFile, PLOTBMDYNFileName);
	if (myResultFileSwitch.lai_mean)
		doFileOpen(&LAI_meanFile, LAI_meanFileName);
	if (myResultFileSwitch.lai_plot)
		doFileOpen(&LAI_plotFile, LAI_plotFileName);
	if (myResultFileSwitch.lai_plot_heightlayer)
		doFileOpen(&LAI_plot_heightFile, LAI_plot_heightFileName);
	if (myResultFileSwitch.fire)
		doFileOpen(&FIREFile, FIREFileName);
	if (myResultFileSwitch.env)
		doFileOpen(&ENVIRONMENTFile, ENVIRONMENTFileName);
	if (myResultFileSwitch.water)
		doFileOpen(&WATERFile, WATERFileName);
	if (myResultFileSwitch.water_plot)
		doFileOpen(&WATERPLOTFile, WATERPLOTFileName);
	if (myResultFileSwitch.water_century_plot)
		doFileOpen(&WATERCENTPLOTFile, WATERCENTPLOTFileName);
	if (myResultFileSwitch.water_century_plot_layer)
		doFileOpen(&WATERCENTPLOTLAYERFile, WATERCENTPLOTLAYERFileName);
	if (myResultFileSwitch.cflux)
		doFileOpen(&CARBONFile, CARBONFileName);
	if (myResultFileSwitch.cfluxplot)
		doFileOpen(&CARBONPlotFile, CARBONPlotFileName);
	if (myResultFileSwitch.cflux_century_plot)
		doFileOpen(&CARBONCENTPlotFile, CARBONCENTPlotFileName);
	if (myResultFileSwitch.nflux_century_plot)
		doFileOpen(&NITROGENCENTPlotFile, NITROGENCENTPlotFileName);
	if (myResultFileSwitch.pin)
		doFileOpen(&PINOUTFile, PINOUTFileName);
	if (myResultFileSwitch.lidarwf)
		doFileOpen(&LIDARWFFile, LIDARWFFileName);
	if (myResultFileSwitch.traits)
		doFileOpen(&TRAITSFile, TRAITSFileName);
	if (myResultFileSwitch.skid)
		doFileOpen(&SKIDTRAILFile, SKIDTRAILFileName);

	return true;
}

// -------------------------------------------------
// Functions for writing headers in output files;
// first line is a short discription of the value and its unit
// second line is the name of the column
// --------------------------------------------------

// ---------------------------------------------------------------

/* !
 \brief          Writes average cumulative crown projection area header
 \param	 	     DestFile      File name
 \return         boolean true if no error occurs.
 */

bool WriteHeaderATS(FILE*DestFile)

{
	fputs("Average cumulative crown pojection area of all trees on the whole simulated area (average over all plots, all plots in the gap-phase, all plots in the building phase, all plots in the mature phase) for every height layer (lower bound)\n"
		 , DestFile);
	fputs("Time [a1]\tHeight [m1]\tAverage cumulative crown area per plot [m2 m-2]\tAverage cumulative crown area per gap-phase plots [m2 m-2]\tAverage cumulative crown area per building-phase plots [m2 m-2]\tAverage cumulative crown area per mature-phase plots [m2 m-2]\n"
		 , DestFile);
	fputs("Time\tHeight\tAverageCumulativeCrownAreaPerPlot\tAverageCumulativeCrownAreaPerGapPlots\tAverageCumulativeCrownAreaPerBuildPlots\tAverageCumulativeCrownAreaPerMaturePlots\n"
		 , DestFile);
	if (EOF == fputs("", DestFile)) {
		cerr << "ERROR: file error \tfile: " << __FILE__ <<
			 "\tfunction: WriteHeaderATS\t line:" << __LINE__ <<
			 "\t Can't write to file." << endl;
		return false;
	}
	return true;
}

// ---------------------------------------------------------------

/* !
 \brief          Writes headers for average cumulative leaf area index and average leaf area density
 \param	    	  DestFile      File name
 \return         boolean true if no error occurs.
 */

bool WriteHeaderLAI(FILE*DestFile) {
	fputs("Average cumulative leaf area index and average leaf area density (average over all plots) on the whole simulated area for every height layer (lower bound). Only for the LAI calculation, the accumulation starts at the highest height layer and the lowest height layer 0 represents the total leaf area index.\n"
		 , DestFile);
	fputs("Time [a1]\tHeight [m1]\tAverage cumulative leaf area index [m2 m-2]\tAverage leaf area density [m2 m-2]\n"
		 , DestFile);
	fputs("Time\tHeight\tAverageCumulativeLeafAreaIndex\tAverageLeafAreaDensity\n"
		 , DestFile);
	if (EOF == fputs("", DestFile)) {
		cerr << "ERROR: file error \tfile: " << __FILE__ <<
			 "\tfunction: WriteHeaderLAI\t line:" << __LINE__ <<
			 "\t Can't write to file." << endl;
		return false;
	}
	return true;
}

// ---------------------------------------------------------------

/* !
 \brief          Writes Plant Functional Type headers
 \param	      	 DestFile      File name
 \param 		    	 maxarray      Number of PFTs
 \param 		    	 name          Measurement variable
 \param 		       unit          Unit
 \param 		       dummy         Auxiliary variable
 */

void WritePFTHeader(FILE*DestFile, int maxarray, const char*name,
	 const char*unit, const char*dummy) {
	int i, dummyint;
	for (i = 0; i < maxarray - 1; i++) {
		dummyint = i + 1;
		fprintf(DestFile, "%s %d [%s%s]\t", name, dummyint, unit, dummy);
	}
	dummyint = maxarray;
	fprintf(DestFile, "%s %d [%s%s]", name, dummyint, unit, dummy);
}

// ----------------------------------------------------------------

/* !
 \brief             Writes Plant Functional Type headers (short)
 \param	       	 DestFile      File name
 \param 		    	 maxarray      Number of PFTs
 \param 		    	 name          Measurement variable
 \param 		       unit          Unit
 \param 		       dummy         Auxiliary variable
 */

void WritePFTHeaderShort(FILE*DestFile, int maxarray, const char*name,
	 const char*unit, const char*dummy) {
	int i, dummyint;
	for (i = 0; i < maxarray - 1; i++) {
		dummyint = i + 1;
		fprintf(DestFile, "%s_%d\t", name, dummyint);
	}
	dummyint = maxarray;
	fprintf(DestFile, "%s_%d", name, dummyint);
}

// ---------------------------------------------------------------

/* !
 \brief         Writes the header of the Carbon Cycle File
 \param	  	    DestFile      File name
 \return        boolean true if no error occurs.
 */

bool WriteHeaderCARBON(FILE*DestFile) {
	fputs("Carbon flux [t_C1 ha-1 a-1]\n", DestFile);
	fputs("Time [a1]\tNet ecosystem exchange [t_C1 ha-1 a-1]\tGross primary production [t_C1 ha-1 a-1]\tTotal respiration [t_C1 ha-1 a-1]\tLiving biomass respiration [t_C1 ha-1 a-1]\tDeadwood respiration [t_C1 ha-1 a-1]\tSoil respiration of slow soil carbon stock [t_C1 ha-1 a-1]\tSoil respiration of fast soil carbon stock [t_C1 ha-1 a-1]\tCarbon flux to deadwood stock [t_C1 ha-1 a-1]\tCarbon flux to fast soil carbon stock [t_C1 ha-1 a-1]\tCarbon flux to slow soil carbon stock [t_C1 ha-1]\tCarbon amount of living biomass stock [t_C1 ha-1]\tCarbon amount of deadwood stock [t_C1 ha-1]\tCarbon amount of fast soil carbon stock [t_C1 ha-1]\tCarbon amount of slow soil crbon stock [t_C1 ha-1]\tActual evapotranspiration [mm1 a-1]\n"
		 , DestFile);
	fputs("Time\tNEE\tGPP\tR_total\tR_biomass\tR_DeadWood\tR_Soil_Slow\tR_Soil_Fast\tCflux_to_deadwood\tCflux_to_soil_fast\tCflux_to_soil_slow\tCPool_Biomass\tCPool_DeadWood\tCPool_Soil_fast\tCPool_Soil_slow\tAET\n"
		 , DestFile);
	if (EOF == fputs("", DestFile)) {
		cerr << "ERROR: file error \tfile: " << __FILE__ <<
			 "\tfunction: WriteHeaderCARBON\t line:" << __LINE__ <<
			 "\t Can't write to file." << endl;
		return false;
	}
	return true;
}

// ---------------------------------------------------------------

/* !
 \brief          Writes the header of the Carbon Cycle File per plot
 \param		    DestFile      File name
 \return         boolean true if no error occurs.
 */

bool WriteHeaderCARBONPlot(FILE*DestFile)

{
	fputs("Output for carbon submodule per Plot. Units in Carbon per Hectare.\n",
		 DestFile);
	fputs("Time [a1]\tHectare number [-]\tPlot number [-]\tGross primary production [t_C1 ha-1 a-1]\tTotal respiration [t_C1 ha-1 a-1]\tNet Ecosystem Exchange [t_C1 ha-1 a-1]\tLiving biomass respiration [t_C1 ha-1 a-1]\tDeadwood respiration [t_C1 ha-1 a-1]\tSoil respiration of slow soil carbon stock [t_C1 ha-1 a-1]\tSoil respiration of fast soil carbon stock [t_C1 ha-1 a-1]\tCarbon flux to deadwood stock [t_C1 ha-1 a-1]\tCarbon flux to fast soil carbon stock [t_C1 ha-1 a-1]\tCarbon flux to slow soil carbon stock [t_C1 ha-1]\tCarbon amount of living biomass stock [t_C1 ha-1]\tCarbon amount of deadwood stock [t_C1 ha-1]\tCarbon amount of fast soil carbon stock [t_C1 ha-1]\tCarbon amount of slow soil crbon stock [t_C1 ha-1]\tActual evapotranspiration [mm1 a-1]\n"
		 , DestFile);
	fputs("Time\tHectare\tPlot\tGPP\tR_total\tNEE\tR_biomass\tR_DeadWood\tR_Soil_Slow\tR_Soil_Fast\tCflux_to_deadwood\tCflux_to_soil_fast\tCflux_to_soil_slow\tCPool_Biomass\tCPool_DeadWood\tCPool_Soil_fast\tCPool_Soil_slow\tAET\n"
		 , DestFile);
	if (EOF == fputs("", DestFile)) {
		cerr << "ERROR: file error \tfile: " << __FILE__ <<
			 "\tfunction: WriteHeaderCARBONPlot\t line:" << __LINE__ <<
			 "\t Can't write to file." << endl;
		return false;
	}
	return true;
}

// ---------------------------------------------------------------

/* !
 \brief          Writes the header of the diameter files
 \param		    DestFile      File name
 \return         boolean true if no error occurs.
 */

bool WriteHeaderLUDWIG(FILE*DestFile) {
	fputs("The number of trees [ha-1] and the basal area [m2 ha-1] per stem diameter class [m] (in total, per PFT) for the whole simulated area (average over all hectare)."
		 , DestFile);
	fputs("\n", DestFile);
	fputs("Time [a1]\tDiameter class [m1]\tTotal number of trees [ha-1]\t",
		 DestFile);
	WritePFTHeader(DestFile, MAXGRP, "Number of Trees per PFT", "ha-1", "");

	fputs("\tTotal basal area [m2 ha-1]\t", DestFile);
	WritePFTHeader(DestFile, MAXGRP, "Basal area per PFT", "m2", " ha-1");
	fputs("\n", DestFile);

	fputs("Time\tDiameterClass\tNumberTreesTotal\t", DestFile);
	WritePFTHeaderShort(DestFile, MAXGRP, "NumberTreesPFT", "", "ha-1");

	fputs("\tBasalAreaTotal\t", DestFile);
	WritePFTHeaderShort(DestFile, MAXGRP, "BasalAreaPFT", "m2", " ha-1");
	fputs("\n", DestFile);

	if (EOF == fputs("", DestFile)) {
		cerr << "ERROR: file error \tfile: " << __FILE__ <<
			 "\tfunction: WriteHeaderLUDWIG\t line:" << __LINE__ <<
			 "\t Can't write to file." << endl;
		return false;
	}
	return true;
}

// ---------------------------------------------------------------

/* !
 \brief          Writes the header of the mortality rate per diameter class
 \param		    DestFile      File name
 \return         boolean true if no error occurs.
 */

bool WriteHeaderDIAMORT(FILE*DestFile) {
	fputs("The mortality rate [ha-1] per stem diameter class [m] (in total, per PFT) for the whole simulated area (average over all hectare)."
		 , DestFile);
	fputs("\n", DestFile);
	fputs("Time [a1]\tDiameter class [m1]\tMortality rate [ha-1]\t", DestFile);
	WritePFTHeader(DestFile, MAXGRP, "Mortality rate per PFT", "ha-1", "");
	fputs("\n", DestFile);

	fputs("Time\tDiameterClass\tMortalityRateTotal\t", DestFile);
	WritePFTHeaderShort(DestFile, MAXGRP, "MortalityRatePFT", "", "ha-1");
	fputs("\n", DestFile);

	if (EOF == fputs("", DestFile)) {
		cerr << "ERROR: file error \tfile: " << __FILE__ <<
			 "\tfunction: WriteHeaderDIAMORT\t line:" << __LINE__ <<
			 "\t Can't write to file." << endl;
		return false;
	}
	return true;
}

// ---------------------------------------------------------------

/* !
 \brief         Writes the header of various output files.
 \param	   	DestFile      File name
 \param			var           Variable type  (svar)
 \param			unit          Variable unit
 \param 			unit2         Variable unit2
 \return        boolean true if no error occurs.
 \details      SV, N, BT, BA, DIV, INGROW, SEEDS, SEEDTREE, SEEDLINGS
 */

bool WriteHeaderVAR(FILE*DestFile, const char*var, const char*unit,
	 const char*unit2) {

	stringstream ssvar;
	string svar;
	ssvar << var;
	ssvar >> svar;

	if (svar == "SV") {
		fprintf(DestFile,
			 "Stem volume [%s %s] (aboveground) of all trees (in total, per PFT, per Commercial group) on the whole simulated area (average of all hectares).\n",
			 unit, unit2);

		fprintf(DestFile, "Time [a1]\tTotal stem volume [%s%s]\t", unit, unit2);
		WritePFTHeader(DestFile, MAXGRP, "Total Stem Volume per PFT", unit,
			 unit2);
		fputs("\t", DestFile);

		fprintf(DestFile, "%s [%s%s] \t",
			 "Total stem volume of all non-commercial species", unit, unit2);
		fprintf(DestFile, "%s [%s%s]",
			 "Total stem volume of all commercial species", unit, unit2);

		fputs("\n", DestFile);

		fprintf(DestFile, "Time\tTotalStemVolume\t");
		WritePFTHeaderShort(DestFile, MAXGRP, "StemVolumePerPFT", unit, unit2);
		fputs("\t", DestFile);

		fprintf(DestFile, "%s\t", "StemVolumePerNonCom");
		fprintf(DestFile, "%s", "StemVolumePerCom");

		fputs("\n", DestFile);

	}
	else if (svar == "BV") {
		fprintf(DestFile,
			 "Bole volume [%s %s] of all trees (in total, per PFT, per Commercial group) on the whole simulated area (average of all hectares).\n",
			 unit, unit2);

		fprintf(DestFile, "Time [a1]\tTotal bole volume [%s%s]\t", unit, unit2);
		WritePFTHeader(DestFile, MAXGRP, "Bole Volume per PFT", unit, unit2);

		fputs("\t", DestFile);
		fprintf(DestFile, "%s [%s%s] \t",
			 "Total bole volume of all non-commercial species", unit, unit2);
		fprintf(DestFile, "%s [%s%s]",
			 "Total bole volume of all commercial species", unit, unit2);

		fputs("\n", DestFile);

		fprintf(DestFile, "Time\tTotalBoleVolume\t");
		WritePFTHeaderShort(DestFile, MAXGRP, "BoleVolumePerPFT", unit, unit2);

		fputs("\t", DestFile);
		fprintf(DestFile, "%s\t", "BoleVolumePerNonCom");
		fprintf(DestFile, "%s", "BoleVolumePerCom");

		fputs("\n", DestFile);
	}
	else if (svar == "N") {
		fprintf(DestFile,
			 "Stem number [%s%s] of all trees (in total, per PFT, per Commercial group) on the whole simulated area (average of all hectares).\n",
			 unit, unit2);

		fprintf(DestFile, "Time [a1]\tTotal stem number [%s%s]\t", unit, unit2);
		WritePFTHeader(DestFile, MAXGRP, "Total stem number per PFT", unit,
			 unit2);

		fputs("\t", DestFile);
		fprintf(DestFile, "%s [%s%s] \t",
			 "Total stem number of all non-commercial species", unit, unit2);
		fprintf(DestFile, "%s [%s%s]",
			 "Total stem number of all commercial species", unit, unit2);

		fputs("\n", DestFile);

		fprintf(DestFile, "Time\tTotalNumber\t");
		WritePFTHeaderShort(DestFile, MAXGRP, "NumberPerPFT", unit, unit2);

		fputs("\t", DestFile);
		fprintf(DestFile, "%s\t", "NumberPerNonCom");
		fprintf(DestFile, "%s", "NumberPerCom");

		fputs("\n", DestFile);

	}
	else if (svar == "BT") {
		fprintf(DestFile,
			 "Aboveground biomass [%s%s] of all trees (in total, per PFT, per Commercial group) on the whole simulated area (average of all hectares).\tMean wood density weighted by basal area [tODM m-3]\n",
			 unit, unit2);

		fprintf(DestFile, "Time [a1]\tTotal biomass [%s%s]\t", unit, unit2);
		WritePFTHeader(DestFile, MAXGRP, "Total biomass per PFT", unit, unit2);

		fputs("\t", DestFile);
		fprintf(DestFile, "%s [%s%s] \t",
			 "Total biomass of all non-commercial species", unit, unit2);
		fprintf(DestFile, "%s [%s%s]", "Total biomass of all commercial species",
			 unit, unit2);

		fputs("\t", DestFile);
		//fprintf(DestFile, "Mean Wood Density [tODM1 m-3]", unit, unit2);
		fprintf(DestFile, "Mean Wood Density [tODM1 m-3]");
		fputs("\n", DestFile);

		fprintf(DestFile, "Time\tTotalBiomass\t");
		WritePFTHeaderShort(DestFile, MAXGRP, "BiomassPerPFT", unit, unit2);

		fputs("\t", DestFile);
		fprintf(DestFile, "%s\t", "BiomassPerNonCom");
		fprintf(DestFile, "%s", "BiomassPerCom");

		fputs("\t", DestFile);
		fprintf(DestFile, "MeanWoodDensity");
		fputs("\n", DestFile);
	}
	else if (svar == "BA") {
		fprintf(DestFile,
			 "Basal area [%s %s] of all trees (in total, per PFT, per Commercial group) on the whole simulated area (average of all hectares).\n",
			 unit, unit2);

		fprintf(DestFile, "Time [a1]\tTotal basal area [%s%s]\t", unit, unit2);
		WritePFTHeader(DestFile, MAXGRP, "Total basal area per PFT", unit, unit2);

		fputs("\t", DestFile);
		fprintf(DestFile, "%s [%s%s] \t",
			 "Total basal area of all non-commercial species", unit, unit2);
		fprintf(DestFile, "%s [%s%s]",
			 "Total basal area of all commercial species", unit, unit2);

		fputs("\n", DestFile);

		fprintf(DestFile, "Time\tTotalBasalArea\t");
		WritePFTHeaderShort(DestFile, MAXGRP, "BasalAreaPerPFT", unit, unit2);

		fputs("\t", DestFile);
		fprintf(DestFile, "%s\t", "BasalAreaPerNonCom");
		fprintf(DestFile, "%s", "BasalAreaPerCom");

		fputs("\n", DestFile);

	}
	else if (svar == "DIV") {
		fprintf(DestFile,
			 "Fraction of aboveground biomass [-] per group relative to the total aboveground biomass of all trees on the whole simulated area (average of all hectares). Groups are plant functional types (PFT) and commercial groups (ComGRP).\n");

		fprintf(DestFile, "Time [a1]\tTotal [%s%s]\t", unit, unit2);
		WritePFTHeader(DestFile, MAXGRP,
			 "Total fraction of aboveground biomass of PFT", unit, unit2);

		fputs("\t", DestFile);
		fprintf(DestFile, "%s [%s%s] \t",
			 "Total fraction of aboveground biomass of all non-commercial species",
			 unit, unit2);
		fprintf(DestFile, "%s [%s%s]",
			 "Total fraction of aboveground biomass of all commercial species",
			 unit, unit2);

		fputs("\n", DestFile);

		fprintf(DestFile, "Time\tTotal\t");
		WritePFTHeaderShort(DestFile, MAXGRP, "FractionBiomassPerPFT",
			 unit, unit2);

		fputs("\t", DestFile);
		fprintf(DestFile, "%s\t", "FractionBiomassPerNonCom");
		fprintf(DestFile, "%s", "FractionBiomassPerCom");
		fputs("\n", DestFile);

	}
	else if (svar == "INGROW") {
		fprintf(DestFile,
			 "Fraction of trees (in total, per group), which exceed a threshold (Switch.Schwelle) within one time step of growth relative to the total number of all trees (above Switch.Schwelle) on the whole simulated area (average of all hectares). Groups are plant functional types (PFT).\n");

		fprintf(DestFile, "Time [a1]\tTotal [%s%s]\t", unit, unit2);
		WritePFTHeader(DestFile, MAXGRP, "Total fraction of ingrowth of PFT",
			 unit, unit2);
		fputs("\n", DestFile);

		fprintf(DestFile, "Time\tTotal\t");
		WritePFTHeaderShort(DestFile, MAXGRP, "FractionIngrowthPerPFT",
			 unit, unit2);
		fputs("\n", DestFile);

	}
	else if (svar == "SEEDS") {
		fprintf(DestFile,
			 "Number of seeds in the seed pool (seed pool size; in total, per group) on the whole simulated area (average of all hectares). Groups are plant functional types (PFT) and commercial groups (ComGRP).\n");

		fprintf(DestFile, "Time [a1]\tTotal seed pool size [%s %s]\t",
			 unit, unit2);
		WritePFTHeader(DestFile, MAXGRP, "Seed pool size per PFT", unit, unit2);

		fputs("\t", DestFile);
		fprintf(DestFile, "%s [%s%s] \t",
			 "Seed pool size of all non-commercial species", unit, unit2);
		fprintf(DestFile, "%s [%s%s]", "Seed pool size of all commercial species",
			 unit, unit2);

		fputs("\n", DestFile);

		fprintf(DestFile, "Time\tTotalSeedPool\t");
		WritePFTHeaderShort(DestFile, MAXGRP, "SeedPoolPerPFT", unit, unit2);

		fputs("\t", DestFile);
		fprintf(DestFile, "%s\t", "SeedPoolPerNonCom");
		fprintf(DestFile, "%s", "SeedPoolPerCom");

		fputs("\n", DestFile);
	}
	else if (svar == "SEEDRAIN") {
		fprintf(DestFile,
			 "Number of seeds thrown (seed rain size; in total, per group) on the whole simulated area (average of all hectares). Groups are plant functional types (PFT) and commercial groups (ComGRP).\n");

		fprintf(DestFile, "Time [a1]\tTotal seed rain value [%s %s]\t",
			 unit, unit2);
		WritePFTHeader(DestFile, MAXGRP, "Seed rain value per PFT", unit, unit2);

		fputs("\t", DestFile);
		fprintf(DestFile, "%s [%s%s] \t",
			 "Seed rain value of all non-commercial species", unit, unit2);
		fprintf(DestFile, "%s [%s%s]",
			 "Seed rain value of all commercial species", unit, unit2);

		fputs("\n", DestFile);

		fprintf(DestFile, "Time\tTotalSeedRain\t");
		WritePFTHeaderShort(DestFile, MAXGRP, "SeedRainPerPFT", unit, unit2);

		fputs("\t", DestFile);
		fprintf(DestFile, "%s\t", "SeedRainPerNonCom");
		fprintf(DestFile, "%s", "SeedRainPerCom");

		fputs("\n", DestFile);
	}
	else if (svar == "SEEDTREES") {
		fprintf(DestFile,
			 "Number of trees throwing seeds (mother trees; in total, per group) on the whole simulated area (average of all hectares). Groups are plant functional types (PFT) and commercial groups (ComGRP).\n");

		fprintf(DestFile, "Time [a1]\tTotal number of mother trees [%s%s]\t",
			 unit, unit2);
		WritePFTHeader(DestFile, MAXGRP, "Number of mother trees per PFT",
			 unit, unit2);

		fputs("\t", DestFile);
		fprintf(DestFile, "%s [%s%s] \t",
			 "Number of mother trees of all non-commercial species", unit, unit2);
		fprintf(DestFile, "%s [%s%s]",
			 "Number of mother trees of all commercial species", unit, unit2);

		fputs("\n", DestFile);

		fprintf(DestFile, "Time\tTotalMotherTrees\t");
		WritePFTHeaderShort(DestFile, MAXGRP, "MotherTreesPerPFT", unit, unit2);

		fputs("\t", DestFile);
		fprintf(DestFile, "%s\t", "MotherTreesPerNonCom");
		fprintf(DestFile, "%s", "MotherTreesPerCom");

		fputs("\n", DestFile);
	}
	else if (svar == "SEEDLINGS") {
		fprintf(DestFile,
			 "Number of established seedlings (in total, per group) on the whole simulated area (average of all hectares). Groups are plant functional types (PFT) and commercial groups (ComGRP).\n");

		fprintf(DestFile, "Time [a1]\tTotal number of seedlings [%s%s]\t",
			 unit, unit2);
		WritePFTHeader(DestFile, MAXGRP, "Number of seedlings per PFT",
			 unit, unit2);
		fputs("\n", DestFile);

		fprintf(DestFile, "Time\tTotalSeedlings\t");
		WritePFTHeaderShort(DestFile, MAXGRP, "SeedlingsPerPFT", unit, unit2);
		fputs("\n", DestFile);
	}
	else if (svar == "RESTARTPLOT") {
		fprintf(DestFile, "Plot aggregated stocks for simulation restart.\n");

		fprintf(DestFile,
			 "Hectare ID [-]\tPlot ID [-]\tLand Code [-]\tCarbon amount of deadwood stock [t_C1 plotarea-1]\tCarbon amount of fast soil carbon stock [t_C1 plotarea-1]\tCarbon amount of slow soil carbon stock [t_C1 plotarea-1]\t");

		WritePFTHeader(DestFile, MAXGRP, "Total seeds of non-commercial PFT",
			 unit, unit2);
		fputs("\t", DestFile);
		WritePFTHeader(DestFile, MAXGRP, "Total seeds of commercial PFT",
			 unit, unit2);
		fputs("\t", DestFile);
		fputs("\n", DestFile);

		fprintf(DestFile,
			 "Hec\tPlot\tLandCode\tCPool_DeadWood\tCPool_Soil_fast\tCPool_Soil_slow\tBiomassMortality\t");
		WritePFTHeaderShort(DestFile, MAXGRP, "SeedPool_NonCom_PFT", unit, unit2);
		fputs("\t", DestFile);
		WritePFTHeaderShort(DestFile, MAXGRP, "SeedPool_Com_PFT", unit, unit2);
		fputs("\t", DestFile);
		fputs("\n", DestFile);
	}

	else if (svar == "MORTPFTTH") {
		fprintf(DestFile,
			 "Mortality rates per PFT in every time step with given threshold \n");

		fprintf(DestFile,
			 "Time [a1]\tTotal number of dead trees [ha-1]\tTotal rate of dead trees [%%]\t");
		WritePFTHeader(DestFile, MAXGRP, "Number of dead trees per PFT",
			 unit, unit2);
		fputs("\t", DestFile);
		WritePFTHeader(DestFile, MAXGRP, "Rate of dead trees per PFT", unit, "%");
		fputs("\n", DestFile);

		fprintf(DestFile, "Time\tTotalDeadTrees\tTotalRateDeadTrees\t");
		WritePFTHeaderShort(DestFile, MAXGRP, "DeadTreesPerPFT", unit, unit2);
		fputs("\t", DestFile);
		WritePFTHeaderShort(DestFile, MAXGRP, "RateDeadTreesPerPFT", unit, unit2);
		fputs("\n", DestFile);
	}

	else if (svar == "MORTPFT") {
		fprintf(DestFile, "Mortality rates per PFT in every time step\n");

		fprintf(DestFile,
			 "Time [a1]\tTotal number of dead trees [ha-1]\tTotal rate of dead trees [%%]\t");
		WritePFTHeader(DestFile, MAXGRP, "Number of dead trees per PFT",
			 unit, unit2);
		fputs("\t", DestFile);
		WritePFTHeader(DestFile, MAXGRP, "Rate of dead trees per PFT", unit, "%");
		fputs("\n", DestFile);

		fprintf(DestFile, "Time\tTotalDeadTrees\tTotalRateDeadTrees\t");
		WritePFTHeaderShort(DestFile, MAXGRP, "DeadTreesPerPFT", unit, unit2);
		fputs("\t", DestFile);
		WritePFTHeaderShort(DestFile, MAXGRP, "RateDeadTreesPerPFT", unit, unit2);
		fputs("\n", DestFile);
	}

	if (EOF == fputs("", DestFile)) {
		cerr << "ERROR: file error \tfile: " << __FILE__ <<
			 "\tfunction: WriteHeaderVAR\t line:" << __LINE__ <<
			 "\t Can't write to file." << endl;
		return false;
	}
	return true;
}

// ---------------------------------------------------------------

/* !
 \brief          Writes header lines that describe file content
 \param				void
 \return         boolean true if no error occurs.
 \details			One detailed line including units and one short line with abbreviations
 */

bool RESULT::WriteHeaderLines(void) {
	char S[1024];

	//strcpy(S, FileName);

	if (myResultFileSwitch.res) {

		// 1. Line
		fputs("Full simulation data of each tree.\n", File);
		// 2. Line
		fputs("Time [a1]\tGroup [-]\tSpecies Code [-]\tTotal number [-]\tAboveground biomass [t_ODM1 ha-1]\tStem diameter [m1]\tHeight [m1]\tStem volume [m3 ha-1]\tCumulative LAI above tree [m2 m-2]\t"
			 , File);
		fputs("Irradiance on top of tree [mumol_photons1 m-2 s-1]\tRelative irradiance on top of tree [%]\tGross productivity [t_ODM1 ha-1]\tBiomass increment [t_ODM1 ha-1]\tDiameter increment [m1]\tOvertopping basal area [m2 ha-1]\tAGE [a1]\tPlot number [-]\tHectar number [-]\tabsolute x Position [m]\tabsolute y Position [m]\t"
			 , File);
		fputs("Crown length part of total height [-]\tTree LAI [-]\tTree Crown Diameter[m1]\tTree Respiration[t_ODM1 ha-1 a-1]\tTree Maintance Respiration[t_ODM1 ha-1 a-1]\tTree Growth Respiration[t_ODM1 ha-1 a-1]\tIdentification Number[UUID]\tCrown Radius[m]\tCommercial group (logging)\tPotential crop tree (thinning)\tLiana\tIs Liana attached to a tree"
			 , File);

		fputs("\n", File);
		// 3.Line
		fputs("Time\tGrp\tSpecies\tN\tBT\tD\tH\tSV\tLAITREE\tIR\tIR_rel\tPB\tBInc\t"
			 , File);
		fputs("DInc\tOBA\tAGE\tPlot\tHec\tX\tY\tCLP\tLAI\tCD\tR\tR_Main\tR_Growth\tID\tCR\tComGrp\tPCT\tLiana\tLianaAttachedTree"
			 , File);

		if (myResultFileSwitch.branch) {
#ifdef underconstruction
			Branching branches;
			fputs(branches.DoHeader().c_str(), File);
#endif
		}
		fputs("\n", File);

		if (EOF == fputs("", File)) {
			sprintf(S, "### 182 ERROR : Can't write to '%s' ", FileName);
			perror(S);
			goto E_EXIT;
		}
	}

	if (myResultFileSwitch.res_th) {

		// 1. Line
		fprintf(ResThFile,
			 "Full simulation data of each tree with DBH above threshold %.3f m.\n",
			 Switch.Schwelle);
		// 2. Line
		fputs("Time [a1]\tGroup [-]\tSpecies Code [-]\tTotal number [-]\tAboveground biomass [t_ODM1 ha-1]\tStem diameter [m1]\tHeight [m1]\tStem volume [m3 ha-1]\tCumulative LAI above tree [m2 m-2]\t"
			 , ResThFile);
		fputs("Irradiance on top of tree [mumol_photons1 m-2 s-1]\tRelative irradiance on top of tree [%%]\tGross productivity [t_ODM1 ha-1]\tBiomass increment [t_ODM1 ha-1]\tDiameter increment [m1]\tOvertopping basal area [m2 ha-1]\tAGE [a1]\tPlot number [-]\tHectar number [-]\tabsolute x Position [m]\tabsolute y Position [m]\t"
			 , ResThFile);
		fputs("Crown length part of total height [-]\tTree LAI [-]\tTree Crown Diameter[m1]\tTree Respiration[t_ODM1 ha-1 a-1]\tTree Maintance Respiration[t_ODM1 ha-1 a-1]\tTree Growth Respiration[t_ODM1 ha-1 a-1]\tIdentification Number[UUID]\tCrown Radius[m]\tCommercial group (logging)\tPotential crop tree (thinning)"
			 , ResThFile);

		fputs("\n", ResThFile);
		// 3.Line
		fputs("Time\tGrp\tSpecies\tN\tBT\tD\tH\tSV\tLAITREE\tIR\tIR_rel\tPB\tBInc\t"
			 , ResThFile);
		fputs("DInc\tOBA\tAGE\tPlot\tHec\tX\tY\tCLP\tLAI\tCD\tR\tR_Main\tR_Growth\tID\tCR\tComGrp\tPCT"
			 , ResThFile);

		if (myResultFileSwitch.branch) {
#ifdef underconstruction
			Branching branches;
			fputs(branches.DoHeader().c_str(), ResThFile);
#endif
		}
		fputs("\n", ResThFile);

		if (EOF == fputs("", ResThFile)) {
			sprintf(S, "### 182 ERROR : Can't write to '%s' ", RESTHFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	if (myResultFileSwitch.cohort) {

		// 1. Line
		fputs("Full simulation data of each cohort of trees.\n", CohortFile);
		// 2. Line
		fputs("Time [a1]\tGroup [-]\tSpecies Code [-]\tTotal number [-]\tAboveground biomass [t_ODM1 ha-1]\tStem diameter [m1]\tHeight [m1]\tStem volume [m3 ha-1]\tCumulative LAI above tree [m2 m-2]\t"
			 , CohortFile);
		fputs("Irradiance on top of tree [mumol_photons1 m-2 s-1]\tRelative irradiance on top of tree [%%]\tGross productivity [t_ODM1 ha-1]\tBiomass increment [t_ODM1 ha-1]\tDiameter increment [m1]\tOvertopping basal area [m2 ha-1]\tAGE [a1]\tPlot number [-]\tHectar number [m]\t"
			 , CohortFile);
		fputs("Crown length part of total height [-]\tTree LAI [-]\tTree Crown Diameter[m1]\tTree Respiration[t_ODM1 ha-1 a-1]\tTree Maintance Respiration[t_ODM1 ha-1 a-1]\tTree Growth Respiration[t_ODM1 ha-1 a-1]\tCommercial group (logging)\tPotential crop tree (thinning)"
			 , CohortFile);

		fputs("\n", CohortFile);
		// 3.Line
		fputs("Time\tGrp\tSpecies\tN\tBT\tD\tH\tSV\tLAITREE\tIR\tIR_rel\tPB\tBInc\t"
			 , CohortFile);
		fputs("DInc\tOBA\tAGE\tPlot\tHec\tCLP\tLAI\tCD\tR\tR_Main\tR_Growth\tComGrp\tPCT"
			 , CohortFile);

		fputs("\n", CohortFile);

		if (EOF == fputs("", CohortFile)) {
			sprintf(S, "### 182 ERROR : Can't write to '%s' ", COHORTFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	if (myResultFileSwitch.cohort_th) {

		// 1. Line
		fprintf(CohortThFile,
			 "Full simulation data of each cohort of trees with DBH above threshold %.3f m.\n",
			 Switch.Schwelle);
		// 2. Line
		fputs("Time [a1]\tGroup [-]\tSpecies Code [-]\tTotal number [-]\tAboveground biomass [t_ODM1 ha-1]\tStem diameter [m1]\tHeight [m1]\tStem volume [m3 ha-1]\tCumulative LAI above tree [m2 m-2]\t"
			 , CohortThFile);
		fputs("Irradiance on top of tree [mumol_photons1 m-2 s-1]\tRelative irradiance on top of tree [%%]\tGross productivity [t_ODM1 ha-1]\tBiomass increment [t_ODM1 ha-1]\tDiameter increment [m1]\tOvertopping basal area [m2 ha-1]\tAGE [a1]\tPlot number [-]\tHectar number [m]\t"
			 , CohortThFile);
		fputs("Crown length part of total height [-]\tTree LAI [-]\tTree Crown Diameter[m1]\tTree Respiration[t_ODM1 ha-1 a-1]\tTree Maintance Respiration[t_ODM1 ha-1 a-1]\tTree Growth Respiration[t_ODM1 ha-1 a-1]\tCommercial group (logging)\tPotential crop tree (thinning)"
			 , CohortThFile);

		fputs("\n", CohortThFile);
		// 3.Line
		fputs("Time\tGrp\tSpecies\tN\tBT\tD\tH\tSV\tLAITREE\tIR\tIR_rel\tPB\tBInc\t"
			 , CohortThFile);
		fputs("DInc\tOBA\tAGE\tPlot\tHec\tCLP\tLAI\tCD\tR\tR_Main\tR_Growth\tComGrp\tPCT"
			 , CohortThFile);

		fputs("\n", CohortThFile);

		if (EOF == fputs("", CohortThFile)) {
			sprintf(S, "### 182 ERROR : Can't write to '%s' ", COHORTTHFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	if (myResultFileSwitch.restart) {

		// 1. Line
		fputs("Information of each tree at last simulation year to continue simulation.\n"
			 , RestartFile);
		// 2. Line
		fputs("Group [-]\tAbsolute x position [m]\tAbsolute y position [m]\tStem diameter [m]"
			 , RestartFile);
		fputs("\n", RestartFile);
		// 3.Line
		fputs("Grp\tX\tY\tD\n", RestartFile);

		if (EOF == fputs("", RestartFile)) {
			sprintf(S, "### 182 ERROR : Can't write to '%s' ", RESTARTFileName);
			perror(S);
			goto E_EXIT;
		}
	}
	if (myResultFileSwitch.restartplot) {
		if (!WriteHeaderVAR(RestartPlotFile, "RESTARTPLOT", "", "-"))
			goto E_EXIT;
	}

	if (myResultFileSwitch.thin) {

		// 1. Line
		fputs("Removed trees after thinning events.\n", ThinFile);
		// 2. Line
		fputs("Time [a1]\tGroup [-]\tSpecies Code [-]\tTotal number [-]\tAboveground biomass [t_ODM1 ha-1]\tStem diameter [m1]\tHeight [m1]\tStem volume [m3 ha-1]\tCumulative LAI above tree [m2 m-2]\t"
			 , ThinFile);
		fputs("Irradiance on top of tree [mumol_photons1 m-2 s-1]\tRelative irradiance on top of tree [%]\tGross productivity [t_ODM1 ha-1]\tBiomass increment [t_ODM1 ha-1]\tDiameter increment [m1]\tOvertopping basal area [m2 ha-1]\tAGE [a1]\tPlot number [-]\tHectar number [-]\tabsolute x Position [m]\tabsolute y Position [m]\t"
			 , ThinFile);
		fputs("Crown length part of total height [-]\tTree LAI [-]\tTree Crown Diameter[m1]\tTree Respiration[t_ODM1 ha-1 a-1]\tTree Maintance Respiration[t_ODM1 ha-1 a-1]\tTree Growth Respiration[t_ODM1 ha-1 a-1]\tIdentification Number[UUID]\tCrown Radius[m]\tCommercial group (logging)\tPotential crop tree (thinning)"
			 , ThinFile);

		fputs("\n", ThinFile);
		// 3.Line
		fputs("Time\tGrp\tSpecies\tN\tBT\tD\tH\tSV\tLAITREE\tIR\tIR_rel\tPB\tBInc\t"
			 , ThinFile);
		fputs("DInc\tOBA\tAGE\tPlot\tHec\tX\tY\tCLP\tLAI\tCD\tR\tR_Main\tR_Growth\tID\tCR\tComGrp\tPCT\n"
			 , ThinFile);

		if (EOF == fputs("", ThinFile)) {
			sprintf(S, "### 182 ERROR : Can't write to '%s' ", THINFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	if (myResultFileSwitch.grass) {

		fputs("Full simulation data of each cohort.\n", GRASSFile);
		fputs("Time [a1]\tPlot number [-]\tHectar number [-]\t", GRASSFile);
		fputs("Group [-]\tAGE [a1]\tTotal number [-]\tStatus [-]\tAboveground shoot biomass [t_ODM1 ha-1]\t"
			 , GRASSFile);
		fputs("Aboveground green shoot biomass [t_ODM1 ha-1]\tAboveground senescent shoot biomass [t_ODM1 ha-1]\t"
			 , GRASSFile);
		fputs("Fraction of green shoot biomass [-]\tFraction of senescent shoot biomass [-]\t"
			 , GRASSFile);
		fputs("Tiller width [m1]\tTiller height [m1]\tGround area of covering cylinder [m2]\t"
			 , GRASSFile);
		fputs("Leaf area index [-]\tGreen leaf area index [-]\tSenescent leaf area index [-]\t"
			 , GRASSFile);
		fputs("Belowground root biomass [t_ODM1 ha-1]\tRooting depth [m1]\tTotal root length [m]\t"
			 , GRASSFile);
		fputs("Irradiance on top of tiller [mumol_photons1 m-2 s-1]\tGross productivity [t_ODM1 ha-1]\t"
			 , GRASSFile);
		fputs("Total respiration [t_ODM1 ha-1]\tMaintenance respiration [t_ODM1 ha-1]\tGrowth respiration [t_ODM1 ha-1]\tNet productivity [t_ODM1 ha-1]\t"
			 , GRASSFile);
		fputs("Shoot biomass increment [t_ODM1 ha-1]\tRoot biomass increment [t_ODM1 ha-1]\tReproduction biomass increment [t_ODM1 ha-1]\t"
			 , GRASSFile);
		fputs("Tiller width increment [m1]\tTiller height increment [m1]\tShading factor [-]\t"
			 , GRASSFile);
		fputs("Water limitation factor [-]\tNitrogen limitation factor [-]\tSpace competition factor[-]\t"
			 , GRASSFile);
		fputs("Water demand [mm1]\tWater uptake [mm1 d-1]\tNitrogen demand [kg ha-1]\tNitrogen uptake [kg1 ha-1]\t"
			 , GRASSFile);
		fputs("Shoot nitrogen content [kg1]\tBiomass cost for symbiosis [t_ODM1]",
			 GRASSFile);
		fputs("\n", GRASSFile);

		fputs("Time\tPlot\tHec\tGrp\tAGE\tN\tStatus\tBs\tBsGreen\tBsBrown\tFracBsGreen\tFracBsBrown\t"
			 , GRASSFile);
		fputs("D\tH\tCover\tLAI\tLAIGreen\tLAIBrown\tBr\tRootDepth\tRootLength\t",
			 GRASSFile);
		fputs("IR\tGPP\tR\tRmain\tRgrowth\tNPP\tBsInc\tBrInc\tBRepInc\tDInc\tHInc\t"
			 , GRASSFile);
		fputs("RS\tRW\tRN\tRC", GRASSFile);
		fputs("\tWdemand\tWuptake\tNdemand\tNuptake\tNshoot\tBCostSymb",
			 GRASSFile);
		fputs("\n", GRASSFile);

		if (EOF == fputs("", GRASSFile)) {
			sprintf(S, "### 182 ERROR : Can't write to '%s' ", GRASSFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// aggregated grassmind result file (used for calibration)
	if (myResultFileSwitch.grasscalib) {

		fputs("Aggregated reduced simulation data for calibration of GRASSMIND.\n"
			 , GRASSCALIBFile);
		fputs("Time [a1]\tHectar number [-]\tPlot number [-]\t", GRASSCALIBFile);
		fputs("Group [-]\tTotal number [-]\tAboveground biomass [t_ODM1 m-2]\t",
			 GRASSCALIBFile);
		fputs("Aboveground green biomass [t_ODM1 m-2]\t", GRASSCALIBFile);
		fputs("Maximum height [m1]\tMean height [m1]\tCoverage [m2]\t",
			 GRASSCALIBFile);
		fputs("Leaf area index [-]\tGreen leaf area index [-]\t", GRASSCALIBFile);
		fputs("Belowground biomass [t_ODM1 m-2]\tMean shoot biomass [t_ODM1]\t",
			 GRASSCALIBFile);
		fputs("Mean rooting depth [m1]\tMean leaf N content per shoot leaf area [kg_N1 m-2]\t"
			 , GRASSCALIBFile);
		fputs("Weighted mean height [t_ODM1 m1]\tBiomass density [t_ODM1 m-3]\t",
			 GRASSCALIBFile);
		fputs("Aboveground biomass above harvest cut [t_ODM1 m-2]",
			 GRASSCALIBFile);
		fputs("\n", GRASSCALIBFile);

		fputs("Time\tHec\tPlot\tGrp\tN\tBs\tBsGreen\t", GRASSCALIBFile);
		fputs("Hmax\tHmean\tCover\tLAI\tLAIGreen\tBr\tBsmean\tRootdepmean\tLeafNmean\tWMHC\tBiomassDens"
			 , GRASSCALIBFile);
		fputs("BiomassDens\tBscalib", GRASSCALIBFile);
		fputs("\n", GRASSCALIBFile);

		if (EOF == fputs("", GRASSCALIBFile)) {
			sprintf(S, "### 182 ERROR : Can't write to '%s' ", GRASSCALIBFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	if (myResultFileSwitch.grassplot) {

		fputs("Aggregated reduced simulation data for calibration of GRASSMIND.\n"
			 , GRASSPLOTFile);
		fputs("Time [a1]\tHectar number [-]\tPlot number [-]\t", GRASSPLOTFile);
		fputs("Group [-]\tTotal number [-]\tAboveground biomass [t_ODM1 m-2]\t",
			 GRASSPLOTFile);
		fputs("Aboveground green biomass [t_ODM1 m-2]\t", GRASSPLOTFile);
		fputs("Maximum height [m1]\tMean height [m1]\tCoverage [m2]\t",
			 GRASSPLOTFile);
		fputs("Leaf area index [-]\tGreen leaf area index [-]\t", GRASSPLOTFile);
		fputs("Belowground biomass [t_ODM1 m-2]\tMean shoot biomass [t_ODM1]\t",
			 GRASSPLOTFile);
		fputs("Mean rooting depth [m1]\tMean leaf N content per shoot leaf area [kg_N1 m-2]\t"
			 , GRASSPLOTFile);
		fputs("Weighted mean height [t_ODM1 m1]\tBiomass density [t_ODM1 m-3]\t",
			 GRASSPLOTFile);
		fputs("Aboveground biomass above harvest cut [t_ODM1 m-2]",
			 GRASSPLOTFile);
		fputs("\n", GRASSPLOTFile);

		fputs("Time\tHec\tPlot\tGrp\tN\tBs\tBsGreen\t", GRASSPLOTFile);
		fputs("Hmax\tHmean\tCover\tLAI\tLAIGreen\tBr\tBsmean\tRootdepmean\tLeafNmean\tWMHC\tBiomassDens"
			 , GRASSPLOTFile);
		fputs("BiomassDens\tBscalib", GRASSPLOTFile);
		fputs("\n", GRASSPLOTFile);

		if (EOF == fputs("", GRASSPLOTFile)) {
			sprintf(S, "### 182 ERROR : Can't write to '%s' ", GRASSPLOTFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	if (myResultFileSwitch.grass_mow) {

		fputs("Aggregated reduced simulation data for calibration of GRASSMIND.\n"
			 , GRASS_MOWFile);
		fputs("Time [a1]\tHectar number [-]\tPlot number [-]\t", GRASS_MOWFile);
		fputs("Group [-]\tTotal number [-]\tAboveground biomass [t_ODM1 m-2]\t",
			 GRASS_MOWFile);
		fputs("Aboveground green biomass [t_ODM1 m-2]\t", GRASS_MOWFile);
		fputs("Maximum height [m1]\tMean height [m1]\tCoverage [m2]\t",
			 GRASS_MOWFile);
		fputs("Leaf area index [-]\tGreen leaf area index [-]\t", GRASS_MOWFile);
		fputs("Belowground biomass [t_ODM1 ha-1]\tMean shoot biomass [t_ODM1]\t",
			 GRASS_MOWFile);
		fputs("Mean rooting depth [m1]\tMean leaf N content per shoot leaf area [kg_N1 m-2]\t"
			 , GRASS_MOWFile);
		fputs("Weighted mean height [t_ODM1 m1]\tBiomass density [t_ODM1 m-3]\tGreen mown biomass [t_ODM1]\tBrown mown biomass [t_ODM1]"
			 , GRASS_MOWFile);
		fputs("\n", GRASS_MOWFile);

		fputs("Time\tHec\tPlot\tGrp\tN\tBs\tBsGreen\t", GRASS_MOWFile);
		fputs("Hmax\tHmean\tCover\tLAI\tLAIGreen\tBr\tBsmean\tRootdepmean\tLeafNmean\tWMHC\tBiomassDens\tMBgreen\tMBbrown"
			 , GRASS_MOWFile);
		fputs("\n", GRASS_MOWFile);

		if (EOF == fputs("", GRASS_MOWFile)) {
			sprintf(S, "### 182 ERROR : Can't write to '%s' ", GRASS_MOWFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .log
	// logging data
	if (myResultFileSwitch.log) {
		fputs("Logging volume [m3 ha-1]\n", LogFile);
		fputs("Time [a1]\tLogged stand volume [m3 ha-1]\tLogged commercial bole volume [m3 ha-1]\tLogged number [ha-1]\tDamaged stand volume [m3 ha-1]\tCumulative logged stand volume [m3 ha-1]\tCumulative logged number of trees [ha-1]\tCumulative damaged stand volume [m3 ha-1]\tdamaged trees of total tree numbers [percent]\tdamaged trees of total tree numbers in diameterclass 1 [percent]\tdamaged trees of total tree numbers in diameterclass 2 [percent]\tdamaged trees of total tree numbers in diameterclass 3 [percent]\tdamaged trees of total tree numbers in diameterclass 4 [percent]\tLogged basal area [m2 ha-1]\tLiving BA on stand prior to harvest [percent]\tDamaged BA on stand prior to harvest [percent]\tLogged BA on stand prior to harvest [percent]\tPercent of canopy cover after gap definition [percent]\tStem volume before logging above mhd [m3 ha-1]\tStem number before logging above mhd [ha-1]\tAverage diameter of logged trees [m1]\trestSV [m3 ha-1]\tRemaining standing logable volume [m3 ha-1]\n"
			 , LogFile);
		fputs("T\tYieldSV\tCommercialBoleVolume\tYieldN\tDamSV\tSumYSV\tSumYN\tSumDamSV\tDamNall\tDamN1\tDamN2\tDamN3\tDamN4\tYieldBA\tgoodBA\tdamBA\tyieldBA\tCover\tallSV\tallN\tav_d\trestSV\tpriorSV\n"
			 , LogFile);
		if (EOF == fputs("", LogFile)) {
			sprintf(S, "### 182g ERROR: Can't write to '%s' ", LogFileName);
			perror(S);
			goto E_EXIT;
		}
	}
	if (myResultFileSwitch.log_ha) {
		fputs("Logging volume [m3 ha-1]\n", LogHAFile);
		fputs("Time [a1]\t Hectar[]\t Logged stand volume [m3]\tLogged commercial bole volume [m3]\tLogged number []\tDamaged stand volume [m3]\n"
			 , LogHAFile);
		fputs("T\tHa\tYieldSV\tCommercialBoleVolume\tYieldN\tDamSV\n"
			 , LogHAFile);
		if (EOF == fputs("", LogHAFile)) {
			sprintf(S, "### 182g ERROR: Can't write to '%s' ", LogHAFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .ats
	// data on crown closure
	if (myResultFileSwitch.ats) {
		if (!WriteHeaderATS(ATSFile))
			goto E_EXIT;
	}

	// .lai
	// data on lai and LAD
	if (myResultFileSwitch.lai) {
		if (!WriteHeaderLAI(LAIFile))
			goto E_EXIT;
	}

	// .dia
	// data on stem-diameter distribution
	if (myResultFileSwitch.dia) {
		if (!WriteHeaderLUDWIG(DIAFile))
			goto E_EXIT;
	}

	if (myResultFileSwitch.sv) {
		if (!WriteHeaderVAR(DYN2File, "SV", "m3", " ha-1"))
			goto E_EXIT;
	}
	if (myResultFileSwitch.bv) {
		if (!WriteHeaderVAR(BVFile, "BV", "m3", " ha-1"))
			goto E_EXIT;
	}
	if (myResultFileSwitch.n) {
		if (!WriteHeaderVAR(DYN3File, "N", "", "ha-1"))
			goto E_EXIT;
	}

	if (myResultFileSwitch.bt) {
		if (!WriteHeaderVAR(DYN4File, "BT", "t_ODM1", " ha-1"))
			goto E_EXIT;
	}
	if (myResultFileSwitch.ba) {
		if (!WriteHeaderVAR(DYN5File, "BA", "m2", " ha-1"))
			goto E_EXIT;
	}
	if (myResultFileSwitch.div) {
		if (!WriteHeaderVAR(DYN6File, "DIV", "-", ""))
			goto E_EXIT;
	}
	if (myResultFileSwitch.in) {
		if (!WriteHeaderVAR(INFile, "INGROW", "", "ha-1 a-1"))
			goto E_EXIT;
	}
	if (myResultFileSwitch.seed) {
		if (!WriteHeaderVAR(SEEDFile, "SEEDS", "", "ha-1"))
			goto E_EXIT;
	}
	if (myResultFileSwitch.seed_rain) {
		if (!WriteHeaderVAR(SEEDRAINFile, "SEEDRAIN", "", "ha-1"))
			goto E_EXIT;
	}
	if (myResultFileSwitch.mort_pft) {
		if (!WriteHeaderVAR(MORTPFTFile, "MORTPFT", "", "ha-1"))
			goto E_EXIT;
	}
	if (myResultFileSwitch.stree) {
		if (!WriteHeaderVAR(SEEDTREEFile, "SEEDTREES", "", "ha-1"))
			goto E_EXIT;
	}
	if (myResultFileSwitch.seedling) {
		if (!WriteHeaderVAR(SEEDLINGFile, "SEEDLINGS", "", "ha-1 a-1"))
			goto E_EXIT;
	}

	// .th
	// dynamic output each output step for d > THRESHOLD
	if (myResultFileSwitch.sv_th) {
		if (!WriteHeaderVAR(DYNTH2File, "SV", "m3", " ha-1"))
			goto E_EXIT;
	}
	if (myResultFileSwitch.bv_th) {
		if (!WriteHeaderVAR(BVTHFile, "BV", "m3", " ha-1"))
			goto E_EXIT;
	}
	if (myResultFileSwitch.n_th) {
		if (!WriteHeaderVAR(DYNTH3File, "N", "", "ha-1"))
			goto E_EXIT;
	}
	if (myResultFileSwitch.bt_th) {
		if (!WriteHeaderVAR(DYNTH4File, "BT", "t_ODM1", " ha-1"))
			goto E_EXIT;
	}
	if (myResultFileSwitch.ba_th) {
		if (!WriteHeaderVAR(DYNTH5File, "BA", "m2", " ha-1"))
			goto E_EXIT;
	}
	if (myResultFileSwitch.div_th) {
		if (!WriteHeaderVAR(DYNTH6File, "DIV", "-", ""))
			goto E_EXIT;
	}
	if (myResultFileSwitch.mort_pft_th) {
		if (!WriteHeaderVAR(MORTPFTTHFile, "MORTPFTTH", "", "ha-1"))
			goto E_EXIT;
	}
	if (myResultFileSwitch.mort_pft_dia) {
		if (!WriteHeaderDIAMORT(MORTPFTDIAFile))
			goto E_EXIT;
	}

	// .bmpl
	// above and belowground biomass per plot
	if (myResultFileSwitch.bmpl) {
		fputs("Above and belowground biomass per plot \n", BMPLFile);
		fputs("Time [a1]\tPlotI [-]\tx-plot number [-]\ty-plot number [-]\tx_low [-]\tx_high [-]\ty_low [-]\ty_high [-]\tForestType [-]\tBiomass [-]\tTimeSinceSlide [-]\tSlideSize [-]\tPlotSlope [-]\tSlideProb [-]\tPlotElevation [-]\n"
			 , BMPLFile);
		fputs("T\tPlotI\tx-PlotNo\ty-PlotNo\tx_low\tx_high\ty_low\ty_high\tForestType\tBiomass\tTimeSinceSlide\tSlideSize\tPlotSlope\tSlideProb\tPlotElevation\n"
			 , BMPLFile);
		if (EOF == fputs("", BMPLFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", BMPLFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .landslide
	if (myResultFileSwitch.landslide) {
		fputs("Above and belowground biomass per plot \n", LANDSLIDEFile);
		fputs("Time [a1]\tPlotI [-]\tx-PlotNo [-]\ty-PlotNo [-]\tx_low [-]\tx_high [-]\ty_low [-]\ty_high [-]\tForestType [-]\tBiomass [-]\tTimeSinceSlide [-]\tSlideSize [-]\tPlotSlope [-]\tSlideProb [-]\tPlotElevation [-]\n"
			 , LANDSLIDEFile);
		fputs("T\tPlotI\tx-PlotNo\ty-PlotNo\tx_low\tx_high\ty_low\ty_high\tForestType\tBiomass\tTimeSinceSlide\tSlideSize\tPlotSlope\tSlideProb\tPlotElevation\n"
			 , LANDSLIDEFile);

		if (EOF == fputs("", LANDSLIDEFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", LANDSLIDEFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .plotbmdyn
	// plot biomass dynamics: biomass, biomassincrease, biomass loss due to mortality/landslides
	if (myResultFileSwitch.plotbmdyn) {
		fputs("Aboveground biomass dynamics per plot \n", PLOTBMDYNFile);
		fputs("Time [a1]\tPlotI [-]\tx-PlotNo [-]\ty-PlotNo [-]\tx_low [-]\tx_high [-]\ty_low [-]\ty_high [-]\tTimeSinceSlide [-]\tBiomass [-]\tBiomass_loss [-]\tBiomass_increase [-]\tBiomass_newTrees [-]\n"
			 , PLOTBMDYNFile);
		fputs("T\tPlotI\tx-PlotNo\ty-PlotNo\tx_low\tx_high\ty_low\ty_high\tTimeSinceSlide\tBiomass\tBiomass_loss\tBiomass_increase\tBiomass_newTrees\n"
			 , PLOTBMDYNFile);
		if (EOF == fputs("", PLOTBMDYNFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", PLOTBMDYNFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .lai_mean
	if (myResultFileSwitch.lai_mean) {
		fputs("Leaf area index [m2 m-2] of whole simulation area over time (average)\n"
			 , LAI_meanFile);
		fputs("Time [a1]\tLeaf area index [m2 m-2]\n", LAI_meanFile);
		fputs("Time\tLAI\n", LAI_meanFile);
		if (EOF == fputs("", LAI_meanFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", LAI_meanFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .lai_plot
	if (myResultFileSwitch.lai_plot) {
		fputs("Mean LAI for all single plots over time \n", LAI_plotFile);
		fputs("Time [a1]\tPlotI [-] \tx-Plot number [-]\ty-Plot number [-]\tLAI [m2 m-2]\n"
			 , LAI_plotFile);
		fputs("Time\tPlotI\tx-PlotNo\ty-PlotNo\tLAI\n", LAI_plotFile);
		if (EOF == fputs("", LAI_plotFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", LAI_plotFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .lai_plot_layer
	if (myResultFileSwitch.lai_plot_heightlayer) {
		fputs("Cumulative LAI and LAD for each plot and each height layer over time \n"
			 , LAI_plot_heightFile);
		fputs("Time [a1]\tHectar number [-]\tPlot number\tHeight layer [m1]\tCumulative LAI [m2 m-2]\tLeaf area density [m2 m-3]\n"
			 , LAI_plot_heightFile);
		fputs("Time\tHecNo\tPlotNo\tHeightLayer\tLAI\tLAD\n",
			 LAI_plot_heightFile);
		if (EOF == fputs("", LAI_plot_heightFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ",
				 LAI_plot_heightFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .fire
	if (myResultFileSwitch.fire) {
		fputs("rf2010: fire module \n", FIREFile);
		fputs("Time [a1]\tNumber of fires  [a-1]\t Fire size [plots]\tFire severity [-]\tHectar Number [-]\tPlot Number [-]\n"
			 , FIREFile);
		fputs("Time\tNumberofFire\tFireSize\tFireSeverity\tHectarNo\tPlotNo\n",
			 FIREFile);
		if (EOF == fputs("", FIREFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", FIREFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .plot
	if (myResultFileSwitch.plot) {
		fputs("Forest properties for each plot: stem volume, aboveground biomass, basal area, stem number, MaxHeight, LAI, Structure indices. All units scaled to one hectare. \n"
			 , AGBPlotFile);
		fputs("Time [a1]\tHectar number [-]\tPlot number [-]\tStem volume [m3 ha-1]\tAbove ground biomass [t_ODM1 ha-1]\tBasal area [m2 ha-1]\tStem numbers [ha-1]\tStem volume TH [m3 ha-1]\tAbove ground biomss TH [t_ODM1 ha-1]\tBasal area TH [m2 ha-1]\tStem numbers TH [ha-1]\tMaximal height\tLeaf area index [m2 m-2]\tStand Density Index [-]\tQuadratic Mean Diameter [m1]\tHorizontal Index [-]\tVertical Index [-]\n"
			 , AGBPlotFile);
		fputs("Time\tHectarNo\tPlotNo\tSV\tAGB\tBA\tSN\tSV_th\tAGB_th\tBA_th\tSN_th\tMaxHeight\tLAI\tSDI\tQuadMeanDiameter\tHorizontalIndex\tVerticalIndex\n"
			 , AGBPlotFile);
		if (EOF == fputs("", AGBPlotFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", AGBPlotFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .diaplot
	if (myResultFileSwitch.diaplot) {
		fputs("Diameter distribution for each plot\n", DIAPLOTFile);
		fputs("Time [a1]\tHectar number [-]\tPlot number [-]\tDiameter class [m1]\tNumber of trees [-]\t"
			 , DIAPLOTFile);
		WritePFTHeader(DIAPLOTFile, MAXGRP, "Number of tress per PFT", "-", "");
		fputs("\n", DIAPLOTFile);
		fputs("Time\tHectarNo\tPlotNo\tDiameterClass\tNumTrees", DIAPLOTFile);
		WritePFTHeader(DIAPLOTFile, MAXGRP, "tNumTreesPFT", "-", "");
		fputs("\n", DIAPLOTFile);
		if (EOF == fputs("", DIAPLOTFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", DIAPLOTFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .ha
	if (myResultFileSwitch.ha) {
		fputs("Forest properties for each Hectare: stem volume, aboveground biomass, basal area, stem number, aboveground biomass per PFT\n"
			 , AttrHaFile);
		fputs("Time [a1]\tHectar number [-]\t\tStem volume [m3 ha-1]\tAboveground biomass [t_ODM1 ha-1]\tBasal area [m2 ha-1]\tStem numbers [ha-1]"
			 , AttrHaFile);
		for (int pft = 0; pft < MAXGRP; pft++) {
			fprintf(AttrHaFile, "\tAboveground biomass for PFT %i [t_ODM1 ha-1]",
				 pft + 1);
		}
		fputs("\nTime\tHectarNo\tSV\tAGB\tBA\tSN", AttrHaFile);
		for (int pft = 0; pft < MAXGRP; pft++) {
			fprintf(AttrHaFile, "\tAGB_PFT_%i", pft + 1);
		}
		fputs("\n", AttrHaFile);
		if (EOF == fputs("", AttrHaFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", AttrHaFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .ha_th
	if (myResultFileSwitch.ha_th) {
		fputs("Forest properties for each Hectare for all trees with DBH > Threshold: stem volume, aboveground biomass, basal area, stem number, aboveground biomass per PFT\n"
			 , AttrHaThFile);
		fputs("Time [a1]\tHectar number [-]\t\tStem volume [m3 ha-1]\tAboveground biomass [t_ODM1 ha-1]\tBasal area [m2 ha-1]\tStem numbers [ha-1]"
			 , AttrHaThFile);
		for (int pft = 0; pft < MAXGRP; pft++) {
			fprintf(AttrHaThFile, "\tAboveground biomass for PFT %i [t_ODM1 ha-1]",
				 pft + 1);
		}
		fputs("\nTime\tHectarNo\tSV\tAGB\tBA\tSN", AttrHaThFile);
		for (int pft = 0; pft < MAXGRP; pft++) {
			fprintf(AttrHaThFile, "\tAGB_PFT_%i", pft + 1);
		}
		fputs("\n", AttrHaThFile);
		if (EOF == fputs("", AttrHaThFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", AttrHaThFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .bt_chave
	if (myResultFileSwitch.biom_chave_th) {
		fputs("Above-ground biomass [t ha-1] of whole simulation area over time\n"
			 , BTCFile);
		fputs("Time [a1]\tAbove-ground biomass for Chave Moist equation from dbh, height and rho  [[t ha-1]\tAbove-ground biomass for Chave Moist equation from dbh and rho  [[t ha-1]\n", BTCFile);
		fputs("Time\tBMdhr\tBMdr\n", BTCFile);
		if (EOF == fputs("", BTCFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", BTCFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .lidarwf
	// lidar waveform per ha information.
	if (myResultFileSwitch.lidarwf) {
#ifdef underconstruction
		fputs("One large footprint lidar waveform for each ha\n", LIDARWFFile);
		fputs("Time [a1]\tHectare number [-]\tHeight bins [m]\n", LIDARWFFile);
		fputs("Time\tHectarNo\t", LIDARWFFile);

		for (int zcor = 0; zcor <= HMAX; zcor++) {
			fprintf(LIDARWFFile, "%2d\t", zcor);
		}
		fputs("\n", LIDARWFFile);
#endif
	}

	if (myResultFileSwitch.h) {
		fputs("Maximum and mean height of vegetation\n", HEIGHTFile);
		fputs("Time [a1]\tMaximum height [m]\tMean height [m]\n", HEIGHTFile);
		fputs("Time\tHMax\tHMean", HEIGHTFile);

		fputs("\n", HEIGHTFile);
		if (EOF == fputs("", HEIGHTFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", HEIGHTFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .speciesplot
	if (myResultFileSwitch.speciesplot) {
		fputs("aboveground biomass, belowground biomass, coverage, stem number per plot and per species \n"
			 , SPECIESPlotFile);
		fputs("Time [a1]\tHectar number [-]\tPlot number [-]\tSpecies Group\tAbove ground biomass [t_ODM1 m-2]\tAbove ground green biomass [t_ODM1 m-2]\tAbove ground brown biomass [t_ODM1 m-2]\tBelow ground biomass [t_ODM1 m-2]\tCoverage [-]\tStem numbers [m-2]\n"
			 , SPECIESPlotFile);
		fputs("Time\tHectarNo\tPlotNo\tGrp\tAGB\tAGBGreen\tAGBBrown\tBGB\tBA\tSN",
			 SPECIESPlotFile);

		fputs("\n", SPECIESPlotFile);
		if (EOF == fputs("", SPECIESPlotFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ",
			SPECIESPlotFileName);
			perror(S);
			goto E_EXIT;
		}
	}
	// .speciesplot_th
	if (myResultFileSwitch.speciesplot_th) {
		fputs("aboveground biomass, belowground biomass, coverage, stem number per plot and per species only for firt hec\n"
			 , SPECIESPlotTHFile);
		fputs("Time [a1]\tHectar number [-]\tPlot number [-]\tAGB th for all trees in Lidar footprint [t/footprint]\tBasal Area th for all trees in Lidar footprint [t/footprint]\tBasal Area th for PFT1 trees in Lidar footprint [t/footprint]\tBasal Area th for PFT2 trees in Lidar footprint [t/footprint]\tBasal Area th for PFT3 trees in Lidar footprint [t/footprint]\tNumber of stems in Lidar footprint [t/footprint]\tGPP th for all trees in Lidar footprint [t/footprint]\tNPP th for all trees in Lidar footprint [t/footprint]\tStem Volume th for all trees in Lidar footprint [t/footprint]\t"
			 , SPECIESPlotTHFile);

		WritePFTHeader(SPECIESPlotTHFile, MAXGRP, "Above ground biomass PFT_",
			 "t_ODM1", "ha-2");
		fputs("\t", SPECIESPlotTHFile);
		WritePFTHeader(SPECIESPlotTHFile, MAXGRP, "Basal area PFT_", "", "");
		fputs("\t", SPECIESPlotTHFile);
		WritePFTHeader(SPECIESPlotTHFile, MAXGRP, "Stem numbers PFT_", "",
			 "ha-1");
		fputs("\t", SPECIESPlotTHFile);
		WritePFTHeader(SPECIESPlotTHFile, MAXGRP, "Above ground biomass th PFT_",
			 "t_ODM1", "ha-2");
		fputs("\t", SPECIESPlotTHFile);
		WritePFTHeader(SPECIESPlotTHFile, MAXGRP, "Basal area th PFT_", "", "");
		fputs("\t", SPECIESPlotTHFile);
		WritePFTHeader(SPECIESPlotTHFile, MAXGRP, "Stem numbers th PFT_", "",
			 "ha-1");

		fputs("\tStem volume [m3 ha-1]\tMax. canopy height [m]\tLeaf area index all [m2 m-2]\tStand Density Index [-]\tQuadratic Mean Diameter [m1]\tHorizontal Index [-]\tVertical Index [-]\tWood density weighted ba\tGPP[tCha-1a-1]\tR[tCha-1a-1]\tNEE[tCha-1a-1]\tNPP[tCha-1a-1]\tNumTrees[-]cm_05\tNumTrees[-]cm_15\tNumTrees[-]cm_25\tNumTrees[-]cm_35\tNumTrees[-]cm_45\tNumTrees[-]cm_55\tNumTrees[-]cm_65\tNumTrees[-]cm_75\tNumTrees[-]cm_85\tNumTrees[-]cm_95\tNumTrees[-]cm_105\tNumTrees[-]cm_115\tNumTrees[-]cm_125\tNumTrees[-]cm_135\tNumTrees[-]cm_145\tNumTrees[-]cm_155\tNumTrees[-]cm_165\tNumTrees[-]cm_175\tNumTrees[-]cm_185\tNumTrees[-]cm_195\tNumTrees[-]cm_205\tNumTrees[-]cm_215\tNumTrees[-]cm_225\tNumTrees[-]cm_235\tNumTrees[-]cm_245\n"
			 , SPECIESPlotTHFile);

		fputs("Time\tHectarNo\tPlotNo\tAGB_th_footprint\tBA_th_footprint\tBA1_th_footprint\tBA2_th_footprint\tBA3_th_footprint\tSN_th_footprint\tGPP_footprint\tNPP_footprint\tSV_th_footprint\t"
			 , SPECIESPlotTHFile);
		WritePFTHeaderShort(SPECIESPlotTHFile, MAXGRP, "AGB", "", "");
		fputs("\t", SPECIESPlotTHFile);
		WritePFTHeaderShort(SPECIESPlotTHFile, MAXGRP, "BA", "", "");
		fputs("\t", SPECIESPlotTHFile);
		WritePFTHeaderShort(SPECIESPlotTHFile, MAXGRP, "SN", "", "");
		fputs("\t", SPECIESPlotTHFile);
		WritePFTHeaderShort(SPECIESPlotTHFile, MAXGRP, "AGB_th", "", "");
		fputs("\t", SPECIESPlotTHFile);
		WritePFTHeaderShort(SPECIESPlotTHFile, MAXGRP, "BA_th", "", "");
		fputs("\t", SPECIESPlotTHFile);
		WritePFTHeaderShort(SPECIESPlotTHFile, MAXGRP, "SN_th", "", "");
		fputs("\tSV_th\tMaxHeight\tLAI\tSDI\tQuadMeanDiameter\tHorizontalIndex\tVerticalIndex\tWoodDen\tGPP\tR\tNEE\tNPP\tDIA_05\tDIA_15\tDIA_25\tDIA_35\tDIA_45\tDIA_55\tDIA_65\tDIA_75\tDIA_85\tDIA_95\tDIA_105\tDIA_115\tDIA_125\tDIA_135\tDIA_145\tDIA_155\tDIA_165\tDIA_175\tDIA_185\tDIA_195\tDIA_205\tDIA_215\tDIA_225\tDIA_235\tDIA_245\n"
			 , SPECIESPlotTHFile);

		if (EOF == fputs("", SPECIESPlotTHFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ",
				 SPECIESPlotTHFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .mort
	if (myResultFileSwitch.mort) {
		fputs("Number and biomass of dead trees and its rates based on different mortalities (background, crowding, etc)\n"
			 , MORTFile);
		fputs("Time [a1]\t", MORTFile);
		fputs("Total number of dead trees [number1 a-1]\tTotal mortality rate[a-1]\tTotal biomass [tODM ha-1 a-1]\tTotal biomass rate[tODM tODM-1 a-1]\t"
			 , MORTFile);
		fputs("Number basic mortality [number1 a-1]\tBasic mortality rate[a-1]\tBiomass basic mortality [tODM ha-1 a-1]\tBiomass basic mortality rate[tODM tODM-1 a-1]\t"
			 , MORTFile);
		fputs("Number falling mortality [number1 a-1]\tFalling mortality rate[a-1]\tBiomass falling mortality [tODM ha-1 a-1]\tBiomass falling mortality rate[tODM tODM-1 a-1]\t"
			 , MORTFile);
		fputs("Number damage mortality [number1 a-1]\tDamage mortality rate[a-1]\tBiomass damage mortality [tODM ha-1 a-1]\tBiomass damage mortality rate[tODM tODM-1 a-1]\t"
			 , MORTFile);
		fputs("Number crowding mortality [number1 a-1]\tCrowding mortality rate[a-1]\tBiomass crowding mortality [tODM ha-1 a-1]\tBiomass crowding mortality rate[tODM tODM-1 a-1]\t"
			 , MORTFile);
		fputs("Number fire mortality [number1 a-1]\tFire mortality rate[a-1]\tBiomass fire mortality [tODM ha-1 a-1]\tBiomass fire mortality rate[tODM tODM-1 a-1]\t"
			 , MORTFile);
		fputs("Number landslide mortality [number1 a-1]\tLandslide mortality rate[a-1]\tBiomass landslide mortality [tODM ha-1 a-1]\tBiomass landslide mortality rate[tODM tODM-1 a-1]\t"
			 , MORTFile);
		fputs("Number logging mortality [number1 a-1]\tLogging mortality rate[a-1]\tBiomass logging mortality [tODM ha-1 a-1]\tBiomass logging mortality rate[tODM tODM-1 a-1]\t"
			 , MORTFile);
		fputs("Number logging damage mortality [number1 a-1]\tLogging damage mortality rate[a-1]\tBiomass logging damage mortality [tODM ha-1 a-1]\tBiomass logging damage mortality rate[tODM tODM-1 a-1]\n"
			 , MORTFile);

		fputs("Time\tTot\tTotR\tTotB\tTotBR\tMN\tMNR\tMB\tMBR\tMNF\tMNFR\tMBF\tMBFR\tMND\tMNDR\tMBD\tMBDR\tMNC\tMNCR\tMBC\tMBCR\tMNFIRE\tMNFIRER\tMBFIRE\tMBFIRER\tMNLAND\tMNLANDR\tMBLAND\tMBLANDR\tMNLOG\tMNLOGR\tMBLOG\tMBLOGR\tMNLOGDAM\tMNLOFDAMR\tMBLOGDAM\tMBLOGDAMR\t\n"
			 , MORTFile);

		if (EOF == fputs("", MORTFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", MORTFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .mort_th
	if (myResultFileSwitch.mort_th) {
		fputs("Number and biomass of dead trees and its rates based on different mortalities (background, crowding, etc)\n"
			 , MORTTHFile);
		fputs("Time [a1]\t", MORTTHFile);
		fputs("Total number of dead trees [number1 a-1]\tTotal mortality rate[a-1]\tTotal biomass [tODM ha-1 a-1]\tTotal biomass rate[tODM tODM-1 a-1]\t"
			 , MORTTHFile);
		fputs("Number basic mortality [number1 a-1]\tBasic mortality rate[a-1]\tBiomass basic mortality [tODM ha-1 a-1]\tBiomass basic mortality rate[tODM tODM-1 a-1]\t"
			 , MORTTHFile);
		fputs("Number falling mortality [number1 a-1]\tFalling mortality rate[a-1]\tBiomass falling mortality [tODM ha-1 a-1]\tBiomass falling mortality rate[tODM tODM-1 a-1]\t"
			 , MORTTHFile);
		fputs("Number damage mortality [number1 a-1]\tDamage mortality rate[a-1]\tBiomass damage mortality [tODM ha-1 a-1]\tBiomass damage mortality rate[tODM tODM-1 a-1]\t"
			 , MORTTHFile);
		fputs("Number crowding mortality [number1 a-1]\tCrowding mortality rate[a-1]\tBiomass crowding mortality [tODM ha-1 a-1]\tBiomass crowding mortality rate[tODM tODM-1 a-1]\t"
			 , MORTTHFile);
		fputs("Number fire mortality [number1 a-1]\tFire mortality rate[a-1]\tBiomass fire mortality [tODM ha-1 a-1]\tBiomass fire mortality rate[tODM tODM-1 a-1]\t"
			 , MORTTHFile);
		fputs("Number landslide mortality [number1 a-1]\tLandslide mortality rate[a-1]\tBiomass landslide mortality [tODM ha-1 a-1]\tBiomass landslide mortality rate[tODM tODM-1 a-1]\t"
			 , MORTTHFile);
		fputs("Number logging mortality [number1 a-1]\tLogging mortality rate[a-1]\tBiomass logging mortality [tODM ha-1 a-1]\tBiomass logging mortality rate[tODM tODM-1 a-1]\t"
			 , MORTTHFile);
		fputs("Number logging damage mortality [number1 a-1]\tLogging damage mortality rate[a-1]\tBiomass logging damage mortality [tODM ha-1 a-1]\tBiomass logging damage mortality rate[tODM tODM-1 a-1]\n"
			 , MORTTHFile);

		fputs("Time\tTot\tTotR\tTotB\tTotBR\tMN\tMNR\tMB\tMBR\tMNF\tMNFR\tMBF\tMBFR\tMND\tMNDR\tMBD\tMBDR\tMNC\tMNCR\tMBC\tMBCR\tMNFIRE\tMNFIRER\tMBFIRE\tMBFIRER\tMNLAND\tMNLANDR\tMBLAND\tMBLANDR\tMNLOG\tMNLOGR\tMBLOG\tMBLOGR\tMNLOGDAM\tMNLOFDAMR\tMBLOGDAM\tMBLOGDAMR\t\n"
			 , MORTTHFile);

		if (EOF == fputs("", MORTTHFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", MORTFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// .prod
	if (myResultFileSwitch.prod) {
		fputs("production, respiration and mortality [t_odm ha^-1 yr^-1]\n",
			 PRODFile);
		fputs("Time [a1]\tGross primary production [t_ODM1 ha-1 a-1]\tRespiartion  [t_ODM1 ha-1 a-1]\tNet ecosystem exchange  [t_ODM1 ha-1 a-1]\tMortality [a-1]\n"
			 , PRODFile);
		fputs("Time\tgpp\trespiration\tnpp\tmortality\n", PRODFile);
		if (EOF == fputs("", PRODFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", PRODFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// climate
	int dummyint;
	if (myResultFileSwitch.env) {
		// 1. line: description
		fputs("Environmental variables", ENVIRONMENTFile);
		fputs("\n", ENVIRONMENTFile);
		// 2. line: long header
		fputs("Time [a1]\tHectare\tPlot\tLength of vegetation period [d1]\t",
				 ENVIRONMENTFile);
		for (int i = 0; i < MAXGRP; i++) {
			dummyint = i + 1;
			fprintf(ENVIRONMENTFile, "%s %d [%s %s]\t",
					 "Global radiation available for PFT", dummyint, "mumol_photon1",
					 "m-2 s-1");
		}
		fputs("Temperature effect on respiration [-]\t", ENVIRONMENTFile);
		for (int i = 0; i < MAXGRP; i++) {
			dummyint = i + 1;
			fprintf(ENVIRONMENTFile, "%s %d [%s%s]\t",
					 "Temperature effect on photosynthesis of PFT", dummyint, "-", "");
		}

		fputs("CO2 effect on photosynthesis [-]\t", ENVIRONMENTFile);
		fputs("Air temperature [�C]\t", ENVIRONMENTFile);
		fputs("Soil temperature [�C]", ENVIRONMENTFile);

		fputs("\n", ENVIRONMENTFile);
		// 3. line: short header
		fputs("Time\tHec\tPlot\tVegPeriod\t", ENVIRONMENTFile);
		for (int i = 0; i < MAXGRP; i++) {
			dummyint = i + 1;
			fprintf(ENVIRONMENTFile, "%s%d\t", "IRPFT", dummyint);
		}
		fputs("RTresp\t", ENVIRONMENTFile);
		for (int i = 0; i < MAXGRP; i++) {
			dummyint = i + 1;
			fprintf(ENVIRONMENTFile, "%s%d\t", "RTphotPFT", dummyint);
		}

		fputs("CO2_effect\t", ENVIRONMENTFile);
		fputs("Air_temp\t", ENVIRONMENTFile);
		fputs("Soil_temp", ENVIRONMENTFile);

		fputs("\n", ENVIRONMENTFile);

		if (EOF == fputs("", ENVIRONMENTFile)) {
			sprintf(S, "#FORMIND io ERROR: Can't write to '%s' ",
					 ENVIRONMENTFileName);
			perror(S);
			goto E_EXIT;
		}
	}

	// water
	if (myResultFileSwitch.water) {
#ifdef underconstruction
		fputs("Water module: All units per year and not per timestep! Soil_Water ist the soil water content at the end of the timestep. \n"
			 , WATERFile);
		fputs("Time [a1]\tSoil water content [mm1]\tPrecipitation [mm1 a-1]\tInterception [mm1 a-1]\tRun-off above-ground [mm1 a-1]\tRun-off below-ground [mm1 a-1]\tTotal run-off [mm1 a-1]\tTranspiration [mm1 a-1]\tPermanent wilting point [mm1]\tMSW [mm1]\tPotential evapotranspiration [mm1 a-1]\twater reduction factor [-]\n"
			 , WATERFile);
		fputs("Time\tSoil_Water\tPrecipitation\tInterception\tRun-Off-above-ground\tRun-Off-below-ground\tTotal-Run-Off\tTranspiration\tPWP\tMSW\tPET\tRW\n"
			 , WATERFile);
		if (EOF == fputs("", WATERFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", WATERFileName);
			perror(S);
			goto E_EXIT;
		}
#endif
	}

	if (myResultFileSwitch.water_plot) {
#ifdef underconstruction
		fputs("Water module per plot: All units per year and not per timestep! Soil_Water ist the soil water content at the end of the timestep. \n"
			 , WATERPLOTFile);
		fputs("Time [a1]\tHectar number [-]\tPlot number [-]\tSoil water content [VolPercent1]\tPrecipitation [mm1]\tInterception [mm1]\tRun-off-above-ground [mm1]\tRun-off-below-ground [mm1]\tTotal-run-off [mm1]\tTranspiration [mm1]\tWater reduction factor [-]\n"
			 , WATERPLOTFile);
		fputs("Time\tHectarNo\tPlotNo\tSoil_Water\tPrecipitation\tInterception\tRun-Off-above-ground\tRun-Off-below-ground\tTotal-Run-Off\tTranspiration\tRW \n"
			 , WATERPLOTFile);
		if (EOF == fputs("", WATERPLOTFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", WATERPLOTFileName);
			perror(S);
			goto E_EXIT;
		}
#endif
	}

	if (myResultFileSwitch.water_century_plot) {
#ifdef underconstruction
		fputs("Water module per plot: All units per year and not per timestep! Soil_Water ist the soil water content at the end of the timestep. \n"
			 , WATERCENTPLOTFile);
		fputs("Time [a1]\tHectar number [-]\tPlot number [-]\tPrecipitation [mm1]\tInterception [mm1]\tEvaporation [mm1]\tPET [mm1]\tSnow pack [mm1]\tLiquid content of snow pack [mm1]\n"
			 , WATERCENTPLOTFile);
		fputs("Time\tHectarNo\tPlotNo\tPrecipitation\tInterception\tEvaporation\tPET\tSnow\tLiqSnow\n"
			 , WATERCENTPLOTFile);
		if (EOF == fputs("", WATERCENTPLOTFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ",
				 WATERCENTPLOTFileName);
			perror(S);
			goto E_EXIT;
		}
#endif
	}

	if (myResultFileSwitch.water_century_plot_layer) {
#ifdef underconstruction
		fputs("Water module per plot: All units per year and not per timestep! Soil_Water ist the soil water content at the end of the timestep. \n"
			 , WATERCENTPLOTLAYERFile);
		fputs("Time [a1]\tHectar number [-]\tPlot number [-]\tSoil layer [-]\tSoil water content [VolPercent1]\tTranspiration [mm1]\tNitrogen content [t_N1 ha-1]\tNitrogen uptake [t_N1 ha-1 a-1]\n"
			 , WATERCENTPLOTLAYERFile);
		fputs("Time\tHectarNo\tPlotNo\tSoil_Layer\tSoil_Water\tTranspiration\tSoil_Nitrogen\tNUptake\n"
			 , WATERCENTPLOTLAYERFile);
		if (EOF == fputs("", WATERCENTPLOTLAYERFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ",
				 WATERCENTPLOTLAYERFileName);
			perror(S);
			goto E_EXIT;
		}
#endif
	}

	if (myResultFileSwitch.traits) {
#ifdef underconstruction
		// 1. Line
		fputs("Full simulation data of each plant.\n", TRAITSFile);
		// 2. Line
		fputs("Time [a1]\tGroup [-]\tStatus [-]\tAboveground biomass [t_ODM1 ha-1]\tHeight [m1]\t"
			 , TRAITSFile);
		fputs("Biomass increment [t_ODM1 ha-1]\tAGE [a1]\tPlot number [-]\tHectar number [-]\tIdentification Number[UUID]\tGeo_AGB_2"
			 , TRAITSFile);
		fputs("\n", TRAITSFile);
		// 3.Line
		fputs("Time\tGrp\tStatus\tBT\tH\tBInc\t", TRAITSFile);
		fputs("\tAGE\tPlot\tHec\tID\tGeoAgb2", TRAITSFile);
		fputs("\n", TRAITSFile);
#endif
	}

	if (myResultFileSwitch.skid) {
#ifdef underconstruction
		fputs("Skid Trails: Damages due to skid trails after each Logging Event. \n"
			 , SKIDTRAILFile);
		fputs("Time [a1]\tNumber of Dead Trees [ha-1]\tBiomass of dead trees due to skidtrails [t1 ha-1]\tBole volume of dead trees due to skidtrails [m3 ha-1]\n"
			 , SKIDTRAILFile);
		fputs("Time\tNumberTrees\tBiomass\tBoleVolume\n", SKIDTRAILFile);
		if (EOF == fputs("", SKIDTRAILFile)) {
			sprintf(S, "### 182f ERROR: Can't write to '%s' ", SKIDTRAILFileName);
			perror(S);
			goto E_EXIT;
		}
#endif
	}

	// carbon_cycle
	if (myResultFileSwitch.cflux) {

		if (!WriteHeaderCARBON(CARBONFile))
			goto E_EXIT;
	}

	if (myResultFileSwitch.cfluxplot) {

		if (!WriteHeaderCARBONPlot(CARBONPlotFile))
			goto E_EXIT;
	}

	if (myResultFileSwitch.cflux_century_plot) {
#ifdef underconstruction
		if (!WriteHeaderCARBONCENTPlot(CARBONCENTPlotFile))
			goto E_EXIT;
#endif
	}

	if (myResultFileSwitch.nflux_century_plot) {
#ifdef underconstruction
		if (!WriteHeaderNITROGENCENTPlot(NITROGENCENTPlotFile))
			goto E_EXIT;
#endif
	}

	return true;
E_EXIT:
	Close();
	return false;

}

// -------------------------------------------------
// Functions for writing the content in output files
// and defining the type as well as the digits of the numbers within the columns
// --------------------------------------------------

// --------------------------------------------------
/* !
 \brief          		Writes height file
 \param			      DestFile      File name
 \return					void
 */

void WriteHEIGHT(FILE*DestFile) {
	HecPointer hec;
	PlotPointer plot;
	TreePointer tree;

	float hmax = 0;
	float hmean = 0;
	float numb = 0;

	hec = FirstHec;
	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {
			tree = plot->FirstTree;
			while (tree != NULL) {
				hmax = (std::max)(tree->H, hmax);
				hmean += tree->N * tree->H;
				numb += tree->N;
				tree = tree->next;
			}
			plot = plot->next;
		}
		hec = hec->next;
	}

	fprintf(DestFile, "%le\t%le\t%le\n", T.T, hmax, hmean / numb);
}

// --------------------------------------------------

/* !
 \brief          	 Writes agb file for each species and plot
 \param		       DestFile      File name
 \return				 void
 */

void WriteSPECIESPlot(FILE*DestFile) {
	HecPointer hec;
	PlotPointer plot;
	TreePointer tree;

	hec = FirstHec;
	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {
			for (int id = 0; id < MAXGRP; id++) {

				fprintf(DestFile,
					 "%le\t%6d\t%6d\t%2d\t%le\t%le\t%le\t%le\t%le\t%le\n", T.T,
					 hec->HecNo, plot->No, id, plot->BiomassGrp[id],
					 plot->BiomassGreenGrp[id], plot->BiomassBrownGrp[id],
					 plot->RootBiomassGrp[id], plot->BasalAreaGrp[id],
					 plot->StemNumberGrp[id]);

			}
			plot = plot->next;
		}
		hec = hec->next;
	}
}

// --------------------------------------------------

/* !
 \brief    Writes important features of one Grass cohort into the ResultFile *.grass
 \param    DestFile      File name
 \param 		 TP            read Tree
 \param 		 PP            read plot
 \return				 void
 */

void WriteGrassRecord(FILE*DestFile, TreePointer TP, PlotPointer PP,
	 HecPointer HP) {

	fprintf(DestFile,
		 "%le\t%2d\t%2d\t%1d\t%le\t%le\t%1d\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\n",
		 T.T, PP->No, HP->HecNo, TP->Grp + 1, TP->AGE, TP->N, TP->Status, TP->BT,
		 TP->BTgreen, TP->BTbrown, TP->BTFracGreen, TP->BTFracBrown, TP->D, TP->H,
		 TP->AC, TP->L, TP->Lgreen, TP->Lbrown, TP->RBT, TP->RH, TP->RootLength,
		 TP->IR, TP->PB, TP->R, TP->RespMain, TP->RespGrowth, TP->binc_yr,
		 TP->BInc, TP->BrInc, TP->Rep, TP->DInc, TP->HInc, TP->RS, TP->RW, TP->RN,
		 TP->minLRF, TP->WDemand, TP->WTotalUptake, TP->NDemand, TP->NTotalUptake,
		 TP->Nshoot, TP->Cost_Rhizobia);

}

// --------------------------------------------------

/* !
 \brief      Writes important features of one TreeRecord into RestartFile *.restart
 \param	    DestFile      File name
 \param 		 TP            read Tree
 \param 		 PP            read plot
 \param 		 HP            read hectare
 \return				 void
 */

void WriteTreeRecordForRestart(FILE*DestFile, TreePointer TP, PlotPointer PP,
	 HecPointer HP) {

	// write into *.restart file
	double isfull, resp, x, y;
	short tree_amount = TP->N;
	short tree_counter = 0;

	// number of individuals in cohort
	tree_amount = TP->N;

	// loop over cohort
	while (tree_counter < tree_amount) {
		// get position of tree
		x = TP->absX(tree_counter);
		y = TP->absY(tree_counter);
		fprintf(DestFile, "%2d\t%le\t%le\t%1e", TP->Grp + 1, x, y, TP->D);
		fputs("\n", DestFile);
		tree_counter++;
	}
}

// --------------------------------------------------

/* !
 \brief      Writes important features of one TreeRecord into CohortFile *.cohort
 \param	    DestFile      File name
 \param 		 TP            read Tree
 \param 		 PP            read plot
 \param 		 HP            read hectare
 \return				 void
 */

void WriteTreeRecordWithoutPos(FILE*DestFile, TreePointer TP, PlotPointer PP,
	 HecPointer HP) {
	double isfull;
	if (N_Par.variable_Irradiance_ON) {
		if (T.T == T.Start)
			isfull = 1;
		else {
			isfull = PP->mean_light_above_canopy[TP->Grp];
		}
		if (isfull == 0)
			isfull = 1;
	}
	else {
		isfull = N_Par.Env_IS_2[0] * N_Par.Env_SeaL_2[0] +
			 N_Par.Env_IS_2[1] * N_Par.Env_SeaL_2[1];
	}

	fprintf(DestFile,
		 "%le\t%2d\t%i\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%2d\t%2d\t%le\t%le\t%le\t%le\t%le\t%le\t%2d\t%2d",
		 T.T, TP->Grp + 1, TP->species, TP->N, TP->BT, TP->D, TP->H, TP->SV,
		 TP->LAITREE_forRes, TP->IR, TP->IR / isfull*100, TP->PB, TP->BInc,
		 TP->DInc, TP->OBA, TP->AGE, PP->No, HP->HecNo, TP->CLP, TP->L,
		 sqrt(TP->AC*4 / PI), TP->R, TP->RespMain, TP->RespGrowth, TP->COMGrp,
		 TP->PCT);

	fputs("\n", DestFile);

}

// --------------------------------------------------

/* !
 \brief      Writes important features of one TreeRecord into ResultFile *.res
 \param	    DestFile      File name
 \param 		 TP            read Tree
 \param 		 PP            read plot
 \param 		 HP            read hectare
 \return				 void
 */
void WriteTreeRecord(FILE*DestFile, TreePointer TP, PlotPointer PP,
	 HecPointer HP) {
	double isfull, resp, x, y;
	short tree_amount = TP->N;
	short tree_counter = 0;
	double cr = sqrt(TP->AC / PI);
	unsigned long long id;

	if (N_Par.variable_Irradiance_ON) {
		if (T.T == T.Start)
			isfull = 1;
		else {
			isfull = PP->mean_light_above_canopy[TP->Grp];
		}
		if (isfull == 0)
			isfull = 1;
	}
	else {
		isfull = N_Par.Env_IS_2[0] * N_Par.Env_SeaL_2[0] +
			 N_Par.Env_IS_2[1] * N_Par.Env_SeaL_2[1];
	}

	if (myResultFileSwitch.branch) {
#ifdef underconstruction
		tree_amount = 1;
		TP->branches.ResetBranchingValues();
		TP->branches.DoBranches();
#endif
	}
	else {
		// amount of individuals in a cohort
		tree_amount = TP->N;
	}

	while (tree_counter < tree_amount) {
		x = TP->absX(tree_counter);
		y = TP->absY(tree_counter);
		id = TP->IdVector[tree_counter];
		long long IDattachedTree = 0;
#ifdef underconstruction
		if(TP->isLiana == 1 && TP->attachedTree!=NULL && TP->N>0)
		 IDattachedTree = TP->attachedTree->IdVector[0];
#endif

		fprintf(DestFile,
			 "%le\t%2d\t%i\t%5d\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%2d\t%2d\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%llu\t%le\t%2d\t%2d\t%d\t%llu",
			 T.T, TP->Grp + 1, TP->species, 1, TP->BT, TP->D, TP->H, TP->SV,
			 TP->LAITREE_forRes, TP->IR, TP->IR / isfull*100, TP->PB, TP->BInc,
			 TP->DInc, TP->OBA, TP->AGE, PP->No, HP->HecNo, x, y, TP->CLP, TP->L,
			 sqrt(TP->AC*4 / PI), TP->R, TP->RespMain, TP->RespGrowth, id, cr,
			 TP->COMGrp, TP->PCT, TP->isLiana, IDattachedTree);

		if (myResultFileSwitch.branch) {

#ifdef underconstruction
			fprintf(DestFile, "\t%le\t", TP->branches.ReturnBranchingBiomass());
			fputs(TP->branches.ReturnBranchingOutputStr().c_str(), DestFile);
#endif
		}

		fputs("\n", DestFile);
		tree_counter++;

	}
}

// ---------------------------------------------------------------

/* !
 \brief      Writes important features of one TreeRecord into ResultFile *.res in binary format.
 \param	    DestFile      File name
 \param 		 TP            read Tree
 \param 		 PP            read plot
 \param 		 HP            read hectare
 \return				 void
 */
struct treeDatafor3D {
	double time;
	double dbh,h,cr,x,y;
	int pft;
};

void WriteTreeRecordBin(FILE*DestFile, TreePointer TP, PlotPointer PP,
	 HecPointer HP) {
	double isfull, resp, x, y;
	short tree_amount = TP->N;
	short tree_counter = 0;
	double cr = sqrt(TP->AC / PI);
	unsigned long long id;

	if (N_Par.variable_Irradiance_ON) {
		if (T.T == T.Start)
			isfull = 1;
		else {
			isfull = PP->mean_light_above_canopy[TP->Grp];
		}
		if (isfull == 0)
			isfull = 1;
	}
	else {
		isfull = N_Par.Env_IS_2[0] * N_Par.Env_SeaL_2[0] +
			 N_Par.Env_IS_2[1] * N_Par.Env_SeaL_2[1];
	}

	if (myResultFileSwitch.branch) {
#ifdef underconstruction
		tree_amount = 1;
		TP->branches.ResetBranchingValues();
		TP->branches.DoBranches();
#endif
	}
	else {
		// amount7 of individuals in a cohort
		tree_amount = TP->N;
	}

	while (tree_counter < tree_amount) {
		x = TP->absX(tree_counter);
		y = TP->absY(tree_counter);
		id = TP->IdVector[tree_counter];
		long long IDattachedTree = 0;
#ifdef underconstruction
		if(TP->isLiana == 1 && TP->attachedTree!=NULL && TP->N>0)
		 IDattachedTree = 0;//TP->attachedTree->IdVector[0];
#endif

		treeDatafor3D data;
		data.time=T.T;
		data.dbh=TP->D;
		data.h=TP->H;
		data.cr=cr;
		data.x=x;
		data.y=y;
		data.pft=TP->Grp;
		fwrite(&data, sizeof(data), 1, DestFile);

//		fprintf(DestFile,
//			 "%le\t%2d\t%i\t%5d\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%2d\t%2d\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%llu\t%le\t%2d\t%2d\t%d\t%llu",
//			 T.T, TP->Grp + 1, TP->species, 1, TP->BT, TP->D, TP->H, TP->SV,
//			 TP->LAITREE_forRes, TP->IR, TP->IR / isfull*100, TP->PB, TP->BInc,
//			 TP->DInc, TP->OBA, TP->AGE, PP->No, HP->HecNo, x, y, TP->CLP, TP->L,
//			 sqrt(TP->AC*4 / PI), TP->R, TP->RespMain, TP->RespGrowth, id, cr,
//			 TP->COMGrp, TP->PCT, TP->isLiana, IDattachedTree);

//		if (myResultFileSwitch.branch) {
//
//#ifdef underconstruction
//			fprintf(DestFile, "\t%le\t", TP->branches.ReturnBranchingBiomass());
//			fputs(TP->branches.ReturnBranchingOutputStr().c_str(), DestFile);
//#endif
//		}
//
//		fputs("\n", DestFile);
		tree_counter++;

	}
}

// ---------------------------------------------------------------

/* !
 \brief      Writes important features of one TreeRecord into ResultFile (diameter increment)
 \param	    DestFile      File name
 \param 		 TP            read Tree
 \return     void
 */

void WriteTreeRecordDINC(FILE*DestFile, TreePointer TP) {
	if (T.T > T.Start + T.D / 2.0) {
		fprintf(DestFile, "%d\t%2.0f\t%.2f\t%le\t%f\t%le\t%le\t%d\n", TP->Grp + 1,
			 T.T, TP->N, TP->D, TP->DInc, TP->OBA, TP->AGE, TP->Grp);
	}
}

// --------------------------------------------------

/* !
 \brief      Writes the data into the CFLUX File
 \param	    DestFile      File name
 \return		 void
 */

void WriteCARBON(FILE*DestFile) {

	fprintf(DestFile,
		 "%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\n",
		 T.T, Out.C_flux, Out.PB, Out.R_total, Out.R_total_biomass,
		 Out.Resp_DeadWood, Out.Resp_Soil_slow, Out.Resp_Soil_fast,
		 Out.Cflux_to_DeadWood, Out.Cflux_to_Soil_fast, Out.Cflux_to_Soil_slow,
		 Out.CPool_alive_biomass, Out.CPool_DeadWood, Out.CPool_Soil_fast,
		 Out.CPool_Soil_slow, Out.aet);
}

// -----------------------------------------------------

/* !
 \brief      Writes the data into the CFLUXPLOT File
 \param	    DestFile      File name
 \return		 void
 */

void WriteCARBONPlot(FILE*DestFile) {

	PlotPointer plot;
	HecPointer hec;

	hec = FirstHec;
	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {
			fprintf(DestFile,
				 "%le\t%i\t%i\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\n",
				 T.T, hec->HecNo, plot->No, ODM_TO_C*25*plot->PB, 25*plot->R_total,
				 25*plot->C_flux, ODM_TO_C*25*plot->R_total_biomass,
				 25*plot->Resp_DeadWood, 25*plot->Resp_Soil_slow,
				 25*plot->Resp_Soil_fast, 25*plot->Cflux_to_DeadWood,
				 25*plot->Cflux_to_Soil_fast, 25*plot->Cflux_to_Soil_slow,
				 ODM_TO_C*25*plot->Biomass, 25*plot->CPool_DeadWood,
				 25*plot->CPool_Soil_fast, 25*plot->CPool_Soil_slow, plot->aet);
			plot = plot->next;
		}
		hec = hec->next;
	}
}

// -----------------------------------------------------

/* !
 \brief      	After Simulation: saves simulated forest in PIN format
 \param	    	DestFile      File name
 \return		 void
 */

void WritePIN(FILE*DestFile) {

	PlotPointer Plot;
	HecPointer Hec;
	std::vector<std::vector<int> >Dia_Grp;
	int maxclass = DCLASS.size();
	Dia_Grp.resize(maxclass);
	for (int i = 0; i < maxclass; i++) {
		Dia_Grp[i].resize(MAXGRP);
	}

	int*arraystart;

	fputs("file pinfile\n", DestFile);
	fprintf(DestFile,
		 "regionheader = \"after FORMIND simulation. Time = %lf \"\n", T.T);
	fputs("dclass       = \n", DestFile);

	for (int ii = 0; ii < DCLASS.size(); ii++)
		fprintf(DestFile, "%1.3f ", std::atof(DCLASS[ii].c_str()));
	fputs("\n", DestFile);

	Hec = FirstHec;
	while (Hec != NULL) {
		Plot = Hec->FirstPlot;
		while (Plot != NULL) {
			CalculatePlotNumbers2(Plot, Dia_Grp);

			fputs("block plot\n", DestFile);
			fprintf(DestFile, " name     = \"%s\" \n", Plot->Name.c_str());
			fprintf(DestFile, " position = %u\t%u\t%u\t%u\n", (int)Plot->LocXL,
				 (int)Plot->LocYL, (int)Plot->LocXH, (int)Plot->LocYH);
			fprintf(DestFile, " code      = %u\n", (int)Plot->landcode);
			fprintf(DestFile, " mel      = %u\n", (int)Plot->MEL);
			fputs(" n0       = \n", DestFile);

			for (int k = 0; k < MAXGRP; k++) {
				for (int i = 0; i < DCLASS.size(); i++) {
					int test;
					test = Dia_Grp[i][k];
					fprintf(DestFile, "%u\t", Dia_Grp[i][k]);

				}

				fputs(", \n", DestFile);

			}
			fputs(" seeds    =  ", DestFile);
			for (int k = 0; k < MAXGRP; k++) {
				fprintf(DestFile, "%u", Plot->SeedPool[k][0] +
					 Plot->SeedPool[k][1]);
				if (k != (MAXGRP - 1))
					fputs("\t", DestFile);
				else
					fputs("\n", DestFile);
			}
			Plot = Plot->next;
		}
		Hec = Hec->next;
	}
}

// -----------------------------------------------------

/* !
 \brief      	Writes Diameter Distribution File
 \param	    	DestFile      File name
 \return		   void
 */

void WriteLUDWIG_DIA(FILE*DestFile) {
	int j, zz;
	int ANZ_DIACLASS = (int) floor(DMAX / (float)N_Par.Div_DiaClassWidth + 1.0);
	double DDOUT = N_Par.Div_DiaClassWidth;

	for (j = 0; j < ANZ_DIACLASS; j++) {
		fprintf(DestFile, "%le\t%le\t%le\t", T.T, DDOUT*j + DDOUT / 2.0,
			 Out.DIASUM[j]);
		for (zz = 0; zz < MAXGRP; zz++) {
			fprintf(DestFile, "%le\t", Out.DIAandGRP_new[j][zz]);
		}
		fprintf(DestFile, "%le\t", Out.BADIASUM_new[j]);
		for (zz = 0; zz < MAXGRP; zz++) {
			fprintf(DestFile, "%le", Out.BADIAandGRP_new[j][zz]);
			if (zz < (MAXGRP - 1))
				fputs("\t", DestFile);
		}
		fputs("\n", DestFile);
	}

}

// -----------------------------------------------------

/* !
 \brief      	Writes mortality rate per diameter class
 \param	    	DestFile      File name
 \return		   void
 */

void WriteDIAMORT(FILE*DestFile) {

	int j, zz;
	int ANZ_DIACLASS = (int) floor(DMAX / (float)N_Par.Div_DiaClassWidth + 1.0);
	double DDOUT = N_Par.Div_DiaClassWidth;

	for (j = 0; j < ANZ_DIACLASS; j++) {
		if (Out.NTOTAL == 0.0) {
			fprintf(DestFile, "%le\t%le\t%le\t", T.T, DDOUT*j + DDOUT / 2.0, 0.0);
		}
		else {
			fprintf(DestFile, "%le\t%le\t%le\t", T.T, DDOUT*j + DDOUT / 2.0,
				 Out.DIADEATH[j] / Out.NTOTAL);
		}
		for (zz = 0; zz < MAXGRP; zz++) {
			if (Out.DIAandGRP_new[j][zz] == 0.0) {
				fprintf(DestFile, "%le\t", 0.0);
			}
			else {
				fprintf(DestFile, "%le\t",
					 Out.DIADEATH_PFT[j][zz] / Out.DIAandGRP_new[j][zz]);
			}
		}
		fputs("\n", DestFile);
	}
}

// --------------------------------------------------------
/* !
 \brief          		Writes biomass of plots in landslide simulations
 \param	      		DestFile      File name
 \param 		  			kk            plot corner
 \param 		  		   j             plot corner
 \param 		  		   x             x coordinate
 \param  		 	   y             y coordinate
 \return 				void
 */

void WriteBMofPLOTS(FILE*DestFile, int kk, int j, int x, int y) {
	fprintf(DestFile,
		 "%le\t%7d\t%7d\t%7d\t%7d\t%7d\t%7d\t%7d\t%7d\t%le\t%7d\t%7d\t%le\t%le\t%le\n",
		 T.T, (kk - 1)*Switch.Maxplot + j, x, y, Out.PlotCorner[kk - 1][j - 1][0],
		 Out.PlotCorner[kk - 1][j - 1][1], Out.PlotCorner[kk - 1][j - 1][2],
		 Out.PlotCorner[kk - 1][j - 1][3], Out.ForType[kk - 1][j - 1],
		 Out.BMPlot[kk - 1][j - 1], Out.TimeSinceSlide[kk - 1][j - 1],
		 Out.SlideSize[kk - 1][j - 1], Out.SLOPE[kk - 1][j - 1],
		 Out.SlideProb[kk - 1][j - 1], Out.ELEV[kk - 1][j - 1]);
}

// --------------------------------------------------------
/* !
 \brief          		Writes landslide file
 \param	      		DestFile      File name
 \param 		  			kk            plot corner
 \param 		  		   j             plot corner
 \param 		  		   x             x coordinate
 \param  		 	   y             y coordinate
 \return 				void
 */

void WriteLANDSLIDE(FILE*DestFile, int kk, int j, int x, int y) {
	if (Out.TimeSinceSlide[kk - 1][j - 1] == 0)
		fprintf(DestFile,
		 "%le\t%7d\t%7d\t%7d\t%7d\t%7d\t%7d\t%7d\t%7d\t%le\t%7d\t%7d\t%le\t%le\t%le\n",
		 T.T, (kk - 1)*Switch.Maxplot + j, x, y, Out.PlotCorner[kk - 1][j - 1][0],
		 Out.PlotCorner[kk - 1][j - 1][1], Out.PlotCorner[kk - 1][j - 1][2],
		 Out.PlotCorner[kk - 1][j - 1][3], Out.ForType[kk - 1][j - 1],
		 Out.BMPlot[kk - 1][j - 1], Out.TimeSinceSlide[kk - 1][j - 1],
		 Out.SlideSize[kk - 1][j - 1], Out.SLOPE[kk - 1][j - 1],
		 Out.SlideProb[kk - 1][j - 1], Out.ELEV[kk - 1][j - 1]);
}

// --------------------------------------------------------
/* !
 \brief          		Writes plot biomass dynamics file
 \param	      		DestFile      File name
 \param 		  			kk            plot corner
 \param 		  		   j             plot corner
 \param 		  		   x             x coordinate
 \param  		 	   y             y coordinate
 \return 				void
 */

void WritePLOTBMDYN(FILE*DestFile, int kk, int j, int x, int y) {
	fprintf(DestFile, "%le\t%7d\t%7d\t%7d\t%7d\t%le\t%le\t%le\t%le\n", T.T,
		 (kk - 1)*Switch.Maxplot + j, x, y, Out.TimeSinceSlide[kk - 1][j - 1],
		 Out.BMPlot[kk - 1][j - 1], Out.NEWDBMPlot[kk - 1][j - 1],
		 Out.BINCPlot[kk - 1][j - 1], Out.BMofNewTrees[kk - 1][j - 1]);

}

// --------------------------------------------------------
/* !
 \brief          		Writes LAI per plot
 \param	      		DestFile      File name
 \param 		  			kk            plot corner
 \param 		  		   j             plot corner
 \param 		  		   x             x coordinate
 \param  		 	   y             y coordinate
 \return 				void
 */

void WritePLOTLAI(FILE*DestFile, int kk, int j, int x, int y) {
	fprintf(DestFile, "%le\t%7d\t%7d\t%7d\t%le\n", T.T,
		 (kk - 1)*Switch.Maxplot + j, x, y, Out.LAI[kk - 1][j - 1]);
}

// --------------------------------------------------------
/* !
 \brief          		Writes result file with cumulative LAI for each plot and height layer
 \param	      		DestFile      File name
 \return 				void
 */

void WritePlotLaiHeightLayer(FILE*DestFile) {
	HecPointer hec;
	PlotPointer plot;
	hec = FirstHec;
	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {
			for (int layer = 0; layer < HIGHESTLAYERNUMBER; layer++) {
				fprintf(DestFile, "%le\t%7d\t%7d\t%le\t%le\t%le\n", T.T, hec->HecNo,
					 plot->No, layer*Switch.DLYR,
					 Out.LAI_Layer[hec->HecNo - 1][plot->No - 1][layer],
					 Out.LAD_Layer[hec->HecNo - 1][plot->No - 1][layer]);
			}
			plot = plot->next;
		}
		hec = hec->next;
	}
}

// --------------------------------------------------------
/* !
 \brief          		Writes fire file
 \param	      		DestFile      File name
 \return 				void
 */

void WriteFIRE(FILE*DestFile) {
	fprintf(DestFile, "%le\t%7d\t%7d\n", T.T, Out.firenum, Out.firesize);
}

// --------------------------------------------------------
/* !
 \brief          		Writes mortality file
 \param	      		DestFile      File name
 \return 				void
 */

void WriteMORT(FILE*DestFile) {

	for (int i = 0; i < 9; i++) {
		Out.DEATHCUM[i] *= (Switch.Hectare / AREASUM);
		Out.BiomassDEATHCUM[i] *= (Switch.Hectare / AREASUM);
		if (Out.NTOTAL > 0)
			Out.DEATHCUMrate[i] = ((double)Out.DEATHCUM[i] / (double)Out.NTOTAL);
		if (Out.BTSUM > 0)
			Out.BiomassDEATHCUMrate[i] = Out.BiomassDEATHCUM[i] / Out.BTSUM;
	}

	fprintf(DestFile, "%le\t", T.T);
	for (int i = 0; i < 9; i++) {
		fprintf(DestFile, "%le\t%le\t%le\t%le\t", Out.DEATHCUM[i],
			 Out.DEATHCUMrate[i], Out.BiomassDEATHCUM[i],
			 Out.BiomassDEATHCUMrate[i]);
	}
	fprintf(DestFile, "\n");
}

// --------------------------------------------------------
/* !
 \brief          		Writes mortality file with minimum diameter threshold (th)
 \param	      		DestFile      File name
 \return 				void
 */

void WriteMORTTH(FILE*DestFile) {
	for (int i = 0; i < 9; i++) {
		Out.TH_DEATHCUM[i] *= (Switch.Hectare / AREASUM);
		Out.TH_BiomassDEATHCUM[i] *= (Switch.Hectare / AREASUM);
		if (Out.TH_NTOTAL > 0)
			Out.TH_DEATHCUMrate[i] = (Out.TH_DEATHCUM[i] / (double)Out.TH_NTOTAL);
		if (Out.TH_BTSUM > 0)
			Out.TH_BiomassDEATHCUMrate[i] =
				 Out.TH_BiomassDEATHCUM[i] / Out.TH_BTSUM;
	}

	fprintf(DestFile, "%le\t", T.T);
	for (int i = 0; i < 9; i++) {
		fprintf(DestFile, "%le\t%le\t%le\t%le\t", Out.TH_DEATHCUM[i],
			 Out.TH_DEATHCUMrate[i], Out.TH_BiomassDEATHCUM[i],
			 Out.TH_BiomassDEATHCUMrate[i]);
	}
	fprintf(DestFile, "\n");
}

// --------------------------------------------------------
/* !
 \brief          		Writes productivity file
 \param	      		DestFile      File name
 \return 				void
 */

void WritePROD(FILE*DestFile) {
	HecPointer hec;
	PlotPointer plot;
	hec = FirstHec;
	float res = 0;
	float mort = 0;
	float bpp = 0;
	float npp = 0;

	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {
			mort += plot->MB + plot->MBC + plot->MBF + plot->MBD + plot->MBFIRE +
				 plot->MBLOGDAM;
			bpp += plot->PB; // gross primary production
			res += plot->R_total_biomass; // respiration
			plot = plot->next;
		}
		hec = hec->next;
	}
	npp += bpp - res; // net primary production
	fprintf(DestFile, "%le\t%le\t%le\t%le\t%le\n", T.T, bpp *=
		 Switch.Hectare / AREASUM, res *= Switch.Hectare / AREASUM, npp *=
		 Switch.Hectare / AREASUM, mort *= Switch.Hectare / AREASUM);
}

// --------------------------------------------------------
/* !
 \brief          		Writes agb plot file
 \param	      		DestFile      File name
 \return 				void
 */

void WriteAGBPlot(FILE*DestFile) {
	HecPointer hec;
	PlotPointer plot;
	hec = FirstHec;
	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {
			fprintf(DestFile,
				 "%le\t%6d\t%6d\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\n",
				 T.T, hec->HecNo, plot->No, plot->StemVolume*25, plot->Biomass*25,
				 plot->BasalArea*25, plot->StemNumber*25, plot->StemVolume_th*25,
				 plot->Biomass_th*25, plot->BasalArea_th*25, plot->StemNumber_th*25,
				 plot->Plotmax, plot->LAI[0], plot->SDI, plot->QuadMeanDiameter,
				 plot->HorizontalIndex, plot->VerticalIndex);
			plot = plot->next;
		}
		hec = hec->next;
	}
}

// --------------------------------------------------------
/* !
 \brief          		Writes diameter distribution per plot
 \param	      		DestFile      File name
 \return 				void
 */

void WriteDIAPLOT(FILE*DestFile) {
	int NumDiaclass = (int) floor(DMAX / (float)N_Par.Div_DiaClassWidth + 1.0);
	double DDOUT = N_Par.Div_DiaClassWidth;
	HecPointer hec;
	PlotPointer plot;
	hec = FirstHec;
	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {
			for (int j = 0; j < NumDiaclass; j++) {
				fprintf(DestFile, "%le\t%6d\t%6d\t%le\t%le", T.T, hec->HecNo,
					 plot->No, DDOUT*j - DDOUT / 2.0, plot->DiaSum[j]);
				for (int k = 0; k < MAXGRP; k++)
					fprintf(DestFile, "\t%le", plot->DiaGrp[j][k]);
				fprintf(DestFile, "\n");
			}
			plot = plot->next;
		}
		hec = hec->next;
	}
}

// --------------------------------------------------------
/* !
 \brief          		Writes forest attributes per ha for all trees with DBH > Threshold
 \param	      		DestFile      File name
 */

void WriteAttrHaTh(FILE*DestFile) {
	HecPointer hec;
	PlotPointer plot;
	hec = FirstHec;
	float StemVolumeSum_th;
	float BiomassSum_th;
	float BasalAreaSum_th;
	float StemNumberSum_th;
	float BiomassSumPerPFT_th[HYPERMAXGRP];

	while (hec != NULL) {
		plot = hec->FirstPlot;
		StemVolumeSum_th = 0.0;
		BiomassSum_th = 0.0;
		BasalAreaSum_th = 0.0;
		StemNumberSum_th = 0.0;
		for (int pft = 0; pft < MAXGRP; pft++) {
			BiomassSumPerPFT_th[pft] = 0.0;
		}

		while (plot != NULL) {
			StemVolumeSum_th += plot->StemVolume_th;
			BiomassSum_th += plot->Biomass_th;
			BasalAreaSum_th += plot->BasalArea_th;
			StemNumberSum_th += plot->StemNumber_th;
			for (int pft = 0; pft < MAXGRP; pft++) {
				BiomassSumPerPFT_th[pft] += plot->TH_BiomassGrp[pft];
			}
			plot = plot->next;
		}
		fprintf(DestFile, "%le\t%6d\t%le\t%le\t%le\t%le", T.T, hec->HecNo,
			 StemVolumeSum_th, BiomassSum_th, BasalAreaSum_th, StemNumberSum_th);
		for (int pft = 0; pft < MAXGRP; pft++) {
			fprintf(DestFile, "\t%le", BiomassSumPerPFT_th[pft]);
		}
		fprintf(DestFile, "\n");

		hec = hec->next;
	}
}

// --------------------------------------------------------
/* !
 \brief          		Writes forest attributes per ha
 \param	      		DestFile      File name
 \return 				void
 */

void WriteAttrHa(FILE*DestFile) {
	HecPointer hec;
	PlotPointer plot;
	hec = FirstHec;
	float StemVolumeSum;
	float BiomassSum;
	float BasalAreaSum;
	float StemNumberSum;
	float BiomassSumPerPFT[HYPERMAXGRP];

	while (hec != NULL) {
		plot = hec->FirstPlot;
		StemVolumeSum = 0.0;
		BiomassSum = 0.0;
		BasalAreaSum = 0.0;
		StemNumberSum = 0.0;
		for (int pft = 0; pft < MAXGRP; pft++) {
			BiomassSumPerPFT[pft] = 0.0;
		}

		while (plot != NULL) {
			StemVolumeSum = StemVolumeSum + plot->StemVolume;
			BiomassSum = BiomassSum + plot->Biomass;
			BasalAreaSum = BasalAreaSum + plot->BasalArea;
			StemNumberSum = StemNumberSum + plot->StemNumber;
			for (int pft = 0; pft < MAXGRP; pft++) {
				BiomassSumPerPFT[pft] += plot->BiomassGrp[pft];
			}
			plot = plot->next;
		}
		fprintf(DestFile, "%le\t%6d\t%le\t%le\t%le\t%le", T.T, hec->HecNo,
			 StemVolumeSum, BiomassSum, BasalAreaSum, StemNumberSum);
		for (int pft = 0; pft < MAXGRP; pft++) {
			fprintf(DestFile, "\t%le", BiomassSumPerPFT[pft]);
		}
		fprintf(DestFile, "\n");

		hec = hec->next;
	}
}

// --------------------------------------------------------
/* !
 \brief          		Writes restartplot file
 \param	      		DestFile      File name
 \return 				void
 */

void WriteRESTARTPLOT(FILE*DestFile) {

	HecPointer HP;
	PlotPointer PP;
	HP = FirstHec;
	while (HP != NULL) {
		PP = HP->FirstPlot;
		while (PP != NULL) {

			fprintf(DestFile, "%2d\t%2d\t%2d\t%le\t%le\t%le\t%le\t", HP->HecNo,
				 PP->No, PP->landcode, PP->CPool_DeadWood, PP->CPool_Soil_fast,
				 PP->CPool_Soil_slow, PP->MB);
			for (int com = 0; com < 2; com++) {
				for (int pft = 0; pft < MAXGRP; pft++) {
					fprintf(DestFile, "%u\t", PP->SeedPool[pft][com]);
				}
			}
			fputs("\n", DestFile);

			PP = PP->next;
		}
		HP = HP->next;
	}
}

// -------------------------------------------------------
/* !
 \brief     Writes result file of GRASSMIND used for calibration
 \param		DestFile
 \return    void
 */

void WriteGRASSPlot(FILE*DestFile) {

	HecPointer hec;
	PlotPointer plot;
	hec = FirstHec;
	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {
			for (int grp = 0; grp < MAXGRP; grp++) {
				if (Out.NGRP[grp] > 0) {
					fprintf(DestFile,
						 "%le\t%6d\t%6d\t%2d\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\n",
						 T.T, hec->HecNo, plot->No, grp, Out.NGRP[grp],
						 Out.BTandGRP[grp], Out.BTGreen[grp], Out.MaxHeight[grp],
						 Out.MeanHeight[grp], N_Par.Geo_OF[grp]*Out.BAandGRP[grp],
						 Out.LAIGrass[grp], Out.LAIGreen[grp], Out.RBTandGRP[grp],
						 Out.ShootBiomassMean[grp], Out.RootDepthMean[grp],
						 Out.LeafNAreaMean[grp], Out.WMHC[grp],
						 Out.BiomassDensity[grp]);
				}
			}

			plot = plot->next;
		}
		hec = hec->next;
	}

}

// -------------------------------------------------------
/* !
 \brief     Writes result file of GRASSMIND if mowing is activated in the management file
 \param		DestFile
 \return    void
 */

void WriteGRASSMow(FILE*DestFile) {
	if (Logging.isMowedrightnow) {
		HecPointer hec;
		PlotPointer plot;
		hec = FirstHec;
		while (hec != NULL) {
			plot = hec->FirstPlot;
			while (plot != NULL) {
				for (int grp = 0; grp < MAXGRP; grp++) {
					if (Out.NGRP[grp] > 0) {
						fprintf(DestFile,
							 "%le\t%6d\t%6d\t%2d\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\n",
							 T.T, hec->HecNo, plot->No, grp, Out.NGRP[grp],
							 Out.BTandGRP[grp], Out.BTGreen[grp], Out.MaxHeight[grp],
							 Out.MeanHeight[grp], N_Par.Geo_OF[grp]*Out.BAandGRP[grp],
							 Out.LAIGrass[grp], Out.LAIGreen[grp], Out.RBTandGRP[grp],
							 Out.ShootBiomassMean[grp], Out.RootDepthMean[grp],
							 Out.LeafNAreaMean[grp], Out.WMHC[grp],
							 Out.BiomassDensity[grp], Logging.HarvestBg[grp],
							 Logging.HarvestBb[grp]);
					}
				}

				plot = plot->next;
			}
			hec = hec->next;
		}
	}
}

// -------------------------------------------------------
/* !
 \brief     Writes result file of GRASSMIND used for calibration
 \param		DestFile
 \return    void
 \details	only for calibration of Jena Experiment; therefore N_Par.Sow_Date is assumed to be similar for all species and timesmon are fix
 */

void WriteGRASSCalibration(FILE*DestFile) {

	bool criteria = false;
	int timesmon[13] = {5, 13, 16, 25, 28, 37, 40, 50, 52, 61, 64, 73, 76};

	if (T.T <= 1.0) {
		criteria = true;
	}
	else {
		for (int k = 0; k < 13; k++) {
			if ((T.T >= (((double)N_Par.Sow_Date[0] / 365.0) +
				 ((double)timesmon[k] / 12.0) - 0.02)) && (T.T <=
				 (((double)N_Par.Sow_Date[0] / 365.0) + ((double)timesmon[k] / 12.0)
				 + 0.02))) {
				criteria = true;
			}
		}
	}

	if (criteria) {
		HecPointer hec;
		PlotPointer plot;
		hec = FirstHec;
		while (hec != NULL) {
			plot = hec->FirstPlot;
			while (plot != NULL) {
				for (int grp = 0; grp < MAXGRP; grp++) {
					if (Out.NGRP[grp] > 0) {
						fprintf(DestFile,
							 "%le\t%6d\t%6d\t%2d\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\t%le\n",
							 T.T, hec->HecNo, plot->No, grp, Out.NGRP[grp],
							 Out.BTandGRP[grp], Out.BTGreen[grp], Out.MaxHeight[grp],
							 Out.MeanHeight[grp], N_Par.Geo_OF[grp]*Out.BAandGRP[grp],
							 Out.LAIGrass[grp], Out.LAIGreen[grp], Out.RBTandGRP[grp],
							 Out.ShootBiomassMean[grp], Out.RootDepthMean[grp],
							 Out.LeafNAreaMean[grp], Out.WMHC[grp],
							 Out.BiomassDensity[grp], Out.Biomass_calib[grp]);
					}
				}
				plot = plot->next;
			}
			hec = hec->next;
		}
	}
}

// -----------------------------------------------------
// ------------- MAIN SAVER ----------------------------
// -----------------------------------------------------

// -------------------------------------------------------
/* !
 \brief     Writes various output files
 \param		void
 \return    void
 */

void RESULT::SaveF11(void) {

	int j, lgrp, hgrp;

	if (myResultFileSwitch.sv)
		fprintf(DYN2File, "%le\t%le\t", T.T, Out.SVSUM);

	if (myResultFileSwitch.bv)
		fprintf(BVFile, "%le\t%le\t", T.T, Out.BVSUM);

	if (myResultFileSwitch.n)
		fprintf(DYN3File, "%le\t%le\t", T.T, Out.NTOTAL);

	if (myResultFileSwitch.bt)
		fprintf(DYN4File, "%le\t%le\t", T.T, Out.BTSUM);

	if (myResultFileSwitch.ba)
		fprintf(DYN5File, "%le\t%le\t", T.T, Out.BASALSUM);

	if (myResultFileSwitch.div) {
		if (Out.BTSUM > 0) {
			fprintf(DYN6File, "%le\t1.000\t", T.T);
		}
		else {
			fprintf(DYN6File, "%le\t0.000\t", T.T);
		}

	}

	if (myResultFileSwitch.in)
		fprintf(INFile, "%le\t%le\t", T.T, Out.InTH);

	if (myResultFileSwitch.seed)
		fprintf(SEEDFile, "%le\t%le\t", T.T, Out.SEEDSUM);

	if (myResultFileSwitch.seed_rain)
		fprintf(SEEDRAINFile, "%le\t%le\t", T.T, Out.SEEDRAINSUM);

	if (myResultFileSwitch.stree)
		fprintf(SEEDTREEFile, "%le\t%le\t", T.T, Out.SEEDTREESUM);

	if (myResultFileSwitch.seedling)
		fprintf(SEEDLINGFile, "%le\t%le\t", T.T, Out.SEEDLINGSUM);

	if (myResultFileSwitch.mort_pft) {
		fprintf(MORTPFTFile, "%le\t%le\t", T.T, Out.DEATH);
		if (Out.NTOTAL == 0.0) {
			fprintf(MORTPFTFile, "%le\t", 0.0);
		}
		else {
			fprintf(MORTPFTFile, "%le\t", Out.DEATH / Out.NTOTAL);
		}
	}

	if (myResultFileSwitch.mort_pft_th) {
		fprintf(MORTPFTTHFile, "%le\t%le\t", T.T, Out.TH_DEATH);
		if (Out.TH_NTOTAL == 0.0) {
			fprintf(MORTPFTTHFile, "%le\t", 0.0);
		}
		else {
			fprintf(MORTPFTTHFile, "%le\t", Out.TH_DEATH / Out.TH_NTOTAL);
		}
	}

	for (j = 0; j < MAXGRP; j++) {
		if (myResultFileSwitch.sv)
			fprintf(DYN2File, "%le\t", Out.SVandGRP[j]);
		if (myResultFileSwitch.bv)
			fprintf(BVFile, "%le\t", Out.BVandGRP[j]);
		if (myResultFileSwitch.n)
			fprintf(DYN3File, "%le\t", Out.NGRP[j]);
		if (myResultFileSwitch.bt)
			fprintf(DYN4File, "%le\t", Out.BTandGRP[j]);
		if (myResultFileSwitch.ba)
			fprintf(DYN5File, "%le\t", Out.BAandGRP[j]);
		if (myResultFileSwitch.div) {
			if (Out.BTSUM > 0.0)
				fprintf(DYN6File, "%le\t", Out.BTandGRP[j] / Out.BTSUM);
			else
				fputs("0.000\t", DYN6File);
		}
		if (myResultFileSwitch.in)
			fprintf(INFile, "%le\t", Out.InTH_GRP[j]);
		if (myResultFileSwitch.seed)
			fprintf(SEEDFile, "%le\t", Out.SEEDandGRP[j]);
		if (myResultFileSwitch.seed_rain)
			fprintf(SEEDRAINFile, "%le\t", Out.SEEDRAINandGRP[j]);
		if (myResultFileSwitch.seedling)
			fprintf(SEEDLINGFile, "%le\t", Out.SEEDLINGandGRP[j]);
		if (myResultFileSwitch.stree)
			fprintf(SEEDTREEFile, "%le\t", Out.SEEDTREEandGRP[j]);
		if (myResultFileSwitch.mort_pft)
			fprintf(MORTPFTFile, "%le\t", Out.DEATH_PFT[j]);
		if (myResultFileSwitch.mort_pft_th)
			fprintf(MORTPFTTHFile, "%le\t", Out.TH_DEATH_PFT[j]);

	}
	if (myResultFileSwitch.sv)
		if (myResultFileSwitch.n)
			if (myResultFileSwitch.bt)
				if (myResultFileSwitch.ba)
					if (myResultFileSwitch.div)
						fprintf(DYN6File, "\t");
	if (myResultFileSwitch.in)
		fprintf(INFile, "\t");
	if (myResultFileSwitch.seed)
		fprintf(SEEDFile, "\t");
	if (myResultFileSwitch.seed_rain)
		fprintf(SEEDRAINFile, "\t");
	if (myResultFileSwitch.seedling)
		fprintf(SEEDLINGFile, "\t");
	if (myResultFileSwitch.stree)
		fprintf(SEEDTREEFile, "\t");
	if (myResultFileSwitch.mort_pft)
		fprintf(MORTPFTFile, "\t");
	if (myResultFileSwitch.mort_pft_th)
		fprintf(MORTPFTTHFile, "\t");

	for (j = 0; j < MAXGRP; j++) {
		if (myResultFileSwitch.mort_pft) {
			if (Out.NGRP[j] == 0.0) {
				fprintf(MORTPFTFile, "%le\t", 0.0);
			}
			else {
				fprintf(MORTPFTFile, "%le\t", Out.DEATH_PFT[j] / Out.NGRP[j]);
			}
		}

		if (myResultFileSwitch.mort_pft_th) {
			if (Out.TH_NGRP[j] == 0.0) {
				fprintf(MORTPFTTHFile, "%le\t", 0.0);
			}
			else {
				fprintf(MORTPFTTHFile, "%le\t",
				Out.TH_DEATH_PFT[j] / Out.TH_NGRP[j]);
			}
		}

	}

	if (myResultFileSwitch.sv)
		fprintf(DYN2File, "%le\t%le\n", Out.SVandCOM[0], Out.SVandCOM[1]);
	if (myResultFileSwitch.bv)
		fprintf(BVFile, "%le\t%le\n", Out.BVandCOM[0], Out.BVandCOM[1]);
	if (myResultFileSwitch.n)
		fprintf(DYN3File, "%le\t%le\n", Out.NCOM[0], Out.NCOM[1]);
	if (myResultFileSwitch.bt)
		fprintf(DYN4File, "%le\t%le\t%le\n", Out.BTandCOM[0], Out.BTandCOM[1],
		 Out.MeanWoodDen);
	if (myResultFileSwitch.ba)
		fprintf(DYN5File, "%le\t%le\n", Out.BAandCOM[0], Out.BAandCOM[1]);
	if (myResultFileSwitch.div) {
		if (Out.BTSUM > 0.0)
			fprintf(DYN6File, "%le\t%le\n", Out.BTandCOM[0] / Out.BTSUM,
			 Out.BTandCOM[1] / Out.BTSUM);
		else
			fputs("0.000\t0.000\n", DYN6File);
	}
	if (myResultFileSwitch.seed)
		fprintf(SEEDFile, "%le\t%le\n", Out.SEEDandCOM[0], Out.SEEDandCOM[1]);
	if (myResultFileSwitch.seed_rain)
		fprintf(SEEDRAINFile, "%le\t%le\n", Out.SEEDRAINandCOM[0],
		 Out.SEEDRAINandCOM[1]);
	if (myResultFileSwitch.stree)
		fprintf(SEEDTREEFile, "%le\t%le\n", Out.SEEDTREEandCOM[0],
		 Out.SEEDTREEandCOM[1]);

	if (myResultFileSwitch.in)
		fprintf(INFile, "\n");
	if (myResultFileSwitch.seedling)
		fprintf(SEEDLINGFile, "\n");
	if (myResultFileSwitch.mort_pft)
		fprintf(MORTPFTFile, "\n");
	if (myResultFileSwitch.mort_pft_th)
		fprintf(MORTPFTTHFile, "\n");
}

// -------------------------------------------------------
/* !
 \brief     Writes simulation results to output file (ONLY for the first hectare)
 \param		void
 \return    void
 */

void RESULT::SaveF12(void) {

	int j;
	if (myResultFileSwitch.sv_th)
		fprintf(DYNTH2File, "%le\t%le\t", T.T, Out.TH_SVSUM);
	if (myResultFileSwitch.bv_th)
		fprintf(BVTHFile, "%le\t%le\t", T.T, Out.TH_BVSUM);
	if (myResultFileSwitch.n_th)
		fprintf(DYNTH3File, "%le\t%le\t", T.T, Out.TH_NTOTAL);
	if (myResultFileSwitch.bt_th)
		fprintf(DYNTH4File, "%le\t%le\t", T.T, Out.TH_BTSUM);
	if (myResultFileSwitch.ba_th)
		fprintf(DYNTH5File, "%le\t%le\t", T.T, Out.TH_BASALSUM);
	if (myResultFileSwitch.div_th) {
		if (Out.TH_BTSUM > 0) {
			fprintf(DYNTH6File, "%le\1.000\t", T.T);
		}
		else {
			fprintf(DYNTH6File, "%le\t0.000\t", T.T);
		}
	}

	for (j = 0; j < MAXGRP; j++) {
		if (myResultFileSwitch.sv_th)
			fprintf(DYNTH2File, "%le\t", Out.TH_SVandGRP[j]);
		if (myResultFileSwitch.bv_th)
			fprintf(BVTHFile, "%le\t", Out.TH_BVandGRP[j]);
		if (myResultFileSwitch.n_th)
			fprintf(DYNTH3File, "%le\t", Out.TH_NGRP[j]);
		if (myResultFileSwitch.bt_th)
			fprintf(DYNTH4File, "%le\t", Out.TH_BTandGRP[j]);
		if (myResultFileSwitch.ba_th)
			fprintf(DYNTH5File, "%le\t", Out.TH_BAandGRP[j]);
		if (myResultFileSwitch.div_th) {
			if (Out.TH_BTSUM > 0.0)
				fprintf(DYNTH6File, "%le\t", Out.TH_BTandGRP[j] / Out.TH_BTSUM);
			else
				fputs("\t0.000\t", DYNTH6File);
		}
	}

	if (myResultFileSwitch.sv_th)
		fprintf(DYNTH2File, "%le\t%le\n", Out.TH_SVandCOM[0], Out.TH_SVandCOM[1]);
	if (myResultFileSwitch.bv_th)
		fprintf(BVTHFile, "%le\t%le\n", Out.TH_BVandCOM[0], Out.TH_BVandCOM[1]);
	if (myResultFileSwitch.n_th)
		fprintf(DYNTH3File, "%le\t%le\n", Out.TH_NCOM[0], Out.TH_NCOM[1]);
	if (myResultFileSwitch.bt_th)
		fprintf(DYNTH4File, "%le\t%le\t%le\n", Out.TH_BTandCOM[0],
		 Out.TH_BTandCOM[1], Out.TH_MeanWoodDen);
	if (myResultFileSwitch.ba_th)
		fprintf(DYNTH5File, "%le\t%le\n", Out.TH_BAandCOM[0], Out.TH_BAandCOM[1]);
	if (myResultFileSwitch.div_th) {
		if (Out.TH_BTSUM > 0.0)
			fprintf(DYNTH6File, "%le\t%le\n", Out.TH_BTandCOM[0] / Out.TH_BTSUM,
			 Out.TH_BTandCOM[1] / Out.TH_BTSUM);
		else
			fputs("0.000\t0.000\n", DYNTH6File);
	}

}

// ---------------------------------------------------------------

/* !
 \brief          Writes simulation results to various output files
 \param	        DoIt        activates output of results
 \param 		     logging     boolean that specifies if logging is activated
 \return         boolean true if no error occurs.
 */

bool RESULT::Save(bool DoIt, int logging) {
	int j, i, x, xx, y, yy, kk;
	bool Status = true;
	TreePointer tree;
	HecPointer hec;
	double gapnof;
	PlotPointer Plot;

	if (((T.T + T.D / 2.0 >= TimeTreeListOut) || (T.T + T.D / 2.0 >= T.End)) &&
		 (myResultFileSwitch.res || myResultFileSwitch.res_th || myResultFileSwitch.res_th_bin ||
		 myResultFileSwitch.cohort || myResultFileSwitch.cohort_th ||
		 myResultFileSwitch.grass || myResultFileSwitch.traits)) {
		hec = FirstHec;
		while (hec != NULL) {
			Plot = hec->FirstPlot;
			while (Plot != NULL) {
				tree = Plot->FirstTree;
				while (tree != NULL) {

					if (myResultFileSwitch.res) {
						if (tree->N > 0)
							WriteTreeRecord(File, tree, Plot, hec);
					}
					if (myResultFileSwitch.cohort) {
						if (tree->N > 0)
							WriteTreeRecordWithoutPos(CohortFile, tree, Plot, hec);
					}
					if (myResultFileSwitch.res_th && (tree->D >= Switch.Schwelle)) {
						if (tree->N > 0)
							WriteTreeRecord(ResThFile, tree, Plot, hec);
					}
					if (myResultFileSwitch.res_th_bin && (tree->D >= Switch.Schwelle)) {
						if (tree->N > 0)
							WriteTreeRecordBin(ResThBinFile, tree, Plot, hec);
					}
					if (myResultFileSwitch.cohort_th && (tree->D >= Switch.Schwelle))
					{
						if (tree->N > 0)
							WriteTreeRecordWithoutPos(CohortThFile, tree, Plot, hec);
					}
#ifdef underconstruction
					if (N_Par.GRASSMIND) {
						if (myResultFileSwitch.grass) {
							if (tree->N > 0) {
								WriteGrassRecord(GRASSFile, tree, Plot, hec);
							}
						}
					}
					if (myResultFileSwitch.traits) {
						if (tree->N > 0)
							WriteTraitRecord(TRAITSFile, tree, Plot, hec);
					}
#endif
					tree = tree->next;
				}
				Plot = Plot->next;
			}
			hec = hec->next;
		}
		TimeTreeListOut = TimeTreeListOut + N_Par.TreeListOutputStep;
	}

	// Write restart file with single tree info
	if (((T.T + T.D / 2.0 >= T.End) && myResultFileSwitch.restart)) {
		hec = FirstHec;
		while (hec != NULL) {
			Plot = hec->FirstPlot;
			while (Plot != NULL) {
				tree = Plot->FirstTree;
				while (tree != NULL) {
					if (tree->N > 0)
						WriteTreeRecordForRestart(RestartFile, tree, Plot, hec);
					tree = tree->next;
				}
				Plot = Plot->next;
			}
			hec = hec->next;
		}
	}

	// Write restartplot file with info on carbon and seed pools
	if ((T.T + T.D / 2.0 >= T.End) && myResultFileSwitch.restartplot)
		WriteRESTARTPLOT(RestartPlotFile);

	// Write output that is produced at each regular output step
	if ((T.T + T.D / 2.0 >= NextTimeOut) || (T.T == T.End) || (DoIt) ||
		 (logging == 1)) {

		// .ATS & .LAI
		for (j = 0; j < HIGHESTLAYERNUMBER; j++) {
			if (myResultFileSwitch.ats) {
				fprintf(ATSFile, "%le\t%le\t%le\t%le\t%le\t%le\n", T.T,
					 j*Switch.DLYR, Out.AvATSUM[j], Out.AvATSF[j][0],
					 Out.AvATSF[j][1], Out.AvATSF[j][2]);
			}

			if (myResultFileSwitch.lai) {
				if (j < MAXLAD)
					fprintf(LAIFile, "%le\t%le\t%le\t%le\n", T.T, j*Switch.DLYR,
					 Out.AvLAI[j], Out.AvLAD[j]);
				else
					fprintf(LAIFile, "%le\t%le\t%le\tNA\tNA\n", T.T, j*Switch.DLYR,
					 Out.AvLAI[j]);
			}
		}

		// .DIA
		if (myResultFileSwitch.dia) {
			WriteLUDWIG_DIA(DIAFile);
		}

		// .biom_chave_th
		if (myResultFileSwitch.biom_chave_th)
			fprintf(BTCFile, "%le\t%le\t%le\n", T.T, Out.TH_BTCHSUM, Out.TH_BTCSUM);

		// .MORT_PFT_DIA
		if (myResultFileSwitch.mort_pft_dia) {
			WriteDIAMORT(MORTPFTDIAFile);
		}

		// Writing .DYN, .SV, .N, .BT, .BA, .DIV, .SEED, .SEEDLING, .STREE, .MORT_PFT, .MORT_PFT_TH
		SaveF11();

		// Writing *.*_th for .DYN, .SV, .N, .BT, .BA, .DIV, .SEED, .SEEDLING, .STREE
		SaveF12();

		// .lai_mean
		if (myResultFileSwitch.lai_mean)
			fprintf(LAI_meanFile, "%le\t%le\n", T.T, Out.LAI_mean);
		// .lai_plot_layer
		if (myResultFileSwitch.lai_plot_heightlayer)
			WritePlotLaiHeightLayer(LAI_plot_heightFile);

		// .lai_plot
		for (kk = 1; kk <= SQR(Switch.Ha); kk++) {
			xx = (kk - 1) % Switch.Ha;
			yy = (kk - 1) / Switch.Ha;
			for (j = 1; j <= Switch.Maxplot; j++) {
				x = (j - 1) % 5 + 1;
				y = (j - 1) / 5 + 1;
				x = xx * 5 + x;
				y = yy * 5 + y;
				if (myResultFileSwitch.lai_plot)
					WritePLOTLAI(LAI_plotFile, kk, j, x, y);

				if (N_Par.Landslide) {
					if (myResultFileSwitch.bmpl)
						WriteBMofPLOTS(BMPLFile, kk, j, x, y);
					if (myResultFileSwitch.landslide)
						WriteLANDSLIDE(LANDSLIDEFile, kk, j, x, y);
					if (myResultFileSwitch.plotbmdyn)
						WritePLOTBMDYN(PLOTBMDYNFile, kk, j, x, y);
				}
			}
		}

		if (N_Par.GRASSMIND) {
			if (myResultFileSwitch.grassplot) {
				WriteGRASSPlot(GRASSPLOTFile);
			}
			if (myResultFileSwitch.grass_mow) {
				WriteGRASSMow(GRASS_MOWFile);
			}
			if (myResultFileSwitch.grasscalib) {
				WriteGRASSCalibration(GRASSCALIBFile);
			}
		}

		// .plot, .diaplot, .mort und .prod und *.cflux
		if (myResultFileSwitch.plot)
			WriteAGBPlot(AGBPlotFile);
		if (myResultFileSwitch.diaplot)
			WriteDIAPLOT(DIAPLOTFile);
		if (myResultFileSwitch.ha)
			WriteAttrHa(AttrHaFile);
		if (myResultFileSwitch.ha_th)
			WriteAttrHaTh(AttrHaThFile);
		if (myResultFileSwitch.speciesplot)
			WriteSPECIESPlot(SPECIESPlotFile);

		if (myResultFileSwitch.h)
			WriteHEIGHT(HEIGHTFile);

#ifdef underconstruction
		if (myResultFileSwitch.speciesplot_th)
			WriteSPECIESPlotTH(SPECIESPlotTHFile);
#endif
		if (myResultFileSwitch.mort)
			WriteMORT(MORTFile);
		if (myResultFileSwitch.mort_th)
			WriteMORTTH(MORTTHFile);
		if (myResultFileSwitch.prod)
			WritePROD(PRODFile);
		if (myResultFileSwitch.cflux)
			WriteCARBON(CARBONFile);
		if (myResultFileSwitch.cfluxplot)
			WriteCARBONPlot(CARBONPlotFile);
		if (myResultFileSwitch.cflux_century_plot)
#ifdef underconstruction
			WriteCARBONCENTPlot(CARBONCENTPlotFile);
#endif
		if (myResultFileSwitch.pin && ((T.T - DELTA) <=
			 T.End && (T.T + DELTA) >= T.End))
			WritePIN(PINOUTFile);

		if (logging == 0)
			NextTimeOut = NextTimeOut + OutputStep;

	}
	else if (T.BeginOfYear(N_Par.GRASSMIND)) {

	}

#ifdef underconstruction
	// Lidar simulation outputs: the Lidar output timestep is independent of the standard output timestep
	if ((myResultFileSwitch.lidarpc || myResultFileSwitch.voxfor ||
		 myResultFileSwitch.lidarwf) && ((T.T - T.D) >= 0.0) &&
		 (T.T == TimeLidarOut)) {
		RunSidar();
		// Write Lidar point cloud to file
		if (myResultFileSwitch.lidarpc == true) {
			WriteLIDARPC(LIDARPCFile);
		}
		// Write voxel forest to file
		if (myResultFileSwitch.voxfor == true) {
			WriteVOXFOR(VOXFORFile);
		}
		// Write lidar waveform to file
		if (myResultFileSwitch.lidarwf == true) {
			WriteLIDARWF(LIDARWFFile);
		}
		// Calculate when to produce the next Lidar sample
		TimeLidarOut = TimeLidarOut + N_Par.LidarOutputStep;
	}
#endif

	goto EXIT;
E_EXIT:
	Status = false;
	Close();
EXIT:
	return Status;
}

// ---------------------------------------------------------------

/* !
 \brief        Update minimum and maximum coordinate values
 \param	   	SL     Minimum plot coordinate value
 \param 			SH     Maximum plot coordinate value
 \param 			VL     Minimum plot coordinate value
 \param	 	   VH     Maximum plot coordinate value
 \return       void
 */

void UpdateMinMax(double*SL, double*SH, double*VL, double*VH) {

	if (*VL < *SL)
		* SL = *VL;
	else if (*VL > *SH)
		* SH = *VL;
	if (*VH < *SL)
		* SL = *VH;
	else if (*VH > *SH)
		* SH = *VH;
}

// ---------------------------------------------------------------

/* !
 \brief          	 Distributes PINFileData to pointer structures.
 \param	      	 PIN         Pin file pointes
 \param			    numberhec   number of simulated hectares
 \return           boolean true if no error occurs.
 */

bool DistributePIN2Pointer(PinPointer PIN, int numberhec, int sizeclasses) {
	PlotPointer nplot, plot;
	HecPointer nhec, hec;
	int hh, pp, dd, gg;
	int dy, dx;
	double hecsize;
	int dummy, c, count, i, species, idice;
	double dice;

	hecsize = Switch.Hecside;

	if (SQR(Switch.Ha) > MAXHA) {
		cerr << "Simulation area too large: please adjust MAXHA and maximal allowed Pointer calls!"
			 << endl;
		goto E_EXIT;
	}

	if (SQR(Switch.Ha) > numberhec) {
		cerr << "You simulate more hectare than in your Pin-file defined. To ensure a proper simulation run, the first defined hectare is reproduced."
			 << endl;
	}

	if (SQR(Switch.Ha) < numberhec) {
		cerr << "You simulate less hectare than in your Pin-file defined. Please adjust your settings to ensure a proper simulation run."
			 << endl;
	}

	FirstHec = NULL;
	for (hh = 0; hh < (int)SQR(Switch.Ha); hh++) {

		hec = new HECTAR;
		if (NULL == hec)
			goto E_EXIT;
		hec->HecNo = hh + 1;
		hec->FirstPlot = NULL;
		hec->next = NULL;
		dx = (hh % Switch.Ha);
		dy = (hh / Switch.Ha);

		hec->LocXL = dx * hecsize;
		hec->LocYL = dy * hecsize;
		hec->LocXH = (dx + 1) * hecsize;
		hec->LocYH = (dy + 1) * hecsize;

		int firstplot, lastplot;
		if (SQR(Switch.Ha) > numberhec) {
			if (numberhec == 1) {
				firstplot = 0;
				lastplot = Switch.Maxplot;
			}
			else {
				cerr << "More than one hectare defined in PIN-file but still less than you want to simulate! Please change!"
					 << endl;
				goto E_EXIT;
			}
		}
		else {
			firstplot = hh * Switch.Maxplot;
			lastplot = (hh + 1) * Switch.Maxplot;
		}

		for (pp = firstplot; pp < lastplot; pp++) {
			plot = new PLOT;
			if (NULL == plot)
				goto E_EXIT;
			plot->FirstTree = NULL;
			plot->No = pp % Switch.Maxplot + 1;
			plot->Name = PIN->name[pp];
			plot->landcode = PIN->landcode[pp];
			plot->MEL = PIN->mel[pp];
			plot->Seeds = 0;

			plot->N0.resize(MAXGRP);
			for (int grp = 0; grp < MAXGRP; grp++) {
				plot->N0[grp].resize(sizeclasses);
			}

			for (gg = 0; gg < MAXGRP; gg++) {

				for (i = 0; i < N_COMMERCIAL; i++) {
					plot->SeedPool[gg][i] = 0;
				}
				if (PIN->seeds[pp][gg] < 1.0)
					dummy = 0;
				else
					dummy = (int)floor((double)PIN->seeds[pp][gg]);
				count = 0;
				for (c = 0; c < dummy; c++) {
					dice = _Random();
					if (dice < N_Par.Div_COMMERCIAL_A[gg])
						count++; // commercial
				}
				dummy -= count; // noncommercial

				plot->SeedPool[gg][0] += dummy; // noncommercial
				plot->SeedPool[gg][1] += count; // commercial
				plot->Seeds += dummy;
				plot->Seeds += count;

				for (i = 0; i < N_COMMERCIAL; i++) {
					plot->NewSeeds[gg][i] = 0;
				}
				for (dd = 0; dd < sizeclasses; dd++) {
					plot->N0[gg][dd] = std::atof(PIN->n0[pp][gg][dd].c_str());
				}
			}

			if (SQR(Switch.Ha) > numberhec) {
				plot->LocXL = PIN->position[pp][0] + dx * hecsize;
				plot->LocYL = PIN->position[pp][1] + dy * hecsize;
				plot->LocXH = PIN->position[pp][2] + dx * hecsize;
				plot->LocYH = PIN->position[pp][3] + dy * hecsize;
			}
			else {
				plot->LocXL = PIN->position[pp][0];
				plot->LocYL = PIN->position[pp][1];
				plot->LocXH = PIN->position[pp][2];
				plot->LocYH = PIN->position[pp][3];
			}

			UpdateMinMax(&Loc.XMin, &Loc.XMax, &(plot->LocXL), &(plot->LocXH));
			UpdateMinMax(&Loc.YMin, &Loc.YMax, &(plot->LocYL), &(plot->LocYH));

			if (hec->FirstPlot == NULL) {
				hec->FirstPlot = plot;
			}
			else {
				nplot->next = plot;
			}
			nplot = plot;
			nplot->next = NULL;
		}

		if (FirstHec == NULL)
			FirstHec = hec;
		else
			nhec->next = hec;
		nhec = hec;
		nhec->next = NULL;

	}

	Loc.XDiff = Loc.XMax - Loc.XMin;
	Loc.YDiff = Loc.YMax - Loc.YMin;

	return true;
E_EXIT:
	cerr << "ERROR: variable error \tfile: " << __FILE__ <<
		 "\tfunction: DistributePIN2Pointer\t line:" << __LINE__ <<
		 "\t Pointererror occured; not enough memory." << endl;
	return false;

}

// -----------------------------

/* !
 \brief          	 Number to string converter
 \param		       numberAsString       number to convert
 \return           valor                string converted number
 */

template<typename T>
T PINFileReader::StringToNumber(const std::string&numberAsString) {
	T valor;
	std::stringstream stream(numberAsString);
	stream >> valor;
	if (stream.fail()) {
	std:
		string errorMessage = "StringToNumber failed for string \"" +
			 numberAsString + "\"";
		PINError(errorMessage);
		std::runtime_error e(numberAsString);
		throw e;
	}
	return valor;
}

// -----------------------------

/* !
 \brief          	 Handles windows files on unix correctly
 \param  	       line       line number to correct
 \return           void
 */

void PINFileReader::readSafeLine(std::string&line) {
	std::getline(pinfile, line);
	if (line.length() && line[line.length() - 1] == '\r') {
		line.erase(line.length() - 1);
	}
	++currentLineNumber;
}

// -----------------------------

/* !
 \brief          	 Skip white-space from i1 forwards & i2 backwards
 \param	       	 s
 \param	       	 i1
 \param	       	 i2
 \return           s.substr(i1, i2-i1+1)
 */

std::string PINFileReader::deleteWhite(const std::string&s, size_t i1,
	 size_t i2) {
	while (whites.find(s[i1]) != std::string::npos && i1 <= i2)
		i1++;
	while (whites.find(s[i2]) != std::string::npos && i2 >= i1)
		i2--;
	return s.substr(i1, i2 - i1 + 1);
}

// -----------------------------

/* !
 \brief          	 Deletes white-space
 \param	       	 string
 \return           void
 */

void PINFileReader::deleteWhite(std::string&s) {
   if(s.length()==0) return; // thanks to codeguard
	s = deleteWhite(s, 0, s.length() - 1);
}

// -----------------------------

/* !
 \brief          	 Extract string from parentheses
 \param		       s
 \return           s.substr(i1+1, i2-(i1+1))
 */

std::string PINFileReader::extractString(const std::string&s) {
	size_t i1 = s.find_first_of("\"");
	size_t i2 = s.find_last_of("\"");
	return s.substr(i1 + 1, i2 - (i1 + 1));
}

// -----------------------------

/* !
 \brief          	 Split string "a=b" into a & b, removing whitespaces
 \param		       s
 \param		       key
 \return           void
 */

void PINFileReader::splitKey(const std::string&s, std::string&key,
	 std::string&val) {
	size_t pos = s.find("=");
	if (pos != std::string::npos) {
		key = deleteWhite(s, 0, pos - 1);
		val = deleteWhite(s, pos + 1, s.length() - 1);
		if (m_verbose) {
			cout << "key '" << key << "'\n";
			cout << "val '" << val << "'\n";
		}
	}
	else
		key = val = "";
}

// -----------------------------

/* !
 \brief          	 Get string vector from string defined by delimiter
 \param  	       string str
 \param            string tokens
 \param            string delimiter
 \return				 void
 */

void PINFileReader::Tokenize(const std::string&str,
	 std::vector<std::string>&tokens, const std::string&delimiters) {

	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos) {
		// Found a token, add it to the vector.
		std::string ts = str.substr(lastPos, pos - lastPos);
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

// -----------------------------

/* !
 \brief          	 Get next token from list, update "pos" to point after token
 \param		       s
 \param		       pos
 \return           sval
 */

std::string PINFileReader::getNextToken(const std::string&s, size_t&pos) {
	pos = s.find_first_not_of(whites, pos);
	if (pos == std::string::npos)
		return "";
	size_t epos = s.find_first_of(whites, pos);
	if (epos == std::string::npos)
		epos = s.length();
	std::string sval = s.substr(pos, epos - pos);
	pos = epos;
	return sval;
}

// -----------------------------

/* !
 \brief          	 Read a line and split into key & value
 \param		       key
 \param		       val
 \return				 void
 */

void PINFileReader::readSplitKey(std::string&key, std::string&val) {
	readSafeLine(line);
	splitKey(line, key, val);
}

// -----------------------------

/* !
 \brief          	 PIN file error message
 \param		       s     PIN file value
 \return				 void
 */

void PINFileReader::PINError(const std::string&s) {
	cerr << "error: ReadPINFile: " << s;
	if (currentLineNumber != 0)
		cerr << "in line " << currentLineNumber;
	cerr << "\n";
}

// -----------------------------

/* !
 \brief          	 Free memory resources
 \param		       void
 \return				 void
 */

void PINFileReader::FreeResources(void) {
	if (Pin) {
		delete Pin;
		Pin = NULL;
	}
	if (pinfile.is_open())
		pinfile.close();
}

// -----------------------------

/* !
 \brief          	 PIN file reader function
 \param	  	       fname     file name
 \return           boolean true if no error occurs.
 */

bool PINFileReader::readPINFile(const std::string&fname) {
	std::string key, val, sval;
	size_t spos;
	int i, dia, grp, plot;
	int len = 0;

	FreeResources();

	Pin = new PIN;

	currentLineNumber = 0;
	pinfile.open(fname.c_str());
	if (pinfile.is_open()) {

		readSafeLine(line);
		deleteWhite(line);
		if (line != "file pinfile") {
			PINError("file pinfile");
			return false;
		};
		readSplitKey(key, val);
		readSplitKey(key, val);
		if (key != "dclass") {
			PINError("dclass");
			return false;
		};

		readSafeLine(line);
		deleteWhite(line);
		DCLASS.clear();
		Tokenize(line, DCLASS, "\t ");
		int lenClass = DCLASS.size();

		int xy_max = Switch.Ha * Switch.Ha * Switch.Maxplot;
		Pin->n0.resize(xy_max);

		for (int it_xcor = 0; it_xcor < xy_max; it_xcor++) {
			Pin->n0[it_xcor].resize(MAXGRP);
		}

		for (plot = 0; plot < Switch.Ha * Switch.Ha * Switch.Maxplot; plot++) {
			readSafeLine(line);
			deleteWhite(line);
			if (line != "block plot")
				break;

			readSplitKey(key, val); // name
			if (key != "name") {
				PINError("name");
				return false;
			};
			Pin->name[plot] = extractString(val);
			readSplitKey(key, val); // position
			if (key != "position") {
				PINError("position");
				return false;
			};
			for (i = 0, spos = 0; i < 4; i++) {
				sval = getNextToken(val, spos);
				Pin->position[plot][i] = StringToNumber<long>(sval);
				if (m_verbose)
					cout << sval << ",";
			}
			readSplitKey(key, val); // landcode
			if (key != "code") {
				PINError("code");
				return false;
			};
			Pin->landcode[plot] = StringToNumber<long>(val);

			readSplitKey(key, val); // mel
			if (key != "mel") {
				PINError("mel");
				return false;
			};
			Pin->mel[plot] = StringToNumber<long>(val);
			readSplitKey(key, val); // n0
			if (key != "n0") {
				PINError("n0");
				return false;
			};

			for (grp = 0; grp < MAXGRP; grp++) {
				readSafeLine(line);
				removeTrailing(line, ',');
				Tokenize(line, Pin->n0[plot][grp], "\t ");
				int lenPlot = Pin->n0[plot][grp].size();
				if (lenPlot != lenClass) {
					std::cerr << "Error in PIN file at plot " << plot <<
						 ", species group " << grp << ":";
					if (lenPlot < lenClass) {
						std::cerr <<
							 " You have less size class records than defined in the header, but all size class data of the plots will be used." <<
							 std::endl;
						len = lenPlot;
					}
					if (lenPlot > lenClass) {
						std::cerr <<
							 " You have more size class records than defined in the header with dclass! Eventually, this error results from a not needed empty space between the last number and the \";\". Please note, Only those size clases corresponding to the header will be used." <<
							 std::endl;
						len = lenClass;
					}
				}
				else
					len = lenClass;
			}
			readSplitKey(key, val); // seeds
			if (key != "seeds") {
				PINError("seeds");
				return false;
			};

			for (grp = 0, spos = 0; grp < MAXGRP; grp++) {
				sval = getNextToken(val, spos);
				Pin->seeds[plot][grp] = StringToNumber<long>(sval);
			}
		}

		int numha = plot / Switch.Maxplot;

		if (plot % Switch.Maxplot) {
			PINError("unexpected end of file: plotnumber%25!=0");
			return false;
		}
		if (numha != 1 && numha != SQR(Switch.Ha)) {
			PINError("invalid number of hectares");
			return false;
		}

		noPlotsRead = plot;

		MAXDD = len;
		if (!DistributePIN2Pointer(Pin, numha, len))
			return false;
		else
			return true;
	}
	else {
		PINError("could not open: " + fname);
		return false;
	};
}

// -----------------------------

/* !
 \brief          	 write data from pin file reader
 \param	  	       fname     file name
 \return           boolean true if no error occurs.
 */

bool PINFileReader::writePINFileFromPin(const std::string&fname,
	 std::string regionheader) {
	// for GUI. Beware: Does write reader-internal Pin, NOT current plots!
	const char sep = ' ';

	std::vector<std::vector<int> >Dia_Grp;
	int maxclass = DCLASS.size();
	Dia_Grp.resize(maxclass);
	for (int i = 0; i < maxclass; i++) {
		Dia_Grp[i].resize(MAXGRP);
	}

	ofstream off(fname.c_str());
	off << "file pinfile\n";
	off << "regionheader = \"" << regionheader << "\"\n";
	off << "dclass = " << std::endl;

	for (int ii = 0; ii < DCLASS.size(); ii++) {
		off << DCLASS[ii].c_str();
		if (ii < DCLASS.size() - 1)
			off << sep;
	}
	off << std::endl;

	for (int p = 0; p < noPlotsRead; p++) {

		off << "block plot\n";
		off << " name     = \"" << Pin->name[p] << "\" \n";
		off << " position = ";
		for (int i = 0; i < 4; i++) {
			off << Pin->position[p][i];
			if (i < 4 - 1)
				off << sep;
		}
		off << "\n";
		off << " code     = " << Pin->landcode[p] << "\n";
		off << " mel      = " << Pin->mel[p] << "\n";
		off << " n0       = " << "\n";

		for (int i = 0; i < MAXGRP; i++) {
			for (int j = 0; j < MAXDD; j++) {
				off << Pin->n0[p][i][j];
				if (j < MAXDD - 1)
					off << sep;
			}
			off << ",\n";

		}
		off << " seeds    = ";
		for (int grp = 0; grp < MAXGRP; grp++) {
			off << Pin->seeds[p][grp];
			if (grp < MAXGRP - 1)
				off << sep;
		}
		off << "\n";
	}
	return true;
}

// --------------------------------------------------------------

// -----------------------------

/* !
 \brief          	 Writes elevation, slope, etc. to output
 \param	  	       void
 \return           void
 */

void OutElevSlope(void) {
	HecPointer hec;
	PlotPointer plot;
	double test;

	hec = FirstHec;
	while (hec != NULL) {
		plot = hec->FirstPlot;
		while (plot != NULL) {

			Out.SLOPE[hec->HecNo - 1][plot->No - 1] = plot->Slope;
			Out.ELEV[hec->HecNo - 1][plot->No - 1] = plot->Elev;
			Out.SlideProb[hec->HecNo - 1][plot->No - 1] = plot->SlideProb;
			Out.ForType[hec->HecNo - 1][plot->No - 1] = plot->ForestType;
			plot = plot->next;
		}
		hec = hec->next;
	}

}
// ----------------------------------------------------------------
// ----------------------------------------------------------------

/* !
 \brief      Calculates stem size distribution for a plot per PFT
 \param	    plot
 \param  	 diameter distribution (diagrp)
 \return		 void
 \details	 classes are handled as the center of the respective class
 */

void CalculatePlotNumbers2(PlotPointer plot,
	 std::vector<std::vector<int> >&diagrp) {

	TreePointer tree;
	for (int g = 0; g < MAXGRP; g++)
		for (int d = 0; d < DCLASS.size(); d++)
			diagrp[d][g] = 0;

	tree = plot->FirstTree;
	double center, above;
	double below = -1.;
	while (tree != NULL) {
		if (tree->N > 0) {
			for (int i = 0; i < (DCLASS.size() - 1); i++) {
				center = std::atof(DCLASS[i].c_str());
				above = center + (std::atof(DCLASS[i + 1].c_str()) - center) / 2;
				if (i > 0)
					below = center - (center - std::atof(DCLASS[i - 1].c_str())) / 2;
				if ((tree->D > (below)) && (tree->D <= above)) {
					diagrp[i][tree->Grp] += tree->N;
				}
			}
		}
		tree = tree->next;
	}

}

/* !
 \brief      reads climate file to plot pointer
 \param	    std::string fn (filename)
 \param      PLOT* plot
 \return     bool true if successful
 \details    prints error message and throws exception on error
 */

bool readClimateFileToPlot(std::string fn, PLOT* plot) {
	ifstream iff(fn.c_str());
	if (!iff.is_open())
		return false;
	std::string line;
	std::getline(iff, line);
	std::vector<double>tokens;
	std::vector<std::string>headers;
	// fixed size for columns expected. This way we can throw an error if there are more or less.
	int nColumnsExpected = 6;
	headers.resize(nColumnsExpected);
	tokens.resize(nColumnsExpected);
	int nTokensFound = forTokenizeNoPushBack(line, headers, "\t ");
	if (nTokensFound != nColumnsExpected) {
		std::string message = "ERROR reading climate file " + fn +
			 ". Header in first line must contain 6 tokens for 6 columns. But I found " +
			 std::to_string(nTokensFound) + ". readClimateFileToPlot().";
		MMErrorMessage(message, MMErrorException);
	}
	if (headers[0] != std::string("rain[mm]")) {
		std::string message = "Reading climate file " + fn +
			 ". Header of first column is " + headers[0] +
			 ". This is UNEXPECTED! Usually it is: rain[mm].";
		MMErrorMessage(message, N_Par.WarningType);
	}
	plot->precipitation.clear();
	plot->temperature.clear();
	plot->irradiance.clear();
	plot->daylength.clear();
	plot->pet.clear();
	plot->CO2_concentration.clear();

	int linesRead = 0;
	do {
		std::getline(iff, line);
		linesRead++;
		if (line.size() == 0) {
			if (linesRead == 0) {
				std::string message = "ERROR reading climate file " + fn +
					 ". The first line does not contain a header but no data! readClimateFileToPlot().";
				MMErrorMessage(message, MMErrorException);
				return false;
			}
			continue; // skip empty lines
		}

		int nTokensFound = forTokenizeNoPushBack(line, tokens, "\t ");
		if (nTokensFound != nColumnsExpected) {
			std::string message = "ERROR reading climate file " + fn +
				 " in line " + std::to_string(linesRead + 1) +
				 ". Error decoding column/token number " + std::to_string
				 (nTokensFound) + ". Contents of line: " + line +
				 " . readClimateFileToPlot().";
			MMErrorMessage(message, MMErrorException);
		}
		// here put tokens to plot:
		plot->precipitation.push_back(tokens[0]);
		plot->temperature.push_back(tokens[1]);
		plot->irradiance.push_back(tokens[2]);
		plot->daylength.push_back(tokens[3]);
		plot->pet.push_back(tokens[4]);
		plot->CO2_concentration.push_back(tokens[5]);
	}
	while (iff.good());

	int rows = floor(T.End * 365 + 0.5);
	if (linesRead < rows) {
		std::string message = "ERROR: Climate file " + fn + " has only " +
			 std::to_string(linesRead) + " lines of data. You need at least " +
			 std::to_string(rows);
		MMErrorMessage(message, MMErrorException);
	}
	return true;
}

// --- Minimal environment writer (added) ---
bool RESULT::WriteEnvironmentOutput() {
	if (!myResultFileSwitch.env) return true;
	HecPointer hec = FirstHec;
	while (hec != NULL) {
		PlotPointer plot = hec->FirstPlot;
		while (plot != NULL) {
			fprintf(ENVIRONMENTFile, "%le\t%2d\t%2d\t%d\t",
				T.T, hec->HecNo, plot->No, plot->length_of_vegetation_periode);
			for (int i = 0; i < MAXGRP; i++) {
				float ir = 0.0f;
				if ((int)plot->mean_light_above_canopy.size() > i) ir = plot->mean_light_above_canopy[i];
				fprintf(ENVIRONMENTFile, "%le\t", (double)ir);
			}
			// RTresp (use temperature reduction as proxy for respiration reduction)
			double rtresp = 1.0;
			if (!plot->temp_photosynthese_reduction.empty()) rtresp = plot->temp_photosynthese_reduction[0];
			fprintf(ENVIRONMENTFile, "%le\t", rtresp);
			for (int i = 0; i < MAXGRP; i++) {
				double rtp = 1.0;
				if ((int)plot->temp_photosynthese_reduction.size() > i) rtp = plot->temp_photosynthese_reduction[i];
				fprintf(ENVIRONMENTFile, "%le\t", rtp);
			}
			fprintf(ENVIRONMENTFile, "%le\t", (double)plot->CO2_inc_GPP);
			double tmean = 0.0;
			if (!plot->temperature.empty()) {
				double s=0.0; for (double v: plot->temperature) s+=v; tmean = s/plot->temperature.size();
			}
			fprintf(ENVIRONMENTFile, "%le\t%le\n", tmean, tmean);
			plot = plot->next;
		}
		hec = hec->next;
	}
	fflush(ENVIRONMENTFile);
	return true;
}
