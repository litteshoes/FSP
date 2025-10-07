/*
 Routines for String input/output of several commonly used types

 They work with both bcc32 and bcc64 and thus probably with most compilers.

 */
// ---------------------------------------------------------------------------
#ifndef __MMStreamIO_H__
#define __MMStreamIO_H__
// ---------------------------------------------------------------------------
// #pragma message "MMStreamIO BEGIN"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>
#include <set>
#include <typeinfo>
#include <iostream>

#include <MMParSet/MMErrorMessage.h>
#include <MMParSet/MMParHelperFunctions.h>
#include <MMParSet/MMParOptions.h>
// #include <MMParSet/MMInterval.h>
// #include <MMParSet/MMValidValues.h>

// #pragma message "MMStreamIO BEGIN"

// ----------------------------------------------------------------------------
// Simple Helper Functions
// ----------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// WRITE TO ostream
// ---------------------------------------------------------------------------
/*

 */

// --- Output for all types with << operator: ---

// The Default output function (just needed as helper in this file).

#define __MMUSETEMPLATEFORREADSTREAM
#ifdef __MMUSETEMPLATEFORREADSTREAM

template<typename T>
bool MMParSetWriteToStream_Default(std::ostream& o, T* value, MMParOptions* options) {
	if (options != NULL) {
		options->applyTo(o);
	}
	o << *value;
	if (o.good())
		return true;
	return false;
}
#else

#define Create_MMParSetWriteToStream_Default( X ) \
bool MMParSetWriteToStream_Default(std::ostream& o, X * value, MMParOptions* options) { \
	if(options!=NULL) { \
		options->applyTo(o); \
	}                        \
	o<<*value;                \
	if(o.good()) return true; \
	return false;              \
}

// Create_MMParSetWriteToStream_Default(int);
bool MMParSetWriteToStream_Default(std::ostream& o, int * value, MMParOptions* options) {
	if (options != NULL) {
		options->applyTo(o);
	}
	o << *value;
	if (o.good())
		return true;
	return false;
}

Create_MMParSetWriteToStream_Default(const int);
Create_MMParSetWriteToStream_Default(long int);
Create_MMParSetWriteToStream_Default(const long int);
Create_MMParSetWriteToStream_Default(unsigned int);
Create_MMParSetWriteToStream_Default(const unsigned int);
Create_MMParSetWriteToStream_Default(unsigned long int);
Create_MMParSetWriteToStream_Default(const unsigned long int);
Create_MMParSetWriteToStream_Default(double);
Create_MMParSetWriteToStream_Default(const double);
Create_MMParSetWriteToStream_Default(long double);
Create_MMParSetWriteToStream_Default(const long double);
Create_MMParSetWriteToStream_Default(string);
Create_MMParSetWriteToStream_Default(const string);
Create_MMParSetWriteToStream_Default(char *);
Create_MMParSetWriteToStream_Default(const char *);

#endif

// declaration (not definition) of global default function. Does not work without this in bcc32:
template<typename T>
bool MMParSetWriteToStream(std::ostream& o, T* value, MMParOptions* options = NULL);

// the global default function definition
template<typename T>
bool MMParSetWriteToStream(std::ostream& o, T* value, MMParOptions* options) {
	return MMParSetWriteToStream_Default(o, value, options);
}

// --- Specialized output for some types ---

// string special: if string contains whitespace: surrond with quotes:
inline bool MMParSetWriteToStream(std::ostream& o, std::string* value, MMParOptions* options = NULL) {
	std::string value2;
	makeSureStringWithSpacesIsQuoted(value2, *value);
	o << value2;
	return o.good();
}

// string special: do not add Quoting. Needed for tags.
inline bool MMParSetWriteToStreamNoQuote(std::ostream& o, std::string* value, MMParOptions* options = NULL) {
	o << *value;
	return o.good();
}

inline bool MMParSetWriteToStream(std::ostream& o, const std::string* value, MMParOptions* options = NULL) {
	return MMParSetWriteToStream(o, (std::string*) value, options);
}

// char* special: if char* string vontains whitespace: surrond with quotes:
inline bool MMParSetWriteToStream(std::ostream& o, char** value, MMParOptions* options = NULL) {
	// mimu: a simple call to the string function did not succeed -  the default template was called. strange.
	std::string input = std::string(*value);
	std::string value2;
	makeSureStringWithSpacesIsQuoted(value2, input);
	o << value2;
	return o.good();
}

// const char* special (like char* special):
inline bool MMParSetWriteToStream(std::ostream& o, const char** value, MMParOptions* options = NULL) {
	return MMParSetWriteToStream(o, (char**) value, options);
}

// char[somenumber] special: if string contains whitespace: surrond with quotes:
template<size_t N1>
inline bool MMParSetWriteToStream(std::ostream& o, char(*value)[N1], MMParOptions* options = NULL) {
	std::string input = std::string(*value);
	std::string value2;
	makeSureStringWithSpacesIsQuoted(value2, input);
	o << value2;
	return o.good();
}

// unsigned char[somenumber] special: if string contains whitespace: surrond with quotes:
template<size_t N1>
inline bool MMParSetWriteToStream(std::ostream& o, unsigned char(*value)[N1], MMParOptions* options = NULL) {
	std::string input = std::string(*value);
	std::string value2;
	makeSureStringWithSpacesIsQuoted(value2, input);
	o << value2;
	return o.good();
}

// STL container special - (with helper function workaround for bcc32):
#define Create_MMParSetWriteToStream_DefaultSTLContainer(U) \
template <typename T>                                 \
bool MMParSetWriteToStream_DefaultSTLContainer(std::ostream& o, U <T>* value, MMParOptions* options=NULL) { \
		std::string seperator=","; \
		std::string vector_start="{"; \
		std::string vector_end="}";     \
		if(options)if(options->outputStyle==styleSISI || options->outputStyle == styleSISINoIndent) { \
			vector_start="";                               \
			vector_end="";                                  \
			seperator="   ";                                 \
		}                                                    \
		o<<vector_start;                           \
		typename U <T>::size_type last=value->size();  \
		if(last==0) {                                   \
			o<<vector_end;                                \
			if(o.good()) return true;                      \
			else return false;                              \
		}                                                   \
		last--;                                              \
		typename U <T>::const_iterator it;                    \
		for(it=value->begin();it!=value->end() && last--;++it) {    \
		  if(!MMParSetWriteToStream(o, &(*it), options)) {   \
			  return false;                                    \
		  }                                                    \
		  o<<seperator;                                         \
		}                                                        \
		if(!MMParSetWriteToStream(o, &(*it), options)) return false;\
		o<<vector_end;                                               \
		if(o.good()) return true;                                     \
		else return false;                                             \
}                                                                     \
template <typename T>                                                                   \
bool MMParSetWriteToStream(std::ostream& o, U<T>* value, MMParOptions* options=NULL) {   \
	return MMParSetWriteToStream_DefaultSTLContainer(o, value, options);                         \
}                                                                                              \
template <typename T>             \
bool MMParSetWriteToStream(std::ostream& o, const U<T>* value, MMParOptions* options=NULL) { \
	return MMParSetWriteToStream_DefaultSTLContainer(o, (U<T>*) value, options);                \
}

#include <list>
Create_MMParSetWriteToStream_DefaultSTLContainer(std::list);
#include <deque>
Create_MMParSetWriteToStream_DefaultSTLContainer(std::deque);

// vector special - (with helper function workaround for bcc32). Same as the default container thingy above.
// kept in here to allow debugging at the vector example. :
template<typename T>
bool MMParSetWriteToStream_DefaultVector(std::ostream& o, std::vector<T> * value, MMParOptions* options = NULL) {
	std::string seperator = ",";
	std::string vector_start = "{";
	std::string vector_end = "}";
	if (options)
		if (options->outputStyle == styleSISI || options->outputStyle == styleSISINoIndent) {
			vector_start = "";
			if (isSTLSTLType(*value)) // vecor of vector or so
					 seperator = "\n";
			else
				seperator = "   ";
			vector_end = "";
		}
	o << vector_start;
	typename std::vector<T>::size_type last = value->size();
	if (last == 0) {
		o << vector_end;
		if (o.good())
			return true;
		else
			return false;
	}
	last--;
	if (options->outputStyle == styleSISI)
		o << "\t";

	typename std::vector<T>::const_iterator it;
	for (it = value->begin(); it != value->end() && last--; ++it) {
		if (!MMParSetWriteToStream(o, &(*it), options)) {
			return false;
		}
		o << seperator;
	}
	if (!MMParSetWriteToStream(o, &(*it), options))
		return false;
	o << vector_end;
	if (o.good())
		return true;
	else
		return false;
}

template<typename T>
bool MMParSetWriteToStream(std::ostream& o, std::vector<T> * value, MMParOptions* options = NULL) {
	return MMParSetWriteToStream_DefaultVector(o, value, options);
}

template<typename T>
bool MMParSetWriteToStream(std::ostream& o, const std::vector<T> * value, MMParOptions* options = NULL) {
	return MMParSetWriteToStream_DefaultVector(o, (std::vector<T> *) value, options);
}
// ----- c-array: (wrapped in mmArrayWrapper)

// MMParNode < mmArrayWrapper < T , N1> > * addByReference( T (&a) [N1] , const char* name, const char* description=NULL) {

template<typename T, size_t N1>
bool MMParSetWriteToStream_DefaultArray(std::ostream& o, mmArrayWrapper<T, N1> * value, MMParOptions* options = NULL) {
	std::string seperator = ",";
	std::string vector_start = "{";
	std::string vector_end = "}";
	if (options)
		if (options->outputStyle == styleSISI || options->outputStyle == styleSISINoIndent) {
			vector_start = "";
			vector_end = "";
			seperator = "   ";
		}
	o << vector_start;
	for (size_t i = 0; i < value->s1; i++) {
		if (!MMParSetWriteToStream(o, &(value->parray[i]), options)) {
			return false;
		}
		if (i < value->s1 - 1)
			o << seperator;
	}
	o << vector_end;
	if (o.good())
		return true;
	else
		return false;
}

template<typename T, size_t N1>
bool MMParSetWriteToStream(std::ostream& o, const mmArrayWrapper<T, N1> * value, MMParOptions* options = NULL) {
	return MMParSetWriteToStream_DefaultArray(o, (mmArrayWrapper<T, N1> *)value, options);
}

template<typename T, size_t N1>
bool MMParSetWriteToStream(std::ostream& o, mmArrayWrapper<T, N1> * value, MMParOptions* options = NULL) {
	return MMParSetWriteToStream_DefaultArray(o, value, options);
}

template<typename T, size_t N1, size_t N2>
bool MMParSetWriteToStream_DefaultArray(std::ostream& o, mmArrayWrapper2D<T, N1, N2> * value,
	 MMParOptions* options = NULL) {
	std::string seperator = ",";
	std::string vector_start = "{";
	std::string vector_end = "}";
	if (options)
		if (options->outputStyle == styleSISI || options->outputStyle == styleSISINoIndent) {
			vector_start = "";
			vector_end = "\n";
			seperator = "   ";
		}
	o << vector_start;
	for (size_t j = 0; j < value->s2; j++) {
		o << vector_start;
		for (unsigned int i = 0; i < value->s1; i++) {
			if (!MMParSetWriteToStream(o, value->getAt(i, j), options)) {
				return false;
			}
			if (i < value->s1 - 1)
				o << seperator;
		}
		if (options->outputStyle == styleSISI || options->outputStyle == styleSISINoIndent) {
			if (j < value->s2 - 1)
				o << vector_end;
		}
		else {
			o << vector_end;
			if (j < value->s2 - 1)
				o << seperator;
		}
	}
	if (options->outputStyle != styleSISI || options->outputStyle == styleSISINoIndent) {
		o << vector_end;
	}
	if (o.good())
		return true;
	else
		return false;
}

template<typename T, size_t N1, size_t N2>
bool MMParSetWriteToStream(std::ostream& o, const mmArrayWrapper2D<T, N1, N2> * value, MMParOptions* options = NULL) {
	return MMParSetWriteToStream_DefaultArray(o, (mmArrayWrapper2D<T, N1, N2> *)value, options);
}

template<typename T, size_t N1, size_t N2>
bool MMParSetWriteToStream(std::ostream& o, mmArrayWrapper2D<T, N1, N2> * value, MMParOptions* options = NULL) {
	return MMParSetWriteToStream_DefaultArray(o, value, options);
}

// ------------------------ pointer array (in mmPointerArrayWrapper) ---------------
template<typename T>
bool MMParSetWriteToStream_DefaultPointerArray(std::ostream& o, mmPointerArrayWrapper<T> * value,
	 MMParOptions* options = NULL) {
	std::string seperator = ",";
	std::string vector_start = "{";
	std::string vector_end = "}";
	if (options)
		if (options->outputStyle == styleSISI || options->outputStyle == styleSISINoIndent) {
			vector_start = "";
			if (isPointerType(*value)) // vecor of vector or so
					 seperator = "\n";
			else
				seperator = "   ";
			vector_end = "";
		}
	o << vector_start;

	size_t last = value->size();
	if (last == 0) {
		o << vector_end;
		if (o.good())
			return true;
		else
			return false;
	}
	last--;

	if (options->outputStyle != styleSISINoIndent)
		o << "\t";

	for (unsigned long i = 0; i < value->size() && last-- != 0; i++) {
		if (!MMParSetWriteToStream(o, &(value->getAt(i)), options)) {
			return false;
		}
		o << seperator;
	}
	if (value->size() > 0)
		if (!MMParSetWriteToStream(o, &(value->getAt(value->size() - 1)), options))
			return false;
	o << vector_end;
	if (o.good())
		return true;
	else
		return false;
}

template<typename T>
bool MMParSetWriteToStream(std::ostream& o, const mmPointerArrayWrapper<T> * value, MMParOptions* options = NULL) {
	return MMParSetWriteToStream_DefaultPointerArray(o, (mmPointerArrayWrapper<T> *)value, options);
}

template<typename T>
bool MMParSetWriteToStream(std::ostream& o, mmPointerArrayWrapper<T> * value, MMParOptions* options = NULL) {
	return MMParSetWriteToStream_DefaultPointerArray(o, value, options);
}

template<typename T>
bool MMParSetWriteToStream_DefaultArray(std::ostream& o, mmPointerArrayWrapper2D<T> * value,
	 MMParOptions* options = NULL) {
	std::string seperator = ",";
	std::string vector_start = "{";
	std::string vector_end = "}";
	if (options)
		if (options->outputStyle == styleSISI || options->outputStyle == styleSISINoIndent) {
			vector_start = "";
			vector_end = "\n";
			seperator = "   ";
		}
	o << vector_start;
#ifdef oldernicerbutnotsisiformat
	for (unsigned long j = 0; j < value->sizey; j++) {
		o << vector_start;
		for (unsigned int i = 0; i < value->sizex; i++) {
			T tval = value->getAt(i, j);
			if (!MMParSetWriteToStream(o, &tval, options)) {
				return false;
			}
			if (i < value->sizex - 1)
				o << seperator;
		}
		if (options) {
			if (options->outputStyle == styleSISI || options->outputStyle == styleSISINoIndent) {
				if (j < value->sizey - 1)
					o << vector_end;
			}
		}
		else {
			o << vector_end;
			if (j < value->sizey - 1)
				o << seperator;
		}
	}
	if (options) {
		if (options->outputStyle == styleSISI || options->outputStyle == styleSISINoIndent) {; // do nothing for sisi
		}
	}
	else {
		o << vector_end;
	}
#else
	// SiSi has other order of values in the file. file x direction is code y index.
	bool usedefault = false;
	if (options) {
		if (options->outputStyle == styleSISI || options->outputStyle == styleSISINoIndent) {
			o << vector_start;
			for (unsigned int i = 0; i < value->sizex; i++) {
				if (options->outputStyle != styleSISINoIndent)
					o << "\t";
				for (unsigned long j = 0; j < value->sizey; j++) {
					T tval = value->getAt(i, j);
					if (!MMParSetWriteToStream(o, &tval, options)) {
						return false;
					}
					if (j < value->sizey - 1)
						o << seperator;
				}
				if (i < value->sizex - 1)
					o << vector_end;
			}
		}
		else {
			usedefault = true;
		}
	}
	else {
		usedefault = true;
	}
	if (usedefault) {
		for (unsigned long j = 0; j < value->sizey; j++) {
			o << vector_start;
			for (unsigned int i = 0; i < value->sizex; i++) {
				T tval = value->getAt(i, j);
				if (!MMParSetWriteToStream(o, &tval, options)) {
					return false;
				}
				if (i < value->sizex - 1)
					o << seperator;
			}
			o << vector_end;
			if (j < value->sizey - 1)
				o << seperator;
		}
		o << vector_end;
	}
#endif

	if (o.good())
		return true;
	else
		return false;
}

template<typename T>
bool MMParSetWriteToStream(std::ostream& o, const mmPointerArrayWrapper2D<T> * value, MMParOptions* options = NULL) {
	return MMParSetWriteToStream_DefaultArray(o, (mmPointerArrayWrapper2D<T> *)value, options);
}

template<typename T>
bool MMParSetWriteToStream(std::ostream& o, mmPointerArrayWrapper2D<T> * value, MMParOptions* options = NULL) {
	return MMParSetWriteToStream_DefaultArray(o, value, options);
}

// ---------------------------------------------------------------------------
// READ FROM istream
// ---------------------------------------------------------------------------
/*

 */

// The Default INPUT function (just needed as helper in this file).
template<typename T>
bool MMParSetReadFromStream_Default(std::istream& in, T* value, MMParOptions* options = NULL) {
	ForwardWhiteSpaces(in);
	if (options != NULL) {
		// options->applyTo(in);
	}
	return (in >> *value); // todo: Better error check
}

// --- now the global functions which uses the helper functions/classes: ---

#ifdef __mitdefault__

// global default function.
// bcc32 wants declaration, well, at least it does not work without.
template<typename T>
bool MMParSetReadFromStream(std::istream& in, T* value, MMParOptions* options = NULL);

template<typename T>
bool MMParSetReadFromStream(std::istream& in, T* value, MMParOptions* options) {
	std::string ts =
		 "ERROR: This function MMParSetReadFromStream is the default version and should only be called for unknown types";
	cerr << ts << endl;
	// MMErrorMessage(ts, 4); // you might want to uncomment this line if you like the default beahviour. This exception is here to make sure you understand the warning
	// bool ok=(in>>*value);
	// cerr<<"       Value is:"<<*value<<endl;
	// cout<<"       and of type: "<<typeid(*value).name()<<endl;
	// return (ok);
	return (in >> *value);
}
#endif

#define Create_MMParSetReadFromStream(U) \
inline bool MMParSetReadFromStream(std::istream& in, U* value, MMParOptions* options=NULL) { \
	 in>>*value; \
	 return (!in.fail()); \
}

Create_MMParSetReadFromStream(bool);
Create_MMParSetReadFromStream(int);
Create_MMParSetReadFromStream(unsigned int);
Create_MMParSetReadFromStream(long int);
Create_MMParSetReadFromStream(unsigned long int);
Create_MMParSetReadFromStream(double);
Create_MMParSetReadFromStream(float);

Create_MMParSetReadFromStream(long long);
Create_MMParSetReadFromStream(unsigned long long);

// Create_MMParSetReadFromStream();

template<typename T>
bool MMParSetReadFromStream(std::istream& in, T* value, MMParOptions* options = NULL) {
	std::cerr <<
		 "ERROR: This function MMParSetReadFromStream is the default version and should only be called for unknown types" <<
		 std::endl;
	in >> *value;
	bool ok = in.good();
	std::cerr << "       Value is:" << *value << std::endl;
	std::cout << "       and of type: " << typeid(*value).name() << std::endl;
	MMErrorMessage(
		 "ERROR: This function MMParSetReadFromStream is the default version and should only be called for unknown types",
		 4); // you might want to uncomment this line if you like the default beahviour. This exception is here to make sure you understand the warning
	return (ok);
	// return (in>>*value);
}

/*
 string special: if first nonspace is quote then all is read until the next quote (quote=");
 else all is read until space,}\t or \n occur.
 mimu TODO: Escaping in MMGetBetweenQuotes not switched on - CHECK !!!
 */
inline bool MMParSetReadFromStream(std::istream& in, std::string* value, MMParOptions* options = NULL) {
	ForwardWhiteSpaces(in);
	std::istream::int_type nextchar = in.peek();
	if (nextchar == -1)
		return false; // peek failed
	if (nextchar != '\"') {
		if (MMGetLine(in, *value, " ,}\t\n\r") >= 1)
			return true;
	}
	else {
		return MMGetBetweenQuotes(in, *value);
	}
	return false;
}

// char* special ----  DANGER: writes into memory - length of allocated memory is not and cannot be checked!!!:
inline bool MMParSetReadFromStream(std::istream& in, char** value, MMParOptions* options = NULL) {
	// mimu todo Hier sollte eigentlich eine WARNUNG ausgegeben werden wegen möglicher Pufferüberläufe!!!
	std::string ts;
	if (!MMParSetReadFromStream(in, &ts, options))
		return false;
	for (unsigned int i = 0; i < ts.size(); i++) {
		(*value)[i] = ts[i];
	}
	(*value)[ts.size()] = 0;
	return true;
}

inline bool MMParSetReadFromStream(std::istream& in, const char** value, MMParOptions* options = NULL) {
	// mimu todo Hier sollte eigentlich eine WARNUNG ausgegeben werden !!! - nicht in const lesen !!!
	char** value2 = (char**)value;
	return MMParSetReadFromStream(in, value2, options);
}

// char[somenumber] special (no buffer overrun - but it cuts strings at buffer end without warning - todo?):
template<size_t N1>
inline bool MMParSetReadFromStream(std::istream& in, char(*value)[N1], MMParOptions* options = NULL) {
	std::string ts;
	if (!MMParSetReadFromStream(in, &ts, options))
		return false;
	long lasti = -1;
	for (unsigned int i = 0; i < ts.size() && i < N1 - 1; i++) {
		(*value)[i] = ts[i];
		lasti = i;
	}
	(*value)[++lasti] = 0;
	return true;
}

// unsigned char[somenumber] special (no buffer overrun - but it cuts strings at buffer end without warning - todo?):
template<size_t N1>
inline bool MMParSetReadFromStream(std::istream& in, unsigned char(*value)[N1], MMParOptions* options = NULL) {
	std::string ts;
	if (!MMParSetReadFromStream(in, &ts, options))
		return false;
	unsigned int lasti;
	for (unsigned int i = 0; i < ts.size() && i < N1 - 1; i++) {
		(*value)[i] = ts[i];
		lasti = i;
	}
	(*value)[++lasti] = 0;
	return true;
}

// STL container special
#define Create_MMParSetReadFromStream_STLContainer(U) \
template <typename T>                                  \
bool MMParSetReadFromStream(std::istream& in, U <T> * value, MMParOptions* options=NULL) { \
	if (options) if (options->outputStyle == styleSISI || options->outputStyle == styleSISINoIndent) { \
		ForwardWhiteSpacesWithoutNewline(in); \
		value->clear();                        \
		while (1) {                             \
			T tvar;                               \
			ForwardWhiteSpacesWithoutNewline(in);  \
			if (in.peek() == '\n') {                \
				in.get();                             \
				break;                                 \
			}                                          \
			if (!MMParSetReadFromStream(in, &tvar, options)) { \
				return false;              \
			}                              \
			value->push_back(tvar);         \
		}                                   \
		if(options) {                        \
			if(options->dimensions.size()>0) { \
				std::cout<<options->dimensions[0]<<std::endl; \
				if(value->size() < options->dimensions[0] ) {  \
					value->resize(options->dimensions[0]);       \
				}                                                \
			}                                                    \
		}                                                        \
		return true;                         \
	}                 \
	\
	ForwardWhiteSpaces(in);                 \
	if( in.get() != '{' ) {                  \
		return false;                          \
	}                                          \
	value->clear();                             \
	while(1) {                                   \
		T tvar;                                    \
		ForwardWhiteSpaces(in);                     \
		if(in.peek()=='}') {                         \
			in.get();                                  \
			break;                                      \
		}                                               \
	  if(in.peek()==',') {                              \
		 in.get();                                        \
	  }                                                   \
	  if(!MMParSetReadFromStream(in, &tvar, options)) {    \
		  return false;                                      \
	  }                                                      \
	  value->push_back(tvar);                                 \
	}                                                          \
	return true;					\
}

Create_MMParSetReadFromStream_STLContainer(std::list);
Create_MMParSetReadFromStream_STLContainer(std::deque);

inline std::string stringpeek(std::istream & in) {
	std::string ts;
	long int tpos = in.tellg();
	if (tpos == -1) {
		return ts;
		// mimu todo: exception ???
	}
	if (!(in >> ts)) {
		// mimu todo: exception ???
	}
	in.seekg(tpos); // mimu todo checken obs ging
	return ts;
}

// vector special (one could use the STLContainer thingy above, but this is kept here for easyer debugging):
template<typename T>
bool MMParSetReadFromStream(std::istream& in, std::vector<T> * value, MMParOptions* options = NULL) {
	if (options)
		if (options->outputStyle == styleSISI || options->outputStyle == styleSISINoIndent) {
			ForwardWhiteSpacesWithoutNewline(in);
			value->clear();
			while (1) {
				T tvar;
				ForwardWhiteSpacesWithoutNewline(in);
				int mypeek = in.peek();
				if (mypeek == std::istream::traits_type::eof()) {
					break;
				}
				if (mypeek == '\n') {
					in.get();
					break;
				}
				if (stringpeek(in) == "end") {
					break;
				}
				if (!MMParSetReadFromStream(in, &tvar, options)) {
					return false;
				}
				value->push_back(tvar);
			}
			// enlarge to dimensions from sisi file if needed:  (mimu todo: this might now tork properly with multidimensional vectors ...)
			if (options) {
				if (options->dimensions.size() > 0) {
					// std::cout<<options->dimensions[0]<<std::endl;
					if (value->size() < options->dimensions[0]) {
						value->resize(options->dimensions[0]);
					}
				}
			}
			return true;
		}
	// if not styleSisi:
	ForwardWhiteSpaces(in);
	if (in.get() != '{') {
		return false;
	}
	value->clear();
	while (1) {
		T tvar;
		ForwardWhiteSpaces(in);
		if (in.peek() == '}') {
			in.get();
			break;
		}
		if (in.peek() == ',') {
			in.get();
		}
		if (!MMParSetReadFromStream(in, &tvar, options)) {
			return false;
		}
		value->push_back(tvar);
	}
	return true;
}

// mmarraywrapper for c-style arrays
template<typename T, size_t N1>
bool MMParSetReadFromStream(std::istream& in, mmArrayWrapper<T, N1> * value, MMParOptions* options = NULL) {
	if (options)
		if (options->outputStyle == styleSISI || options->outputStyle == styleSISINoIndent) {
			ForwardWhiteSpacesWithoutNewline(in);
			for (unsigned int i = 0; i < value->s1; i++) {
				T tvar;
				ForwardWhiteSpacesWithoutNewline(in);
				int mypeek = in.peek();
				if (mypeek == std::istream::traits_type::eof()) {
					break;
				}
				if (mypeek == '\n') {
					in.get();
					break;
				}
				if (!MMParSetReadFromStream(in, &tvar, options)) {
					return false;
				}
				value->parray[i] = tvar;
			}
			return true;
		}
	// if not styleSisi:
	ForwardWhiteSpaces(in);
	if (in.get() != '{') {
		return false;
	}
	// value->clear();
	for (unsigned int i = 0; i < value->s1; i++) {
		T tvar;
		ForwardWhiteSpaces(in);
		if (in.peek() == '}') {
			in.get();
			break;
		}
		if (in.peek() == ',') {
			in.get();
		}
		if (!MMParSetReadFromStream(in, &tvar, options)) {
			return false;
		}
		value->parray[i] = tvar;
	}
	ForwardWhiteSpaces(in);
	if (in.peek() == '}') {
		in.get();
	}
	return true;
}

template<typename T, size_t N1, size_t N2>
bool MMParSetReadFromStream(std::istream& in, mmArrayWrapper2D<T, N1, N2> * value, MMParOptions* options = NULL) {
	if (options)
		if (options->outputStyle == styleSISI || options->outputStyle == styleSISINoIndent) {
			ForwardWhiteSpacesWithoutNewline(in);
			for (unsigned int j = 0; j < value->s2; j++) {
				ForwardWhiteSpaces(in);
				for (unsigned int i = 0; i < value->s1; i++) {
					T tvar;
					ForwardWhiteSpacesWithoutNewline(in);
					int mypeek = in.peek();
					if (mypeek == std::istream::traits_type::eof()) {
						break;
					}
					if (mypeek == '\n') {
						in.get();
						break;
					}
					if (!MMParSetReadFromStream(in, &tvar, options)) {
						return false;
					}
					value->setAt(i, j, tvar);
				}
				ForwardWhiteSpaces(in);
			}
			return true;
		}
	// else if not sisStyle:
	ForwardWhiteSpaces(in);
	if (in.get() != '{') {
		return false;
	}
	// value->clear();
	for (unsigned int j = 0; j < value->s2; j++) {
		ForwardWhiteSpaces(in);
		if (in.get() != '{') {
			return false;
		}
		for (unsigned int i = 0; i < value->s1; i++) {
			T tvar;
			ForwardWhiteSpaces(in);
			if (in.peek() == '}') {
				in.get();
				break;
			}
			if (in.peek() == ',') {
				in.get();
			}
			if (!MMParSetReadFromStream(in, &tvar, options)) {
				return false;
			}
			// value->parray[i]=tvar;
			value->setAt(i, j, tvar);
		}
		ForwardWhiteSpaces(in);
		if (in.peek() == '}') {
			in.get();
		}
		ForwardWhiteSpaces(in);
		if (in.peek() == ',') {
			in.get();
		}
	}
	ForwardWhiteSpaces(in);
	if (in.peek() == '}') {
		in.get();
	}
	return true;
}

// ---------- mmPointerArrayWraper for pointer arrays. assuming memory needs to be created on read. all vecause of sisi.

template<typename T>
bool MMParSetReadFromStream(std::istream& in, mmPointerArrayWrapper<T> * value, MMParOptions* options = NULL) {
	if (options)
		if (options->outputStyle == styleSISI || options->outputStyle == styleSISINoIndent) {
			ForwardWhiteSpacesWithoutNewline(in);
			value->clear();
			while (1) {
				T tvar;
				ForwardWhiteSpacesWithoutNewline(in);
				int mypeek = in.peek();
				if (mypeek == std::istream::traits_type::eof()) {
					break;
				}
				if (mypeek == '\n') {
					in.get();
					break;
				}
				if (stringpeek(in) == "end") {
					break;
				}
				if (!MMParSetReadFromStream(in, &tvar, options)) {
					return false;
				}
				value->push_back(tvar);
			}
			if (options) {
				// if(!isPointerType(T)) options->dimensions.clear();
				options->dimensions.clear();
				options->dimensions.push_back(value->size());
			}
			return true;
		}
	// if not styleSisi assume styleINI:
	ForwardWhiteSpaces(in);
	if (in.get() != '{') {
		return false;
	}
	value->clear();
	while (1) {
		T tvar;
		ForwardWhiteSpaces(in);
		if (in.peek() == '}') {
			in.get();
			break;
		}
		if (in.peek() == ',') {
			in.get();
		}
		if (!MMParSetReadFromStream(in, &tvar, options)) {
			return false;
		}
		value->push_back(tvar);
	}
	if (options) {
		// if(!isPointerType(T)) options->dimensions.clear();
		options->dimensions.clear();
		options->dimensions.push_back(value->size());
	}
	return true;
}

template<typename T>
bool MMParSetReadFromStream(std::istream& in, mmPointerArrayWrapper2D<T> * value, MMParOptions* options = NULL) {
	if (options)
		if (options->outputStyle == styleSISI || options->outputStyle == styleSISINoIndent) {
			ForwardWhiteSpacesWithoutNewline(in);
			// value->clear();
			value->reset(); // clear plus resetting input position
			bool doContinue = true;
			while (doContinue) {
				while (1) {
					T tvar;
					ForwardWhiteSpacesWithoutNewline(in);
					int mypeek = in.peek();
					if (mypeek == std::istream::traits_type::eof()) {
						doContinue = false;
						break;
					}
					if (mypeek == '\n') {
						in.get();
						break;
					}
					if (stringpeek(in) == "end") {
						break;
					}
					if (!MMParSetReadFromStream(in, &tvar, options)) {
						return false;
					}
					value->push_back(tvar);
				}
				ForwardWhiteSpacesWithoutNewline(in);
				if (stringpeek(in) == "end") {
					break;
				}
				value->newline();
			}
			value->optimizeMem();
			if (options) {
				options->dimensions.clear();
				options->dimensions.push_back(value->sizey);
				options->dimensions.push_back(value->sizex);
				// sisi wants it this way. really strange.
			}
			return true;
		}
	// else if not sisStyle:
	// if not styleSisi:
	ForwardWhiteSpaces(in);
	if (in.get() != '{') {
		return false;
	}
	value->clear();
	while (1) {
		while (1) {
			T tvar;
			ForwardWhiteSpaces(in);
			if (in.peek() == '}') {
				in.get();
				break;
			}
			if (in.peek() == ',') {
				in.get();
			}
			if (!MMParSetReadFromStream(in, &tvar, options)) {
				return false;
			}
			value->push_back(tvar);
		}
		value->newline();
	}

	// mimun todo : this is unreachable code here:
	value->optimizeMem();
	if (options) {
		options->dimensions.clear();
		options->dimensions.push_back(value->sizey);
		options->dimensions.push_back(value->sizex);
		// sisi wants it in this order. strange eh?
	}
	return true;
}

/*
 Further specials are included with the files which define the classes, like e.g. MMValidValues.
 Otherwise we have problems with circular inclusion of templates - some (most, all?) compilers do not like this.

 */

#endif
