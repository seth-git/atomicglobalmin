////////////////////////////////////////////////////////////////////////////////
// Purpose: This file manages an atom.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "argumentParser.h"

vector<ArgumentParser*> ArgumentParser::s_argumentList;
vector<ArgumentParser*> ArgumentParser::s_unRecognizedArgumentList;
int ArgumentParser::s_argumentListIndex = -1;
int ArgumentParser::s_argumentListUnrecognizedIndex = -1;
vector<const char*> *ArgumentParser::s_recognizedOptions;
vector<int> *ArgumentParser::s_argumentsPerOption;
vector<const char*> *ArgumentParser::s_optionMessages;
int ArgumentParser::s_argc = 0;
char** ArgumentParser::s_argv = NULL;

ArgumentParser::ArgumentParser ()
{
	m_option = NULL;
	m_numArguments = 0;
	m_arguments = NULL;
}

ArgumentParser::~ArgumentParser (void)
{
	if (m_arguments != NULL)
		delete[] m_arguments;
}

void ArgumentParser::init(vector<const char *> *recognizedOptions, vector<int> *argumentsPerOption, vector<const char*> *optionMessages) {
	s_recognizedOptions = recognizedOptions;
	s_argumentsPerOption = argumentsPerOption;
	s_optionMessages = optionMessages;
}

bool ArgumentParser::parse(int argc, char *argv[])
{
	ArgumentParser* optionWithArgs;
	const char* message;
	int i, j;
	bool recognized;
	i = 1;

	while (i < argc) {
		optionWithArgs = new ArgumentParser();
		optionWithArgs->m_option = argv[i];
		optionWithArgs->m_numArguments = 0;
		recognized = false;
		for (j = 0; j < (int)s_recognizedOptions->size(); ++j)
			if (strcmp(optionWithArgs->m_option,(*s_recognizedOptions)[j]) == 0) {
				recognized = true;
				optionWithArgs->m_numArguments = (*s_argumentsPerOption)[j];
				message = (*s_optionMessages)[j];
				break;
			}
		if (optionWithArgs->m_numArguments == -1)
			optionWithArgs->m_numArguments = argc-1-i;
		if (optionWithArgs->m_numArguments > 0) {
			if (argc-1-i < optionWithArgs->m_numArguments) {
				if ((message == NULL)  || (strlen(message) == 0))
					cout << "The " << optionWithArgs->m_option << " option takes " << optionWithArgs->m_numArguments << " argument(s)." << endl;
				else
					cout << message << endl;
				delete optionWithArgs;
				cleanUp();
				return false;
			}
			optionWithArgs->m_arguments = new char* [optionWithArgs->m_numArguments];
			for (j = 0; j < (int)optionWithArgs->m_numArguments; ++j) {
				++i;
				optionWithArgs->m_arguments[j] = argv[i];
			}
		}
		s_argumentList.push_back(optionWithArgs);
		if (!recognized)
			s_unRecognizedArgumentList.push_back(optionWithArgs);
		
		++i;
	}
	s_argumentListIndex = 0;
	s_argumentListUnrecognizedIndex = 0;
	s_argc = argc;
	s_argv = argv;
	return true;
}

void ArgumentParser::cleanUp(void)
{
	for (int i = 0; i < (int)s_argumentList.size(); ++i) {
		delete s_argumentList[i];
	}
	s_argumentList.clear();
	s_unRecognizedArgumentList.clear();
}

const char* ArgumentParser::next(void)
{
	if (s_argumentListIndex == -1)
		return NULL;
	if (s_argumentListIndex >= (int)s_argumentList.size()) {
		s_argumentListIndex = 0;
		return NULL;
	}
	return s_argumentList[s_argumentListIndex++]->m_option;
}

const char* ArgumentParser::nextUnrecognized(void)
{
	if (s_argumentListUnrecognizedIndex == -1)
		return NULL;
	if (s_argumentListUnrecognizedIndex >= (int)s_unRecognizedArgumentList.size()) {
		s_argumentListUnrecognizedIndex = 0;
		return NULL;
	}
	return s_unRecognizedArgumentList[s_argumentListUnrecognizedIndex++]->m_option;
}

void ArgumentParser::getOptionArguments(const char *option, int &numArguments, char*** arguments)
{
	for (int i = 0; i < (int)s_argumentList.size(); ++i)
		if (strcmp(option, s_argumentList[i]->m_option) == 0) {
			numArguments = s_argumentList[i]->m_numArguments;
			*arguments = s_argumentList[i]->m_arguments;
			return;
		}
	numArguments = 0;
	*arguments = NULL;
}

bool ArgumentParser::optionPresent(const char* option)
{
	for (int i = 0; i < (int)s_argumentList.size(); ++i)
		if (strcmp(option, s_argumentList[i]->m_option) == 0)
			return true;
	return false;
}

