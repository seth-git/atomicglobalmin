
#ifndef __SEED_H_
#define __SEED_H_

#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdElementUtil.h"
#include "xsd/xsdTypeUtil.h"
#include "translation/strings.h"
#include "externalEnergy/externalEnergy.h"
#include "structure.h"
#include "constraints.h"

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <errno.h>
#include <list>

enum SeedSource { RESULTS, POPULATION };

class Seed {
public:
	unsigned int m_iFreezingIterations;
	
	unsigned int m_iAgmlFiles;
	std::string* m_agmlFilePaths; // An array of size m_iFiles (the others below are also arrays of the same size)
	bool* m_bUseAllFromAgmlFiles; // if true, use all structures from the agmlFile
	unsigned int* m_numberFromAgmlFiles; // Number of structures to use from each agml file
	SeedSource* m_sourceInAgmlFiles;
	
	unsigned int m_iDirectories;
	std::string* m_dirPaths; // An array of size m_iDirectories (same below)
	bool* m_bUseAllFromDir; // if true, use all files in the directory
	unsigned int* m_numberFromDir; // An array of size m_iDirectories
	ExternalEnergy::Impl* m_dirFileTypes; // An array of size m_iFiles
	
	unsigned int m_iEnergyFiles;
	std::string* m_energyFilePaths; // An array of size m_iEnergyFiles
	ExternalEnergy::Impl* m_energyFileTypes; // An array of size m_iEnergyFiles
	
	Seed();
	~Seed();
	bool load(const rapidxml::xml_node<>* pSeedElem);
	bool save(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pParentElem);

	/**************************************************************************
	 * Purpose: Call this function to read the seeded structures.
	 * Parameters: structures - destination structures
	 *************************************************************************/
	bool readStructures(std::list<Structure*> &structures);

private:
	void clear();
	
	static const bool s_attRequired[];
	
	static const unsigned int s_minOccurs[];
	static const unsigned int s_maxOccurs[];
	
	static const bool s_fileAttRequired[];
	
	static const bool s_dirAttRequired[];
	
	static const bool s_enFileAttRequired[];
};

#endif

