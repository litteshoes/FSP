#ifndef _MMVALIUD_VALUES_H
#define _MMVALIUD_VALUES_H

#include "MMParSet/MMStreamIO.h"
#include <MMParSet/MMInterval.h>
#include <MMParSet/MMParOptions.h>
#include <MMParSet/MMParHelperFunctions.h>

// #pragma message "MMValidValues.h BEGIN"
/*

 - interval sollte mit MMWriteToStream ein und ausgelesen werden, damit es allgemeingültiger ist. Momentan wird es teilweise
 mit << >> - operatoren gelesen. In der Praxis ist das wiederum ok, da es meistens nur mit int, double und ähnlichem gentutzt wird.

 - TODO: geht momentan nur mit einem Interval beim Einlesen - es werden auch zwei korrekt eingelesen, aber irgendwie nicht geadded oder wieder ausgegeben. TODO!

 --> momentan nur ein interval erlaubt, Schreibweise interval= [ 123 , 214343 ] "und hier eine Beschreibung"
 */

template<class T>
class MMValidValueInterval : public MMInterval<T> {
public:
	std::string description;

	// write / read todo : ist mit operatoren aber nciht mit MMParSetREadFrom STream geschrieben.
	bool writeToStream(std::ostream &o) const {
		if (this->isLeftOpen)
			o << "(";
		else
			o << "[";
		if (!MMParSetWriteToStream(o, &this->left))
			return false;
		// bcc64 outupts nonsense or clashes if no this here - name conflict ?
		// o<<(this->left);
		if (!(o << ","))
			return false;
		if (!MMParSetWriteToStream(o, &this->right))
			return false;
		// o<<this->right;
		if (this->isRightOpen) {
			if (!(o << ")"))
				return false;
		}
		else if (!(o << "]"))
			return false;
		if (!description.empty()) {
			if (!(o << " \"" << description << "\" "))
				return false;
		}
		return true;
	};

	bool writeToStream_styleSISI(std::ostream &o) const {
		if (!MMParSetWriteToStream(o, &this->left))
			return false;
		if (!(o << ":"))
			return false;
		return MMParSetWriteToStream(o, & this->right);
	};

	// todo: here vielleicht auch die MMParSetReadFromStream Methoden nehmen???? ---
	bool readFromStream(std::istream& in) {
		ForwardWhiteSpaces(in);
		std::istream::int_type ti = in.peek();

		bool sisiStyle = false;
		if (ti == '(') {
			this->isLeftOpen = true;
			in.get();
		}
		else if (ti == '[') {
			this->isLeftOpen = false;
			in.get();
		}
		else if (isdigit(ti)) {
			sisiStyle = true; // just a guess.
		}
		else
			return false;

		if (!MMParSetReadFromStream(in, &(this->left)))
			return false;

		ForwardWhiteSpaces(in);
		ti = in.get();
		if (ti != ',' && ti != ':' && ti != '-')
			 // allow , : - as interval seperators.
				 return false;
		ForwardWhiteSpaces(in);

		if (!MMParSetReadFromStream(in, &(this->right)))
			return false;

		if (sisiStyle)
			return true;

		ForwardWhiteSpaces(in);
		ti = in.get();
		if (ti == ')') {
			this->isRightOpen = true;
		}
		else if (ti == ']') {
			this->isRightOpen = false;
		}
		else
			return false;
		ForwardWhiteSpaces(in);
		ti = in.peek();
		if (ti == -1)
			return true; // cannot peek=no description
		if (ti == '/') { // decription in c-line format //
			ti = in.get();
			ti = in.peek();
			if (ti != '/')
				return false;
			ti = in.get();
			bool ok = getline(in, description);
			if (ok) {
				ForwardWhiteSpaces(in);
				return true;
			}
			return false;
		}
		if (ti != '\"')
			return true; // no description
		// there is a description (in double quotes): read it:
		if (!MMGetBetweenQuotes(in, description))
			return false;
		// mimu todo: check if trailing double quote was read!!
		return true;
	}
};

#include <ctype.h>

template<class T>
class MMValidValues {
public:
	std::set<MMValidValueInterval<T> >values;

	bool isInside(T value) { // mimu todo this function does not make sense yet!!!
		MMValidValueInterval<T>ttt;
		ttt.left = value;
		ttt.right = value;
		return (values.find(ttt) != values.end());
		// mimu todo - should this not be past the end ?  Too fast hack here. TODO!!!
	}

	void addInterval(T left, T right, std::string _description = "", bool leftopen = false, bool rightopen = false) {
		MMValidValueInterval<T>ti;
		ti.left = left;
		ti.right = right;
		ti.isLeftOpen = leftopen;
		ti.isRightOpen = rightopen;
		ti.description = _description;
		addValidValueInterval(ti);
		/*
		 typename set< MMValidValueInterval < T > >::const_iterator it=values.find(ti);
		 //auto it=values.find(ti);
		 if(it==values.end()) {
		 values.insert(ti);
		 } else {
		 // mimu todo : sich überschndeiden´de Intervalle und values behandeln, z.B. bestehendes zerschneiden. oder mergen, aber dann geht description verloren.
		 }
		 */
	}

	void addValue(T left, std::string aDescription) {
		addInterval(left, left, aDescription);
	}

	bool isEmpty() {
		if (values.size() > 0) {
			return false;
		}
		return true;
	}

	void clear() {
		values.clear();
	}

	bool addValidValueInterval(MMValidValueInterval<T>& ti) {
		typename std::set<MMValidValueInterval<T> >::const_iterator it = values.find(ti);
		// auto it=values.find(ti);
		if (it == values.end()) {
			values.insert(ti);
		}
		else {
			// mimu todo : sich überschndeiden´de Intervalle und values behandeln, z.B. bestehendes zerschneiden. oder mergen, aber dann geht description verloren.
			return false;
		}
		return true;
	}

	void writeToStream(std::ostream &o) const
	{ // mimu todo korrektes Format ausgeben!!! Momentan nur Test, damits kompiliert. Format ausdenken!!!
		typename std::set<MMValidValueInterval<T> >::const_iterator it;
		for (it = values.begin(); it != values.end(); it++) {
			it->writeToStream(o);
		}
	}

	bool writeToStream_styleSISI(std::ostream &o) const {
		typename std::set<MMValidValueInterval<T> >::const_iterator it;
		for (it = values.begin(); it != values.end(); it++) {
			if (!it->writeToStream_styleSISI(o))
				return false;
		}
		return true;
	};

	// mimu todo: DAS HIER GEHT GAR NICHT FÜR SISI!!!! - ungetestet, von vector übernommen ....
	bool readFromStream(std::istream& in) {
		ForwardWhiteSpacesWithoutNewline(in);
		int firstchar = in.peek();
		bool onlyone = false;
		bool sisiStyle = false;
		if (firstchar == '[') {
			onlyone = true;
		}
		else if (isdigit(firstchar)) {
			sisiStyle = true;
		}
		else if (firstchar == '-') {
			sisiStyle = true;
			in.get();
			std::string ts;
			getline(in, ts);
			if (ts.length() >= 1) {
				int k = 33;
				return false;
			}
			return true;
		}
		else if (firstchar == '\r' || firstchar == '\n') {
			return true;
		}
		else if (firstchar != '{') {
			return false;
		}
		else {
			in.get(); // get '{'
		}
		clear();
		if (sisiStyle) {
			MMValidValueInterval<T>tvar;
			if (!tvar.readFromStream(in)) {
				return false;
			}
			addValidValueInterval(tvar);
			return true; // sisi allows one interval only.
		}
		while (1) {
			MMValidValueInterval<T>tvar;
			ForwardWhiteSpaces(in);
			if (in.peek() == '}') {
				in.get();
				break;
			}
			if (in.peek() == ',') {
				in.get();
			}
			if (!tvar.readFromStream(in)) {
				return false;
			}
			addValidValueInterval(tvar);
			if (onlyone)
				return true;
		}
		return true;
	}
};

// validvalues special:
template<typename T>
bool MMParSetWriteToStream(std::ostream& o, MMValidValues<T> * value, MMParOptions* options = NULL) {
	value->writeToStream(o);
	/*
	 typename std::set< MMValidValueInterval < T > > ::const_iterator it;
	 for(it=value->values.begin();it!=value->values.end();it++) {
	 if(it->isLeftOpen) o<<"(";
	 else o<<"[";
	 MMParSetWriteToStream(o, & it->left);
	 o<<",";
	 MMParSetWriteToStream(o, & it->right);
	 if(it->isRightOpen) o<<")";
	 else o<<"]";
	 if(!description.empty()){
	 o<<" { "<<description<<" } ";
	 }
	 }
	 */
}

// valid value interval special:

template<typename T>
bool MMParSetReadFromStream(std::istream& in, MMValidValues<T> * value, MMParOptions* options = NULL) {
	return value->readFromStream(in);
	/*
	 ForwardWhiteSpaces(in);
	 unsigned int ti=in.get();
	 if( ti == '(' ) {
	 isLeftOpen=true;
	 } else if(ti=='[') {
	 isLeftOpen=false;
	 } else return false;
	 if(! (in>>left)) return false;
	 ForwardWhiteSpaces(in);
	 ti=in.get();
	 if(ti!=',') return false;
	 ForwardWhiteSpaces(in);
	 if(! (in>>right)) return false;
	 if( ti == ')' ) {
	 isRightOpen=true;
	 } else if(ti==']') {
	 isRightOpen=false;
	 } else return false;
	 // mimu todo Kommentar felt noch
	 return true;
	 */
}

#endif
