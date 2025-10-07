#ifndef __MMINTERVAL_H
#define __MMINTERVAL_H
#include <MMParSet/MMParHelperFunctions.h>
// #pragma message "MMInterval.h BEGIN"
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
/* Name
 * Description
 * valid values (intervals or single values) and their description
 *
 * Units
 *
 * ? other spcific descriptions
 *
 *
 *  ? display options ( like combo box for multiple choice of valid values and their description)
 */

// int globalid=0;
template<class T>
class MMInterval {
public:
	T left;
	T right;
	bool isLeftOpen;
	bool isRightOpen;

	MMInterval() {
		isLeftOpen = false;
		isRightOpen = false;
	}

	MMInterval(const MMInterval& i) {
		Copy(i);
	}

	MMInterval(T & leftBorder, T & rightBorder, bool leftopen = false, bool rightopen = false) {
		left = leftBorder;
		right = rightBorder;
		isLeftOpen = leftopen;
		isRightOpen = rightopen;
	}

	~MMInterval() {
	}

	bool operator > (const MMInterval<T> & x) const {
		if (x.right < left)
			return true;
		if (!(x.right > left)) // equal
			if (x.isRightOpen || isLeftOpen)
				return true;
		return false;
	}

	bool operator < (const MMInterval<T> & x) const {
		if (x.left > right)
			return true;
		if (!(x.left < right)) // equal
			if (x.isLeftOpen || isRightOpen)
				return true;
		return false;
	}

	void Copy(const MMInterval<T> & x) {
		left = x.left;
		right = x.right;
		// isInterval=x.isInterval;
		isLeftOpen = x.isLeftOpen;
		isRightOpen = x.isRightOpen;
	}
	MMInterval<T>operator = (const MMInterval<T>&in) {
		Copy(in);
		return *this;
	}

	bool Contains(T d) {
		if (left < d && d < right)
			return true;
		else if (left == d && !isLeftOpen)
			return true;
		else if (right == d && !(isRightOpen))
			return true;
		else
			return false;
	}

	bool Contains(const MMInterval<T> &ci) {
		bool containsLeft = false;
		bool containsRight = false;
		if (Contains(ci.left))
			containsLeft = true;
		else if (ci.left == left && (ci.isLeftOpen && isLeftOpen))
			containsLeft = true;
		if (Contains(ci.right))
			containsRight = true;
		else if (ci.right == right && (ci.isRightOpen && isRightOpen))
			containsRight = true;
		if (containsLeft && containsRight)
			return true;
		else
			return false;
	}

	bool writeToStream(std::ostream &o) const {
		if (isLeftOpen)
			o << "(";
		else
			o << "[";
		o << this->left;
		o << ",";
		o << this->right;
		if (isRightOpen)
			o << ")";
		else
			o << "]";
		return o.good();
	};

	bool readFromStream(std::istream& in) {
		ForwardWhiteSpaces(in);
		unsigned int ti = in.get();
		if (ti == '(') {
			isLeftOpen = true;
		}
		else if (ti == '[') {
			isLeftOpen = false;
		}
		else
			return false;
		if (!(in >> left))
			return false;
		ForwardWhiteSpaces(in);
		ti = in.get();
		if (ti != ',')
			return false;
		ForwardWhiteSpaces(in);
		if (!(in >> right))
			return false;
		if (ti == ')') {
			isRightOpen = true;
		}
		else if (ti == ']') {
			isRightOpen = false;
		}
		else
			return false;
		return true;
	}
};

#endif
