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
// File         						for_io.h
// Description  	              File input and output functions of FORMIND.
//
///////////////////////////////////////////////////////////////////


#ifndef  for_ioH
#define  for_ioH

#include "for_global.h"
#include "for_var.h"

class PIN;
typedef PIN*PinPointer;

bool SaveSimInitialisation(const char*FileName);
bool ReadPlotSpecification(const char*FileName);
bool ReadTableFunctions(void);
bool ReadSimFile(const char*FileName);
bool ReadParFile(void);
bool ReadInFiles(void);
bool ReadTerrainElevation(void);
bool ReadTerrainSlope(void);
bool ReadTerrainForestType(void);
bool ReadLandslideParameters(void);
bool DistributePIN2Pointer(PinPointer, int, int);
void UpdateMinMax(double*, double*, double*, double*);
void doFileOpen(FILE**, char*);

void WritePIN(FILE*);

bool readClimateFileToPlot(std::string fn, PLOT* plot);

// read pin-file
class PINFileReader {
	std::string whites;

public:
	PINFileReader(bool verbose) : whites(" \t\r\n"), Pin(NULL),
		 m_verbose(verbose), noPlotsRead(0) {

	};

	~PINFileReader() {
		FreeResources();
	};
	bool readPINFile(const std::string&fname);
	bool writePINFileFromPin(const std::string&fname, std::string regionheader =
		 "resaved using PINFileReader::writePINFileFromPin");
	// for GUI. Beware: Does write Pin, NOT current plots!

	template<typename T>
	T StringToNumber(const std::string&numberAsString);

	int noPlotsRead;
	PIN*Pin;

private:
	void FreeResources();
	void readSafeLine(std::string&line);
	void splitKey(const std::string&s, std::string&key, std::string&val);
	void PINError(const std::string&s);
	void readSplitKey(std::string&key, std::string&val);
	std::string getNextToken(const std::string&s, size_t&pos);
	static void Tokenize(const std::string&str, std::vector<std::string>&tokens,
		 const std::string&delimiters = " ");
	std::string deleteWhite(const std::string&s, size_t i1, size_t i2);
	void deleteWhite(std::string&s);
	std::string extractString(const std::string&s);
	std::string line;

	bool m_verbose;
	size_t currentLineNumber;

	std::ifstream pinfile;
};

#endif
