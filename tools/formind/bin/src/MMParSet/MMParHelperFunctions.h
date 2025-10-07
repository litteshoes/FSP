#ifndef __MMPARHELPERFUNCTIONS_H
#define __MMPARHELPERFUNCTIONS_H
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <stdexcept>
#if !(defined(_WIN32) || defined(_WIN64))
#include <string.h>
#endif

// replaces mmValidValues because there are conceptual problems
// so this is not really a range but just a string using some functions
// which are implemented in mmValidValues so that I can keep the code used
// calling it in MMParSEt ...
class mmStringRange {
public:
	std::string s;
};

template<class T, size_t N1>
class mmArrayWrapper {
public:
	size_t s1, s2, s3;
	T* parray;

	mmArrayWrapper(T(&a)[N1]) {
		s1 = N1;
		s2 = 0;
		s3 = 0;
		parray = (T*)&a;
	}

	mmArrayWrapper() {
		s1 = 0;
		s2 = 0;
		s3 = 0;
		parray = NULL;
	}

	void Copy(const mmArrayWrapper<T, N1> & x) {
		this->parray = x.parray;
		this->s1 = x.s1;
		this->s2 = x.s2;
		this->s3 = x.s3;
	}

	mmArrayWrapper(const mmArrayWrapper& i) {
		Copy(i);
	}
	mmArrayWrapper<T, N1>operator = (const mmArrayWrapper<T, N1>&in) {
		Copy(in);
		return *this;
	}

	bool operator > (const mmArrayWrapper<T, N1> & x) const {
		if (this->s1 > x.s1)
			return true;
		return false;
	}

	bool operator < (const mmArrayWrapper<T, N1> & x) const {
		if (this->s1 < x.s1)
			return true;
		return false;
	}

	T* getAt(size_t const & i1) {
		return parray(i1);
	}
	// const T_return& X::operator[](T_index const& index) const;
};

/*
 this implementation is not the best one can do. I would prefer something
 like using an mmArrayWrapper of mmArrayWrapper . That might work more generally
 and one would not need all the extra funtions for 2D (3D) for input/output etc...
 But it was late and I needet a working version ...
 I one starts thinking about 3D - thats the point where you should consider to
 really use mmArrayWrapper of mmArrayWrapper of mmArrayWrapper.
 todo!!! mimu!!!
 you would still need a class here for each dimension,
 but inside the class you would use the wrapper for the lower dimension.
 Thus input/output functions would need only be implemented for 1-dim.
 */
template<class T, size_t N1, size_t N2>
class mmArrayWrapper2D {
public:
	size_t s1, s2, s3;
	T* parray;

	mmArrayWrapper2D(T(&a)[N1][N2]) {
		s1 = N1;
		s2 = N2;
		s3 = 0;
		parray = (T*)&a;
	}

	mmArrayWrapper2D() {
		s1 = 0;
		s2 = 0;
		s3 = 0;
		parray = NULL;
	}

	void Copy(const mmArrayWrapper2D<T, N1, N2> & x) {
		this->parray = x.parray;
		this->s1 = x.s1;
		this->s2 = x.s2;
		this->s3 = x.s3;
	}

	mmArrayWrapper2D(const mmArrayWrapper2D& i) {
		Copy(i);
	}
	mmArrayWrapper2D<T, N1, N2>operator = (const mmArrayWrapper2D<T, N1, N2>&in) {
		Copy(in);
		return *this;
	}

	bool operator > (const mmArrayWrapper2D<T, N1, N2> & x) const {
		if (this->s1 > x.s1)
			return true;
		return false;
	}

	bool operator < (const mmArrayWrapper2D<T, N1, N2> & x) const {
		if (this->s1 < x.s1)
			return true;
		return false;
	}

	T* getAt(size_t const & i1, size_t const & i2) {
		// return parray+i1+i2*s1;
		return parray + i2 + i1 * s2;
	}

	void setAt(size_t const & i1, size_t const & i2, T& val) {
		// *(parray+i1+i2*s1)=val;
		*(parray + i2 + i1*s2) = val;
	}

};

template<class T>
class mmPointerArrayWrapper {
public:
	T* buf; // pointer to internal memory. To make it easyier to read and be used without external pointer. One could leave it out for the external pointer.
	T** pbuf; // pointer to external pointer which should be kept updated.
	size_t bufsize;
	bool memIsMine;
	size_t pos; // points to the next empty element
	int* pCopiesCounter;

private:
	void setbuf(T* p) {
		buf = p;
		if (pbuf)
			* pbuf = buf;
	}

public:
	void init() {
		buf = NULL;
		pbuf = NULL;
		memIsMine = false;
		bufsize = 0;
		pos = 0;
		pCopiesCounter = NULL;
		/* if(!resize(1024)) {  //minimum size - dirty eh?
		 string ts2="ERROR MMParSet mmPointerArrayWrapper: Cannot allocate memory with new. ";
		 throw std::runtime_error(ts2);
		 } */
	}

	mmPointerArrayWrapper() {
		init();
	}

	mmPointerArrayWrapper(T* &pArray) {
		init();
		pbuf = &pArray;
	}

	~mmPointerArrayWrapper() {
		if (pCopiesCounter != NULL) {
			(*pCopiesCounter)--;
			if (*pCopiesCounter <= 0) {
				delete pCopiesCounter;
				pCopiesCounter = NULL;
			}
		}
		else {
			clear();
		}
	}

	void copy(const mmPointerArrayWrapper<T> & x) { // copies pointers only - might need smart pointers or something ...
		std::string ts2 = "ERROR MMParSet mmPointerArrayWrapper copy(): \
		This function has not been tested. Uncomment this Exception if you want to. \
		Beware: This function was just hacked together without much thinking and I do not think it works.";
		throw std::runtime_error(ts2);
		this->buf = x.buf;
		this->pbuf = x.pbuf;
		this->bufsize = x.bufsize;
		this->pos = x.pos;
		if (!x.pCopiesCounter) {
			// x.pCopiesCounter = new int;
			if (!x.pCopiesCounter) {
				std::string ts2 = "ERROR MMParSet pCopiesCounter: Cannot allocate memory with new. ";
				throw std::runtime_error(ts2);
			}
			*(this->pCopiesCounter) = 0;
		}
		this->pCopiesCounter = x.pCopiesCounter;
		*(this->pCopiesCounter)++;
	}

	mmPointerArrayWrapper(const mmPointerArrayWrapper& i) {
		copy(i);
	}
	mmPointerArrayWrapper<T>operator = (const mmPointerArrayWrapper<T>&in) {
		copy(in);
		return *this;
	}

	bool resize(size_t s) {
		T* buf2 = new T[s];
		if (!buf2) {
			std::string ts2 = "ERROR MMParSet mmPointerArrayWrapper: Cannot allocate memory with new. ";
			throw std::runtime_error(ts2);
		}
		if (memIsMine) {
			if (buf) {
				if (s >= bufsize)
					memcpy(buf2, buf, bufsize*sizeof(T));
				else
					memcpy(buf2, buf, s*sizeof(T));
				delete[]buf;
				setbuf(NULL);
			}
		}
		setbuf(buf2);
		memIsMine = true;
		bufsize = s;
		return true;
	}

	bool push_back(T value) {
		if (bufsize <= 0) {
			resize(1024);
		}
		if (pos >= bufsize) {
			if (!resize(bufsize * 2))
				return false;
		}
		buf[pos] = value;
		pos++;
		return true;
	}

	void clear() {
		if (memIsMine) {
			if (buf) {
				delete[]buf;
			}
			setbuf(NULL);
		}
		bufsize = 0;
		pos = 0;
	}

	bool operator > (const mmPointerArrayWrapper<T> & x) const {
		if (this->pos > x.pos)
			return true;
		return false;
	}

	bool operator < (const mmPointerArrayWrapper<T> & x) const {
		if (this->pos < x.pos)
			return true;
		return false;
	}

	T& getAt(size_t const & i1) {
		return buf[i1];
	}

	size_t size() {
		return pos;
	}
	// const T_return& X::operator[](T_index const& index) const;
};

// #define XDIRECTIONINFILEISXMENAINGFIRSTCOORDINATEINARRAY
#ifdef XDIRECTIONINFILEISXMENAINGFIRSTCOORDINATEINARRAY

template<class T>
class mmPointerArrayWrapper2D {
public:
	T*** pbuf; // pointer to external memory T** (the one used in the program).
	// *pbuf is kept in sync with mybuf. Using just pbuf gives problem in destruction - external *pbuf might not exist anymore or something like that
	T** mybuf; // internally managed memory
	bool memIsMine;
	unsigned long bufsizex, bufsizey;
	unsigned long posx; // points to the next empty element
	unsigned long posy;
	unsigned long sizex, sizey;
	int* pCopiesCounter;
	bool newlinePrepared;

private:
	void setBufferReference(T** &p) {
		pbuf = &p;
	}

public:
	void init() {
		pbuf = NULL;
		mybuf = NULL;
		memIsMine = false;
		bufsizex = 0;
		bufsizey = 0;
		posx = 0;
		posy = 0;
		sizex = 0;
		sizey = 0;
		pCopiesCounter = NULL;
		newlinePrepared = false;
	}

	mmPointerArrayWrapper2D() {
		init();
	}

	mmPointerArrayWrapper2D(T** &pArray) {
		init();
		setBufferReference(pArray);
	}

	~mmPointerArrayWrapper2D() {
		if (pCopiesCounter != NULL) {
			(*pCopiesCounter)--;
			if (*pCopiesCounter <= 0) {
				delete pCopiesCounter;
				pCopiesCounter = NULL;
			}
		}
		else {
			clear();
		}
	}

	void copy(const mmPointerArrayWrapper2D<T> & x)
	{ // copies pointers only - might need smart pointers or something ...
		std::string ts2 = "ERROR MMParSet mmPointerArrayWrapper2D copy(): \
		This function has not been tested. Uncomment this Exception if you want to. \
		Beware: This function was just hacked together without much thinking.";
		throw std::runtime_error(ts2);

		this->buf = x.buf;
		this->pbuf = x.pbuf;
		this->pbuf2 = x.pbuf2;
		this->memIsMine = x.memIsMine;
		this->bufsizex = x.bufsizex;
		this->bufsizey = x.bufsizey;
		this->posx = x.posx;
		this->posy = x.posy;
		this->newlinePrepred = x.newlinePrepred;
		if (!x.pCopiesCounter) {
			x.pCopiesCounter = new int;
			if (!x.pCopiesCounter) {
				std::string ts2 = "ERROR MMParSet pCopiesCounter: Cannot allocate memory with new. ";
				throw std::runtime_error(ts2);
			}
			*(this->pCopiesCounter) = 0;
		}
		this->pCopiesCounter = x.pCopiesCounter;
		*(this->pCopiesCounter)++;
	}

	mmPointerArrayWrapper2D(const mmPointerArrayWrapper2D& i) {
		copy(i);
	}
	mmPointerArrayWrapper2D<T>operator = (const mmPointerArrayWrapper2D<T>&in) {
		copy(in);
		return *this;
	}

	bool resize(size_t x, size_t y) {
		T **buf2 = new T*[x];
		if (!buf2) {
			std::string ts2 = "ERROR MMParSet mmPointerArrayWrapper2D: Cannot allocate memory with new. ";
			throw std::runtime_error(ts2);
		}
		for (int i = 0; i < x; ++i) {
			buf2[i] = new T[y];
			if (!buf2[i]) {
				std::string ts2 = "ERROR MMParSet mmPointerArrayWrapper2D: Cannot allocate memory with new. ";
				throw std::runtime_error(ts2);
			}
		}
		if (memIsMine)
			if (pbuf)
				if ((*pbuf)) {
					size_t minx = bufsizex;
					if (minx > x)
						minx = x;
					size_t miny = bufsizey;
					if (miny > y)
						miny = y;
					for (int i = 0; i < minx; ++i) {
						if (miny > 0)
							memcpy(buf2[i], (*pbuf)[i], miny*sizeof(T));
					}
					clear();
				}
		*pbuf = buf2;
		mybuf = buf2;
		buf2 = NULL;
		bufsizex = x;
		bufsizey = y;
		memIsMine = true;
		if (sizey == 0 && y > 0)
			sizey = 1;
		return true;
	}

	bool push_back(T value) {
		if (bufsizex <= 0 || bufsizey <= 0) {
			resize(5, 5); // was 256 256 mimu todo
		}
		newline_execute();
		// inserts newline if asked for before with newline(). This makes for easyier handling in loops.
		if (posx >= bufsizex) {
			resize(bufsizex*2, bufsizey);
		}
		(*pbuf)[posx][posy] = value;
		posx++;
		if (posx > sizex)
			sizex = posx;
		return true;
	}

	void newline() {
		newline_prepare();
	}

	void newline_prepare() {
		newlinePrepared = true;
	}

	void newline_execute() {
		if (!newlinePrepared)
			return;
		newlinePrepared = false;
		posy++;
		if (posy >= bufsizey) {
			resize(posx, bufsizey*2);
		}
		posx = 0;
		if (posy >= sizey)
			sizey = posy + 1;
	}

	void clear() {
		if (!memIsMine)
			return;
		for (int i = 0; i < bufsizex; ++i) {
			delete[]mybuf[i];
			mybuf[i] = NULL;
		}
		delete[]mybuf;
		mybuf = NULL;
		(*pbuf) = NULL;
		memIsMine = false;
		bufsizex = 0;
		bufsizey = 0;

		posx = 0;
		posy = 0;
		sizex = 0;
		sizey = 0;
		newlinePrepared = false;
	}

	bool operator > (const mmPointerArrayWrapper<T> & x) const {
		if (this->posx*this->posy > x.posx * x.posy)
			return true;
		return false;
	}

	bool operator < (const mmPointerArrayWrapper<T> & x) const {
		if (this->posx*this->posy < x.posx * x.posy)
			return true;
		return false;
	}

	T& getAt(size_t const & x, size_t const & y) {
		return (*pbuf)[x][y];

	}

	void optimizeMem() {
		resize(sizex, sizey);
	}
};
#else

template<class T>
class mmPointerArrayWrapper2D {
public:
	T*** pbuf; // pointer to external memory T** (the one used in the program).
	// *pbuf is kept in sync with mybuf. Using just pbuf gives problem in destruction - external *pbuf might not exist anymore or something like that
	T** mybuf; // internally managed memory
	bool memIsMine;
	unsigned long bufsizex, bufsizey; // x,y of array [x][y]
	unsigned long posx;
	// points to the next empty element  x,y, position in file - array is the other way round (SiSi)
	unsigned long posy;
	unsigned long sizex, sizey; // x,y of array
	int* pCopiesCounter;
	bool newlinePrepared;

private:
	void setBufferReference(T** &p) {
		pbuf = &p;
	}

public:
	void init() {
		pbuf = NULL;
		mybuf = NULL;
		memIsMine = false;
		bufsizex = 0;
		bufsizey = 0;
		posx = 0;
		posy = 0;
		sizex = 0;
		sizey = 0;
		pCopiesCounter = NULL;
		newlinePrepared = false;
	}

	mmPointerArrayWrapper2D() {
		init();
	}

	mmPointerArrayWrapper2D(T** &pArray) {
		init();
		setBufferReference(pArray);
	}

	~mmPointerArrayWrapper2D() {
		if (pCopiesCounter != NULL) {
			(*pCopiesCounter)--;
			if (*pCopiesCounter <= 0) {
				delete pCopiesCounter;
				pCopiesCounter = NULL;
			}
		}
		else {
			clear();
		}
	}

	void copy(const mmPointerArrayWrapper2D<T> & x)
	{ // copies pointers only - might need smart pointers or something ...
		std::string ts2 = "ERROR MMParSet mmPointerArrayWrapper2D copy(): \
		This function has not been tested. Uncomment this Exception if you want to. \
		Beware: This function was just hacked together without much thinking.";
		throw std::runtime_error(ts2);

		// this->buf = x.buf;
		this->pbuf = x.pbuf;
		// this->pbuf2 = x.pbuf2;
		this->memIsMine = x.memIsMine;
		this->bufsizex = x.bufsizex;
		this->bufsizey = x.bufsizey;
		this->posx = x.posx;
		this->posy = x.posy;
		this->newlinePrepared = x.newlinePrepared;
		if (!x.pCopiesCounter) {
			// x.pCopiesCounter = new int;
			if (!x.pCopiesCounter) {
				std::string ts2 = "ERROR MMParSet pCopiesCounter: Cannot allocate memory with new. ";
				throw std::runtime_error(ts2);
			}
			*(this->pCopiesCounter) = 0;
		}
		this->pCopiesCounter = x.pCopiesCounter;
		*(this->pCopiesCounter)++;
	}

	mmPointerArrayWrapper2D(const mmPointerArrayWrapper2D& i) {
		copy(i);
	}
	mmPointerArrayWrapper2D<T>operator = (const mmPointerArrayWrapper2D<T>&in) {
		copy(in);
		return *this;
	}

	bool resize(size_t x, size_t y) {
		T **buf2 = new T*[x];
		if (!buf2) {
			std::string ts2 = "ERROR MMParSet mmPointerArrayWrapper2D: Cannot allocate memory with new. ";
			throw std::runtime_error(ts2);
		}
		for (int i = 0; i < x; ++i) {
			buf2[i] = new T[y];
			if (!buf2[i]) {
				std::string ts2 = "ERROR MMParSet mmPointerArrayWrapper2D: Cannot allocate memory with new. ";
				throw std::runtime_error(ts2);
			}
		}
		if (memIsMine)
			if (pbuf)
				if ((*pbuf)) {
					size_t minx = bufsizex;
					if (minx > x)
						minx = x;
					size_t miny = bufsizey;
					if (miny > y)
						miny = y;
					for (int i = 0; i < minx; ++i) {
						if (miny > 0)
							memcpy(buf2[i], (*pbuf)[i], miny*sizeof(T));
					}
					clear();
				}
		*pbuf = buf2;
		mybuf = buf2;
		buf2 = NULL;
		bufsizex = x;
		bufsizey = y;
		memIsMine = true;
		if (sizey == 0 && y > 0)
			sizey = 1;
		return true;
	}

	bool push_back(T value) {
		if (bufsizex <= 0 || bufsizey <= 0) {
			resize(5, 5); // was 256 256 mimu todo
		}
		newline_execute();
		// inserts newline if asked for before with newline(). This makes for easyier handling in loops.
		if (posx >= bufsizey) {
			resize(bufsizex, bufsizey*2);
		}
		(*pbuf)[posy][posx] = value;
		posx++;
		if (posx > sizey)
			sizey = posx;
		return true;
	}

	void newline() {
		newline_prepare();
	}

	void newline_prepare() {
		newlinePrepared = true;
	}

	void newline_execute() {
		if (!newlinePrepared)
			return;
		newlinePrepared = false;
		posy++;
		if (posy >= bufsizex) {
			resize(bufsizex*2, posx);
		}
		posx = 0;
		if (posy >= sizex)
			sizex = posy + 1;
	}

	void clear() {
		if (!memIsMine)
			return;
		for (unsigned int i = 0; i < bufsizex; ++i) {
			delete[]mybuf[i];
			mybuf[i] = NULL;
		}
		delete[]mybuf;
		mybuf = NULL;
		(*pbuf) = NULL;
		memIsMine = false;
		bufsizex = 0;
		bufsizey = 0;
	}

	void reset() {
		clear();
		posx = 0;
		posy = 0;
		sizex = 0;
		sizey = 0;
		newlinePrepared = false;
	}

	bool operator > (const mmPointerArrayWrapper<T> & x) const {
		if (this->posx*this->posy > x.posx * x.posy)
			return true;
		return false;
	}

	bool operator < (const mmPointerArrayWrapper<T> & x) const {
		if (this->posx*this->posy < x.posx * x.posy)
			return true;
		return false;
	}

	T& getAt(size_t const & x, size_t const & y) {
		return (*pbuf)[x][y];

	}

	void optimizeMem() {
		resize(sizex, sizey);
	}
};

#endif

inline std::string typeToSisiType(std::string s) {
	std::string sisiTypeName = s;
	if (sisiTypeName == "string" || sisiTypeName == "char*" || sisiTypeName == "char" || sisiTypeName == "std::string")
		sisiTypeName = "string";
	else if (sisiTypeName == "double" || sisiTypeName == "long double" || sisiTypeName == "float")
		sisiTypeName = "float";
	else if (sisiTypeName == "int" || sisiTypeName == "long int" || sisiTypeName == "long" ||
		 sisiTypeName == "unsigned int" || sisiTypeName == "unsigned long" || sisiTypeName == "bool") {
		sisiTypeName = "int";
	}
	else
		sisiTypeName = s; // aeh todo or error ?
	return sisiTypeName;
}

inline int ForwardWhiteSpaces(std::istream& in) {
	int ti;
	int ccount = 0;
	while (1) {
		ti = in.peek();
		if (ti == in.eof())
			return ccount;
		if (!isspace(ti))
			break;
		ti = in.get();
		ccount++;
	}
	return ccount;
}

inline int ForwardWhiteSpacesWithoutNewline(std::istream& in) {
	int ti;
	int ccount = 0;
	while (1) {
		ti = in.peek();
		if (ti == in.eof())
			return ccount;
		if (ti != ' ' && ti != '\t' && ti != '\v' && ti != '\f' && ti != '\r')
			break;
		ti = in.get();
		ccount++;
	}
	return ccount;
}

inline int ForwardNoWhiteSpacesStopOnEqual(std::istream& in) {
	int ti;
	int ccount = 0;
	while (1) {
		ti = in.peek();
		if (ti == in.eof() || ti == -1)
			return ccount;
		if (isspace(ti) || (ti == '='))
			break;
		ti = in.get();
		ccount++;
	}
	while (!isspace(ti) && !(ti == '='));
	return ccount;
}

inline size_t MMGetLine(std::istream & is, std::string& s, std::string delims) {
	s = "";
	while (is.good() && !is.eof()) {
		std::istream::int_type ti = is.peek();
		if (ti == -1)
			return s.size();
		// peek returns -1 if not successful mimu 2006 and moved here 2014
		for (unsigned int i = 0; i < delims.size(); i++) {
			char tc = ti;
			char tc1 = delims[i];
			if (tc == tc1) {
				return s.size();
			}
		}
		s += is.get();
	}
	return 0;
}

inline bool getLineNoCR(std::istream & is, std::string& s) {
	s.clear();
	getline(is, s);
	if (!is.good()) {
		if (s.size() <= 0) {
			return false;
		}
	}
	if (s.length() > 0) {
		std::string::iterator it = s.end() - 1;
		if (*it == '\r') {
			s.erase(it);
		}
	}
	return true;
}

// ----------------------------------------------------------------------------
// gets quoted strings: "trallalla\"istgut\"   "
// returns false if not including starting and ending quote.
inline bool MMGetBetweenQuotes(std::istream & is, std::string& s) {
	s = "";
	ForwardWhiteSpaces(is);
	if (is.get() != '\"')
		return false;
	while (is.good() && !is.eof()) {
		int ti = is.get();
		if (ti == '\"') {
			while (is.peek() == '\"')
				is.get(); // forget more additional trailing quotes.
			return true;
		}
		s += ti;
	}
	return false; // mimu 2014 changed - false if End quote missing.
}

// helper Function, strings including spaces AND empty strings get quoted
void inline makeSureStringWithSpacesIsQuoted(std::string& out, std::string& in) {
	int quoteAction = -1;
	if (in.size() == 0)
		quoteAction = 1; // quote empty strings.
	else if (in.size() >= 2) {
		if (in[0] == '\"' && in[in.size() - 1] == '\"') {
			quoteAction = 0; // do not quote already quoted strings
		}
	}
	if (quoteAction == -1) { // quote strings with space, comma and empty ones.
		if (in.find(" ") != std::string::npos || in.size() == 0 ||
				in.find(",") != std::string::npos ) {
			quoteAction = 1;
		}
	}
	if (quoteAction == 1) {
		out = "\"" + in + "\"";
	}
	else {
		out = in;
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

inline bool mmStringBeginningCompare(std::string a, std::string b) {
	if (a.size() < b.size())
		return false;
	std::string a2 = a.substr(0, b.size());
	if (a2 == b)
		return true;
	return false;
}

inline bool isStlType(const std:: type_info& t, std::string s) {
	std::string infoname = t.name();
	return mmStringBeginningCompare(infoname, s);
}

#include <iostream>

template<typename T>
void inline myTypeid(T& value, std::vector<std::string> &types, std::vector<int> &dimensions) {
	// types.clear();
	const std:: type_info& info = typeid(value);
	// string tname=info.name();

	if (info == typeid(std::string))
		types.push_back("string");

	else if (info == typeid(char*))
		types.push_back("char*");

	else if (info == typeid(bool))
		types.push_back("bool");
	else if (info == typeid(const bool))
		types.push_back("const bool");

	else if (info == typeid(int))
		types.push_back("int");
	else if (info == typeid(unsigned int))
		types.push_back("unsigned int");
	else if (info == typeid(const int))
		types.push_back("const int");
	else if (info == typeid(const unsigned int))
		types.push_back("const unsigned int");

	else if (info == typeid(long))
		types.push_back("long");
	else if (info == typeid(unsigned long))
		types.push_back("unsigned long");
	else if (info == typeid(const long))
		types.push_back("const long");
	else if (info == typeid(const unsigned long))
		types.push_back("const unsigned long");

	else if (info == typeid(long long))
		types.push_back("long long");
	else if (info == typeid(unsigned long long))
		types.push_back("unsigned long long");
	else if (info == typeid(const long long))
		types.push_back("const long long");
	else if (info == typeid(const unsigned long long))
		types.push_back("const unsigned long long");

	else if (info == typeid(double))
		types.push_back("double");
	else if (info == typeid(const double))
		types.push_back("const double");
	else if (info == typeid(long double))
		types.push_back("long double");
	else if (info == typeid(const long double))
		types.push_back("const long double");

	else if (info == typeid(float))
		types.push_back("float");
	else if (info == typeid(const float))
		types.push_back("const float");

	else
		types.push_back(typeid(value).name()); // value_type
}

// stl types are differently implemented in Embarcadero 32 and 64 bit compilers
// 64 bit the allocator argument is needed to hit the specialised/overloaded function.
#ifdef _WIN64

template<template<class, class>class stlType, class valueType>
void inline myTypeid(stlType<valueType, std::allocator<valueType> > &value, std::vector<std::string>& types,
	 std::vector<int> &dimensions) {
#else

template<template<class>class stlType, class valueType>
void inline myTypeid(stlType<valueType> & value, std::vector<std::string>& types, std::vector<int> &dimensions) {
#endif
	const std:: type_info& info = typeid(value);
	types.clear();
	dimensions.push_back(value.size());
	if (typeid(std::vector<valueType>) == info)
		types.push_back("vector");
	else if (typeid(std::list<valueType>) == info)
		types.push_back("list");
	else if (typeid(std::set<valueType>) == info)
		types.push_back("set");

	valueType dummy;
	myTypeid(dummy, types, dimensions);

	// types.push_back(typeid(valueType).name());  //value_type
}

// special for vector/list <string > - for 32 bit embarcadero:
#ifndef __WIN64

void inline myTypeid(std::vector<std::string> & value, std::vector<std::string>& types, std::vector<int> &dimensions) {
	types.clear();
	dimensions.push_back(value.size());
	types.push_back("vector");
	types.push_back("string");
}
#endif

// SPECIAL: treat char[150] or similar as string !!!
template<size_t N1>
void inline myTypeid(char(&a)[N1], std::vector<std::string>& types, std::vector<int> &dimensions) {
	types.clear();
	types.push_back("string");
}

template<class T, size_t N1>
void inline myTypeid(T(&a)[N1], std::vector<std::string>& types, std::vector<int> &dimensions) {
	types.clear();
	types.push_back("array");
	dimensions.push_back(N1);
	T dummyValue;
	myTypeid(dummyValue, types, dimensions);
}

template<class T>
bool isSTLType(T &value) {
	return false;
}
#ifdef _WIN64

template<template<class, class>class stlType, class valueType>
bool isSTLType(stlType<valueType, std::allocator<valueType> > &value) {
	return true;
}
#else

template<template<class>class stlType, class valueType>
bool isSTLType(stlType<valueType> & value) {
	return true;
}
#endif

template<class T>
bool isSTLSTLType(T &value) {
	return false;
}
#ifdef _WIN64

template<template<class, class>class stlType, class valueType>
bool isSTLSTLType(stlType<valueType, std::allocator<valueType> > &value) {
	valueType dummy;
	return isSTLType(dummy);
}
#else

template<template<class>class stlType, class valueType>
bool isSTLSTLType(stlType<valueType> & value) {
	valueType dummy;
	return isSTLType(dummy);
}
#ifdef __BORLANDC__

// very special for emba 32 :
bool isSTLSTLType(std::vector<std::string> & value) {
	return false;
}
#endif
#endif

template<class T>
bool isPointerType(T &value) {
	return false;
}

template<class T>
bool isPointerType(T* value) {
	return true;
}

/*
 template < class T, size_t N1>
 void inline myTypeid(mmArrayWrapper < T , N1> &value, vector<string> &types, vector<int> &dimensions) {
 types.clear();
 types.push_back("array");
 dimensions.push_back(N1);
 T dummyValue;
 myTypeid(dummyValue, types, dimensions);
 }
 */

// mimu: todo: xml comments ...
inline void SkipComments(std::istream & in) {
	while (1) {
		ForwardWhiteSpaces(in);
		int ti = in.peek();
		// int k='\r';
		// char c=ti;
		if (ti == '#') {
			std::string line;
			getline(in, line);
		}
		else if (ti == '/') {
			int ti2 = in.peek();
			if (ti2 == '/') {
				std::string line;
				getline(in, line);
			}
			else
				break;
		}
		/*
		 else if (ti=='\r' && skipEndOfLine) {
		 in.get();
		 }
		 else if (ti=='\n' && skipEndOfLine) {
		 in.get();
		 } */
		else
			break;
	}
}

class mybuftype {
public:
	char* buf;
	size_t size;

	mybuftype() {
		buf = NULL;
		size = 0;
		resize(256);
	}

	mybuftype(size_t s) {
		buf = NULL;
		size = 0;
		resize(s);
	}

	~mybuftype() {
		if (buf != NULL)
			delete[]buf;
		size = 0;
		buf = NULL;
	}

	bool resize(size_t s) {
		if (size >= s)
			return true;
		if (buf != NULL)
			delete[]buf;
		size = 0;
		buf = new char[s];
		if (buf == NULL) {
			std::string ts = "Error allocationg memory for function GetNameInistyle";
			throw std::runtime_error(ts.c_str());
			return false;
		}
		size = s;
		return true;
	}
};

// mimu todo: der gante memory kram sieht nicht gut aus - geht besser.
inline bool GetNameInistyle(std::string& name, std::istream& in) {
	ForwardWhiteSpaces(in);
	if (in.eof() || !in.good())
		return false;
	long startpos = in.tellg();
	ForwardNoWhiteSpacesStopOnEqual(in);
	long stoppos = in.tellg();
	in.seekg(startpos);
	long size = stoppos - startpos;
	name.resize(size, ' ');
	if (size == 0)
		return false;
	// mimu todo 10.9.2008 diese Zeile eingefuegt, gegen endless loop
	// mimu todo: this can be done more elegant!!!:
	static mybuftype mybuf;
	if (!mybuf.resize(size + 2))
		return false;
	// in.read((char*)name.data(), size);
	in.read(mybuf.buf, size);
	mybuf.buf[size] = 0;
	name = mybuf.buf;
	return true;
	// mimu: todo: error management !!!
}

inline bool NameIsParsetName(std::string& NameBase, std::string& name) {
	if (name.size() >= 3) {
		if (name[0] == '[' && name[name.size() - 1] == ']') {
			NameBase.assign(name, 1, name.size() - 2);
			return true;
		}
	}
	else {
		int k = 22;
	}
	return false;
}

inline void SkipAssignInistyle(std::istream & in) {
	ForwardWhiteSpaces(in);
	int ti = in.peek();
	if (ti == '=') {
		in.get();
		ForwardWhiteSpaces(in);
	}
}

inline std::string& replaceAll(std::string& s, const std::string& from, const std::string& to) {
	if (!from.empty())
		for (size_t pos = 0; (pos = s.find(from, pos)) != std::string::npos; pos += to.size())
			s.replace(pos, from.size(), to);
	return s;
}

/* inline std::string& replaceAll(std::string& s, const char* from, const char* to)
 {
 std::string sfrom=from;
 std::string sto=to;
 return replaceAll(s, sfrom, sto);
 } */

// ----------------------------------------------------------------------------

#endif
