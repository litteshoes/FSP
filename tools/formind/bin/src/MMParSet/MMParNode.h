#ifndef __MMPARNODE_H
#define __MMPARNODE_H
#include <MMParSet/MMStreamIO.h>
#include <climits>
#ifdef _WIN64
#pragma option -w-
#endif
#ifdef _WIN32
#pragma warn -8058
#endif
// --------------------- Start of the real thing -------------------------------------------------

#define MMParNodeAlreadyReadFromStream 0x01
#define MMParNodeChanged 0x02

enum MMParNodeType {
	typeValue, typeParset, typeComment
};

class MMParNodeBase {
public:
	std::string name;
	std::string description;
	std::string longDescription;
	std::string units;
	std::string range;
	std::string tags;
	unsigned char statusFlags;  // binary flags. 0x02 changed, 0x01 alreadyReadFromFile, ...

	bool isConstant; // no Editing possible - works in GUI Editor only - for others nyi.

	//
	std::vector<std::string>valueTypes;
	// these types are only needed for SISI so we can output the types.

	std::vector<std::string>comments;
	// all comments before one node in file (at the moment 2015-06-30 sisi only)

	unsigned long output_position;
	// file position order.

	MMParNodeType type;
	MMParOptions options;

	// return statements below just to suppress compiler warnings. No meaning.
	virtual ~MMParNodeBase() {
	}

	MMParNodeBase() {
		output_position = ULONG_MAX;
		isConstant = false;
      statusFlags=0x00;
	}

	bool isConst() {
		return isConstant;
	}

	virtual std::string getValueAsString() const {
		std::string ts;
		return ts;
	};

	virtual void * getValuePointer() {
		return NULL;
	};

	virtual bool isInRange() {
		return false;
	};

	virtual bool isParset() {
		return false;
	};

	virtual bool isComment() {
		return false;
	};

	virtual bool hasRange() {
		return false;
	};

//	virtual void valueIntoStream(std::ostream &o) const {
//	};

	virtual void valueIntoStream(std::ostream &o) {
	};

	virtual void rangeIntoStream(std::ostream &o, MMParSetOutputStyle os = styleINI) const {
	};

	virtual MMParSetOutputStyle getOutputStyle() {
		MMParSetOutputStyle ts;
		return ts;
	};

//	virtual void valueFromStream(std::istream &in) const {
//	};

	virtual void valueFromStream(std::istream &in) {
	};

	virtual bool rangeFromStream(std::istream &in) {
		getLineNoCR(in, range);
		return true;
	};

	// ------- get/set properties -----------
	// they all return the node pointer. So they can be used in a row,
	// e.g. myparset.Add(somevariable)->setUnits("km")->setDescription("some description");
	MMParNodeBase* setName(std::string s) {
		name = s;
		return this;
	}

	MMParNodeBase* setDescription(std::string s) {
		description = s;
		return this;
	}

	MMParNodeBase* setLongDescription(std::string s) {
		longDescription = s;
		return this;
	}

	MMParNodeBase* setUnits(std::string s) {
		units = s;
		return this;
	}

	virtual bool setValueFromString(std::string valuestring, MMParOptions* options = NULL) {
		return false;
	}

	std::string getName() {
		return name;
	}

	std::string getDescription() {
		return description;
	}

	std::string getLongDescription() {
		return longDescription;
	}

	std::string getUnits() {
		return units;
	}

	bool hasTag(std::string t) {
		if (tags.find(t) == tags.npos)
			return false;
		return true;
	}

	virtual bool isBaseClass() {
		return true;
	}

	void addToStatusFlags(unsigned char flag) {
		statusFlags = statusFlags | flag;
	}

	void removeFromStatusFlags(unsigned char flag) {
		statusFlags = statusFlags & (~flag);
	}

	bool hasStatusFlag(unsigned char flag) {
		return (statusFlags & flag);
	}

	/*
	 template <typename T>
	 void getValue(T& v){
	 ((MMParNode <T> *)this)->getValue(v);
	 }
	 */
	/* template <typename T>
	 bool getValue(const char* name, T& v){
	 stringstream ss;
	 ss<<getValueAsString();
	 return(ss>>v);
	 } */

};

bool inline sortFunctionMMParNodeBase(MMParNodeBase *a, MMParNodeBase *b) {
	return (a->output_position < b->output_position);
}

template<class T, class U = double>
class MMParNode : public MMParNodeBase {
public:
	T* pValue;
	bool pValueMemIsMine;

	MMValidValues<U> *pRange;

	void ifNoRangeCreateIt() {
		if (pRange) {
			return;
		}
		else {
			pRange = new MMValidValues<U>;
			if (!pRange) {
				std::runtime_error("ERROR MMParSet: Cannot allocate memory with new. 324234");
			}
		}
	}

	void addRangeInterval(U left, U right, std::string _description = "", bool leftopen = false, bool rightopen = false)
	{
		ifNoRangeCreateIt();
		pRange->addInterval(left, right, _description, leftopen, rightopen);
	}

	bool isBaseClass() {
		return false;
	}

	void init(bool doCreateMem = false) {
		pValue = NULL;
		pValueMemIsMine = false;
		type = typeValue;
		// pRange=NULL;
		if (doCreateMem) {
			pValue = new T;
			if (!pValue) {
				throw std::runtime_error("Cannot get memory vor value in MMParSet.");
			}
			pValueMemIsMine = true;
		}
	}

	MMParNode() {
		init();
	}

	MMParNode(bool doCreateMem) {
		init(doCreateMem);
	}

	void Copy(const MMParNode &node) {
		if (this->pValueMemIsMine && this->pValue != NULL) {
			delete pValue;
			pValueMemIsMine = false;
			pValue = NULL;
		}
		if (node.pValueMemIsMine) {
			pValue = new T;
			if (!pValue)
				std::runtime_error("ERROR MMParSet: Cannot allocate memory with new. 1");
			*pValue = *node.pValue;
			pValueMemIsMine = true;
		}
		else {
			pValue = node.pValue;
		}
		type = node.type;
		name = node.name;
		description = node.description;
		longDescription = node.longDescription;
		units = node.units;
		valueTypes = node.valueTypes;
		options = node.options;
		statusFlags = node.statusFlags;
	}

	MMParNode(const MMParNode& node) {
		Copy(node);
	}
	MMParNode& operator = (const MMParNode & node) {
		Copy(node);
		return *this;
	}

	virtual ~MMParNode() {
		if (pValue && pValueMemIsMine) {
			delete pValue;
			pValue = NULL;
		}
		// if(pRange)
		// delete pRange;
	}

	MMParNode(T& value, const char* _name, const char* _description = NULL) {
		init();
		name = _name;
		if (_description)
			description = _description;
		pValue = &value;
		pValueMemIsMine = false;
		myTypeid(value, valueTypes, options.dimensions);
	}

	/*
	 MMParNode(bool reserveMemoryForValue) {
	 init();
	 if(! reserveMemoryForValue)
	 return;
	 pValue = new T;
	 if (!pValue)
	 runtime_error(
	 "ERROR MMParSet: Cannot allocate memory with new. 1");
	 pValueMemIsMine = true;
	 myTypeid(*pValue, valueTypes, options.dimensions);
	 }
	 */
	bool isParset() {
		if (type == typeParset)
			return true;
		return false;
	}

	bool isComment() {
		if (type == typeComment)
			return true;
		return false;
	};

	// ---------- set value -------------------
	/*
	 If pValue is a valid pointer: Set its contents.
	 If it is NULL: allocate memory and set its contents.
	 */
	// sets the value, allocates memory if node is not yet connected to existing mem
	void setValue(T&v) {
		if (pValue) {
			*pValue = v;
		}
		else {
			pValue = new T;
			if (!pValue)
				std::runtime_error("ERROR MMParSet: Cannot allocate memory with new. 1");
			*pValue = v;
			pValueMemIsMine = true;
		}
		// mimu todo
		// myTypeid(v , valueTypes, dimensions);
		// wrapTest(v);
	}

	// connects to external variable - no internal mem
	void setValueReference(T &v) { // not used yet
		if (pValue && pValueMemIsMine)
			delete pValue;
		pValue = &v;
	}

	// connects to external variable - no internal mem
	void setValuePointer(T *pv) { // not used yet
		if (pValue && pValueMemIsMine)
			delete pValue;
		pValue = pv;
	}

	bool setValueFromString(std::string valuestring, MMParOptions* _options = NULL) {
		if (_options == NULL)
			_options = &options;
		std::stringstream ss;
		ss << valuestring;
		return MMParSetReadFromStream(ss, pValue, _options);
	}

	// --------  get Value ------------------
	void getValue(T&v) {
		v = *pValue;
	}

	void * getValuePointer() {
		return (void*) pValue;
	}

	std::string getValueAsString() const {
		std::stringstream ss;
		MMParSetWriteToStream(ss, pValue, (MMParOptions*)&options);
		// ss<<setprecision(precision)<<*pValue;
		return ss.str();
	}

	// ------- get/set properties -----------
	// they all return the node pointer. So they can be used in a row,
	// e.g. myparset.Add(somevariable)->setUnits("km")->setDescription("some description");
	MMParNode<T, U> * setName(std::string s) {
		name = s;
		return this;
	}

	MMParNode<T, U> * setDescription(std::string s) {
		description = s;
		return this;
	}

	MMParNode<T, U> * setLongDescription(std::string s) {
		longDescription = s;
		return this;
	}

	MMParNode<T, U> * setUnits(std::string s) {
		units = s;
		return this;
	}

	std::string getName() {
		return name;
	}

	std::string getDescription() {
		return description;
	}

	std::string getLongDescription() {
		return longDescription;
	}

	std::string getUnits() {
		return units;
	}
	// ------ range tests --------------
	/* does not work if range is always double
	 bool isInRange(T& value) {
	 if(!pRange) return false;
	 return pRange->isInside(value);
	 }
	 bool isInRange() {
	 return isInRange(*pValue);
	 }
	 */

	bool hasRange() {
		// if(!pRange) return false;
		// return ! pRange->isEmpty();
		if (range.size() > 0)
			return true;
		return false;
	}

	// ------ convenience / special use -----
	MMParNodeBase* getMMParNodeBasePointer() {
		return (MMParNodeBase*) this;
	}

	// ------ Stream operations --------
//	void valueIntoStream(std::ostream &o) const {
//		// string sValue=getValueAsString();
//		// MMParSetWriteToStream(o, &sValue);
//		MMParSetWriteToStream(o, pValue, (MMParOptions*)&options);
//	}
	void valueIntoStream(std::ostream &o) {
		MMParSetWriteToStream(o, pValue, (MMParOptions*)&options);
		removeFromStatusFlags(MMParNodeChanged);
		// is strange - does not work if not read from file at all but
		// output to stdout only. : addToStatusFlags(MMParNodeAlreadyReadFromStream); // seems strange - but if it has been changed and written we want to  write it the second time as well
	}



	void rangeIntoStream(std::ostream &o, MMParSetOutputStyle os) const {
		MMParSetWriteToStream(o, &range);
		/*
		 if(!pRange) return;
		 if(os==styleSISI)
		 pRange->writeToStream_styleSISI(o);
		 else
		 pRange->writeToStream(o);
		 */
	}

//	void valueFromStream(std::istream &in) const {
//		// MMParSetReadFromStream(in, pValue);
//		MMParSetReadFromStream(in, pValue, (MMParOptions*)&options);
//		addToStatusFlags(MMParNodeAlreadyReadFromStream);
//	}

	void valueFromStream(std::istream &in) {
		// MMParSetReadFromStream(in, pValue);
		MMParSetReadFromStream(in, pValue, (MMParOptions*)&options);
		if(hasStatusFlag(MMParNodeAlreadyReadFromStream)) {
			std::string ts="MMParSet: Parameter " + name +" read from file a second time. "
				+std::string("Maybe the same parameter is defined twice in te file? ")
				+std::string("Maybe you are trying to read the same file twice? ");
			MMErrorMessage(ts, options.ErrorMessageType);
		}
		addToStatusFlags(MMParNodeAlreadyReadFromStream);
	}

	bool rangeFromStream(std::istream &in) {
		// ifNoRangeCreateIt();
		// return pRange->readFromStream(in);
		getLineNoCR(in, range);
		return true;
	}
};

#ifdef _WIN64
#pragma option -Weverything
#endif
#ifdef _WIN32
#pragma warn .8058
#endif

#endif










