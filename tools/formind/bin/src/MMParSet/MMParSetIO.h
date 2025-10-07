// ---------------------------------------------------------------------------

#ifndef MMParSetIOH
#define MMParSetIOH
#include <MMParSet/MMParSet.h>
#include <algorithm>
// ---------------------------------------------------------------------------
/*
 SISI - port doc:
 A.: Output
 1. normal variables (int, float, string, ...) work  with \d \u \r .
 \o will not get written - hope it is not needed? Does not make much sense in MMParset - but could be implemented.
 2. Subsets of MMParset will get interpreted as result list of variable types. Does not work for array yet. Does not include the active flat yet. Abnd maybe some more. Is it needed at all???????
 3. vector <something> : working on it, will be array of something.



 */
// ---------------------------------------------------------------------------

class MMParSetIO {
public:
	// MMParSet * par;
	MMParSetIO() {
	}

	bool writeToStream(std::ostream& o, MMParSet* par) {
		if (par->outputStyle == styleINI || par->outputStyle == styleINIV) {
			return writeToStream_styleINI(o, par);
		}
		else if (par->outputStyle == styleCMD) {
			// return ps.IntoStreamCommandlineStyle(o);
		}
		else if (par->outputStyle == styleSISI || par->outputStyle == styleSISINoIndent) {
			return writeToStream_styleSISI(o, par);
		}
		else {
			// return ps.IntoStreamXMLStyoperatorle(o);
			// mimu todo might want to throw exception here - and outputfile might have been deleted by now.
			return false;
		}
		return false; // to avoid compiler warnings
	}

	bool readFromStream(std::istream& in, MMParSet* par) {
		if (!in.good()) {
			std::string ts = "MMParSet ERROR: Input stream is not good. Cannot read from input stream. ";
			MMParSetError(ts, par->options.ErrorMessageType);
		}

		// -------- workaround for utf-8 files:
		// utf-8 seems to start with EE BB BF and continue in a normal way.
		// so we check if the first 3 chars are EE BB BF and get them here
		// one could implement it better, saving the position and looking
		// if all three are there. But it works this way.
		int ti = in.peek();
		if (ti == 0xEf) { // 239
			in.get();
			int ti = in.peek();
			if (ti == 0xBB) { // 187
				in.get();
				int ti = in.peek();
				if (ti == 0xBF) {
					in.get();
				}
			}
		}
		// end workaround utf-8

		MMParSetOutputStyle guessedStyle = guessParsetStyle(in);

		if (guessedStyle == styleINI) {
			return readFromStream_styleINI(in, par);
		}
		else if (guessedStyle == styleSISI) {
			return readFromStream_styleSISI(in, par);
		}
		else {
			std::string ts = "MMParSet: Cannot read input stream. Style of input unknown.";
			MMParSetError(ts, par->options.ErrorMessageType);
			return false;
		}
	}

	// ------------------------------------------------------------------
	MMParSetOutputStyle guessParsetStyle(std::istream &in) {
		MMParSetOutputStyle style = styleINI;
		ForwardWhiteSpaces(in);
		SkipComments(in);
		ForwardWhiteSpaces(in);
		int ti = in.peek();
		if (ti == '<')
			style = styleXML;
		else {
			std::string ts;
			getline(in, ts);
			if (ts.find("[") == std::string::npos && ts.find("=") == std::string::npos) {
				style = styleSISI;
			}
			std::stringstream ss;
			ss << ts;
			ss >> ts;
			if (ts == "comment")
				style = styleSISI;
		}
		in.seekg(0);

		// -------- workaround for utf-8 files:
		// utf-8 seems to start with EE BB BF and continue in a normal way.
		// so we check if the first 3 chars are EE BB BF and get them here
		// one could implement it better, saving the position and looking
		// if all three are there. But it works this way.
		ti = in.peek();
		if (ti == 0xEf) { // 239
			in.get();
			int ti = in.peek();
			if (ti == 0xBB) { // 187
				in.get();
				int ti = in.peek();
				if (ti == 0xBF) {
					in.get();
				}
			}
		}
		// end workaround utf-8

		return style;
	}
	// ------------------------------------------------------------------
	// ------------------------------------------------------------------

protected:

	/*******************************************************************
	 styleSISI
	 ****************************************************************** */

	// needs to_string from c++11:

#if __cplusplus <= 201103L
#include <sstream>

	template<typename T>
	std::string to_string(T value) {
		std::stringstream ss;
		ss << value;
		std::string ts;
		ss >> ts;
		return ts;
	}
	// #pragma message "WARNING: HALLO !!! This is not C++11. "
#endif

	// ---------- write - styleSISI -------------------------------------
	bool writeToStream_styleSISI(std::ostream& o, MMParSet* par) {
		std::vector<MMParNodeBase*>ParsetQueue;
		if (par->name != "" && par->name != "NN") {
			o << "\nlist\t" << par->name << std::endl;
			o << "\ttype\tparameter";
		}
		else {
			// o << "\nlist\t" << "NN" << "\n\ttype\tparameter"<<endl; // no default name for sisi.
		}
		if (par->subsetLevel > 0)
			 // these two lines might be a relic and might be trash. Parsets of Parsets not supportet for SISI par files.
				 o << "\ndata";
		// sort by read order (output_position):
		// std::sort(par->nodes.begin(), par->nodes.end(),
		// sortFunctionMMParNodeBase);
		par->sort();
		//
		bool headerWritten = false;
		for (unsigned int j = 0; j < par->nodes.size(); j++) {
			if (par->nodes[j]->isParset()) {
				// Output subsets after all parameters.
				ParsetQueue.push_back(par->nodes[j]);
			}
			else {
				// check if node was read from file or changed
				bool doOutputNode=true;
				if(par->outputOnlyReadAndChanged) {
					doOutputNode=false;
					if(par->nodes[j]->statusFlags & MMParNodeAlreadyReadFromStream
					  || par->nodes[j]->statusFlags & MMParNodeChanged) {
						doOutputNode=true;
					}
				}
				if(!doOutputNode)
					continue;

				// for nodes which were not read from file
				// write a header before output
				// (if no file was read write no header)
				if (!headerWritten && par->nodes[j]->output_position == ULONG_MAX && par->nodes[0]
					 ->output_position != ULONG_MAX) {
					// o << std::endl;
					o << std::endl;
					o << "comment =============================================================================" <<
						 std::endl;
					o << "comment ----------------------------------------------------------------------------- " <<
						 std::endl;
					o << "comment =============================================================================" <<
						 std::endl;
					headerWritten = true;
				}
				// first ouput comments which belong to this node:
				// o<<"*** comment id="<<par->nodes[j]->output_position<<std::endl;
				if (par->nodes[j]->comments.size())
					o << std::endl;
				for (unsigned int m = 0; m < par->nodes[j]->comments.size(); m++) {
					o << "comment";
//					if(par->nodes[j]->comments[m].size()) {
//						if(!isspace((par->nodes[j]->comments[m][0]))) {
//							o<<" ";
//						}
//					}
               o<<" ";
					o << par->nodes[j]->comments[m] << std::endl;
				}
				//

				if (par->nodes[j]->isComment()) {
					o << std::endl << "comment with name: " << par->nodes[j]->name;
					continue;
				}
				/*
				 if(par->subsetLevel > 0) {
				 o << endl << "\tresult\t" ; MMParSetWriteToStream(o, &par->nodes[j]->name);
				 o << endl << "\t\ttype\t" ; MMParSetWriteToStream(o, &par->nodes[j]->valueType);
				 //par->nodes[j]->valueIntoStream(o);  // SISI results are without value - crazy eh?
				 } */
				if (par->nodes[j]->valueTypes.size() == 1) {
					std::string sisiTypeName = typeToSisiType(par->nodes[j]->valueTypes[0]);
					o << std::endl << sisiTypeName;
					o << "\t" << par->nodes[j]->name << "\t";
					par->nodes[j]->valueIntoStream(o);
				}
				else { // stl type and array wrapper (and others? - not nice maybe - but only for sisi):
					o << std::endl << "array" << "\t" << par->nodes[j]->name;
				}
				if (par->nodes[j]->units.size()) {
					o << std::endl;
					if (par->subsetLevel > 0)
						o << "\t";
					o << "\t\\u ";
					MMParSetWriteToStream(o, &par->nodes[j]->units);
				}
				if (par->nodes[j]->description.size()) {
					o << std::endl;
					std::string lineBegin = "";
					if (par->subsetLevel > 0)
						lineBegin += "\t";
					lineBegin += "\t\\d ";
					o << lineBegin;
					lineBegin = "\n" + lineBegin;
					std::string ts = par->nodes[j]->description;
					stringReplaceAll(ts, "\n", lineBegin);
					o << ts; // unquoted output
				}
				if (par->nodes[j]->longDescription.size()) {
					o << std::endl;
					std::string lineBegin = "";
					if (par->subsetLevel > 0)
						lineBegin += "\t";
					lineBegin += "\t\\ld ";
					o << lineBegin;
					lineBegin = "\n" + lineBegin;
					std::string ts = par->nodes[j]->longDescription;
					stringReplaceAll(ts, "\n", lineBegin);
					o << ts; // unquoted output
					// o << std::endl;
					// if (par->subsetLevel > 0)
					// o << "\t";
					// o << "\t\\ld ";
					// MMParSetWriteToStream(o, &par->nodes[j]->longDescription);
				}
				if (par->nodes[j]->hasRange()) {
					o << std::endl;
					if (par->subsetLevel > 0)
						o << "\t";
					o << "\t\\r ";
					par->nodes[j]->rangeIntoStream(o, styleSISI);
				}
				if (par->nodes[j]->tags.size()) {
					o << std::endl;
					if (par->subsetLevel > 0)
						o << "\t";
					o << "\t\\i ";
					// MMParSetWriteToStream(o, &par->nodes[j]->tags);
					MMParSetWriteToStreamNoQuote(o, &par->nodes[j]->tags);
				}
				if (par->nodes[j]->valueTypes.size() > 1) { // more stl type:
					std::string sisiTypeName =
						 typeToSisiType(par->nodes[j]->valueTypes[par->nodes[j]->valueTypes.size() - 1]);
					o << std::endl << "\t" << "typeOfArray" << "\t" << sisiTypeName;
					std::string dim = "";
					for (unsigned int k = 0; k < par->nodes[j]->options.dimensions.size(); k++) {
						if (k > 0)
							dim += "   ";
						dim += to_string(par->nodes[j]->options.dimensions[k]);
					}
					o << std::endl << "\t" << "dimension" << "\t" << dim;
					o << std::endl << "data" << std::endl;
					par->nodes[j]->valueIntoStream(o);
					o << std::endl << "end" << std::endl;
				}
				// mimu todo hier Ausgabe für arrays ???? Oder kann man es oben mit einbauen???
				o << std::endl;
			}
		}
		o << std::endl;
		if (par->subsetLevel > 0)
			o << "end" << std::endl;
		// now output subsets:
		for (unsigned int i = 0; i < ParsetQueue.size(); i++) {
			o << "\n";
			ParsetQueue[i]->valueIntoStream(o);
		}
		return o.good();
	}

	bool readFromStream_styleSISI(std::istream& in, MMParSet* par) {
		// cerr<<"\n WARNING: readFromStream_styleSISI( ): not yet fully implemented !!!";
		std::string name;
		static std::string lastname;
		MMParNodeBase * tnode = NULL;
		MMParNodeBase * descriptionLastTnode = NULL;
		MMParNodeBase * longDescriptionLastTnode = NULL;
		std::vector<int>dimensions; // for array types
		std::vector<std::string>lastCommentsRead;
		// save comments read to add them to the next node read
		unsigned long filePositionCounter = 1;
		while (1) {
			long tpos = in.tellg();
			if (tpos == -1)
				break;
			// SkipComments(in);
			std::string ts;
			if (!(in >> ts)) {
				if (in.eof() || !in.good()) {
					return false; // just reached end of file.
				}
				else {
					std::string ts = "Input error";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
			}
			if (ts.size() == 1) {
				if (ts[0] == '\\') {
					std::string ts2;
					if (!(in >> ts2)) {
						if (in.eof() || !in.good()) {
							return false; // just reached end of file.
						}
						else {
							std::string ts = "Input error";
							MMParSetError(ts, par->options.ErrorMessageType);
						}
					}
					ts += ts2;
				}
			}
			// now check what we have :
			if (ts == "int" || ts == "float" || ts == "string" || ts == "array" || ts == "double" || ts == "char" ||
				 ts == "list") {
				lastname = name;
				name = "";
				if (!(in >> name)) {
					if (in.eof() || !in.good()) {
						return false; // just reached end of file.
					}
					else {
						std::string ts = "Input error";
						MMParSetError(ts, par->options.ErrorMessageType);
					}
				}
				tnode = par->findByName(name);
				if (!tnode) {
					bool autoCreateNode = false;
					if (autoCreateNode) {
						if (ts == "int")
							tnode = new MMParNode<int>(true);
						else if (ts == "float")
							tnode = new MMParNode<double>(true);
						else if (ts == "string") {
							std::string * sp = new std::string;
							MMParNode<std::string> *tpp = par->addByReference(*sp, name.c_str());
							// tpp->pValueMemIsMine=true;
							tnode = tpp;
						}
						else if (ts == "double")
							tnode = new MMParNode<double>(true);
						else if (ts == "char")
							tnode = new MMParNode<std::string>(true);
						// todo - vielleicht muss das auch irgendein dummy char klasse sein, damit bei output wieder alles stimmt?
						// todo mimu keine ahnung ob das geht:
						else if (ts == "list" || ts == "array") {
							tnode = new MMParNodeBase;
						}
						tnode->setName(name);

					}
					else {
						std::string ts = "Name \"" + name + "\" is not defined in MMParSet \"" + par->name +
							 "\". Value not read. Hint: Last name read was: \"" + lastname + "\"";
						MMParSetError(ts, par->options.ErrorMessageType);
						return false; // cannot find container for name
					}
				}
				// adjust output_position
				tnode->output_position = filePositionCounter++;
				// now read value:
				if (ts != "array" && ts != "list") {
					tnode->valueFromStream(in);
				}
				else {
					int k = 22; // do nothing - this line is here for debugging only
				}
				// save all comments read into this node:
				if (lastCommentsRead.size() > 0)
					tnode->comments = lastCommentsRead;
				lastCommentsRead.clear();
				continue; // get netxt token
			}
			else if (ts == "comment") {
				std::string sdummy;
				if (!getline(in, sdummy)) {
					std::string ts = "FAILED reading comment. Parameter name is: \"" + name +
						 "\". Last parameter name was\"" + lastname + "\"";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
				if(sdummy.size()) {
					if(sdummy[0]==' ') {
						sdummy.erase(0,1);
					}
				}
				lastCommentsRead.push_back(sdummy);
				// cout<<ts<<" "<<sdummy<<std::endl;
			}
			else if (ts == "\\r") {
				ForwardWhiteSpacesWithoutNewline(in);
				if (!tnode) {
					std::string ts = "\\r range identifier used without parameter name. Last parameter name was\"" +
						 lastname + "\"";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
				if (!tnode->rangeFromStream(in)) {
					std::string ts = "\\r Error reading range. Parameter name is: \"" + name +
						 "\". Last parameter name was\"" + lastname + "\"";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
			}
			else if (ts == "\\u") {
				ForwardWhiteSpacesWithoutNewline(in);
				if (!tnode) {
					std::string ts = "\\u unit identifier used without parameter name. Last parameter name was\"" +
						 lastname + "\"";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
				if (!getLineNoCR(in, tnode->units)) {
					std::string ts = "\\u FAILED reading units. Parameter name is: \"" + name +
						 "\". Last parameter name was\"" + lastname + "\"";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
			}
			else if (ts == "\\d") {
				ForwardWhiteSpacesWithoutNewline(in);
				if (!tnode) {
					std::string ts = "\\d description identifier used without parameter name. Last parameter name was\"" +
						 lastname + "\"";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
				bool wasFirstD = false;
				if (descriptionLastTnode != tnode) {
					descriptionLastTnode = tnode;
					tnode->description = "";
					wasFirstD = true;
				}
				std::string ts;
				if (!getLineNoCR(in, ts)) {
					std::string ts = "\\d FAILED reading description. Parameter name is: \"" + name +
						 "\". Last parameter name was\"" + lastname + "\"";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
				if (!wasFirstD) {
					tnode->description += "\n";
				}
				tnode->description += ts;
			}
			else if (ts == "\\ld") {
				ForwardWhiteSpacesWithoutNewline(in);
				if (!tnode) {
					std::string ts =
						 "\\ld long description identifier used without parameter name. Last parameter name was\"" +
						 lastname + "\"";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
				bool wasFirstLD = false;
				if (longDescriptionLastTnode != tnode) {
					longDescriptionLastTnode = tnode;
					tnode->longDescription = "";
					wasFirstLD = true;
				}
				std::string ts;
				if (!getLineNoCR(in, ts)) {
					std::string ts = "\\ld FAILED reading long description. Parameter name is: \"" + name +
						 "\". Last parameter name was\"" + lastname + "\"";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
				if (!wasFirstLD) {
					tnode->longDescription += "\n";
				}
				tnode->longDescription += ts;
			}
			else if (ts == "\\i" || ts == "\\t") {
				ForwardWhiteSpacesWithoutNewline(in);
				if (!tnode) {
					std::string ts = "\\i identifier used without parameter name. Last parameter name was\"" +
						 lastname + "\"";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
				if (!getLineNoCR(in, tnode->tags)) {
					std::string ts = "\\i FAILED reading description. Parameter name is: \"" + name +
						 "\". Last parameter name was\"" + lastname + "\"";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
			}
			else if (ts == "typeOfArray") {
				if (!tnode) {
					std::string ts = "typeOfArray identifier used without parameter name. Last parameter name was\"" +
						 lastname + "\"";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
				std::string dummy7; // int, double, string ....
				in >> dummy7;
				// we need to check that only if we autocreate values and have created an empty base class for list or vector:
				if (tnode->isBaseClass()) {
					MMParNodeBase* bp = tnode;
					if (dummy7 == "int") {
						tnode = new MMParNode<std::vector<int> >(true);
					}
					else if (dummy7 == "float" || dummy7 == "double") {
						tnode = new MMParNode<std::vector<double> >(true);
					}
					else {
						tnode = new MMParNode<std::vector<std::string> >(true);
					}
					*tnode = *bp;
					tnode->options = par->options;
					tnode->options.outputStyle = styleSISI;
					delete bp;

				}
			}
			else if (ts == "dimension") {
				if (!tnode) {
					std::string ts = "dimension identifier used without parameter name. Last parameter name was\"" +
						 lastname + "\"";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
				// mimu todo: MMParSEt should know dimension, is not interested - but we might want to check it ...
				tnode->options.dimensions.clear();
				std::string sdummy;
				getline(in, sdummy);
				std::stringstream ss;
				ss << sdummy;
				int i;
				while (ss >> i) {
					tnode->options.dimensions.push_back(i);
				}
			}

			else if (ts == "data") {
				if (!tnode) {
					std::string ts = "data identifier used without parameter name. Last parameter name was\"" +
						 lastname + "\"";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
				ForwardWhiteSpaces(in);
				tnode->valueFromStream(in);
				/*
				 int x=0, y=1;
				 if(dimensions.size()>=1) x=dimensions[0];
				 if(dimensions.size()>=2) y=dimensions[1];
				 int k=22;
				 for(int j=0;j<y;j++) {
				 for(int i=0;i<x;i++) {
				 ForwardWhiteSpaces(in);
				 tnode->valueFromStream(in);
				 }
				 }
				 // mimu todo read array here (unsing some functions ???? which )
				 // here dummy read:
				 //string ts7="";
				 //while(ts7!="end") in>>ts7;
				 */
				std::string ts7;
				if (!(in >> ts7)) {
					std::string ts = "not enought data in array \"" + name + "\". Lastname was: \"" + lastname + "\"";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
				if (ts7 != "end") {
					std::string ts = "Cannot find end identifier in array \"" + name + "\". Lastname was: \"" +
						 lastname + "\"";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
			}

			/* string sisiType;
			 string name;
			 if(!(in>>sisiType)) {
			 if (in.eof() || !in.good()) {
			 return false; // just reached end of file.
			 }
			 else {
			 string ts = "Input error";
			 MMParSetError(ts, par->options.ErrorMessageType);
			 }
			 }
			 if(!(in>>name)) {
			 if (in.eof() || !in.good()) {
			 return false; // just reached end of file.
			 }
			 else {
			 string ts = "Input error";
			 MMParSetError(ts, par->options.ErrorMessageType);
			 }
			 }
			 // have name and type.
			 if (!readFromStream_value_styleSISI(in, name, par, sisiType) ) {
			 // dann war der par nicht im set. error wird schon von funktion ausgegeben.
			 return false;
			 }
			 */
		}

		return true;
	}

	// reads values and roperties (everything except parsets).
	bool readFromStream_value_styleSISI(std::istream& in, std::string name, MMParSet* par, std::string sisiType) {
		static std::string lastname;
		MMParNodeBase * tnode = par->findByName(name);
		if (!tnode) {
			std::string ts = "Name \"" + name + "\" is not defined in MMParSet \"" + name +
				 "\". Value not read. Hint: Last name read was: \"" + lastname + "\"";
			MMParSetError(ts, par->options.ErrorMessageType);
			return false; // cannot find container for name
		}
		if (sisiType != "array") {
			tnode->valueFromStream(in);
		}

		// write to container:
		// mimu: tod: die naechsten muessten alle Fehler abfangen und zurückgeben !!!
		// bool haveError=false;
		// string errorMessage;
		/*
		 case 0 : tnode->valueFromStream(in);break;  //skipws;break;
		 case 1 : MMParSetReadFromStream(in,&tnode->units);break;  //skipws
		 case 2 : {//in.setf(ios::skipws);
		 MMParSetReadFromStream(in,&tnode->description);
		 break;
		 }
		 //case 3 : tcont->MinvalueReadFromStream(in);break;
		 //case 4 : tcont->MaxvalueReadFromStream(in);break;
		 case 5 : if(!tnode->rangeFromStream(in)) {
		 haveError=true;
		 errorMessage="Could not read range from stream. trying to read name=\""+name+"\". las name read was\""+lastname+"\".";
		 }
		 break;

		 // all other cases error:
		 string ts="Could not read \""+name+"\" from file. Type could not be classified. Hint: Last name read was: \"" + lastname +"\"";
		 MMParSetError(ts,par->options.ErrorMessageType);
		 return false;  //cannot find container for name
		 };
		 if(haveError){
		 MMParSetError(errorMessage,par->options.ErrorMessageType);
		 return false;
		 }
		 */
		lastname = name;
		return (true); // ok
	}

	/*******************************************************************
	 styleINI
	 ****************************************************************** */

	// ---------- write - styleINI -------------------------------------
	bool writeToStream_styleINI(std::ostream& o, MMParSet* par) {
		std::vector<MMParNodeBase*>ParsetQueue;
		if (par->getFullName() != "NN" || par->subsetLevel != 0)
			o << "\n[" << par->getFullName() << "]";
		for (unsigned int j = 0; j < par->nodes.size(); j++) {
			if (par->nodes[j]->isParset()) {
				// Output subsets after all parameters.
				ParsetQueue.push_back(par->nodes[j]);
			}
			else {
				if (par->nodes[j]->isComment()) {
					o << "\n# " << par->nodes[j]->name;
					continue;
				}
				else {
					o << "\n" << par->nodes[j]->name << " = ";
				}
				par->nodes[j]->valueIntoStream(o);
				if (par->nodes[j]->getOutputStyle() != styleINIV) {
					if (par->nodes[j]->units.size()) {
						o << "\n" << par->nodes[j]->name << ".units = ";
						MMParSetWriteToStream(o, &par->nodes[j]->units);
					}
					if (par->nodes[j]->description.size()) {
						o << "\n" << par->nodes[j]->name << ".description = ";
						MMParSetWriteToStream(o, &par->nodes[j]->description);
					}
					if (par->nodes[j]->longDescription.size()) {
						o << "\n" << par->nodes[j]->name << ".longDescription = ";
						MMParSetWriteToStream(o, &par->nodes[j]->longDescription);
					}
					if (par->nodes[j]->hasRange()) {
						o << "\n" << par->nodes[j]->name << ".range = ";
						par->nodes[j]->rangeIntoStream(o);
					}
				}
			}
		}
		o << std::endl;
		// now output subsets:
		for (unsigned int i = 0; i < ParsetQueue.size(); i++) {
			o << "\n";
			ParsetQueue[i]->valueIntoStream(o);
		}
		return o.good();
	}

	// ---------- read - helper functions ---------------------------------

	// Classify Name: 5=Parset, 0=value, 1=units, 2=description, 3=minvalue, 4=maxvalue, -1=ohgottogott
	int ClassifyName_IniFile(std::string& NameBase, std::string& s) {
		// first check if PARSET
		if (s.size() >= 3)
			if (s[0] == '[' && s[s.size() - 1] == ']') {
				NameBase = s.substr(1, s.size() - 2);
				return 5; // PARSET
			}
		// now check if par property:
		std::string::size_type tdot = s.rfind(".");
		if (tdot == std::string::npos) {
			NameBase = s;
			return 0;
		}
		// else:
		std::string sub = s.substr(tdot + 1, s.size() - (tdot + 1));
		NameBase = s.substr(0, tdot);
		if (sub == "units")
			return 1;
		if (sub == "description")
			return 2;
		if (sub == "minvalue")
			return 3; // deprecated
		if (sub == "maxvalue")
			return 4; // deprecated
		if (sub == "range")
			return 5;
		return -1; // nothing of sense
	}

	// ---------- read - styleINI --------------------------------------

	// the main read function. distributes to parset-read or value-read deending on name.
	bool readFromStream_styleINI(std::istream& in, MMParSet* par) {
		while (1) {
			long tpos = in.tellg();
			if (tpos == -1)
				break;
			SkipComments(in);
			std::string name;
			if (!GetNameInistyle(name, in)) {
				if (in.eof() || !in.good()) {
					return false; // just reached end of file.
				}
				else {
					std::string ts = "Input error";
					MMParSetError(ts, par->options.ErrorMessageType);
				}
			}
			std::string namewithoutbrackets;
			// IF we found a parset then special handling:
			if (NameIsParsetName(namewithoutbrackets, name)) { // PARSETS
				if (!readFromStream_parset_styleINI(in, name, par))
					continue;
				// but this will probably result in chaos - error is given in readFromStream_parset_styleINI
			}
			// it is not a parset but a parameter:
			else { // PARAMETERS
				SkipAssignInistyle(in);
				if (!readFromStream_value_styleINI(in, name, par)) {
					// dann war der par nicht im set. error wird schon von funktion ausgegeben.
					return false;
				}
			}
		}
		return true;
	}

	// reads parsets. I think recursively or something.
	bool readFromStream_parset_styleINI(std::istream& in, std::string name, MMParSet* par) {
		std::string namewithoutbrackets;
		NameIsParsetName(namewithoutbrackets, name); // remove brackets.

		if (par->name == namewithoutbrackets) { // i am myself hihi
			return readFromStream_SUB_styleINI(in, par);
		}
		// all other cases:

		// find the right parset - should be subset - can be several levels deep:
		MMParSet* someSubset = par->findParsetByFullNameRecursive(namewithoutbrackets);
		if (someSubset == NULL) {
			std::string ts = "Unknown Parset " + name +
				 " found in file. Processing of file continued but this might produce strange unexpected behaviour. !!!";
			MMParSetError(ts, par->options.ErrorMessageType);
			return false; // this should not happen.
		}
		// now someSubset should be the on e to read to:
		return readFromStream_SUB_styleINI(in, someSubset);
	}

	// reads values and roperties (everything except parsets).
	bool readFromStream_value_styleINI(std::istream& in, std::string name, MMParSet* par) {
		//
		// 1. check if it is a par or a minvalue, maxvalue, description, units etc ...
		// and make sure name contains only the base name.
		int NameClass = ClassifyName_IniFile(name, name);
		static std::string lastname;
		// find assosiated container
		// first: It might be myself:
		std::string debugsss = name;

		if (NameClass == -1)
			return false;
		// could not classify -  Parset ???? mimu todo : error is not caught in calling function yet!!!

		MMParNodeBase * tnode = par->findByName(name);

		if (!tnode) {
			std::string ts = "Name \"" + name + "\" is not defined in MMParSet \"" + name +
				 "\". Value not read. Hint: Last name read was: \"" + lastname + "\"";
			MMParSetError(ts, par->options.ErrorMessageType);
			return false; // cannot find container for name
		}
		// write to container, depending on NameClass:
		// mimu: tod: die naechsten muessten alle Fehler abfangen und zurückgeben !!!
		bool haveError = false;
		std::string errorMessage;
		switch (NameClass) {
		case 0:
			tnode->valueFromStream(in);
			break; // skipws;break;
		case 1:
			MMParSetReadFromStream(in, &tnode->units);
			break; // skipws
		case 2: { // in.setf(ios::skipws);
				MMParSetReadFromStream(in, &tnode->description);
				break;
			}
			// case 3 : tcont->MinvalueReadFromStream(in);break;
			// case 4 : tcont->MaxvalueReadFromStream(in);break;
		case 5:
			if (!tnode->rangeFromStream(in)) {
				haveError = true;
				errorMessage = "Could not read range from stream. trying to read name=\"" + name +
					 "\". las name read was\"" + lastname + "\".";
			}
			break;

			// all other cases error:
			std::string ts = "Could not read \"" + name +
				 "\" from file. Type could not be classified. Hint: Last name read was: \"" + lastname + "\"";
			MMParSetError(ts, par->options.ErrorMessageType);
			return false; // cannot find container for name
		};
		if (haveError) {
			MMParSetError(errorMessage, par->options.ErrorMessageType);
			return false;
		}
		lastname = name;
		return (true); // ok
	}

	// is called from the parset-read function. aeh. mimu todo: more struckture here - what does it do????
	bool readFromStream_SUB_styleINI(std::istream& in, MMParSet* par) {
		bool returnValue = true;
		while (1) {
			long tpos = in.tellg();
			if (tpos == -1)
				break;
			SkipComments(in);
			std::string name;
			if (!GetNameInistyle(name, in)) {
				if (in.eof() || !in.good()) {
					break; // just reached end of file.
				}
				else {
					std::string ts = "Input error";
					MMParSetError(ts, par->options.ErrorMessageType);
					returnValue = false;
					break;
				}
			}
			std::string namewithoutbrackets;
			if (NameIsParsetName(namewithoutbrackets, name)) {
				// beginning of new parset reached - we do not want to read this here.
				in.seekg(tpos);
				return true;
			}
			SkipAssignInistyle(in);
			if (!readFromStream_value_styleINI(in, name, par)) {
				// dann war der par nicht im set.
				// mimu: todo: error message
				std::string ts = "Name \"" + name + "\" is not defined in MMParSet \"" + name + "\". Value not read. ";
				MMParSetError(ts, par->options.ErrorMessageType);
				continue;
			}
		}
		return (returnValue);
	}

private:
	void stringReplaceAll(std::string & data, std::string searchString, std::string replaceString) {
		size_t pos = data.find(searchString);
		while (pos != std::string::npos) {
			data.replace(pos, searchString.size(), replaceString);
			pos = data.find(searchString, pos + searchString.size());
		}
	}

};

inline std::ostream& operator << (std::ostream& o, const MMParSet& ps) {
	MMParSetIO pario;
	pario.writeToStream(o, (MMParSet*)&ps);
	return o;
}

inline std::istream& operator >> (std::istream& in, MMParSet& ps) {
	MMParSetIO pario;
	pario.readFromStream(in, &ps);
	return in;
}

#endif

#ifdef compilieredenganzetrashmitwasaberechtdaemlichwaere
#error compilieredenganzetrashmitwasaberechtdaemlichwaere - are you sure ?

class sisiKeywords {
public:
	set<std::string>typeKeys;
	set<std::string>optionKeys;

	sisiKeywords() {
		typeKeys.insert("array");
		typeKeys.insert("int");
		typeKeys.insert("float");
		typeKeys.insert("string");
		typeKeys.insert("comment"); // ?????? nclude this here or not ????
		// these are used in formind, but there might be more ...
		optionKeys.insert("\\u");
		optionKeys.insert("\\r");
		optionKeys.insert("\\d");
		optionKeys.insert("typeOfArray");
		optionKeys.insert("dimension");
	}

	bool isTypeKey(std::string &s) {
		if (typeKeys.end() == typeKeys.find(s))
			return false;
		else
			return true;
	}

	bool isOptionKey(std::string &s) {
		if (optionKeys.end() == optionKeys.find(s))
			return false;
		else
			return true;
	}

	bool isKey(std::string &s) {
		if (isTypeKey(s))
			return true;
		else if (isOptionKey(s))
			return true;
		else
			return false;
	}
	/*
	 int isKey(istream & in) {
	 SkipComments(in);
	 long tpos = in.tellg();
	 if (tpos == -1)
	 return -1;
	 int ti=in.peek();
	 if(ti==-1) return -1;
	 if(ti=="\\") {
	 in.get();
	 int ti2=in.get();
	 unsigned char tc=ti2;
	 string ts=string("\\") + string(tc);

	 }
	 } */

};
#endif
