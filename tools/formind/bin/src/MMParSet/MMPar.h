#ifndef __MMPAR_H
#define __MMPAR_H

/*
 Easy to use Parameters/Variables/Data I/O

 HowTo:

 e.g.: You have some variables and want file I/O in inifile style

 int myint;
 double mydoublearray[100];
 string mystring;


 0. Include MMPar.h

 #include <MMPar.h>

 1. Create an instance of MMParSet. This instance must live at least as long
 as the parameters/variables added to it. Usually one global instance is
 a good start:

 MMParSet par;

 2. Tell the MMParSet which parameters/variables it should administer.
 Tell it once at the start of the program.
 ( e.g. inside main() or the Form Constructor e.g. Form1::Form1(..) )

 par.addPar(myint);
 par.addPar(mydoublearry);
 par.addPar(mystring);

 3. to read the parameters/variables from file just use the normal stream
 operators:

 ifstream infile("input.txt");
 infile>>par;

 4. to write the parmeters/variables to a file use the normal stream
 operators:

 ofstream outfile("output.txt");
 outfile<<par;


 Optional:
 3a. When reading from a file MMPar tries to guess the format and remember it
 for writing. If MMPar did not read a file yet, it will use inifile style
 as output format. To change the default output format:


 Things worth knowing:


 -> MMParSet usually does not keep its own parameters/variables. It just keeps
 pointers to the variables in the program. Once exception: If a pointer
 to a basic type like int is given to MMParSet, it will allocate
 the memory needed when reading from file. Similar for pointer to pointer
 (2D arrays). Not implemented for 3D and more, because its is just there
 for Sisi compatibility.

 -> Error Handling: MMParSet gives an error if the file contains a parameter
 which cannot be found in the parset. It does NOT give an error if not all
 of the parameters in the parset are found in the file.
 Additionally it tries to give useful error messages if it does
 not understand things in the file.
 Error output is an exception by default (with additional cerr output).
 This can be changed. Error


 Structure:
 each parameter has the following properties:
 - name
 - description
 - units
 - range  ( intervals (and their descriptions))

 structure:
 MMParNodeBase - for beeing able to store MMParNode pointers
 MMParNode::MMParNodeBase <T> - stores pointers to parameters of all types
 MMParSet - keeps a vector of MMParNodeBase and provides methods to add/aaccess parameters of all types

 MMParNode::setValue does NOT check if the value is inside the range. Use MMParNode::isInRange to check for yourself.

 MMParSetIO : Input and output for MMParSet
 MMStreamIO : intended to be more general Input/Output - but beacuse of Sisi
 still specailized for MMParSet.

 MMErrorMessage: Just for giving ErrorMessages which jumt to the right line
 in the code when debugging but still give useful
 error messsages and choice of method (exception or not)

 MMPar maeks extensive use of templates. It has been testet with different
 Embarcdero Compilers in 32 and 64 bit.

 For help ask michael.mueller@ufz.de
 */

#include <MMParSet/MMParSet.h>
#include <MMParSet/MMParSetIO.h>

#endif
