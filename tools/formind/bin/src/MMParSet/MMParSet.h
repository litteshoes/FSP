/*
 *** Useage: ***
 -- 0. Include MMParSet.h:
 #include "MMParSet.h"
 -- 1. Assume you have two variables you want to have managed by MMParSet
 int myint;
 std::string mystring;
 std::vector <double> myvec;
 -- 2. Create MMParSet object:
 MMParSet par;
 -- 3. Tell MMParSet about the variables:
 par.AddPar(myint);
 par.AddPar(mystring);
 -- 4. To read from file:
 std::ifstream infile("somefilename.par");
 infile>>par;
 -- 5. Wo write to file:
 std::ofstream outfile("anotherfilename.par");
 outfile<<par
 -- 6. You might want to read from the commandline (styleINI without spaces):
 par.ReadFromCommandline(argc, argv);


 *** File Formats / output styles :  ***
 - styleINI:
 This format is very similar to the traditional Windows INI format. E.g.:
 myint = 22
 mystring = "hallo du"
 Files can be very compact and still very readable.
 vectors/arrays are accepted in a C-style (JSON like) format, e.g.:
 myvec = {3.0, 7.3, 11.22, 21.44}
 More dimensions just use vectors of vetors, so a 2 dimensional vector would
 be something like that:
 myvec2D = { {1, 2, 3}, {9, 8, 7} }
 - styleSISI:
 This is a format developed mainly for the formind forest model. Please see
 the formind documentation for specific information or have a look at the
 example files.
 - styleSISINoIndent:
 The same as styleSISI, but arrays do not get indentation. This style is
 usually used internally for GUI output only.

 When reading a file the file usually MMParset can guess the file format. Thus
 one does not need to specify the format when reading a file.
 When writing a file the format which has been determined when reading is used.
 If no file was read before writing styleINI is used unless you specify a format
 manually using setOutputStyle(...).

 *** Additinal parameter properties: ***
 Each parameter has the following properties:
 - name
 - description
 - units
 - range  ( intervals (and their descriptions))
 - tags ( space seperated names for sorting or special actions.
 e.g. if \i 0 is used in formind parfile - "0" is the tag. )
 Useage of these properies is optional.


 *** Structure: ***
 MMParNodeBase - for beeing able to store MMParNode pointers
 MMParNode::MMParNodeBase <T> - stores pointers to parameters of all types
 MMParSet - keeps a vector of MMParNodeBase and provides methods to add/aaccess parameters of all types

 MMParNode::setValue does NOT check if the value is inside the range. Use MMParNode::isInRange to check for yourself.

 *** todo: ***

 */
// ---------------------------------------------------------------------------
#ifndef MMParSetH
#define MMParSetH
// ---------------------------------------------------------------------------
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>
#include <set>
#include <typeinfo>
#include <fstream>
#include <MMParSet/MMErrorMessage.h>
#include <MMParSet/MMInterval.h>
#include <MMParSet/MMValidValues.h>
#include <MMParSet/MMParHelperFunctions.h>
#include <MMParSet/MMParNode.h>
#include <MMParSet/MMStreamIO.h>
#include <algorithm>
// ---------------------------------------------------------------------------

#define MMParSetError(MESSAGE, TYPE) MMErrorMessage(MESSAGE, TYPE)

#define addPar(X) addByReference(X,#X);
#define addPar2(X,Y) addByReference(X,#X,Y);

// enum MMParSetErrorStyle { errorNONE, errorEXCEPTION, errorFILE };

class MMParSet {
	friend class MMParSetIO;

public:
	std::vector<MMParNodeBase*>nodes;

	std::string name;
	std::string description;

	int subsetLevel;
	// just for nice output format, could be deduced by parentSet
	MMParSet* parentSet;

	// vector<MMParNodeBase*> supplements;
	bool outputOnlyReadAndChanged;
protected:
	MMParSetOutputStyle outputStyle;

public:
	// int errorStyle and others.;
	MMParOptions options;

	// ------ Constructor/Destructor -----
	void initDefault() {
		outputStyle = styleINI;
		subsetLevel = 0;
		parentSet = NULL;
		name = "NN";
		outputOnlyReadAndChanged=false;
		// errorStyle=errorException;
	}

	MMParSet() {
		initDefault();
	}

	MMParSet(const char* n) {
		initDefault();
		name = n;
	}

	MMParSet(std::string n) {
		name = n;
		initDefault();
	}

	/*
	 MMParSet(MMParSet& pp) {
	 initDefault()
	 } */
	~MMParSet() {
		clear();
	}

	void clear() {
		// free with new allocated nodes:
		while (!nodes.empty()) {
			MMParNodeBase * tnp = nodes.back();
			if (tnp != NULL) {
				delete(tnp);
				tnp = NULL;
			}
			nodes.pop_back();
		}
	}

	// ------ Add Methods -------
	template<typename T>
	MMParNode<T> * addByReferenceTemplate(T& value, const char* name, const char* description = NULL) {
		// if name already in parset throw runtime error
		for (std::vector<MMParNodeBase*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			std::string ts = (*it)->name;
			if (ts == name) {
				std::string ts2 = "ERROR MMParSet.addByreference(): name \"" + ts + "\" already exists.";
				throw std::runtime_error(ts2);
			}
		}
		// now create new node and add:
		MMParNode<T> *pNewNode = new MMParNode<T>(value, name, description);
		if (pNewNode == NULL) {
			std::runtime_error("ERROR MMParSet: Cannot allocate memory with new. 2.");
		}
		//
		pNewNode->options = options; // inherit options
		nodes.push_back(pNewNode->getMMParNodeBasePointer());
		return pNewNode;
	}

	template<typename T>
	MMParNode<T> * addByReference(T& value, const char* name, const char* description = NULL) {
		return addByReferenceTemplate(value, name, description);
	}

	template<typename T>
	MMParNode<T> * addByReference(T& value, const char* name, T left, T right, const char* description = NULL,
		 const char* intervalDescription = NULL) {
		MMParNode<T> *mpp = addByReferenceTemplate(value, name, description);
		if (intervalDescription)
			mpp->addRangeInterval(left, right, intervalDescription);
		else
			mpp->addRangeInterval(left, right);
		return mpp;
	}

	MMParNode<MMParSet> * addByReference(MMParSet& value, const char* name, const char* description = NULL) {
		value.setOutputStyle(outputStyle); // subsets inherit my output style.
		value.subsetLevel = subsetLevel + 1;
		value.parentSet = this;
		MMParNode<MMParSet> *pnp = addByReferenceTemplate(value, name, description);
		pnp->type = typeParset;
		return pnp;
	}
	// ------ character string array special

	// todo : aeh - we might want to delete this function and do the array wrappter do the special string handling? ?????
	// this way now it still comes as an array - not wat we want.

	template<size_t N1>
	MMParNode < char[N1] > * addByReference(char(&value)[N1], const char* name, const char* description = NULL) {
		return addByReferenceTemplate(value, name, description);
	}

	// -------  c-array-specials -----
	template<typename T, size_t N1, size_t N2>
	MMParNode<mmArrayWrapper2D<T, N1, N2> > * addByReference(T(&a)[N1][N2], const char* name,
		 const char* description = NULL) {
		int k = 22;
		// if name already in parset throw runtime error
		for (std::vector<MMParNodeBase*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			std::string ts = (*it)->name;
			if (ts == name) {
				std::string ts2 = "ERROR MMParSet.addByreference(): name \"" + ts + "\" already exists.";
				throw std::runtime_error(ts2);
			}
		}
		// now create new node and add:
		MMParNode<mmArrayWrapper2D<T, N1, N2> > *pNewNode = new MMParNode<mmArrayWrapper2D<T, N1, N2> >();
		if (pNewNode == NULL) {
			std::runtime_error("ERROR MMParSet: Cannot allocate memory with new. 2.");
		}

		//////mmArrayWrapper < T, N1> wrap(a);
		//////pNewNode->setValue(wrap);  // need my own memory just for the wrapper structure.
		// the above setValue (myTypeid in setValue) does not work for some reason on 32bit Emba - thus the lines here:
		pNewNode->pValue = new mmArrayWrapper2D<T, N1, N2>(a);
		if (!pNewNode->pValue)
			std::runtime_error("ERROR MMParSet: Cannot allocate memory with new. 8");
		pNewNode->options = options; // inherit options - do this first
		pNewNode->pValueMemIsMine = true;
		pNewNode->valueTypes.push_back("array");
		pNewNode->options.dimensions.push_back(N1);
		pNewNode->options.dimensions.push_back(N2);
		T dummy;
		myTypeid(dummy, pNewNode->valueTypes, pNewNode->options.dimensions);
		//

		if (name)
			pNewNode->name = name;
		if (description)
			pNewNode->description = description;
		//

		nodes.push_back(pNewNode->getMMParNodeBasePointer());
		return pNewNode;
	}

	template<typename T, size_t N1>
	MMParNode<mmArrayWrapper<T, N1> > * addByReference(T(&a)[N1], const char* name, const char* description = NULL) {
		int k = 22;
		// if name already in parset throw runtime error
		for (std::vector<MMParNodeBase*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			std::string ts = (*it)->name;
			if (ts == name) {
				std::string ts2 = "ERROR MMParSet.addByreference(): name \"" + ts + "\" already exists.";
				throw std::runtime_error(ts2);
			}
		}
		// now create new node and add:
		MMParNode<mmArrayWrapper<T, N1> > *pNewNode = new MMParNode<mmArrayWrapper<T, N1> >();
		if (pNewNode == NULL) {
			std::runtime_error("ERROR MMParSet: Cannot allocate memory with new. 2.");
		}

		pNewNode->options = options; // inherit options

		//////mmArrayWrapper < T, N1> wrap(a);
		//////pNewNode->setValue(wrap);  // need my own memory just for the wrapper structure.
		// the above setValue (myTypeid in setValue) does not work for some reason on 32bit Emba - thus the lines here:
		pNewNode->pValue = new mmArrayWrapper<T, N1>(a);
		if (!pNewNode->pValue)
			std::runtime_error("ERROR MMParSet: Cannot allocate memory with new. 8");
		pNewNode->pValueMemIsMine = true;
		pNewNode->valueTypes.push_back("array");
		pNewNode->options.dimensions.push_back(N1);
		T dummy;
		myTypeid(dummy, pNewNode->valueTypes, pNewNode->options.dimensions);
		//

		if (name)
			pNewNode->name = name;
		if (description)
			pNewNode->description = description;
		//
		nodes.push_back(pNewNode->getMMParNodeBasePointer());
		return pNewNode;
	}

	// -------- pointer array specials ------- this is for sisi - assuming a pointer does no make sense an wants memory assigned.

	template<typename T>
	MMParNode<mmPointerArrayWrapper<T> > * addByReference(T* (&a), const char* name, const char* description = NULL) {
		// if name already in parset throw runtime error
		for (std::vector<MMParNodeBase*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			std::string ts = (*it)->name;
			if (ts == name) {
				std::string ts2 = "ERROR MMParSet.addByreference(): name \"" + ts + "\" already exists.";
				throw std::runtime_error(ts2);
			}
		}
		// now create new node and add:
		MMParNode<mmPointerArrayWrapper<T> > *pNewNode = new MMParNode<mmPointerArrayWrapper<T> >();
		if (pNewNode == NULL) {
			std::runtime_error("ERROR MMParSet: Cannot allocate memory with new. 2.");
		}

		//////mmArrayWrapper < T, N1> wrap(a);
		//////pNewNode->setValue(wrap);  // need my own memory just for the wrapper structure.
		// the above setValue (myTypeid in setValue) does not work for some reason on 32bit Emba - thus the lines here:
		pNewNode->pValue = new mmPointerArrayWrapper<T>(a);
		if (!pNewNode->pValue)
			std::runtime_error("ERROR MMParSet: Cannot allocate memory with new. 8");
		pNewNode->pValueMemIsMine = true;
		pNewNode->valueTypes.push_back("array");
		pNewNode->options.dimensions.push_back(pNewNode->pValue->bufsize);
		// which should be 0
		T dummy;
		myTypeid(dummy, pNewNode->valueTypes, pNewNode->options.dimensions);
		if (name)
			pNewNode->name = name;
		if (description)
			pNewNode->description = description;
		//
		pNewNode->options = options; // inherit options
		nodes.push_back(pNewNode->getMMParNodeBasePointer());
		return pNewNode;
	}

	template<typename T>
	MMParNode<mmPointerArrayWrapper2D<T> > * addByReference(T** (&a), const char* name, const char* description = NULL) {
		// if name already in parset throw runtime error
		for (std::vector<MMParNodeBase*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			std::string ts = (*it)->name;
			if (ts == name) {
				std::string ts2 = "ERROR MMParSet.addByreference(): name \"" + ts + "\" already exists.";
				throw std::runtime_error(ts2);
			}
		}
		// now create new node and add:
		MMParNode<mmPointerArrayWrapper2D<T> > *pNewNode = new MMParNode<mmPointerArrayWrapper2D<T> >();
		if (pNewNode == NULL) {
			std::runtime_error("ERROR MMParSet: Cannot allocate memory with new. 2.");
		}

		//////mmArrayWrapper < T, N1> wrap(a);
		//////pNewNode->setValue(wrap);  // need my own memory just for the wrapper structure.
		// the above setValue (myTypeid in setValue) does not work for some reason on 32bit Emba - thus the lines here:
		pNewNode->pValue = new mmPointerArrayWrapper2D<T>(a);
		if (!pNewNode->pValue)
			std::runtime_error("ERROR MMParSet: Cannot allocate memory with new. 8");
		pNewNode->pValueMemIsMine = true;
		pNewNode->valueTypes.push_back("array");
		pNewNode->options.dimensions.push_back(pNewNode->pValue->sizex);
		// which should be 0
		pNewNode->options.dimensions.push_back(pNewNode->pValue->sizey);
		// which should be 0

		T dummy;
		myTypeid(dummy, pNewNode->valueTypes, pNewNode->options.dimensions);
		//

		if (name)
			pNewNode->name = name;
		if (description)
			pNewNode->description = description;
		//
		pNewNode->options = options; // inherit options
		nodes.push_back(pNewNode->getMMParNodeBasePointer());
		return pNewNode;
	}

	// ------ Commandline special --------
	void ReadCommandLine(int argc, char** argv) {
		std::stringstream tstream;
		for (int i = 1; i < argc; i++) {
			std::string ts = argv[i];
			std::size_t pos = ts.find("=");
			if (pos != std::string::npos) {
				std::string parName = ts.substr(0, pos);
				if (pos + 1 >= ts.size())
					continue;
				std::string value = ts.substr(pos + 1, ts.size());
				MMParNodeBase* pNode = findByNameRecursive(parName);
				if (pNode == NULL) {
					std::string tError = "ERROR: MMParSet cannot find parameter with name \"" + name +
						 "\" . Reading commandline argumet \"" + ts + "\"";
					MMErrorMessage(tError, options.ErrorMessageType);
				}
				MMParOptions opt;
				opt.outputStyle = styleINI;
				if (!pNode->setValueFromString(value, &opt)) {
					std::string tError = "ERROR: MMParSet cannot set parameter with name \"" + name +
						 "\" . Reading commandline argumet \"" + ts + "\"";
					MMErrorMessage(tError, options.ErrorMessageType);
				}
			}
		}
	}

	// ------ Access Methods -------
	MMParNodeBase * findByName(std::string name) {
		for (std::vector<MMParNodeBase*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			if (name == (*it)->name)
				return *it;
		}
		return NULL;
	}

	MMParNodeBase * findByName(const char* name) {
		std::string ts = name;
		return findByName(ts);
	}

	MMParNodeBase * findByNameRecursive(std::string name) { // Achtung!: recursive functionality has not been tested !!!
		std::vector<MMParNodeBase*>parsetQueue;
		for (std::vector<MMParNodeBase*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			if ((*it)->type == typeParset) {
				parsetQueue.push_back(*it);
			}
			else if (name == (*it)->name)
				return *it;
		}
		for (std::vector<MMParNodeBase*>::iterator it2 = parsetQueue.begin(); it2 != parsetQueue.end(); it2++) {
			MMParSet* tpp;
			tpp = (MMParSet*)((*it2)->getValuePointer());
			MMParNodeBase* retval = findByNameRecursive(name);
			if (retval != NULL)
				return retval;
		}
		return NULL;
	}

	MMParSet* findParsetByFullNameRecursive(std::string fname) {
		for (std::vector<MMParNodeBase*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			if ((*it)->type == typeParset) {
				MMParSet* psp = (MMParSet*)((*it)->getValuePointer());
				if (psp->getFullName() == fname)
					return psp;
				MMParSet* psp2 = findParsetByFullNameRecursive(fname);
				if (psp2)
					return psp2;
			}
		}
		return NULL;
	}

	MMParNodeBase * findByNameRecursive(const char* name) {
		std::string ts = name;
		return findByNameRecursive(ts);
	}

	bool getByNameAsString(const char* name, std::string& value) {
		MMParNodeBase * tpp = findByName(name);
		if (tpp == NULL)
			return false;
		value = tpp->getValueAsString();
		return true;
	}

	template<typename T>
	bool getByName(const char* name, T& value) {
		MMParNode<T> *tpp = (MMParNode<T> *) findByName(name);
		if (tpp == NULL)
			return false;
		tpp->getValue(value);
		return true;
	}

	template<typename T>
	bool getByNameAutocast(const char* name, T& value) {
		MMParNodeBase * tpp = findByName(name);
		if (tpp == NULL)
			return false;
		std::stringstream ss;
		ss << tpp->getValueAsString();
		bool doThrowError = false;
		if (!(ss >> value)) {
			doThrowError = 1;
		}
		else {
			std::string ts;
			ss >> ts;
			if (ts.size() != 0)
				doThrowError = 2;
		}
		if (doThrowError) {
			std::string xs;
			std::stringstream xss;
			xss << tpp->getValueAsString();
			xss >> xs;
			throw std::runtime_error("ERROR MMParset getByNameAutocast(" + std::string(name) +
				 ", ... ): String conversion failed. String=\"" + xs + "\"");
		}
		return (ss >> value);
	}

	bool set(std::string name, std::string value) {
		MMParNodeBase *tpp = findByName(name);
		if (tpp == NULL)
			return false;
		if (!tpp->setValueFromString(value)) {
			return false;
		}
		return true;
	}

	void setOutputStyle(MMParSetOutputStyle s, bool recursive = true) {
		options.outputStyle = s;
		// all below is deprecated. If option carries output style we should not need it additionally
		// --- well , but we still should propagate options.outputStyle to all nodes.
		// mimu todo
		outputStyle = s;
		if (recursive) {
			for (std::vector<MMParNodeBase*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
				if ((*it)->type == typeParset) {
					MMParSet* psp = (MMParSet*)((*it)->getValuePointer());
					psp->setOutputStyle(s, true);
				}
				else {
					(*it)->options.outputStyle = s;
				}
			}
		}
	}

	MMParSetOutputStyle getOutputStyle() {
		return outputStyle;
	}

	// sets ErrorMessageType of all nodes of parset except parset nodes.
	// if recursive=true all parset nodes and subnodes are set as well.
	void setErrorMessageType(unsigned char _errorMessageType, bool recursive = true) {
		options.ErrorMessageType = _errorMessageType; // myself
		for (std::vector<MMParNodeBase*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			if ((*it)->type == typeParset) {
				if (recursive) {
					MMParSet* psp = (MMParSet*)((*it)->getValuePointer());
					psp->setErrorMessageType(_errorMessageType, true);
				}
			}
			else {
				(*it)->options.ErrorMessageType = _errorMessageType;
			}
		}

	}

	// --------------specials:
	std::string getFullName(std::string seperator = ".") { // including parents name:
		std::string fullname = name;
		MMParSet* tp = this;
		while (tp->parentSet != NULL) {
			tp = tp->parentSet;
			fullname = tp->name + seperator + fullname;
		}
		return fullname;
	}

	// return set of all tags used in all nodes
	// MIMU TODO : BESARE: recursive does not work yet !!!
	std::set<std::string>getAvailableTags(bool recursive = false, bool excludeTagsIfZeroTagIsUsed = false) {
		std::set<std::string>allTags;
		for (std::vector<MMParNodeBase*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			if ((*it)->type == typeParset && recursive == true) {
				MMParSet* psp = (MMParSet*)((*it)->getValuePointer());
				std::set<std::string>set2 = psp->getAvailableTags(recursive);
				std::set<std::string>::iterator it2;
				for (it2 = set2.begin(); it2 != set2.end(); it2++) {
					allTags.insert(*it2);
				}
			}
			else {
				if ((*it)->tags != "") {
					std::string newtags = (*it)->tags;
					// MiMU TODO 2016 split new tags.
					size_t pos = 0;
					while (pos != std::string::npos) {
						std::string ws = " \t\r\n";
						pos = newtags.find_first_not_of(ws, pos);
						if (pos == std::string::npos)
							break;
						size_t pos2 = newtags.find_first_of(ws, pos);
						if (pos2 == std::string::npos)
							pos2 = newtags.length();
						std::string newtag = newtags.substr(pos, pos2 - pos);
						if (excludeTagsIfZeroTagIsUsed == true) {
							if (newtag == std::string("0")) {
								break;
							}
						}
						allTags.insert(newtag);
						pos = pos2;
					}
				}
			}
		}
		return allTags;
	}

	// ------ < > operators - they do not make sense but are needed if parsets are added to parsets.
	bool operator < (const MMParSet & x) const {
		if (x.name > name)
			return true;
		else
			return false;
	}

	bool operator > (const MMParSet & x) const {
		if (x.name < name)
			return true;
		else
			return false;
	}

	bool operator == (const MMParSet & x) const {
		if (x.name == name)
			return true;
		else
			return false;
	}

	void sort() {
		std::sort(nodes.begin(), nodes.end(), sortFunctionMMParNodeBase);
	}
};

// this MUST come after declaration of parset. It needs to know parset:
#include <MMParSet/MMParSetIO.h>

// ------

// falls kein parset bezeichner in file werde die ersten eintraege automatisch in den
// parset eingelesen
// schoen waere es, dass der parset wenn er selber keinen Namen hat zusaetzlich
// noch die subparsets ohne .dot benennen koennte und auch so schreiben und einlesen.
// Und schoen waere es auch, wenn die Namensvererbung nicht von der reihenfolge von
// Addpar(parset) abhaengen wuerde

#endif
