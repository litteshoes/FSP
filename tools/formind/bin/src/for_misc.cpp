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
// File						for_misc.cpp
// Description				____
//
///////////////////////////////////////////////////////////////////

#include <for_misc.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <time.h>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <MMParSet/MMErrorMessage.h>
#include "for_var.h"
#include <cstring>
#include <string>

// for mkdir:
#if (defined(_WIN32) || defined(_WIN64))
#if (defined _MSC_VER)
#include <direct.h>
#define mkdir _mkdir
#else
#include <dir.h>
#endif
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

// --------------------------------------------------------------------

/*!
	\brief	Removes last byte
*/
void removeTrailing(std::string& path, const char c) {
	if (path.length() > 0) {
		std::string::iterator it = path.end() - 1;
		if (*it == c) {
			path.erase(it);
		}
	}
}

size_t for_strlen_s(char* s, int maxSize) {
	size_t i=0;
	while(s[i++]!=0) {
		if(i>=maxSize)
			return maxSize;
	}
	return i;
}

bool stod_secure(std::string s, double& outputValue) {
	char* endptr;
	if(s.length()==0) {
		return false;
	}
	double d=strtod (s.c_str(), &endptr);
	int nLeftOver=for_strlen_s(endptr, 1024)-1;
	if(nLeftOver>0)
		return false;
	outputValue=d;
	return true;
}

#ifndef __NO_FORMIND_STANDALONE__
// --------------------------------------------------------------------

/*!
	\brief	Prints progress bar in the shell
*/
void for_progress(bool FinalLine) {
	// Give some output of progress made during calculation:
	int counter = 1;
	if (T.T == T.Start) {
		// Some space after the summary:
		std::cout << std::endl;
		std::cout << std::endl;
	}
	else {
		// Make progress bar:

		int AllreadyDone = ((double)T.T / (double)T.End) * 100.0;
		int BarPos = AllreadyDone * 0.58;
		if (AllreadyDone % 5 == 1 || FinalLine) {
			std::cout << '\r';
			std::cout << "[";
			while (counter <= 58) {
				if (counter == 25) {
					std::cout << " " << AllreadyDone << " % ";
					counter += 4 + floor(log10((double) abs(AllreadyDone))) + 1;
				}
				else if (counter < BarPos) {
					std::cout << "=";
					counter++;
				}
				else if (counter == BarPos) {
					std::cout << ">";
					counter++;
				}
				else if (counter > BarPos) {
					std::cout << " ";
					counter++;
				}
			}
			if (FinalLine) {
				std::cout << "]" << std::endl;
				std::cout << std::flush;
				std::cout << std::endl;
			}
			else
				std::cout << "]";
		}
	}
}
#endif



#if defined(_WIN64) || defined(_WIN32)
 // --------------------------------------------------------------------

/*!
	\brief  returns directory without filename (name + extension) for windows compiler
*/

std::string for_dirname(std::string path) {
	char tdrive[_MAX_DRIVE * 2];
	char tdir[4096]; // _MAX_DIR usually is 256 which might not be long enough
	char tfile[4096];
	// _MAX_FNAME usually is 256 which might not be long enough
	char text[4096]; // _MAX_EXT usually is 256 which might not be long enough
	_splitpath(path.c_str(), tdrive, tdir, tfile, text);
	std::string result = std::string(tdrive);
	result += std::string(tdir);
	removeTrailing(result, '\\');
	return result;
}

 // --------------------------------------------------------------------

/*!
	\brief  returns only filename (name + extension) with without directory for windows compiler
*/
std::string for_basename(std::string path) {
	char tdrive[_MAX_DRIVE * 2];
	char tdir[4096]; // _MAX_DIR usually is 256 which might not be long enough
	char tfile[4096];
	// _MAX_FNAME usually is 256 which might not be long enough
	char text[4096]; // _MAX_EXT usually is 256 which might not be long enough
	_splitpath(path.c_str(), tdrive, tdir, tfile, text);
	std::string result = std::string(tfile);
	result += std::string(text);
	return result;
}
#else
#include <libgen.h>
#include <string.h>
 // --------------------------------------------------------------------

/*!
	\brief  returns directory without filename (name + extension) for unix compiler
*/

std::string for_dirname(std::string path) {
	size_t length = path.length() + 2;
	char* pathTMP = new char[length];
	if (!pathTMP)
		return NULL;
	strcpy(pathTMP, path.c_str());
	char* resultTMP = dirname(pathTMP);
	std::string result = resultTMP;
	delete[]pathTMP;
	return result;
}

 // --------------------------------------------------------------------

/*!
	\brief  returns only filename (name + extension) with without directory for unix compiler
*/
std::string for_basename(std::string path) {
	size_t length = path.length() + 2;
	char* pathTMP = new char[length];
	if (!pathTMP)
		return NULL;
	strcpy(pathTMP, path.c_str());
	char* resultTMP = basename(pathTMP);
	std::string result = resultTMP;
	delete[]pathTMP;
	return result;
}
#endif

// --------------------------------------------------------------------

/*!
	\brief  removes extension of filename.
*/
std::string removeExtension(const std::string fn) {
	// fn.erase(s.find_last_of("."), string::npos);
	size_t lastdot = fn.find_last_of(".");
	if (lastdot == std::string::npos)
		return fn;
	else
		return fn.substr(0, lastdot);
}

 // --------------------------------------------------------------------

/*!
	\brief returns extension of filename without the dot
*/
std::string extractExtension(const std::string fn) {
	size_t lastdot = fn.find_last_of(".");
	if (lastdot == std::string::npos)
		return std::string("");
	else
		return fn.substr(lastdot+1, fn.length() - lastdot -1 );
}

// --------------------------------------------------------------------

/*!
	\brief converts \\ to / for windows
*/
std::string makeNicePathForYourOS(std::string path) {
#if defined(_WIN64) || defined(_WIN32)
	std::replace(path.begin(), path.end(), '/', '\\');
	return path;
#else

// --------------------------------------------------------------------

/*!
	\brief converts \\ to / for unix
*/
	std::replace(path.begin(), path.end(), '\\', '/');
	return path;
#endif
}

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

// --------------------------------------------------------------------

/*!
	\brief	checks if directory exists
	\param	char
*/
bool dirExists(const char *path) {
	struct stat info;

	if (stat(path, &info) != 0)
		return false;
	else if (info.st_mode & S_IFDIR)
		return true;
	else
		return false;
}

/*!
	\brief	checks if directory exists
	\param	string
*/
bool dirExists(std::string dir) {
	return dirExists(dir.c_str());
}

// --------------------------------------------------------------------
/*!
	\brief	create a directory (linux, windows)
*/
bool myCreateDirectory(std::string dir) {
#if defined(_WIN64) || defined(_WIN32)
	if (0 == mkdir(dir.c_str()))
		return true;
	else
		return false;
#else
	// 递归创建父目录
	std::string::size_type pos = 0;
	std::string current_dir = "";

	// 确保路径以/开头
	if (dir.empty() || dir[0] != '/') {
		return false;
	}

	// 逐级创建目录
	while ((pos = dir.find('/', pos + 1)) != std::string::npos) {
		current_dir = dir.substr(0, pos);
		if (current_dir.length() > 1) { // 避免创建根目录
			if (0 != mkdir(current_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
				if (errno != EEXIST) {
					return false;
				}
			}
		}
	}

	// 创建目标目录
	if (0 == mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
		return true;
	else if (errno == EEXIST)
		return true; // 目录已存在也算成功
	else
		return false;
#endif
}

// --------------------------------------------------------------------
#include <dirent.h>

/*!
	\brief	get parfile from directory
*/
bool getParFileFromDirectory(std::string dirFullPath,
	 std::string& parFileNameFullPath) {
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(dirFullPath.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			std::string foundFileName = ent->d_name;
			if (extractExtension(foundFileName) == std::string(".par")) {
				parFileNameFullPath =
					 makeNicePathForYourOS(dirFullPath + "\\" + foundFileName);
				closedir(dir);
				return true;
			}
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		return false;
	}
	return false;
}
//------------------------------------------------------------------------------
/*!
	\brief	print all the files and directories within directory
*/
bool getInfoFileFromDirectory(std::string dirFullPath,
	 std::string& fileNameFullPath) {
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(dirFullPath.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			std::string foundFileName = ent->d_name;
			if (extractExtension(foundFileName) == std::string(".txt")) {
				fileNameFullPath =
					 makeNicePathForYourOS(dirFullPath + "\\" + foundFileName);
				closedir(dir);
				return true;
			}
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		return false;
	}
	return false;
}

//------------------------------------------------------------------------------
/*!
	\brief	returns absoöute path of file
*/
std::string getInfoFileNameFromParFileName(std::string fileNameFullPath) {
	std::string infoFileNameAbsolutePath;
	fileNameFullPath = for_dirname(fileNameFullPath);
	bool ok = getInfoFileFromDirectory(fileNameFullPath + "\\..\\..",
		 infoFileNameAbsolutePath);
	if (!ok) {
		return std::string("");
	}
	else
		return infoFileNameAbsolutePath;
}

// --------------------------------------------------------------------

/*!
	\brief	checks if file exists
*/
bool myFileExists(std::string fn) {
	std::ifstream iff;
	iff.open(fn.c_str());
	if (iff.is_open()) {
		iff.close();
		return true;
	}
	return false;
}

// --------------------------------------------------------------------

/*!
	\brief	checks if project-folder is present in the current directory
*/
bool formindProjectDirIsSubdirOf(std::string dir) {
	std::string fn = dir;
	if (dir.size() > 0)
		fn += "\\";
	fn += "formind-projects\\projects_menue.txt";
	fn = makeNicePathForYourOS(fn);
	return myFileExists(fn);
}

// --------------------------------------------------------------------

/*!
	\brief	checks if model-folder is present in the current directory
*/
bool formindModelDirIsSubdirOf(std::string dir) {
	std::string fn = dir;
	if (dir.size() > 0)
		fn += "\\";
	fn += "formind-model\\src\\for_misc.cpp";
	fn = makeNicePathForYourOS(fn);
	return myFileExists(fn);
}

// --------------------------------------------------------------------

/*!
	\brief	searches for project-folder and returns path
*/
std::string findFormindProjectDir(std::string _dir)
{ // starting eg. with exe file name
	std::string dir = _dir;
	// 1. check current dir:
	if (formindProjectDirIsSubdirOf(dir)) {
		dir += "\\formind-projects";
		dir = makeNicePathForYourOS(dir);
		return dir;
	}
	for (int i = 0; i < 15; i++)
	{ // go back 15 directories maximum. Stupid workaround as long as there is no function to resolve paths.
		dir += "\\..";
		if (formindProjectDirIsSubdirOf(dir)) {
			dir += "\\formind-projects";
			dir = makeNicePathForYourOS(dir);
			return dir;
		}
	}
	return std::string(""); // not found.
}

// ---------------------------------------------------------------

/*!
	\brief	searches for formind-folder and returns absolute path
*/
std::string findFormindDirAbsolutePath(std::string startDir) {
	std::string dir = startDir;
	// go back 15 directories maximum. Stupid workaround as long as there is no function to resolve paths.
	for (int i = 0; i < 15; i++) {
		if (formindProjectDirIsSubdirOf(dir) && formindModelDirIsSubdirOf(dir)) {
			return makeNicePathForYourOS(dir);
		}
		dir += "\\..";
	}
	return std::string(""); // not found.
}
// ---------------------------------------------------------------

std::string currentDateTime()
	 /* !
	  \brief          Inserts current Date and Time
	  \param	      	void
	  \return         const string
	  */
{
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y_%m_%d_%H_%M_%S", &tstruct);

	return buf;
}

// ---------------------------------------------------------------

void Add_Ext(char *FileName, const char *Ext)
	 /* !
	  \brief          Adds  <Ext> to <FileName> if not yet existing
	  \param	      	char *FileName
	  \param 		  	const char *Ext
	  \return         void
	  */
{
	char *p;

	p = strrchr(FileName, '.');
	if (NULL == p) {
		strcat(FileName, ".");
		strcat(FileName, Ext);
		return;
	}
	if (FileName == p) { // '.' at  1. place
		strcpy(FileName + 1, Ext);
		return;
	}
	if (*(p - 1) == '.') { // = '..'
		strcat(FileName, ".");
		strcat(FileName, Ext);
		return;
	}
	strcpy(p + 1, Ext);
}

#if (defined(_WIN32) || defined(_WIN64))
#include <windows.h>

#if (defined UNICODE)

// ---------------------------------------------------------------

/*!
	\brief	returns absolute path of executable for unix
*/
std::string getFilenameAbsolutePathOfExecutable(const char* argv0) {
	TCHAR ownPth[MAX_PATH * 2];
	// Will contain exe path
	HMODULE hModule = GetModuleHandle(NULL);
	if (hModule != NULL) {
		GetModuleFileName(hModule, ownPth, (sizeof(ownPth)));
		std::wstring tws = ownPth;
		std::string ts(tws.begin(), tws.end());
		return std::string(ts);
	}
	else {
		return std::string("");
	}
}
#else

// ---------------------------------------------------------------

/*!
	\brief	returns absolute path of executable for windows
*/
std::string getFilenameAbsolutePathOfExecutable(const char* argv0) {
	char ownPth[MAX_PATH];
	// Will contain exe path
	HMODULE hModule = GetModuleHandle(NULL);
	if (hModule != NULL) {
		GetModuleFileName(hModule, ownPth, (sizeof(ownPth)));
		return std::string(ownPth);
	}
	else {
		return std::string("");
	}
}
#endif
#else

// ---------------------------------------------------------------

/*!
	\brief	changes char into string
*/
std::string getFilenameAbsolutePathOfExecutable(const char* argv0) {
	return std::string(argv0);
}
#endif

// --------------------------------------------------------------------

/*!
	\brief	constructor
*/
forFileNames::forFileNames() {
	emptyCharString=new char[256];  // could be 1
	emptyCharString[0]=0;
	result_file_prefix=std::string("");
	parFileNameAbsolutePath="";
}

/*!
	\brief	destructor
*/

forFileNames::~forFileNames() {
	if(emptyCharString!=NULL) {
		delete [] emptyCharString;
      emptyCharString = NULL;
	}
}

// --------------------------------------------------------------------

/*!
	\brief	call after init only or inside init after exeFileDirAbsolutePath
				and others are defined
*/
void forFileNames::setParFileName(std::string pfn) {
	bool isAbsolutePath = false;
	if (pfn.size() > 1) {
		if (pfn[1] == ':') {
			parFileNameAbsolutePath = pfn;
			isAbsolutePath = true;
		}
		if (pfn[0] == '\\') {
			parFileNameAbsolutePath = exeFileDirAbsolutePath.substr(0, 2) + pfn;
			isAbsolutePath = true;
		}
		if (pfn[0] == '/') { // e.g. Linux
			parFileNameAbsolutePath = pfn;
			isAbsolutePath = true;
		}
	}
	if (!isAbsolutePath)
	{
		parFileNameAbsolutePath = exeFileDirAbsolutePath + "\\" + pfn;
	}
	parFileNameAbsolutePath = makeNicePathForYourOS(parFileNameAbsolutePath);
	parFileDirAbsolutePath = for_dirname(parFileNameAbsolutePath);
	parFileDirAbsolutePath = makeNicePathForYourOS(parFileDirAbsolutePath);
	parFileNameNoPath = for_basename(parFileNameAbsolutePath);
	parFileNameNoPathNoExtension = removeExtension(parFileNameNoPath);
	resultDirAbsolutePath = parFileDirAbsolutePath + "\\..\\results";
	resultDirAbsolutePath = makeNicePathForYourOS(resultDirAbsolutePath);
	infoFileNameAbsolutePath = getInfoFileNameFromParFileName
		 (parFileNameAbsolutePath);
}

bool forFileNames::hasParFileName() {
	if(parFileNameAbsolutePath!=std::string(""))
		return true;
	else
		return false;
}

/* !
	\brief	Initialyzes all most file and directory names. Expects arguments
				as given to main(int myargc, char*myargv[]).
	/param 	doNotAsk If no parameterfile was given on commandline and
				doNotAsk==true the virtual parametrisation is choosen and no questions asked.
				If doNotAsk==false (default) a dialog is shown where you cann choose a
				parametrisation of your liking.

 */

void forFileNames::init(int myargc, char*myargv[], bool doNotAsk) {
	if (myargc < 1) {
		std::runtime_error
			 ("forFileNames::init(,) has been called with wrong arguments. myargc needs to be >=1."
			 );
	}
	// exeFileNameAbsolutePath = myargv[0];
	exeFileNameAbsolutePath = getFilenameAbsolutePathOfExecutable(myargv[0]);
	exeFileDirAbsolutePath = for_dirname(exeFileNameAbsolutePath);
	std::string exeFileNameNoPath = for_basename(exeFileNameAbsolutePath);
	std::string exeFileNameNoPathNoExtension =
		 removeExtension(exeFileNameNoPath);

	// check if parfile was given on commandline:
	autoFindParFile = true;
	if (myargc >= 2) {
		std::string parFileArgument = myargv[1];
		// empty quoted strings and strings containing "=" cannot be parfile names
		if (parFileArgument != std::string("")  && parFileArgument.find("=") == std::string::npos) {
			parFileNameAbsolutePath = "";
			myargv[1]=emptyCharString;
			autoFindParFile = false;
			setParFileName(parFileArgument);
		}
	}

	// find formind Dir and others:
	formindDirAbsolutePath = findFormindDirAbsolutePath(exeFileDirAbsolutePath);
	projectDirAbsolutePath = findFormindProjectDir(exeFileDirAbsolutePath);

	if (autoFindParFile) {
		bool parFileFound = false;

#ifdef thisworksButResultDirectoryWillBeSubdirectoryOfParDirectory
		// 1. try if file with extension .par exists in same directory as exe

		if (getParFileFromDirectory(exeFileDirAbsolutePath,
			 parFileNameAbsolutePath)) {
			parFileDirAbsolutePath = exeFileDirAbsolutePath;
			parFileDirAbsolutePath = makeNicePathForYourOS(parFileDirAbsolutePath);
			resultDirAbsolutePath = exeFileDirAbsolutePath + "\\results";
			resultDirAbsolutePath = makeNicePathForYourOS(resultDirAbsolutePath);
			parFileFound = true;
		}
#endif
		// 2. try if file with extension .par exists in formind_parameters
		// (subdirectory of executable)

		std::string testDir = exeFileDirAbsolutePath + "\\formind_parameters";
		testDir = makeNicePathForYourOS(testDir);
		bool foundParFileInSubdirectory = false;
		if (dirExists(testDir)) {
			if (getParFileFromDirectory(testDir, parFileNameAbsolutePath)) {
				foundParFileInSubdirectory = true;
				parFileDirAbsolutePath = testDir;
				parFileDirAbsolutePath =
					 makeNicePathForYourOS(parFileDirAbsolutePath);
				resultDirAbsolutePath = exeFileDirAbsolutePath + "\\results";
				resultDirAbsolutePath =
					 makeNicePathForYourOS(resultDirAbsolutePath);
				parFileFound = true;
			}
		}

		// 3. try to find formind-projects directory
		// default parameter new formind file structure 2014/15:
		if (!parFileFound) {
			if (projectDirAbsolutePath == std::string("")) {
				std::cerr <<
					 "ERROR (forFileNames): Could not determine Project directory." <<
					 std::endl;
			}
			else {
				std::ifstream iff(getMenueFileNameAbsolutePath().c_str());
				std::string ts;
				while (std::getline(iff, ts)) {
					std::stringstream ss;
					ss << ts;
					ss >> ts;
					if (ts.size() <= 0)
						break;
					if (ts[0] == '#')
						continue;
					std::vector<std::string>tsv;
					tsv.resize(5);
					tsv[0] = ts;
					for (int i = 1; i < 5; i++) {
						ss >> tsv[i];
					}
					if (tsv[3] == "1")
						menue.push_back(tsv);
				}

				parFileDirAbsolutePath = projectDirAbsolutePath +
					 "\\Tropical_general\\virtualTropicalForest_3pft\\formind_parameters";
				parFileDirAbsolutePath =
					 makeNicePathForYourOS(parFileDirAbsolutePath);
				parFileNameAbsolutePath = parFileDirAbsolutePath + "\\virtual.par";
				parFileNameAbsolutePath =
					 makeNicePathForYourOS(parFileNameAbsolutePath);
				resultDirAbsolutePath = projectDirAbsolutePath +
					 "\\Tropical_general\\virtualTropicalForest_3pft\\results";
				resultDirAbsolutePath =
					 makeNicePathForYourOS(resultDirAbsolutePath);
				parFileFound = true;
			}
		}

		if (parFileFound) { // = menue file found
			if (!doNotAsk) {
				std::cout <<
					 "WARNING! No Parameter file was given on command line. You can choose one of the following projects: " <<
					 std::endl << std::endl;
				for (int i = 0; i < menue.size(); i++) {
					std::cout << i << "\t" << menue[i][0] << " \t";
					if (menue[i][0].length() <= 13)
						std::cout << "\t";
					if (menue[i][0].length() <= 7)
						std::cout << "\t";
					std::cout << menue[i][1] << "\t";
					if (menue[i][1].length() <= 13)
						std::cout << "\t";
					if (menue[i][1].length() <= 7)
						std::cout << "\t";
					std::cout << menue[i][2] << std::endl;
				}
				std::cout << std::endl <<
					 "Enter the number of the project you want to use (\"x\" or nothing to exit): ";
				std::cout.flush();
				std::string parfileChooser;
				std::cin >> parfileChooser;
				if (parfileChooser.length() <= 0)
					exit(-1);
				if (parfileChooser[0] == 'x')
					exit(-1);
				int parfileChooserInt;
				std::stringstream tss7;
				tss7 << parfileChooser;
				tss7 >> parfileChooserInt;
				if (parfileChooserInt < 0 || parfileChooserInt >= menue.size()) {
					std::cout << std::endl <<
						 "Wrong number. Game over. 0 life left. " << std::endl;
					exit(-1);
				}


				std::cout << std::endl;
				std::cout << parfileChooserInt << "\t" << menue[parfileChooserInt]
					 [0] << " \t" << menue[parfileChooserInt][1] << std::endl;

				parFileDirAbsolutePath = projectDirAbsolutePath + "\\" +
					 menue[parfileChooserInt][0] + "\\" +
					 menue[parfileChooserInt][1] + "\\formind_parameters";
				parFileDirAbsolutePath =
					 makeNicePathForYourOS(parFileDirAbsolutePath);
				if (!getParFileFromDirectory(parFileDirAbsolutePath,
					 parFileNameAbsolutePath)) {
					std::cout <<
						 "Cannot find parfile for menue[parfileChooserInt][0]" <<
						 " \t" << menue[parfileChooserInt][1] << std::endl;
					exit(-1);
				}
				parFileNameAbsolutePath =
					 makeNicePathForYourOS(parFileNameAbsolutePath);
				resultDirAbsolutePath = projectDirAbsolutePath + "\\" +
					 menue[parfileChooserInt][0] + "\\" +
					 menue[parfileChooserInt][1] + "\\results";
				resultDirAbsolutePath =
					 makeNicePathForYourOS(resultDirAbsolutePath);


				std::cout << std::endl;
				std::cout << parFileNameAbsolutePath << std::endl;
				std::cout << std::endl <<
					 "Press y or j + Enter if you want to continue. " <<
					 "Press any other character to exit: ";


				std::cout.flush();
				char dummyCharacter;
				std::cin >> dummyCharacter;
				if (dummyCharacter != 'y' && dummyCharacter != 'j') {
					std::cout << "Exiting ..." << std::endl;
					exit(-1);
				}
			}
		}
		else {
			std::cerr <<
				 "ERROR (forFileNames): Could not determine parameter file (*.par). \n ";
			std::cerr << " Exiting ..." << std::endl;
			exit(-1);
		}

	}
}

// --------------------------------------------------------------------

/*!
	\brief	set pin-file name
*/
void forFileNames::setPinFileName(std::string pfn) {
	bool isAbsolutePath = false;
	if (pfn.size() > 1) {
		if (pfn[1] == ':') {
			pinFileNameAbsolutePath = pfn;
			isAbsolutePath = true;
		}
		if (pfn[0] == '\\') {
			pinFileNameAbsolutePath = exeFileDirAbsolutePath.substr(0, 2) + pfn;
			isAbsolutePath = true;
		}
	}
	if (!isAbsolutePath)
	{
		pinFileNameAbsolutePath = getParFileDirAbsolutePath() + "\\" + pfn;
	}
	pinFileNameAbsolutePath = makeNicePathForYourOS(pinFileNameAbsolutePath);
}

std::string forFileNames::getExeFileDirAbsolutePath() {
	return exeFileDirAbsolutePath;
}

std::string forFileNames::getExeFileNameAbsolutePath() {
	return exeFileNameAbsolutePath;
}

std::string forFileNames::getParFileNameAbsolutePath() {
	return parFileNameAbsolutePath;
}

std::string forFileNames::getParFileDirAbsolutePath() {
	return parFileDirAbsolutePath;
}

std::string forFileNames::getParFileNameNoPathNoExtension() {
	return parFileNameNoPathNoExtension;
}

std::string forFileNames::getParFileNameNoPath() {
	return parFileNameNoPath;
}

std::string forFileNames::getInvFileNameAbsolutePath(std::string invFileNameNoPath) {
	std::string ts = getPinFileDirAbsolutePath() + directorySeparator + invFileNameNoPath;
	return makeNicePathForYourOS(ts);
}

std::string forFileNames::getInitPoolsFileNameAbsolutePath() {
	std::string ts = getParFileDirAbsolutePath() + "/" +
		 getParFileNameNoPathNoExtension() + ".initpools";
	return makeNicePathForYourOS(ts);
}

std::string forFileNames::getResultDirAbsolutePath() {
	resultDirAbsolutePath = parFileDirAbsolutePath + directorySeparator + resultDirectory ; // resultdir
	resultDirAbsolutePath = makeNicePathForYourOS(resultDirAbsolutePath);
	return resultDirAbsolutePath;
}

bool forFileNames::hasParFileArgument() {
	return !autoFindParFile;
}

std::string forFileNames::getPinFileNameAbsolutePath() {
	return pinFileNameAbsolutePath;
}

std::string forFileNames::getPinFileDirAbsolutePath() {
	return for_dirname(pinFileNameAbsolutePath);
}

std::string forFileNames::getErrorFileNameAbsolutePath() {
	std::string parFileNameWithoutExtensionNoPath =
		 for_basename(getParFileNameAbsolutePath());
	parFileNameWithoutExtensionNoPath =
		 removeExtension(parFileNameWithoutExtensionNoPath) + ".err";
	std::string errfn = getResultDirAbsolutePath() + "\\" +
		 parFileNameWithoutExtensionNoPath;
	return makeNicePathForYourOS(getResultDirAbsolutePath() + "\\formind.err");
}

std::string forFileNames::getOutputFileNameAbsolutePath() {
	std::string parFileNameWithoutExtensionNoPath =
		 for_basename(getParFileNameAbsolutePath());
	parFileNameWithoutExtensionNoPath =
		 removeExtension(parFileNameWithoutExtensionNoPath) + ".out";
	std::string outfn = getResultDirAbsolutePath() + "\\" +
		 parFileNameWithoutExtensionNoPath;

	return makeNicePathForYourOS(getResultDirAbsolutePath() + "\\formind.out");
}

std::string forFileNames::getClimateFileNameAbsolutePath
	 (std::string climateName) {
	std::string climateFileNameAbsolutePath = getParFileDirAbsolutePath() +
		 "\\Climate\\" + climateName;
	return makeNicePathForYourOS(climateFileNameAbsolutePath);
}

std::string forFileNames::getSoilFileNameAbsolutePath(std::string soilName) {
	std::string soilFileNameAbsolutePath = getParFileDirAbsolutePath() +
		 "\\Soil\\" + soilName;
	return makeNicePathForYourOS(soilFileNameAbsolutePath);
}

std::string forFileNames::getManagementFileNameAbsolutePath
	 (std::string managementName) {
	std::string managementFileNameAbsolutePath = getParFileDirAbsolutePath() +
		 "\\Management\\" + managementName;
	return makeNicePathForYourOS(managementFileNameAbsolutePath);
}

std::string forFileNames::getFormindDirAbsolutePath() {
	return formindDirAbsolutePath;
}

std::string forFileNames::getProjectsDirAbsolutePath() {
	return projectDirAbsolutePath;
}

std::string forFileNames::getRDirAbsolutePath() {
	std::string RDirAbsolutePath = getFormindDirAbsolutePath() +
		 "\\formind-analysis\\R-Portable\\App\\R-Portable";
	return makeNicePathForYourOS(RDirAbsolutePath);
}

std::string forFileNames::getRLibraryDirAbsolutePath() {
	std::string RDirAbsolutePath = getRDirAbsolutePath() + "\\library";
	return makeNicePathForYourOS(RDirAbsolutePath);
}

std::string forFileNames::getMenueFileNameAbsolutePath() {
	std::string menueFileNameAbsolutePath =
		 makeNicePathForYourOS(getProjectsDirAbsolutePath() +
		 "\\projects_menue.txt");
	return makeNicePathForYourOS(menueFileNameAbsolutePath);
}

void forFileNames::setResultFilePrefix(std::string s){
	result_file_prefix = s;
}

std::string forFileNames::getResultFileNameAbsolutePath (std::string fileExtension) {
	std::string ts3 = getResultDirAbsolutePath() + "/" + result_file_prefix +
		 getParFileNameNoPathNoExtension() + "." + fileExtension;
	return (makeNicePathForYourOS(ts3));
}

std::string forFileNames::getParFileNameAbsolutePathFromMenueFileIndex
	 (int parfileChooserInt) {
	std::string tempParFileDirAbsolutePath = projectDirAbsolutePath + "\\" +
		 menue[parfileChooserInt][0] + "\\" + menue[parfileChooserInt][1] +
		 "\\formind_parameters";
	tempParFileDirAbsolutePath =
		 makeNicePathForYourOS(tempParFileDirAbsolutePath);
	std::string tempParFileNameAbsolutePath;
	if (!getParFileFromDirectory(tempParFileDirAbsolutePath,
		 tempParFileNameAbsolutePath)) {
		std::cout << "Cannot find parfile for menue[parfileChooserInt][0]" <<
			 " \t" << menue[parfileChooserInt][1] << std::endl;
		exit(-1);
	}
	tempParFileNameAbsolutePath =
		 makeNicePathForYourOS(tempParFileNameAbsolutePath);
	return tempParFileNameAbsolutePath;
}

void forFileNames::setParFileNameAbsolutePathFromMenueFileIndex
	 (std::string index) {
	int parfileChooserInt; // =std::stoi(parfileChooser);
	std::stringstream tss7;
	tss7 << index;
	tss7 >> parfileChooserInt;
	parFileDirAbsolutePath = projectDirAbsolutePath + "\\" +
		 menue[parfileChooserInt][0] + "\\" + menue[parfileChooserInt][1] +
		 "\\formind_parameters";
	parFileDirAbsolutePath = makeNicePathForYourOS(parFileDirAbsolutePath);
	if (!getParFileFromDirectory(parFileDirAbsolutePath,
		 parFileNameAbsolutePath)) {
		std::cout << "Cannot find parfile for menue[parfileChooserInt][0]" <<
			 " \t" << menue[parfileChooserInt][1] << std::endl;
		exit(-1);
	}
	parFileNameAbsolutePath = makeNicePathForYourOS(parFileNameAbsolutePath);
	setParFileName(parFileNameAbsolutePath);
}

std::string forFileNames::getInfoFileNameAbsolutePath() {
	return infoFileNameAbsolutePath;
}

forFileNames fileNames; // global object - constructor initializes file names.

// --------------------------------------------------------------------
// --------------- resultFileReader -----------------------------------
// --------------------------------------------------------------------

/*
	\brief	Tokenizer (similar to tokenizer in PinFileReader class).
				Splits string by delimiters into tokens and saves them in vector.
				Vector is filled with Push_back()
*/
void forTokenize(const std::string&str, std::vector<std::string>&tokens,
	 const std::string&delimiters) {
	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos) {
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

 /*
	\brief	Tokenizer - double .
				Splits string by delimiters into tokens and saves them in vector.
				Same as above, but reads double into vector<double>.
				Vector is filled by Push_back()
 */

//void forTokenize(const std::string&str, std::vector<double>&tokens,
//	 const std::string&delimiters) {
//	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0); 	// Skip delimiters at beginning.
//	std::string::size_type pos = str.find_first_of(delimiters, lastPos); 	// Find first "non-delimiter".
//
//	while (std::string::npos != pos || std::string::npos != lastPos) {
//		// Found a token, convert it to a double and add it to the vector.
//		tokens.push_back(stod(str.substr(lastPos, pos - lastPos)));
//		// Skip delimiters.  Note the "not_of"
//		lastPos = str.find_first_not_of(delimiters, pos);
//		// Find next "non-delimiter"
//		pos = str.find_first_of(delimiters, lastPos);
//	}
//}

int forTokenize(const std::string&str, std::vector<double>&tokens, const std::string&delimiters) {
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0); // Skip delimiters at beginning.
	std::string::size_type pos = str.find_first_of(delimiters, lastPos); // Find first "non-delimiter".
	int index = 0;
	tokens.clear();
	while (std::string::npos != pos || std::string::npos != lastPos) {
		// Found a token, add it to the vector.
		double value;
		std::string valueString=str.substr(lastPos, pos - lastPos);
		if(!stod_secure(valueString, value)) {
			return tokens.size();
		}
		tokens.push_back(value);
		lastPos = str.find_first_not_of(delimiters, pos); // Skip delimiters.  Note the "not_of"
		pos = str.find_first_of(delimiters, lastPos); // Find next "non-delimiter"
	}
	return tokens.size();
}



/*
	\brief	Tokenizer - by index, string
				Splits string by delimiters into tokens and saves them in vector.
				*Vector is filled by index*
				Returns number of successful extracted tokens (and more if line too long)
 */
int forTokenizeNoPushBack(const std::string&str, std::vector<std::string>&tokens, const std::string&delimiters) {
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0); // Skip delimiters at beginning.
	std::string::size_type pos = str.find_first_of(delimiters, lastPos); // Find first "non-delimiter".
	int index = 0;
	int NTokensExpected=tokens.size();
	int tokenCounter=0;
	while (std::string::npos != pos || std::string::npos != lastPos) {
		// Found a token, add it to the vector.
		tokenCounter++;
		if(tokenCounter > NTokensExpected) {
			return tokenCounter;
		}
		tokens[index++] = str.substr(lastPos, pos - lastPos);
		lastPos = str.find_first_not_of(delimiters, pos); // Skip delimiters.  Note the "not_of"
		pos = str.find_first_of(delimiters, lastPos); // Find next "non-delimiter"
	}
	return tokenCounter;
}

/*
	\brief	Tokenizer - by index, double
				Splits string by delimiters into tokens and saves them in vector.
				Vector of double is filled by index.
				Returns number of successful extracted tokens (and more if line too long)
 */
int forTokenizeNoPushBack(const std::string&str,
	std::vector<double>&tokens, const std::string&delimiters) {
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0); // Skip delimiters at beginning.
	std::string::size_type pos = str.find_first_of(delimiters, lastPos); // Find first "non-delimiter".
	int index = 0;
	int NTokensExpected=tokens.size();
	int tokenCounter=0;
	while (std::string::npos != pos || std::string::npos != lastPos) {
		// Found a token, add it to the vector.
		tokenCounter++;
		if(tokenCounter > NTokensExpected) {
			return tokenCounter;
		}
		double value;
		std::string valueString=str.substr(lastPos, pos - lastPos);
		if(!stod_secure(valueString, value)) {
			return tokenCounter;
		}
		tokens[index++] = value;
		lastPos = str.find_first_not_of(delimiters, pos); // Skip delimiters.  Note the "not_of"
		pos = str.find_first_of(delimiters, lastPos); // Find next "non-delimiter"
	}
	return tokenCounter;
}

/*
	\brief	Helper function - Embarcadero 32bit does not know std:stod.
 */
#if ((defined __BORLANDC__))
#ifdef __WIN32__
#include <sstream>

namespace std {
	double stod(const std::string & __str, size_t* __idx, int __base) {
		if (__idx != 0 || __base != 10) {
			throw std::runtime_error
				 ("ERROR in dummy stod() for 32bit Embarcadero: Arguments cannot be changed and must be like this: __idx = 0, __base=10"
				 );
		}

		std::stringstream tss;
		tss << __str;
		double d;
		tss >> d;
		if (tss.fail()) {
			throw std::runtime_error
				 ("ERROR in dummy stod() for 32bit Embarcadero: Cannot convert: "
				 + __str);
		}
		return d;
	}
}
#endif
#endif

/*
	\brief	Reads formind result files, reads all columns. Expects the first line to
				be a description, the second line to be a tab-separated list of column units
				and the third line to be tab-separated column headers.
				All lines after are tab (or space)-separated double values.
 */
bool resultFileReader::openFile(std::string _fn, bool throwErrorIfNotExists) {
	fn = _fn;
	iff.open(fn.c_str());
	if (!iff.is_open()) {
		if(throwErrorIfNotExists) {
			std::string message="Cannot open result file" + fn + ". resultFileReader::openFile()";
			MMErrorMessage(message, N_Par.ErrorType);
			return false;
		} else {
			return false;
		}
	}
	// 1. find first line with number entries :
	std::vector<std::string> firstLines;
	std::vector<double> dummyvalues;
	bool eofReached=false;
	size_t pos=0;
	for(int i=0;i<4;i++) {
		std::string ts;
		if(!iff.good()) {
			eofReached=true;
		}
		pos=iff.tellg();
		std::getline(iff, ts);
		if(forTokenize(ts, dummyvalues, "\t") ==0 ) {
			firstLines.push_back(ts);
		}
		if (dummyvalues.size()!=0)
			break;
	}
	if(firstLines.size()<1) {
		std::string message="File" + fn + "should contain at least one column header line (tab seperated).";
		MMErrorMessage(message, N_Par.ErrorType);
	}
	int nTokensFirstValueLine=dummyvalues.size();
	iff.seekg(pos, std::ios_base::beg);

	// 2. work on already read lines - allow 3 line (comment, units, headers)
	//    and 1 line (headers)
	headers.clear();
	units.clear();
	if(firstLines.size() == 1 || firstLines.size()==3) {
		forTokenize(firstLines[firstLines.size()-1], headers, "\t");
			if(firstLines.size()==3) {
				forTokenize(firstLines[firstLines.size()-2], units, "\t");
				if(units.size()!=headers.size()) {
						std::string message="File" + fn + ": Number of column headers does not correspond to number of units.";
					MMErrorMessage(message, N_Par.ErrorType);
				}
				description=firstLines[0];
			}
	}
	// allocate memory:
	nCol = headers.size();
	valuesstring.resize(nCol);
	validValues.resize(nCol);
	if(values.size()!=nCol) {
		values.resize(nCol);
	}
	return true;
}

bool resultFileReader::getNext() {
	std::string ts;
	std::getline(iff, ts);

	if (iff.fail() || ts=="")
		return false;
	forTokenizeNoPushBack(ts, valuesstring, " \t");
	for (int i = 0; i < valuesstring.size(); i++) {

		try {
			if (valuesstring[i] == "NA" || valuesstring[i] == "NaN" ||
				 valuesstring[i] == "NAN" || valuesstring[i] == "-NA" ||
				 valuesstring[i] == "-NaN" || valuesstring[i] == "-NAN") {
				values[i] = 0.0;
				validValues[i] = false;
			}
			else {
				values[i] = stod(valuesstring[i]);
				validValues[i] = true;
			}
		}
		catch (...) {
			validValues[i] = false;
		}
	}
	return true;
}

void resultFileReader::closeFile() {
	iff.close();
}
/*
	\brief	Using second line of result-files as header in the GUI.
 */

std::string resultFileReader::getHeadingGUI() {
	std::size_t pos;
	if ((pos = description.find("]")) != std::string::npos) {
		return description.substr(0, pos + 1);
	}
	else if ((pos = description.find("(")) != std::string::npos) {
		return description.substr(0, pos);
	}
	else {
		return description;
	}
}

std::string resultFileReader::getColumnHeaderWithUnitsGUIFriendly(int col) {
	return units[col];
}

int resultFileReader::getNumberOfPFT() {
	int nPFT = 0;
	for (int i = 0; i < headers.size(); i++) {
		if (headers[i].find("PFT") != std::string::npos) {
			++nPFT;
		}
	}
	return nPFT;
}

int resultFileReader::getNumberOfColumns() {
	return headers.size();
}

std::string resultFileReader::getXAxisTitleGUI() {
	if (units.size() > 0)
		return units[0];
	else
		return (std::string("-"));
}

std::string resultFileReader::getYAxisTitleGUI() {
	if (units.size() > 0)
		return units[1];
	else
		return (std::string("-"));
}

int resultFileReader::getColumnIndex(const std::string & columnHeaderName) {
	for (int i = 0; i < headers.size(); i++) {
		if (headers[i] == columnHeaderName) {
			return i;
		}
	}
	std::string message="columnHeaderName "+columnHeaderName+" not found in result file " + fn + ".";
	MMErrorMessage(message, N_Par.ErrorType);
	return -1;
}


// -----------------------------------------------------------
// ----------------- end of for_misc.cpp -------------------
// -----------------------------------------------------------



