#ifndef __MMErrorMessage_h
#define __MMErrorMessage_h

/*
 HOWTO:
 MMErrorMessage("ERRROR: This can be any error text", 1);

 This will write the error message "ERRROR: This can be any error text"
 to cerr. Additionally the cpp file and the line are shown in the output.

 You can change the output path of the error message by changing the second
 argument to the function.

 e.g.:
 MMErrorMessage("ERRROR: This can be any error text", MMErrorException); // Exception

 valid values for the second argument can be found below as #defines under
  "// implemented error types:"

 You might want to define a variable in your Program so you can change the
 output path at one place in the program.

 This is still a very basic error functionality not including any error
 levels. You might want to define something more sophisticated in your
 application.
 e.g. like that:
 // here you can change the error output behaviour by changing the global vars:
 int minimumErrorLevelForErrorOutput=0;
 int errorOutputDevice=4; // 0=none, 1=cout, 2=cerr, 3=messagebox, 4=exception oder so.
 #define errorMessage(MESSAGE, LEVEL) \
 if(LEVEL>=minimumErrorLevelForErrorOutput) MMErrorMessage(MESSAGE,errorOutputDevice)
 #define errorMessage(MESSAGE) MMErrorMessage(MESSAGE,errorOutputDevice)

 MMErrorMessage is implemented as a macro. Because the Macro is in one line
 of code the correct __LINE__ is shown in the output.

 There is a macro and a function version of MMErrorMessageFunction in
 this file. Both have exactly the same code. The function version is here
 to have readable version. If you change the macro, please
 do change function as well.

 License: GPL 2.0
 Copyright: michael.mueller@toomai.de

 */

// enum MMErrorMessageType { ErrorMessageNone=0, ErrorMessageCerr=1, ErrorMessageCout=2, ErrorMessageFile=3, ErrorMessageException=4,  ErrorMessageBox=5};
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>  // exit(..)
#include <stdexcept>
// the followin macros call the function macro. Magic to automatically fill in file and line.

#define MMErrorMessage(MESSAGE, TYPE) MMErrorMessageFunction(MESSAGE, TYPE, __FILE__, __LINE__);
#define MMErrorMessage4(MESSAGE) MMErrorMessageFunction(MESSAGE, 4, __FILE__, __LINE__);

// implemented error types:

#define MMErrorNone 0
#define MMErrorCerr 1
#define MMErrorCout 2
#define MMErrorFile 3
#define MMErrorException 4
#define MMErrorCerrExit 5
#define MMErrorMessageBox 6
#define MMErrorMessageBoxExit 7

// Warning types emit "WARNING: " instead of "ERROR:"
#define MMWarningNone 10
#define MMWarningCerr 11
#define MMWarningCout 12
#define MMWarningFile 13
#define MMWarningException 14
#define MMWarningCerrExit 15
#define MMWarningMessageBox 16
#define MMWarningMessageBoxExit 17

/*
  Formind Standard:
  Error: 4 (cerr + exception)
  Warning: 11 (cerr);
*/


#ifndef __FORMIND_GUI__
inline int MyMessageBox(const char* text, const char* caption, int flags=0x00000000L) {
	std::cerr << "\n" << text << std::endl;
}
inline void guiExit(int i) {
	exit(i);
}
#else
#include "MyMessageBox.h"
#ifdef _WIN64
#include <windows.h>
inline void guiExit(int i) {
	ExitProcess(-1);
}
#else
inline void guiExit(int i) {
	exit(-1);
}
#endif
#endif


//#define MMERRORMESSAGEASFUNCTION____
#ifdef MMERRORMESSAGEASFUNCTION____
void inline MMErrorMessageFunction(std::string description, int outputtype, const char* cppfile, int cppline) {
	int thisOutputType=outputtype;
	if (thisOutputType != 0) {
		const char* filename = "ErrorMessage.txt";
		std::stringstream ss;
		std::string errorOrWarning;
		if(thisOutputType>=10) {
			errorOrWarning="WARNING";
			thisOutputType-=10;
		} else {
			errorOrWarning="ERROR: ";
		}
		ss << errorOrWarning << description;
		if (cppfile != NULL)
			ss << " . File: " << cppfile;
		if (cppline != 0)
			ss << "  Line: " << cppline;
		std::string errorString;
		getline(ss, errorString);
		if (thisOutputType == 1) {
			std::cerr << "\n" << errorString;
		}
		else if (thisOutputType == 2) {
			std::cout << "\n" << errorString;
		}
		else if (thisOutputType == 3) {
			static std::ofstream MMErrorMessageStream;
			if (!MMErrorMessageStream.is_open()) {
				MMErrorMessageStream.open(filename, std::ios::out | std::ios::app);
			}
			MMErrorMessageStream << errorString << std::endl;
		}
		else if (thisOutputType == 4) {
			std::cerr << "\n" << errorString << std::endl;
			throw std::runtime_error(errorString.c_str());
		}
		else if (thisOutputType == 5) {
			std::cerr << "\n" << errorString << std::endl;
			exit(-1);
		}
		else if (thisOutputType == 6) {
			MyMessageBox(errorString.c_str(), errorOrWarning.c_str());
		}
		else if (thisOutputType == 7) {
			MyMessageBox(errorString.c_str(), errorOrWarning.c_str());
			guiExit(-1);
		}
		else {
			throw std::runtime_error("ERROR MMErrorMessage: Outputtype > 7 not defined. Using exception !!! ERROR:" +
				 errorString);
		}
	}
}

#else

#define MMErrorMessageFunction(description, outputtype, cppfile, cppline) { \
	int thisOutputType=outputtype;   \
	if (thisOutputType != 0) {        \
		const char* filename = "ErrorMessage.txt"; \
		std::stringstream ss;                       \
		std::string errorOrWarning;                  \
		if(thisOutputType>=10) {                      \
			errorOrWarning="WARNING";                   \
			thisOutputType-=10;                          \
		} else {                                         \
			errorOrWarning="ERROR: ";                      \
		}                                                  \
		ss << errorOrWarning << description;                \
		if (cppfile != NULL)                                 \
			ss << " . File: " << cppfile;                      \
		if (cppline != 0)                                      \
			ss << "  Line: " << cppline;                         \
		std::string errorString;                                 \
		getline(ss, errorString);                                 \
		if (thisOutputType == 1) {                                 \
			std::cerr << "\n" << errorString;                        \
		}                                                            \
		else if (thisOutputType == 2) {       \
			std::cout << "\n" << errorString;   \
		}                                       \
		else if (thisOutputType == 3) {          \
			static std::ofstream MMErrorMessageStream; \
			if (!MMErrorMessageStream.is_open()) {      \
				MMErrorMessageStream.open(filename, std::ios::out | std::ios::app); \
			}                                                                       \
			MMErrorMessageStream << errorString << std::endl;                        \
		}                                                                            \
		else if (thisOutputType == 4) {                                               \
			std::cerr << "\n" << errorString << std::endl;   \
			throw std::runtime_error(errorString.c_str());    \
		}                                                     \
		else if (thisOutputType == 5) {                        \
			std::cerr << "\n" << errorString << std::endl;       \
			exit(-1);                                             \
		}                                                         \
		else if (thisOutputType == 6) {                            \
			MyMessageBox(errorString.c_str(), errorOrWarning.c_str());  \
		}                                                               \
		else if (thisOutputType == 7) {                                  \
			MyMessageBox(errorString.c_str(), errorOrWarning.c_str());     \
			guiExit(-1);                                                       \
		}                                                                   \
		else {                                                               \
			throw std::runtime_error("ERROR MMErrorMessage: Outputtype > 7 not defined. Using exception !!! ERROR:" \
			+ errorString); \
		} \
	}     \
}



#endif

#endif
