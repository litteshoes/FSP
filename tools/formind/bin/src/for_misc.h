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

#ifndef __FOR_MISC_H
#define __FOR_MISC_H
#include <string>
#include <stdlib.h>
#include <vector>

/*
 - functions for path and directory manipulation
 - class for finding formind parameter files and deriving formind directories

 */

 #define __NO_FORMIND_STANDALONE__


 #if defined(_WIN64) || defined(_WIN32)
const char directorySeparator = '\\';
#else
const char directorySeparator = '/';
#endif

/* !
 \brief          little progressbar for the commandline
 \author		 Sebastian Paulick
 \date           2016-10-21
 */
 void for_progress(bool);

/* !
 \brief          changes slashes to backslashes or vice versa according to Operating System used
 \author		  Michael M�ller
 \date           2015-06-10
 */
std::string makeNicePathForYourOS(std::string path);

/* !
 \brief          returns the directory part of a path. Not including trailing slashes (mostly). portable version, works on linux as well.
 \author		  Michael M�ller
 \date           2015-06-10
 */
std::string for_dirname(std::string path);

/* !
 \brief          returns the filename part (including extension) of a path. portable version, works on linux as well.
 \author		  Michael M�ller
 \date           2015-06-10
 */
std::string for_basename(std::string path);

/* !
 \brief          removes extension of a filename (everything after last dot)
 \author		  Michael M�ller
 \date           2015-06-10
 */
std::string removeExtension(const std::string fn);

/* !
 \brief       returns extension of filename without the dot
 \author		  Michael M�ller
 \date           2015-06-10
 */
std::string extractExtension(const std::string fn);


/* !
 \brief          creates a directory. portable version, works on linux as well.
 \author		  Michael M�ller
 \date           2015-06-10
 */
////std::string removeLastDirectory(const std::string fn); // only in string

std::string currentDateTime();

void Add_Ext(char *, const char *);

bool myCreateDirectory(std::string dir);

void removeTrailing(std::string& path, const char c = '\\');

#if defined(_WIN64) && defined (__BORLANDC__) && ( __BORLANDC__ <= 0x0680 )
// -------------string conversion for backwards compatibility ------------
// mainly to_string for int type

#include <sstream>
#include <iomanip>

template<typename T1, typename T2>
inline T2 to_somestring(T1 value, T2 s, int precision = 0) {
	std::stringstream tss;
	if (precision != 0)
		tss << std::setprecision(precision) << value;
	else
		tss << value;
	tss >> s;
	return s;
}

namespace std {
	inline std::string to_string(int x) {
		std::string s;
		return to_somestring(x, s);
	}
	inline std::string to_string(unsigned int x) {
		std::string s;
		return to_somestring(x, s);
	}
	inline std::string to_string(long x) {
		std::string s;
		return to_somestring(x, s);
	}
	inline std::string to_string(unsigned long x) {
		std::string s;
		return to_somestring(x, s);
	}
	inline std::string to_string(float x) {
		std::string s;
		return to_somestring(x, s);
	}
	inline std::string to_string(double x) {
		std::string s;
		return to_somestring(x, s);
	}
}
/* might be nice as well - but as it is not used I leave it as a comment: */
/*
 template <typename T>
 AnsiString to_AnsiString(T x, int precision=0) {
 AnsiString s;
 return to_somestring(x, s, precision);
 }
 */
#endif


#if (__cplusplus <= 201103L) && defined(__WIN32__) && defined(__BORLANDC__)
#ifndef __MM_TO_STRING_WORKAROUND__
#define __MM_TO_STRING_WORKAROUND__
#include <sstream>
namespace std {
	template<typename T>
	std::string to_string(T value) {
		std::stringstream ss;
		ss << value;
		std::string ts;
		ss >> ts;
		return ts;
	}
};
#endif
#endif


// resize vector of vector KEEPING existing values on enlargement
template <class T>
void vectorVectorResize(std::vector <std::vector <T> > & matrix, size_t sizeX, size_t sizeY) {
	matrix.resize(sizeX);
	for(size_t i=0;i<matrix.size();i++)
		matrix[i].resize(sizeY);
}

std::string getInfoFileNameFromParFileName(std::string fileNameFullPath);

// ------------------------- formind directories -------------------------

/* !
 \brief       All filenames in one place. With additional functionality
 to automatically find the default parameter files if no
 file was given on command line.
 Planned: Menue of all possible parameter files if no file was
 given on command line.
 \author		  Michael M�ller
 \date        2015-10-01
 */

class forFileNames {
public:
	forFileNames();
	~forFileNames();

	// initialyse using commandline parameters:
	void init(int myargc, char*myargv[], bool doNotAsk = false);

	// these will always be available after init:
	std::string getExeFileDirAbsolutePath();
	std::string getExeFileNameAbsolutePath();

	// these should be available after init, but they might return empty strings
	// if parfile has not been found:
	std::string getParFileDirAbsolutePath();
	std::string getParFileNameAbsolutePath();
	std::string getParFileNameNoPathNoExtension();
	std::string getParFileNameNoPath();
	std::string getInvFileNameAbsolutePath(std::string invFileNameNoPath);
	std::string getInitPoolsFileNameAbsolutePath();


	std::string getResultDirAbsolutePath();
	std::string getResultFileNameAbsolutePath(std::string fileExtension);
		// e.g.: getResultFileNameAbsolutePath("lai") will return full path of file.

	// If set this string s will be prepended to all result file names (not paths).
	// Use before using getResultFileNameAbsolutePath(..)
	void setResultFilePrefix(std::string s);

	std::string getInfoFileNameAbsolutePath();

	std::string getErrorFileNameAbsolutePath();
	std::string getOutputFileNameAbsolutePath();

	std::string getClimateFileNameAbsolutePath(std::string climateName);
	std::string getSoilFileNameAbsolutePath(std::string soilName);
	std::string getManagementFileNameAbsolutePath(std::string managementName);

	// Software:
	std::string getRDirAbsolutePath();
	std::string getRLibraryDirAbsolutePath();

	// this is known only after using setPinFileName, which is done in forio.init
	std::string getPinFileNameAbsolutePath();
	std::string getPinFileDirAbsolutePath();

	bool hasParFileArgument(); //on command line
	bool hasParFileName(); // has name of parfile (and usually most other names)

	// --- for advanced use only. do not call without calling init() first.

	// absolut path or path relative to exe is accepted
	void setParFileName(std::string path);

	// this does not change anything inside forFileNAmes:
	std::string getParFileNameAbsolutePathFromMenueFileIndex(int);
	// but this does:
	void setParFileNameAbsolutePathFromMenueFileIndex(std::string index);

	//     absolut path or path relative to ParFileDir is accepted
	void setPinFileName(std::string path);

	/*
	 typically use setPinFileName after reading the parameters, e.g.:
	 fileNames.init(argc, argv);
	 forio.init();
	 forio.readParFile(fileNames.getParFileNameAbsolutePath());
	 fileNames.setPinFileName(PinFileNameX);
	 */

	std::vector< std::vector <std::string > > menue;

	private:
	// these are always found from commandline parameters:
	std::string exeFileDirAbsolutePath;
	std::string exeFileNameAbsolutePath;

	// these can be found from commandline parameters. If not found they are empty.
	std::string parFileNameAbsolutePath;
	std::string parFileDirAbsolutePath;
	std::string parFileNameNoPathNoExtension;
	std::string parFileNameNoPath;
	std::string resultDirAbsolutePath;
	std::string projectDirAbsolutePath;
	std::string formindDirAbsolutePath;

	std::string infoFileNameAbsolutePath;

	// needs to be set externally.
	std::string pinFileNameAbsolutePath;
	bool autoFindParFile;
	std::string result_file_prefix; // is prefixed to all result file names. e.g. time.

	// not used yet outside , but here they are:
	std::string getFormindDirAbsolutePath();
	std::string getProjectsDirAbsolutePath();
	std::string getMenueFileNameAbsolutePath();

	char* emptyCharString; // dummy - do not change
};

extern forFileNames fileNames;

// ------------------------- portable timer -------------------------
// for linux remember to add -lrt to your makefile/project file
#if (  ( defined (_WIN32) || defined(WIN32) ) && !defined __MINGW32__  )
#include <windows.h>
#ifdef __BORLANDC__
#include <mmsystem.h>
#endif

class portableTimer {
public:
	void start() {
		startTimeGetTime = timeGetTime();
	}

	void stop() {
		stopTimeGetTime = timeGetTime();
	}

	double getTimeDifference() { // system time difference in seconds
		return (stopTimeGetTime - startTimeGetTime) / 1000.0;
	}

private:
	DWORD startTimeGetTime;
	DWORD stopTimeGetTime;
};
#elif __APPLE__
#include <mach/mach_time.h>
#define ORWL_NANO (+1.0E-9)
#define ORWL_GIGA UINT64_C(1000000000)

class portableTimer {
public:
	void start() {

		startTime = orwl_gettime();
		// clock_gettime(CLOCK_MONOTONIC, &startTime);
	}

	void stop() {
		stopTime = orwl_gettime();
		// clock_gettime(CLOCK_MONOTONIC, &stopTime);
	}

	double getTimeDifference() { // system time difference in seconds
		timespec timeDifference = calcTimespecDifference(startTime, stopTime);
		double seconds = double(timeDifference.tv_sec) +
			 timeDifference.tv_nsec / 1000000000.0;
		return seconds;
	}

	portableTimer() {
		orwl_timebase = 0.0;
		orwl_timestart = 0;
	}

private:
	double orwl_timebase;
	uint64_t orwl_timestart;

	timespec startTime;
	timespec stopTime;

	struct timespec orwl_gettime(void) {
		// be more careful in a multithreaded environement
		if (!orwl_timestart) {
			mach_timebase_info_data_t tb = {0};
			mach_timebase_info(&tb);
			orwl_timebase = tb.numer;
			orwl_timebase /= tb.denom;
			orwl_timestart = mach_absolute_time();
		}

		struct timespec t;

		double diff = (mach_absolute_time() - orwl_timestart) * orwl_timebase;
		t.tv_sec = diff * ORWL_NANO;
		t.tv_nsec = diff - (t.tv_sec * ORWL_GIGA);

		return t;
	}

	timespec calcTimespecDifference(timespec start, timespec stop) {
		timespec temp;
		if ((stop.tv_nsec - start.tv_nsec) < 0) {
			temp.tv_sec = stop.tv_sec - start.tv_sec - 1;
			temp.tv_nsec = 1000000000 + stop.tv_nsec - start.tv_nsec;
		}
		else {
			temp.tv_sec = stop.tv_sec - start.tv_sec;
			temp.tv_nsec = stop.tv_nsec - start.tv_nsec;
		}
		return temp;
	}
};
#else
#include <time.h>

class portableTimer {
public:
	void start() {
		clock_gettime(CLOCK_MONOTONIC, &startTime);
	}

	void stop() {
		clock_gettime(CLOCK_MONOTONIC, &stopTime);
	}

	double getTimeDifference() { // system time difference in seconds
		timespec timeDifference = calcTimespecDifference(startTime, stopTime);
		double seconds = double(timeDifference.tv_sec) +
			 timeDifference.tv_nsec / 1000000000.0;
		return seconds;
	}

private:
	timespec startTime;
	timespec stopTime;

	timespec calcTimespecDifference(timespec start, timespec stop) {
		timespec temp;
		if ((stop.tv_nsec - start.tv_nsec) < 0) {
			temp.tv_sec = stop.tv_sec - start.tv_sec - 1;
			temp.tv_nsec = 1000000000 + stop.tv_nsec - start.tv_nsec;
		}
		else {
			temp.tv_sec = stop.tv_sec - start.tv_sec;
			temp.tv_nsec = stop.tv_nsec - start.tv_nsec;
		}
		return temp;
	}
};
#endif


// --------------------------------------------------------------------
// --------------- resultFileReader -----------------------------------
// --------------------------------------------------------------------

#include <fstream>
/*
   Tokenizer (same as in PinFileReader class).
   Splits string by delimiters into tokens and saves them in vector.
*/
void forTokenize(const std::string&str, std::vector<std::string>&tokens, const std::string&delimiters=" \t");

/*
	Helper function - Embarcadero 32bit does not know std:stod.
*/
#if ((defined __BORLANDC__))
#ifdef __WIN32__
#include <sstream>
namespace std {
	inline double stod(const std::string& __str, size_t* __idx = 0, int __base = 10);
}
#endif
#endif


/*
	Tokenizer.
	Splits string by delimiters into tokens and saves them in vector.
	Same as above, but reads double into vector<double>.
*/
inline int forTokenize(const std::string&str, std::vector<double>&tokens, const std::string&delimiters=" \t");

/*
	Reads formind result files, reads all columns. Expects the first line to
	be a description, the second line to be a tab-separated list of column units
	and the third line to be tab-separated column headers.
	All lines after are tab (or space)-separated double values.

	If string in file is NA or NaN or NAN the value will be 0.

	Not much error detection yet!!! todo.
*/
class resultFileReader {
	public:
	std::string description;
	std::vector <std::string> units;
	std::vector <std::string> headers;
	std::vector <double> values;
	 std::vector <bool> validValues;
	std::string fn;
	std::ifstream iff;
	unsigned int nCol;
	bool openFile(std::string _fn, bool throwErrorIfNotExists=true);
	bool getNext();
	void closeFile();
	int getColumnIndex(const std::string & columnHeaderName);
	std::string getHeadingGUI();
	std::string getColumnHeaderWithUnitsGUIFriendly(int col);
	std::string getXAxisTitleGUI();
	std::string getYAxisTitleGUI();
	int getNumberOfPFT();
	int getNumberOfColumns();
	protected:
	std::vector<std::string>valuesstring;
};

class PLOT;
bool readClimateFileToPlot(std::string fn, PLOT* plot);

int forTokenizeNoPushBack(const std::string&str, std::vector<std::string>&tokens, const std::string&delimiters);
int forTokenizeNoPushBack(const std::string&str, std::vector<double>&tokens, const std::string&delimiters);


// extern portableTimer formindTimer;

#endif
