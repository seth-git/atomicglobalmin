
#ifndef __SEED_H_
#define __SEED_H_

#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdElementUtil.h"
#include "xsd/xsdTypeUtil.h"
#include "translation/strings.h"
#include "externalEnergy.h"

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
	unsigned int* m_numberFromDir; // An array of size m_iFiles
	ExternalEnergy::Method* m_dirFileTypes; // An array of size m_iFiles
	
	unsigned int m_iEnergyFiles;
	std::string* m_energyFilePaths; // An array of size m_iEnergyFiles
	ExternalEnergy::Method* m_energyFileTypes; // An array of size m_iEnergyFiles
	
	Seed();
	~Seed();
	bool load(TiXmlElement *pSeedElem, const Strings* messages);
	bool save(TiXmlElement *pParentElem, const Strings* messages);
	
private:
	void cleanUp();
	
	static const bool s_attRequired[];
	
	static const unsigned int s_minOccurs[];
	static const unsigned int s_maxOccurs[];
	
	static const bool s_fileAttRequired[];
	
	static const bool s_dirAttRequired[];
	
	static const bool s_enFileAttRequired[];
};

#endif

