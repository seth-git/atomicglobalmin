////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code that parses command line arguments.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARGUMENT_PARSER_H__
#define __ARGUMENT_PARSER_H__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

using namespace std;

class ArgumentParser
{
public:
	/*******************************************************************************
	 * Purpose: This function is used to tell the parser, what options to look for
	 *    and how many argument should go with each option.
	 * Parameters: recognizedOptions - a vector of options
	 *             argumentsPerOption - the number of arguments that go with each
	 *             option. Put -1 to specify as many as the user types after the option.
	 * Returns: nothing
	 ******************************************************************************/
	static void init(vector<const char *> *recognizedOptions, vector<int> *argumentsPerOption, vector<const char*> *optionMessages);
	static bool parse(int argc, char *argv[]);
	static const char* next(void);
	static const char* nextUnrecognized(void);
	static bool optionPresent(const char* option);
	static int getNumOptions(void) { return (int)s_argumentList.size(); }
	static void getOptionArguments(const char *option, int &numArguments, char*** arguments);

	ArgumentParser(void);
	~ArgumentParser(void);

	static void cleanUp(void);
	static int s_argc;
	static char** s_argv;

private:
	static vector<ArgumentParser*> s_argumentList;
	static int s_argumentListIndex;
	static vector<ArgumentParser*> s_unRecognizedArgumentList;
	static int s_argumentListUnrecognizedIndex;
	static vector<const char*> *s_recognizedOptions;
	static vector<int> *s_argumentsPerOption;
	static vector<const char*> *s_optionMessages;
	const char* m_option;
	int m_numArguments;
	char** m_arguments;
};

#endif
