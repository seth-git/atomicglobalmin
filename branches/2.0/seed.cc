
#include "seed.h"
#include "input.h"

Seed::Seed()
{
	m_iFreezingIterations = 0;
	
	m_iAgmlFiles = 0;
	m_agmlFilePaths = NULL;
	m_bUseAllFromAgmlFiles = NULL;
	m_numberFromAgmlFiles = NULL;
	m_sourceInAgmlFiles = NULL;
	
	m_iDirectories = 0;
	m_dirPaths = NULL;
	m_bUseAllFromDir = NULL;
	m_numberFromDir = NULL;
	m_dirFileTypes = NULL;
	
	m_iEnergyFiles = 0;
	m_energyFilePaths = NULL;
	m_energyFileTypes = NULL;
}

Seed::~Seed()
{
	cleanUp();
}

void Seed::cleanUp()
{
	m_iFreezingIterations = 0;
	
	m_iAgmlFiles = 0;
	if (m_agmlFilePaths != NULL) {
		delete[] m_agmlFilePaths;
		m_agmlFilePaths = NULL;
	}
	if (m_bUseAllFromAgmlFiles != NULL) {
		delete[] m_bUseAllFromAgmlFiles;
		m_bUseAllFromAgmlFiles = NULL;
	}
	if (m_numberFromAgmlFiles != NULL) {
		delete[] m_numberFromAgmlFiles;
		m_numberFromAgmlFiles = NULL;
	}
	if (m_sourceInAgmlFiles != NULL) {
		delete[] m_sourceInAgmlFiles;
		m_sourceInAgmlFiles = NULL;
	}
	
	m_iDirectories = 0;
	if (m_dirPaths != NULL) {
		delete[] m_dirPaths;
		m_dirPaths = NULL;
	}
	if (m_bUseAllFromDir != NULL) {
		delete[] m_bUseAllFromDir;
		m_bUseAllFromDir = NULL;
	}
	if (m_numberFromDir != NULL) {
		delete[] m_numberFromDir;
		m_numberFromDir = NULL;
	}
	if (m_dirFileTypes != NULL) {
		delete[] m_dirFileTypes;
		m_dirFileTypes = NULL;
	}
	
	m_iEnergyFiles = 0;
	if (m_energyFilePaths != NULL) {
		delete[] m_energyFilePaths;
		m_energyFilePaths = NULL;
	}
	if (m_energyFileTypes != NULL) {
		delete[] m_energyFileTypes;
		m_energyFileTypes = NULL;
	}
	
}

//const char*      Seed::s_attNames[]     = {"freezingIterations"};
const bool         Seed::s_attRequired[]  = {false};
//const char*      Seed::s_attDefaults[]  = {"0"};

//const char*      Seed::s_elementNames[] = {"agmlFile"   , "directory"  , "energyFile"};
const unsigned int Seed::s_minOccurs[]    = {0            , 0            , 0           };
const unsigned int Seed::s_maxOccurs[]    = {XSD_UNLIMITED, XSD_UNLIMITED, XSD_UNLIMITED};

//const char*      Seed::s_fileAttNames[]     = {"path", "number", "source"};
const bool         Seed::s_fileAttRequired[]  = {true  , true    , true};
//const char*      Seed::s_fileAttDefaults[]  = {""    , "all"   , "results"};

//const char*      Seed::s_dirAttNames[]     = {"path", "number", "type"};
const bool         Seed::s_dirAttRequired[]  = {true  , true    , true};
//const char*      Seed::s_dirAttDefaults[]  = {""    , "all"   , ""};

//const char*      Seed::s_enFileAttNames[]     = {"path", "type"};
const bool         Seed::s_enFileAttRequired[]  = {true  , true};
//const char*      Seed::s_enFileAttDefaults[]  = {""    , ""};

bool Seed::load(TiXmlElement *pSeedElem, const Strings* messages)
{
	cleanUp();
	
	const char** values;
	const char* attNames[]     = {messages->m_sxFreezingIterations.c_str()};
	const char* attDefaults[]  = {"0"};
	XsdAttributeUtil attUtil(pSeedElem->Value(), attNames, s_attRequired, attDefaults);
	if (!attUtil.process(pSeedElem))
		return false;
	values = attUtil.getAllAttributes();
	if (!XsdTypeUtil::getNonNegativeInt(values[0], m_iFreezingIterations, attNames[0], pSeedElem))
		return false;
	
	const char* elementNames[] = {messages->m_sxAgmlFile.c_str(), messages->m_sxDirectory.c_str(), messages->m_sxEnergyFile.c_str()};
	XsdElementUtil elemUtil(pSeedElem->Value(), XSD_SEQUENCE, elementNames, s_minOccurs, s_maxOccurs);
	TiXmlHandle handle(0);
	handle=TiXmlHandle(pSeedElem);
	if (!elemUtil.process(handle))
		return false;
	std::vector<TiXmlElement*>* elements = elemUtil.getSequenceElements();
	unsigned int i;
	
	if (elements[0].size() > 0) {
		m_iAgmlFiles = elements[0].size();
		m_agmlFilePaths = new std::string[m_iAgmlFiles];
		m_bUseAllFromAgmlFiles = new bool[m_iAgmlFiles];
		m_numberFromAgmlFiles = new unsigned int[m_iAgmlFiles];
		m_sourceInAgmlFiles = new SeedSource[m_iAgmlFiles];
		
		const char* fileAttNames[]     = {messages->m_sxPath.c_str(), messages->m_sxNumber.c_str(), messages->m_sxSource.c_str()};
		const char* fileAttDefaults[]  = {NULL                      , messages->m_spAll.c_str()   , messages->m_sxResults.c_str()};
		
		const char* sources[] = {messages->m_sxResults.c_str(), messages->m_sxPopulation.c_str()};
		
		for (i = 0; i < m_iAgmlFiles; ++i) {
			XsdAttributeUtil fileAttUtil(elements[0][i]->Value(), fileAttNames, s_fileAttRequired, fileAttDefaults);
			if (!fileAttUtil.process(elements[0][i]))
				return false;
			values = fileAttUtil.getAllAttributes();
			
			if (!XsdTypeUtil::checkDirectoryOrFileName(values[0], m_agmlFilePaths[i], fileAttNames[0], elements[0][i]))
				return false;
			
			m_bUseAllFromAgmlFiles[i] = (messages->m_spAll.compare(values[1]) == 0); // are the two strings equal
			
			if (!m_bUseAllFromAgmlFiles[i])
				if (!XsdTypeUtil::getPositiveInt(values[1], m_numberFromAgmlFiles[i], fileAttNames[1], elements[0][i]))
					return false;
			
			if (!XsdTypeUtil::getEnumValue(fileAttNames[2], values[2], m_sourceInAgmlFiles[i], elements[0][i], sources))
				return false;
		}
	}
	
	if (elements[1].size() > 0) {
		m_iDirectories = elements[1].size();
		m_dirPaths = new std::string[m_iDirectories];
		m_bUseAllFromDir = new bool[m_iDirectories];
		m_numberFromDir = new unsigned int[m_iDirectories];
		m_dirFileTypes = new ExternalEnergy::Impl[m_iDirectories];
		
		const char* dirAttNames[]     = {messages->m_sxPath.c_str(), messages->m_sxNumber.c_str(), messages->m_sxType.c_str()};
		const char* dirAttDefaults[]  = {NULL                      , messages->m_spAll.c_str()   , NULL};
		
		for (i = 0; i < m_iDirectories; ++i) {
			XsdAttributeUtil dirAttUtil(elements[1][i]->Value(), dirAttNames, s_dirAttRequired, dirAttDefaults);
			if (!dirAttUtil.process(elements[1][i]))
				return false;
			values = dirAttUtil.getAllAttributes();
			
			if (!XsdTypeUtil::checkDirectoryOrFileName(values[0], m_dirPaths[i], dirAttNames[0], elements[1][i]))
				return false;
			
			m_bUseAllFromDir[i] = (messages->m_spAll.compare(values[1]) == 0); // are the two strings equal
			
			if (!m_bUseAllFromDir[i])
				if (!XsdTypeUtil::getPositiveInt(values[1], m_numberFromDir[i], dirAttNames[1], elements[1][i]))
					return false;
			
			if (!ExternalEnergy::getEnum(dirAttNames[2], values[2], m_dirFileTypes[i], elements[1][i], messages))
				return false;
		}
	}
	
	if (elements[2].size() > 0) {
		m_iEnergyFiles = elements[2].size();
		m_energyFilePaths = new std::string[m_iEnergyFiles];
		m_energyFileTypes = new ExternalEnergy::Impl[m_iEnergyFiles];
		
		const char* enFileAttNames[]     = {messages->m_sxPath.c_str(), messages->m_sxType.c_str()};
		const char* enFileAttDefaults[]  = {NULL                      , NULL};
		
		for (i = 0; i < m_iEnergyFiles; ++i) {
			XsdAttributeUtil enFileAttUtil(elements[2][i]->Value(), enFileAttNames, s_enFileAttRequired, enFileAttDefaults);
			if (!enFileAttUtil.process(elements[2][i]))
				return false;
			values = enFileAttUtil.getAllAttributes();
			
			if (!XsdTypeUtil::checkDirectoryOrFileName(values[0], m_energyFilePaths[i], enFileAttNames[0], elements[2][i]))
				return false;
			
			if (!ExternalEnergy::getEnum(enFileAttNames[1], values[1], m_energyFileTypes[i], elements[2][i], messages))
				return false;
		}
	}
	
	if (m_iAgmlFiles == 0 && m_iDirectories == 0 && m_iEnergyFiles == 0) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sMissingChildElements3.c_str(), pSeedElem->Row(),
				pSeedElem->Value(), messages->m_sxAgmlFile.c_str(),
				messages->m_sxDirectory.c_str(),
				messages->m_sxEnergyFile.c_str());
		return false;
	}
	
	return true;
}

bool Seed::save(TiXmlElement *pParentElem, const Strings* messages)
{
	unsigned int i;
	TiXmlElement* pSeed = new TiXmlElement(messages->m_sxSeed.c_str());
	pParentElem->LinkEndChild(pSeed);
	
	if (m_iFreezingIterations != 0)
		pSeed->SetAttribute(messages->m_sxFreezingIterations.c_str(), m_iFreezingIterations);
	
	if (m_iAgmlFiles > 0) {
		const char* sources[] = {messages->m_sxResults.c_str(), messages->m_sxPopulation.c_str()};
		
		for (i = 0; i < m_iAgmlFiles; ++i) {
			TiXmlElement* pAgmlFile = new TiXmlElement(messages->m_sxAgmlFile.c_str());
			pSeed->LinkEndChild(pAgmlFile);
			
			pAgmlFile->SetAttribute(messages->m_sxPath.c_str(), m_agmlFilePaths[i].c_str());
			if (!m_bUseAllFromAgmlFiles[i])
				pAgmlFile->SetAttribute(messages->m_sxNumber.c_str(), m_numberFromAgmlFiles[i]);
			if (m_sourceInAgmlFiles[i] != RESULTS)
				pAgmlFile->SetAttribute(messages->m_sxSource.c_str(), sources[m_sourceInAgmlFiles[i]]);
		}
	}
	
	for (i = 0; i < m_iDirectories; ++i) {
		TiXmlElement* pDir = new TiXmlElement(messages->m_sxDirectory.c_str());
		pSeed->LinkEndChild(pDir);
		
		pDir->SetAttribute(messages->m_sxPath.c_str(), m_dirPaths[i].c_str());
		if (!m_bUseAllFromDir[i])
			pDir->SetAttribute(messages->m_sxNumber.c_str(), m_numberFromDir[i]);
		pDir->SetAttribute(messages->m_sxType.c_str(), ExternalEnergy::getEnumString(m_dirFileTypes[i], messages));
	}
	
	for (i = 0; i < m_iEnergyFiles; ++i) {
		TiXmlElement* pEnergyFile = new TiXmlElement(messages->m_sxEnergyFile.c_str());
		pSeed->LinkEndChild(pEnergyFile);
		
		pEnergyFile->SetAttribute(messages->m_sxPath.c_str(), m_energyFilePaths[i].c_str());
		pEnergyFile->SetAttribute(messages->m_sxType.c_str(), ExternalEnergy::getEnumString(m_energyFileTypes[i], messages));
	}
	
	return true;
}

bool Seed::readStructures(std::vector<Structure*> &structures) {
	unsigned int i, count;
	Structure* pStructure;

	DIR *dp = NULL;
	struct dirent *dirp;
	const char* outputExt;
	unsigned int outputExtLen;
	bool exception = false;
	std::string fullPathFileName;

	Input input;
	for (i = 0; i < m_iAgmlFiles; ++i) {
		if (!input.load(m_agmlFilePaths[i].c_str()))
			return false;
		if (m_sourceInAgmlFiles[i] == RESULTS) {
			printf("Unable to read seeded structures from results of %s in Seed::readStructures.\n", m_agmlFilePaths[i].c_str());
			return false;
		} else { // Population
			Action* action = input.m_pAction;
			Structure* agmlStructures = action->m_structures;
			count = 0;
			unsigned int numFromFile;
			if (m_bUseAllFromAgmlFiles[i])
				numFromFile = action->m_iStructures;
			else if (m_numberFromAgmlFiles[i] <= action->m_iStructures)
				numFromFile = m_numberFromAgmlFiles[i];
			else {
				printf("Unable to read %u structures from %s.", m_numberFromAgmlFiles[i], m_agmlFilePaths[i].c_str());
				return false;
			}

			for (count = 0; count < numFromFile; ++count) {
				pStructure = new Structure();
				pStructure->copy(agmlStructures[count]);
				structures.push_back(pStructure);
			}
		}
	}

	for (i = 0; i < m_iDirectories; ++i) {
		try {
			if ((dp = opendir(m_dirPaths[i].c_str())) == NULL) {
				printf("Error(%1$d) opening directory: %2$s.\n", errno, m_dirPaths[i].c_str());
				throw "";
			}

			outputExt = ExternalEnergy::getOutputFileExtension(m_dirFileTypes[i]);
			if (outputExt == NULL)
				throw "";
			outputExtLen = strlen(outputExt);

			count = 0;
			while ((dirp = readdir(dp)) != NULL) {
				if (strncmp(outputExt, dirp->d_name + strlen(dirp->d_name) - outputExtLen, outputExtLen) != 0)
					continue;

				fullPathFileName = m_dirPaths[i] + "/" + dirp->d_name;

				pStructure = new Structure();
				printf("Reading file: %1$s\n", fullPathFileName.c_str());
				if (ExternalEnergy::readOutputFile(m_dirFileTypes[i], fullPathFileName.c_str(), *pStructure, true)) {
					structures.push_back(pStructure);
				} else {
					delete pStructure;
					throw "";
				}
				++count;
				if (!m_bUseAllFromDir[i] && count >= m_numberFromDir[i])
					break;
			}
		} catch (const char* msg) {
			if (PRINT_CATCH_MESSAGES && strlen(msg) > 0)
				printf("Caught message in Seed.readStructures: %s", msg);
			exception = true;
		}
		closedir(dp);
		if (exception)
			return false;
	}

	for (i = 0; i < m_iEnergyFiles; ++i) {
		printf("Reading file: %1$s\n", m_energyFilePaths[i].c_str());

		pStructure = new Structure();
		if (ExternalEnergy::readOutputFile(m_energyFileTypes[i], m_energyFilePaths[i].c_str(), *pStructure, true)) {
			structures.push_back(pStructure);
		} else {
			delete pStructure;
			return false;
		}
	}

	if (structures.size() == 0) {
		printf("Error: zero seeded structures were read.\n");
		return false;
	}

	return true;
}
