#ifndef __MMPAROPTIONS_H
#define __MMPAROPTIONS_H

// #pragma message "MMParOptions.h BEGIN"
enum MMParSetOutputStyle {
	styleINI, styleXML, styleJSON, styleCMD, styleINIV, styleSISI, styleSISINoIndent
};

// ----------------------------------------------------------------------------
/*
 OPTIONS for stream input/output.
 precision: default is full precision, meaning std::numeric_limits<double>::digits10 + 2;

 */
class MMParOptions {
public:
	MMParOptions(int _precision = -1) {
		setPrecision(_precision);
		ErrorMessageType = 4; // exception
	}

	MMParOptions(MMParOptions const & other) {
		precision = other.precision;
		ErrorMessageType = other.ErrorMessageType;
		outputStyle = other.outputStyle;
		// dimensions is not assigned. This is usually set when creating a new
	}
	MMParOptions& operator = (MMParOptions other) // note: argument passed by value
	{
		precision = other.precision;
		ErrorMessageType = other.ErrorMessageType;
		outputStyle = other.outputStyle;
		// dimensions is not assigned. This is usually set when creating a new
		// instance and should not be overwritten by inheritance.
		return *this;
	}

	void setPrecision(int _precision = -1) {
		if (_precision == -1)
			precision = std::numeric_limits<double>::digits10; // +2
		else
			precision = _precision;
	}

	void applyTo(std::ostream& ost) {
		if (precision != -1)
			ost << std::setprecision(precision);
	}

	int precision;
	int ErrorMessageType;
	MMParSetOutputStyle outputStyle;

	// for sisi:
	// if you wonder why it'shere and not in MMParNodeBase: We need it in the stream/IO functions for
	// sisi-style-files to determine dimension when reading from file.
	// these are only needed for SISI so we can output the array extends (SISI dimensions).
	std::vector<int>dimensions;
};

#endif
