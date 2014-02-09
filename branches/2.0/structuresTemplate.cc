
#include "structuresTemplate.h"
#include "random/randomc.h"

unsigned int StructuresTemplate::defaultMaxInitializationTries = 50;
unsigned int StructuresTemplate::defaultMaxGroupInitializationTries = 50;

//const char* StructuresTemplate::s_attributeNames[] = {"constraints"};
const bool    StructuresTemplate::s_attRequired[]    = {false};
const char*   StructuresTemplate::s_attDefaults[]    = {NULL};

//const char*      StructuresTemplate::s_elementNames[] = {"structureTemplate", "linear", "planar", "threeD", "bondRotationalSearch", "seed"};
const unsigned int StructuresTemplate::s_minOccurs[]    = {0                  , 0       , 0       , 0       , 0                     , 0     };

//const char*      StructuresTemplate::s_popTempElementNames[] = {"moleculeTemplate", "atomTemplate"};
const unsigned int StructuresTemplate::s_popTempMinOccurs[]    = {0                 , 0             };
const unsigned int StructuresTemplate::s_popTempMaxOccurs[]    = {XSD_UNLIMITED     , XSD_UNLIMITED };

StructuresTemplate::StructuresTemplate()
{
	m_iAtomGroupTemplates = 0;
	m_atomGroupTemplates = NULL;
	
	m_pConstraints = NULL;
	m_iLinear = 0;
	m_pLinearConstraints = NULL;
	m_iPlanar = 0;
	m_pPlanarConstraints = NULL;
	m_i3D = 0;
	m_p3DConstraints = NULL;
	
	m_bondRotationalSearchAngle = NULL;
	
	m_pSeed = NULL;
}

StructuresTemplate::~StructuresTemplate()
{
	clear();
}

void StructuresTemplate::clear()
{
	m_iAtomGroupTemplates = 0;
	if (m_atomGroupTemplates != NULL) {
		delete[] m_atomGroupTemplates;
		m_atomGroupTemplates = NULL;
	}
	
	m_pConstraints = NULL;
	m_iLinear = 0;
	m_pLinearConstraints = NULL;
	m_iPlanar = 0;
	m_pPlanarConstraints = NULL;
	m_i3D = 0;
	m_p3DConstraints = NULL;
	
	if (m_bondRotationalSearchAngle != NULL) {
		delete m_bondRotationalSearchAngle;
		m_bondRotationalSearchAngle = NULL;
	}
	
	if (m_pSeed != NULL) {
		delete m_pSeed;
		m_pSeed = NULL;
	}
}

bool StructuresTemplate::load(TiXmlElement *pStructuresTemplateElem,
		std::map<std::string, Constraints*> &constraintsMap, const Strings* messages) {
	clear();
	
	unsigned int i;
	const char** values;
	const char* attributeNames[]  = {messages->m_sxConstraints.c_str()};
	XsdAttributeUtil attUtil(pStructuresTemplateElem->Value(), attributeNames, s_attRequired, s_attDefaults);
	if (!attUtil.process(pStructuresTemplateElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();
	if (values[0] != NULL) {
		m_pConstraints = constraintsMap[values[0]];
		if (m_pConstraints == NULL) {
			const Strings* messagesDL = Strings::instance();
			printf(messagesDL->m_sConstraintNameMisMatch.c_str(), pStructuresTemplateElem->Row(), messages->m_sxConstraints.c_str(), values[0]);
			return false;
		}
	}
	
	const char* elementNames[] = {messages->m_sxStructureTemplate.c_str(), messages->m_sxLinear.c_str(), messages->m_sxPlanar.c_str(), messages->m_sxThreeD.c_str(), messages->m_sxBondRotationalSearch.c_str(), messages->m_sxSeed.c_str()};
	XsdElementUtil ptUtil(pStructuresTemplateElem->Value(), XSD_ALL, elementNames, s_minOccurs);
	TiXmlHandle hRoot(0);
	hRoot=TiXmlHandle(pStructuresTemplateElem);
	if (!ptUtil.process(hRoot))
		return false;
	TiXmlElement** ptElements = ptUtil.getAllElements();
	
	if (ptElements[0] != NULL) {
		const char* popTempElementNames[] = {messages->m_sxMoleculeTemplate.c_str(), messages->m_sxAtomTemplate.c_str()};
		XsdElementUtil mtUtil(ptElements[0]->Value(), XSD_SEQUENCE, popTempElementNames, s_popTempMinOccurs, s_popTempMaxOccurs);
		hRoot=TiXmlHandle(ptElements[0]);
		if (!mtUtil.process(hRoot))
			return false;
		std::vector<TiXmlElement*>* templates = mtUtil.getSequenceElements();
		m_iAtomGroupTemplates = templates[0].size() + templates[1].size();
		if (m_iAtomGroupTemplates == 0) {
			const Strings* messagesDL = Strings::instance();
			printf(messagesDL->m_sEmptyStructureTemplate.c_str(), ptElements[0]->Value(), ptElements[0]->Row(), messages->m_sxMoleculeTemplate.c_str(), messages->m_sxAtomTemplate.c_str());
			return false;
		}
		m_atomGroupTemplates = new AtomGroupTemplate[m_iAtomGroupTemplates];
		for (i = 0; i < templates[0].size(); ++i)
			if (!m_atomGroupTemplates[i].loadMolecule(templates[0][i], messages))
				return false;
		for (i = 0; i < templates[1].size(); ++i)
			if (!m_atomGroupTemplates[i+templates[0].size()].loadAtom(templates[1][i], messages))
				return false;
	}
	
	if (ptElements[1] != NULL)
		if (!readInitializationType(ptElements[1], constraintsMap, m_iLinear, m_pLinearConstraints, messages))
			return false;
	
	if (ptElements[2] != NULL)
		if (!readInitializationType(ptElements[2], constraintsMap, m_iPlanar, m_pPlanarConstraints, messages))
			return false;
	
	if (ptElements[3] != NULL)
		if (!readInitializationType(ptElements[3], constraintsMap, m_i3D, m_p3DConstraints, messages))
			return false;
	
	if (ptElements[4] != NULL) {
		m_bondRotationalSearchAngle = new FLOAT;
		if (!XsdTypeUtil::read1PosFloatAtt(ptElements[4], *m_bondRotationalSearchAngle, messages->m_sxDegrees.c_str(), true, NULL))
			return false;
		if (!XsdTypeUtil::inRange(*m_bondRotationalSearchAngle, 0, 360, ptElements[4], messages->m_sxDegrees.c_str()))
			return false;
		*m_bondRotationalSearchAngle *= DEG_TO_RAD;
	}
	
	if (ptElements[5] != NULL) {
		m_pSeed = new Seed();
		if (!m_pSeed->load(ptElements[5], messages))
			return false;
	}

	if (m_atomGroupTemplates == NULL && m_pSeed == NULL) {
		printf("The %1$s element must contain either a %2$s element or a %3$s element.", messages->m_sxStructuresTemplate.c_str(), messages->m_sxStructureTemplate.c_str(), messages->m_sxSeed.c_str());
		return false;
	}

	return true;
}

//const char* StructuresTemplate::s_initTypeAttNames[]    = {"number", "constraints"};
const bool    StructuresTemplate::s_initTypeAttRequired[] = {true    , false};
const char*   StructuresTemplate::s_initTypeAttDefaults[] = {NULL    , NULL};

bool StructuresTemplate::readInitializationType(TiXmlElement *pElem, std::map<std::string,Constraints*> &constraintsMap, unsigned int &numberOfThisType, Constraints* &pConstraints, const Strings* messages) {
	const char** values;
	const char* attributeNames[] = {messages->m_sxNumber.c_str(), messages->m_sxConstraints.c_str()};
	XsdAttributeUtil attUtil(pElem->Value(), attributeNames, s_initTypeAttRequired, s_initTypeAttDefaults);
	if (!attUtil.process(pElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();
	
	if (!XsdTypeUtil::getPositiveInt(values[0], numberOfThisType, attributeNames[0], pElem))
		return false;
	
	if (values[1] != NULL) {
		pConstraints = constraintsMap[values[1]];
		if (pConstraints == NULL) {
			const Strings* messagesDL = Strings::instance();
			printf(messagesDL->m_sConstraintNameMisMatch.c_str(), pElem->Row(), messages->m_sxConstraints.c_str(), values[1]);
			return false;
		}
	}
	return true;
}

bool StructuresTemplate::save(TiXmlElement *pParentElem, const Strings* messages)
{
	unsigned int i;
	TiXmlElement* structuresTemplate = new TiXmlElement(messages->m_sxStructuresTemplate.c_str());
	pParentElem->LinkEndChild(structuresTemplate);
	
	if (m_pConstraints != NULL)
		structuresTemplate->SetAttribute(messages->m_sxConstraints.c_str(), m_pConstraints->m_sName.c_str());
	
	if (m_iAtomGroupTemplates > 0) {
		TiXmlElement* structureTemplate = new TiXmlElement(messages->m_sxStructureTemplate.c_str());
		structuresTemplate->LinkEndChild(structureTemplate);
		
		for (i = 0; i < m_iAtomGroupTemplates; ++i)
			if (!m_atomGroupTemplates[i].save(structureTemplate, messages))
				return false;
	}
	
	if (m_iLinear > 0) {
		TiXmlElement* linear = new TiXmlElement(messages->m_sxLinear.c_str());
		structuresTemplate->LinkEndChild(linear);
		linear->SetAttribute(messages->m_sxNumber.c_str(), m_iLinear);
		if (m_pLinearConstraints != NULL)
			linear->SetAttribute(messages->m_sxConstraints.c_str(), m_pLinearConstraints->m_sName.c_str());
	}
	
	if (m_iPlanar > 0) {
		TiXmlElement* linear = new TiXmlElement(messages->m_sxPlanar.c_str());
		structuresTemplate->LinkEndChild(linear);
		linear->SetAttribute(messages->m_sxNumber.c_str(), m_iPlanar);
		if (m_pPlanarConstraints != NULL)
			linear->SetAttribute(messages->m_sxConstraints.c_str(), m_pPlanarConstraints->m_sName.c_str());
	}
	
	if (m_i3D > 0) {
		TiXmlElement* linear = new TiXmlElement(messages->m_sxThreeD.c_str());
		structuresTemplate->LinkEndChild(linear);
		linear->SetAttribute(messages->m_sxNumber.c_str(), m_i3D);
		if (m_p3DConstraints != NULL)
			linear->SetAttribute(messages->m_sxConstraints.c_str(), m_p3DConstraints->m_sName.c_str());
	}
	
	if (m_bondRotationalSearchAngle != NULL) {
		TiXmlElement* bondRotationalSearch = new TiXmlElement(messages->m_sxBondRotationalSearch.c_str());
		structuresTemplate->LinkEndChild(bondRotationalSearch);
		bondRotationalSearch->SetDoubleAttribute(messages->m_sxDegrees.c_str(), *m_bondRotationalSearchAngle * RAD_TO_DEG);
	}
	
	if (m_pSeed != NULL)
		if (!m_pSeed->save(structuresTemplate, messages))
			return false;
	
	return true;
}

/*****************************************************************************
 * Purpose: This method examines differences between the template and the
 *    structure.  A "difference" means two atom groups have differing
 *    numbers of atoms or different atomic numbers.
 * Parameters:
 *    structure - the structure
 *    firstDiffTemplateIndex - If a difference is found, this will be the
 *       index in the m_atomGroupTemplates of the first different atom group.
 *    firstDiffAtomGroupIndex - If a difference is found, this will be the
 *       atom group index in the structure of the first difference.
 *    firstDiffMissing - If true, firstDiffTemplateIndex will be an index
 *       in the template of the first atom group that's missing in the
 *       structure; firstDiffAtomGroupIndex will be where to insert the
 *       atom group.  If false, firstDiffAtomGroupIndex will be an index to
 *       the extra atom group that should be removed from the structure.
 * Returns: the number of different atom groups
 ****************************************************************************/
unsigned int StructuresTemplate::checkCompatabilityWithGroups(const Structure &structure,
		unsigned int &firstDiffTemplateIndex, unsigned int &firstDiffAtomGroupIndex,
		bool &firstDiffMissing) {

	firstDiffTemplateIndex = -1;
	firstDiffAtomGroupIndex = -1;

	const AtomGroup* atomGroups = structure.getAtomGroups();
	unsigned int n = structure.getNumberOfAtomGroups();
	unsigned int sgroup = 0;
	unsigned int tgroup = 0;
	unsigned int tgroup_num = m_atomGroupTemplates[tgroup].m_iNumber;
	unsigned int tgroupi = 0;
	bool match;
	unsigned int numDifferences = 0;
	while (true) {
		match = sgroup < n && m_atomGroupTemplates[tgroup].atomicNumbersMatch(atomGroups[sgroup]);
		if ((!match && tgroupi < tgroup_num) ||
				(match && tgroupi == tgroup_num)) {
			++numDifferences;
			if (numDifferences == 1) {
				firstDiffTemplateIndex = tgroup;
				firstDiffAtomGroupIndex = sgroup;
				firstDiffMissing = !match;
			}
		}
		if (match)
			++sgroup;
		++tgroupi;
		if ((!match && tgroupi == tgroup_num) ||
				tgroupi > tgroup_num) {
			++tgroup;
			if (tgroup >= m_iAtomGroupTemplates) {
				// If we have groups in the structure that are of a different
				// type than those in the template, they're incompatible.
				if (sgroup < n)
					numDifferences = 1000000;
				break;
			}
			tgroup_num = m_atomGroupTemplates[tgroup].m_iNumber;
			tgroupi = 0;
		}
	}
	return numDifferences;
}

/**************************************************************************
 * Purpose: This method performs the same function as checkCompatabilityWithoutGroups,
 *    except this method assumes the structure was read from an output file
 *    and that it doesn't yet have atom groups (has all atoms in one group).
 *    It does assume that atoms are in the right order.
 * Parameters:
 *    structure - the structure
 *    firstDiffTemplateIndex - If a difference is found, this will be the
 *       index in the m_atomGroupTemplates of the first different atom group.
 *    firstDiffAtomGroupIndex - If a difference is found, this will be the
 *       atom group index in the structure of the first difference.
 *    firstDiffMissing - If true, firstDiffTemplateIndex will be an index
 *       in the template of the first atom group that's missing in the
 *       structure; firstDiffAtomGroupIndex will be where to insert the
 *       atom group.  If false, firstDiffAtomGroupIndex will be an index to
 *       the extra atom group that should be removed from the structure.
 * Returns: the number of different atom groups
 *************************************************************************/
unsigned int StructuresTemplate::checkCompatabilityWithoutGroups(const Structure &structure,
		unsigned int &firstDiffTemplateIndex, unsigned int &firstDiffAtomGroupIndex,
		bool &firstDiffMissing) {

	firstDiffTemplateIndex = -1;
	firstDiffAtomGroupIndex = -1;

	if (structure.getNumberOfAtomGroups() != 1) {
		return 1000000; // Some big number indicating they're incompatible
	}

	const unsigned int* atomicNumbers = structure.getAtomicNumbers();
	unsigned int n = structure.getNumberOfAtoms();
	unsigned int structAtomIndex = 0;
	unsigned int structAtomGroupIndex = 0;
	unsigned int tgroup = 0;
	unsigned int tgroup_num = m_atomGroupTemplates[tgroup].m_iNumber;
	unsigned int tgroupi = 0;
	bool match;
	unsigned int numDifferences = 0;
	while (true) {
		match = structAtomIndex < n
				&& m_atomGroupTemplates[tgroup].atomicNumbersMatch(
						&(atomicNumbers[structAtomIndex]), n-structAtomIndex);
		if ((!match && tgroupi < tgroup_num) ||
				(match && tgroupi == tgroup_num)) {
			++numDifferences;
			if (numDifferences == 1) {
				firstDiffTemplateIndex = tgroup;
				firstDiffAtomGroupIndex = structAtomGroupIndex;
				firstDiffMissing = !match;
			}
		}
		if (match) {
			structAtomIndex += m_atomGroupTemplates[tgroup].m_atomicNumbers.size();
			++structAtomGroupIndex;
		}
		++tgroupi;
		if ((!match && tgroupi == tgroup_num) ||
				tgroupi > tgroup_num) {
			++tgroup;
			if (tgroup >= m_iAtomGroupTemplates) {
				// If we have groups in the structure that are of a different
				// type than those in the template, they're incompatible.
				if (structAtomIndex < n)
					numDifferences = 1000000;
				break;
			}
			tgroup_num = m_atomGroupTemplates[tgroup].m_iNumber;
			tgroupi = 0;
		}
	}
	return numDifferences;
}

/**************************************************************************
 * Purpose: This method checks the structure for compatibility with the
 *    template.  If the structure has the same atom groups, they're
 *    compatible. If the structure has the same atom groups as the
 *    template, except one less or one more of a particular atom group,
 *    this method will add or delete as needed.
 * Parameters:
 *    structure - the structure
 *    structureNumber - needed for printing error messages
 *    constraints - used for adding new atom groups
 * Returns: true if the structure is or was made to be compatible with the
 *    template, and if the structure meets the constraints.
 *************************************************************************/
bool StructuresTemplate::ensureCompatibile(Structure &structure,
		unsigned int structureNumber, const Constraints &constraints) {
	unsigned int numDifferences;
	unsigned int firstDiffTemplateIndex;
	unsigned int firstDiffAtomGroupIndex;
	bool firstDiffMissing;

	if (m_iAtomGroupTemplates == 0)
		return true;

	numDifferences = checkCompatabilityWithGroups(structure,
			firstDiffTemplateIndex, firstDiffAtomGroupIndex, firstDiffMissing);

	if (numDifferences == 0) {
		return true;
	} else if (numDifferences == 1) {
		if (firstDiffMissing) {
			structure.insertAtomGroup(m_atomGroupTemplates[firstDiffTemplateIndex], firstDiffAtomGroupIndex);

			InitResult result = initializeAtomGroup(structure, firstDiffAtomGroupIndex, StructuresTemplate::ThreeD, constraints);
			if (Success != result) {
				printInitFailureMessage(result);
				return false;
			}
		} else {
			structure.deleteAtomGroup(firstDiffAtomGroupIndex);
			structure.update();
		}
		return true;
	}

	numDifferences = checkCompatabilityWithoutGroups(structure,
			firstDiffTemplateIndex, firstDiffAtomGroupIndex, firstDiffMissing);

	if (numDifferences == 0) {
		structure.setAtomGroups(m_iAtomGroupTemplates, m_atomGroupTemplates);
		return true;
	} else if (numDifferences == 1) {
		AtomGroupTemplate* agt = &m_atomGroupTemplates[firstDiffTemplateIndex];
		if (firstDiffMissing) {
			--agt->m_iNumber;
			structure.setAtomGroups(m_iAtomGroupTemplates, m_atomGroupTemplates);
			++agt->m_iNumber;
			structure.insertAtomGroup(*agt, firstDiffAtomGroupIndex);

			InitResult result = initializeAtomGroup(structure, firstDiffAtomGroupIndex, StructuresTemplate::ThreeD, constraints);
			if (Success != result) {
				printInitFailureMessage(result);
				return false;
			}
		} else {
			++agt->m_iNumber;
			structure.setAtomGroups(m_iAtomGroupTemplates, m_atomGroupTemplates);
			--agt->m_iNumber;
			structure.deleteAtomGroup(firstDiffAtomGroupIndex);
			structure.update();
		}
		return true;
	} else {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sSeededStructureDoesntMatchTemplate.c_str(), structureNumber);
		return false;
	}

	return false;
}

bool StructuresTemplate::initializeStructures(std::list<Structure*> &structures,
		const Constraints* pActionConstraints) {
	unsigned int i;
	Constraints combinedConstraints;
	Constraints* pTempConstraints = NULL;
	Constraints* pTempConstraintsUsed;

	if (NULL != pActionConstraints)
		combinedConstraints.combineConstraints(*pActionConstraints);
	if (NULL != m_pConstraints)
		combinedConstraints.combineConstraints(*m_pConstraints);

	bool success = true;
	std::list<Structure*> seededStructures;
	try {
		if (m_pSeed != NULL)
			if (!m_pSeed->readStructures(seededStructures))
				throw "";

		i = 1;
		for (std::list<Structure*>::iterator it = seededStructures.begin(); it != seededStructures.end(); ++it) {
			Structure* pStructure = new Structure();
			pStructure->copy(**it);
			structures.push_back(pStructure);
			if (!ensureCompatibile(*pStructure, i, combinedConstraints))
				return false;
			if (!combinedConstraints.validate(*pStructure)) {
				const Strings* messagesDL = Strings::instance();
				printf(messagesDL->m_sSeededStructureDoesntMatchConstraints.c_str(), i);
				return false;
			}
			++i;
		}

		if (NULL == m_pLinearConstraints)
			pTempConstraintsUsed = &combinedConstraints;
		else {
			pTempConstraints = new Constraints();
			pTempConstraints->copy(combinedConstraints);
			pTempConstraints->combineConstraints(*m_pLinearConstraints);
			pTempConstraintsUsed = pTempConstraints;
		}
		for (i = 1; i <= m_iLinear; ++i) {
			Structure* pStructure = new Structure();
			structures.push_back(pStructure);
			pStructure->setAtomGroups(m_iAtomGroupTemplates, m_atomGroupTemplates);
			if (!initializeStructure(*pStructure, *pTempConstraintsUsed, StructuresTemplate::Linear))
				throw "";
		}
		if (NULL != pTempConstraints)
			delete pTempConstraints;

		if (NULL == m_pPlanarConstraints)
			pTempConstraintsUsed = &combinedConstraints;
		else {
			pTempConstraints = new Constraints();
			pTempConstraints->copy(combinedConstraints);
			pTempConstraints->combineConstraints(*m_pPlanarConstraints);
			pTempConstraintsUsed = pTempConstraints;
		}
		for (i = 1; i <= m_iPlanar; ++i) {
			Structure* pStructure = new Structure();
			structures.push_back(pStructure);
			pStructure->setAtomGroups(m_iAtomGroupTemplates, m_atomGroupTemplates);
			if (!initializeStructure(*pStructure, *pTempConstraintsUsed, StructuresTemplate::Planar))
				throw "";
		}
		if (NULL != pTempConstraints)
			delete pTempConstraints;

		if (NULL == m_p3DConstraints)
			pTempConstraintsUsed = &combinedConstraints;
		else {
			pTempConstraints = new Constraints();
			pTempConstraints->copy(combinedConstraints);
			pTempConstraints->combineConstraints(*m_p3DConstraints);
			pTempConstraintsUsed = pTempConstraints;
		}
		for (i = 1; i <= m_i3D; ++i) {
			Structure* pStructure = new Structure();
			structures.push_back(pStructure);
			pStructure->setAtomGroups(m_iAtomGroupTemplates, m_atomGroupTemplates);
			if (!initializeStructure(*pStructure, *pTempConstraintsUsed, StructuresTemplate::ThreeD))
				throw "";
		}
		if (NULL != pTempConstraints)
			delete pTempConstraints;
	} catch (const char* message) {
		success = false;
		if (NULL != pTempConstraints)
			delete pTempConstraints;
	}

	for (std::list<Structure*>::iterator it = seededStructures.begin(); it != seededStructures.end(); ++it)
		delete *it;
	seededStructures.clear();

	i = 0;
	for (std::list<Structure*>::iterator it = structures.begin(); it != structures.end(); ++it)
		(*it)->setId(++i);

	return success;
}

unsigned int getRandomKey(const std::map<unsigned int, bool> &map) {
	unsigned int randomIndex = Random::getInt(0, map.size() - 1);
	unsigned int i = 0;
	for (std::map<unsigned int, bool>::const_iterator it = map.begin(); it != map.end(); it++, i++)
		if (i == randomIndex)
			return it->first;
	return 0;
}

StructuresTemplate::InitResult StructuresTemplate::initializeStructure(Structure &structure,
		const Constraints &constraints, Type type,
		unsigned int iMaxTries, unsigned int &iTries,
		unsigned int iMaxGroupTries, unsigned int &iGroupTries) {
	unsigned int n = structure.getNumberOfAtomGroups();
	unsigned int i;
	std::map<unsigned int, bool> atomGroupsInitialized;
	std::map<unsigned int, bool> atomGroupsNotInitialized;
	unsigned int currentAtomGroupIndex;
	int previousAtomGroupIndex = -1;
	InitResult status;

	iTries = 0;
	do {
		atomGroupsNotInitialized.clear();
		atomGroupsInitialized.clear();
		for (i = 0; i < n; ++i)
			atomGroupsNotInitialized[i] = true;
		do {
			currentAtomGroupIndex = getRandomKey(atomGroupsNotInitialized);
			#if INIT_DEBUG
				printf("\tInitializing atom group with index %u (%u out of %u groups initialized, this is try %u out of %u)...\n",
						currentAtomGroupIndex, atomGroupsInitialized.size(),
						structure.getNumberOfAtomGroups(), iTries+1, iMaxTries);
			#endif
			status = initializeAtomGroup(structure, currentAtomGroupIndex, type, previousAtomGroupIndex,
					constraints, iMaxGroupTries, iGroupTries, atomGroupsInitialized);
			if (Success == status) {
				atomGroupsNotInitialized.erase(currentAtomGroupIndex);
				previousAtomGroupIndex = currentAtomGroupIndex;
			}
			#if INIT_DEBUG
				printf("\tInitialization finished for atom group with index %u with success = %u and %u groups left to initialize.\n",
						currentAtomGroupIndex, status == Success,
						structure.getNumberOfAtomGroups()
								- atomGroupsInitialized.size());
			#endif
		} while (atomGroupsNotInitialized.size() > 0 && Success == status);
		++iTries;
	} while (iTries < iMaxTries && Success != status);

	return status;
}

StructuresTemplate::InitResult StructuresTemplate::initializeAtomGroup(
		Structure &structure, unsigned int index, Type type,
		const Constraints &constraints) {
	unsigned int n = structure.getNumberOfAtomGroups();
	std::map<unsigned int, bool> atomGroupsInitialized;
	for (unsigned int i = 0; i < n; ++i)
		if (i != index)
			atomGroupsInitialized[i] = true;
	unsigned int iTries;
	return initializeAtomGroup(structure, index, type, -1, constraints,
			defaultMaxGroupInitializationTries * 2, iTries,
			atomGroupsInitialized);
}

StructuresTemplate::InitResult StructuresTemplate::initializeAtomGroup(
		Structure &structure, unsigned int index, Type type, int prevIndex,
		const Constraints &constraints, unsigned int iMaxTries,
		unsigned int &iTries,
		std::map<unsigned int, bool> &atomGroupsInitialized) {
	if (!constraints.hasContainerOrMaxDist()) {
		return MissingContainerOrMaxDist;
	}
	AtomGroup* currentAtomGroup = structure.getAtomGroup(index);
	COORDINATE3 center, angles;
	COORDINATE4 unitVector;
	COORDINATE3 zeros = {0};
	FLOAT angleX, angleY;
	InitResult status;

	iTries = 0;
	do {
		status = Success;
		if (currentAtomGroup->getNumberOfAtoms() > 1) {
			angles[0] = Random::getFloat(0,PIE_X_2);
			angles[1] = Random::getFloat(-PIE_OVER_2,PIE_OVER_2);
			angles[2] = Random::getFloat(0,PIE_X_2);
			currentAtomGroup->setAngles(angles);
		}

		memcpy(center, zeros, SIZEOF_COORDINATE3);
		if (NULL == constraints.m_pfGeneralMaxAtomicDistance) {
			switch(type) {
			case ThreeD:
				center[1] = Random::getFloat(-*constraints.m_pfHalfCubeLWH, *constraints.m_pfHalfCubeLWH);
				// Notice there's no break here
			case Planar:
				center[0] = Random::getFloat(-*constraints.m_pfHalfCubeLWH, *constraints.m_pfHalfCubeLWH);
				// Notice there's no break here
			case Linear:
				center[2] = Random::getFloat(-*constraints.m_pfHalfCubeLWH, *constraints.m_pfHalfCubeLWH);
				break;
			}
			currentAtomGroup->setCenter(center);
			currentAtomGroup->initRotationMatrix();
			currentAtomGroup->localToGlobal();
			structure.updateAtomDistanceMatrix();
			status = Success;
		} else {
			if (atomGroupsInitialized.size() == 0) {
				currentAtomGroup->setCenter(zeros);
				currentAtomGroup->initRotationMatrix();
				currentAtomGroup->localToGlobal();
				structure.updateAtomDistanceMatrix();
			} else {
				unsigned int referenceIndex;
				if (type == Linear) {
					if (prevIndex == -1)
						referenceIndex = getRandomKey(atomGroupsInitialized);
					else
						referenceIndex = (unsigned int)prevIndex;
					unitVector[0] = 0;
					unitVector[1] = 0;
					unitVector[2] = 1;
					unitVector[3] = 1;
				} else {
					referenceIndex = getRandomKey(atomGroupsInitialized);
					if (type == ThreeD)
						angleX = Random::getFloat(-PIE_OVER_2,PIE_OVER_2);
					else
						angleX = 0;
					angleY = Random::getFloat(0,PIE_X_2);
					getVectorInDirection(angleX, angleY, 1, unitVector);
				}
				status = placeAtomGroupRelativeToAnother(index, referenceIndex, structure, unitVector, constraints);
			}
		}
		#if INIT_DEBUG
			printf("\tAtom group initialized with success = %u:\n", status == Success);
			currentAtomGroup->print(true, true);
		#endif
		if (status == Success) {
			atomGroupsInitialized[index] = true;
			structure.updateAtomGroupDistanceMatrix();
			#if INIT_DEBUG
				printf("\tChecking constraints...");
			#endif
			if (!constraints.minDistancesOK(atomGroupsInitialized, structure))
				status = MinDistanceProblem;
			else if (!constraints.maxDistancesOK(atomGroupsInitialized, structure))
				status = MaxDistanceProblem;
			else if (!constraints.ensureInsideContainer(atomGroupsInitialized, structure, false))
				status = WontFitInContainer;
			if (status != Success) {
				atomGroupsInitialized.erase(index);
				#if INIT_DEBUG
					printInitFailureMessage(status);
				#endif
			}
			#if INIT_DEBUG
				if (Success == status)
					printf(" Passed!\n");
			#endif
		}
		++iTries;
	} while (iTries < iMaxTries && status != Success);

	return status;
}

void StructuresTemplate::printInitFailureMessage(InitResult result) {
	switch (result) {
	case Success:
		break;
	case MissingContainerOrMaxDist:
		printf("Failed to initialize structure because a required container or maximum distance constraint is missing.\n");
		break;
	case MinDistanceProblem:
		printf("Failed to initialize structure because minimum distance constraints could not be met.\n");
		break;
	case MaxDistanceProblem:
		printf("Failed to initialize structure because maximum distance constraints could not be met.\n");
		break;
	case WontFitInContainer:
		printf("Failed to initialize structure because it would not fit in the container.\n");
		break;
	case PlaceAtomGroupRelativeToAnotherMethodFailed:
		printf("Failed to initialize structure because the StructuresTemplate::placeAtomGroupRelativeToAnother method failed.\n");
		break;
	}
}

void StructuresTemplate::getVectorInDirection(FLOAT angleX, FLOAT angleY,
		FLOAT length, COORDINATE4 &result) {
	FLOAT matrixX[MATRIX_WIDTH][MATRIX_WIDTH];
	FLOAT matrixY[MATRIX_WIDTH][MATRIX_WIDTH];
	FLOAT temp[MATRIX_WIDTH][MATRIX_WIDTH];
	COORDINATE4 vector;

	FLOAT sinAngleX = sin(angleX);
	FLOAT sinAngleY = sin(angleY);
	FLOAT cosAngleX = cos(angleX);
	FLOAT cosAngleY = cos(angleY);

	vector[0] = 0;
	vector[1] = 0;
	vector[2] = length;
	vector[3] = 1;

	memcpy(matrixX, IDENTITY_MATRIX, SIZEOF_MATRIX);
	memcpy(matrixY, IDENTITY_MATRIX, SIZEOF_MATRIX);

	// Note that the column index comes before the row index (matrix[col][row])
	matrixX[1][1] = cosAngleX;
	matrixX[2][1] = -sinAngleX;
	matrixX[1][2] = sinAngleX;
	matrixX[2][2] = cosAngleX;

	matrixY[0][0] = cosAngleY;
	matrixY[2][0] = sinAngleY;
	matrixY[0][2] = -sinAngleY;
	matrixY[2][2] = cosAngleY;

	Matrix::matrixMultiplyMatrix(matrixX,matrixY,temp);
	Matrix::matrixMultiplyPoint(temp,vector,result);
}

StructuresTemplate::InitResult StructuresTemplate::placeAtomGroupRelativeToAnother(
		unsigned int agToPlaceIndex, unsigned int otherAgIndex,
		Structure &structure, const FLOAT* unitVector,
		const Constraints &constraints) {
	AtomGroup* agToPlace = structure.getAtomGroup(agToPlaceIndex);
	const AtomGroup* otherAg = structure.getAtomGroup(otherAgIndex);
	FLOAT distance;
	COORDINATE3 vector;
	COORDINATE3 center;
	const FLOAT* otherCenter;
	unsigned int i;

	if (agToPlace->getNumberOfAtoms() == 1 && otherAg->getNumberOfAtoms() == 1) {
		FLOAT minDistance = constraints.getMinDistance(agToPlace->getAtomicNumbers()[0], otherAg->getAtomicNumbers()[0]);
		if (NULL == constraints.m_pfGeneralMaxAtomicDistance) {
			distance = Random::getFloat(minDistance, minDistance+1);
		} else {
			distance = Random::getFloat(minDistance, *constraints.m_pfGeneralMaxAtomicDistance);
		}
		otherCenter = otherAg->getCenter();
		for (i = 0; i < 3; ++i) {
			vector[i] = unitVector[i] * distance;
			center[i] = otherCenter[i] + vector[i];
		}
		agToPlace->setCenter(center);
		agToPlace->initRotationMatrix();
		agToPlace->localToGlobal();
		structure.updateAtomDistanceMatrix();
	} else {
		// Find the atoms in the two molecules that are closest to one another
		distance = 1000; // some big number
		otherCenter = otherAg->getCenter();
		for (i = 0; i < 3; ++i) {
			vector[i] = unitVector[i] * distance;
			center[i] = otherCenter[i] + vector[i];
		}
		agToPlace->setCenter(center);
		agToPlace->initRotationMatrix();
		agToPlace->localToGlobal();

		const FLOAT* atom1; // atom in agToPlace closest to the otherAg (closest to atom2)
		const FLOAT* atom2; // atom in the otherAg closest to agToPlace (closest to atom1)
		FLOAT minDistanceBetweenAtoms;
		getClosestAtoms(*agToPlace, *otherAg, constraints, atom1, atom2, minDistanceBetweenAtoms);

		#if INIT_DEBUG == 2
			structure.updateAtomDistanceMatrix();
			structure.print(Structure::PRINT_LOCAL_COORDINATES | Structure::PRINT_DISTANCE_MATRIX);

			unsigned int j;
			FLOAT minDist;
			const FLOAT* const* distanceMatrix = structure.getAtomDistanceMatrix();
			FLOAT** adjustedDistanceMatrix = new FLOAT*[structure.getNumberOfAtoms()];
			for (i = 0; i < structure.getNumberOfAtoms(); ++i) {
				adjustedDistanceMatrix[i] = new FLOAT[structure.getNumberOfAtoms()];
				memcpy(adjustedDistanceMatrix[i], distanceMatrix[i], sizeof(FLOAT)*structure.getNumberOfAtoms());
			}
			for (i = 0; i < structure.getNumberOfAtoms(); ++i) {
				for (j = i+1; j < structure.getNumberOfAtoms(); ++j) {
					minDist = constraints.getMinDistance(structure.getAtomicNumbers()[i], structure.getAtomicNumbers()[j]);
					adjustedDistanceMatrix[i][j] -= minDist;
					adjustedDistanceMatrix[j][i] -= minDist;
				}
			}
			printf("Adjusted ");
			Structure::printDistanceMatrix(adjustedDistanceMatrix, structure.getAtomicNumbers(), structure.getNumberOfAtoms());
			for (i = 0; i < structure.getNumberOfAtoms(); ++i)
				delete[] adjustedDistanceMatrix[i];
			delete[] adjustedDistanceMatrix;

			printf("atom1: ");
			AtomGroup::printPoint(atom1);
			printf("\natom2: ");
			AtomGroup::printPoint(atom2);
			printf("\n");
		#endif

		// Let R be a line defined by unitVector and atom2;
		COORDINATE3 pointOnRClosestToAtom1;
		closestPointFromALineToAPoint(atom2, unitVector, atom1,
				pointOnRClosestToAtom1);

		// Let L be a line defined by unitVector and otherAg->getCenter();
		COORDINATE3 pointOnLClosestToAtom1;
		COORDINATE3 pointOnLClosestToAtom2;
		closestPointFromALineToAPoint(otherAg->getCenter(), unitVector, atom1, pointOnLClosestToAtom1);
		closestPointFromALineToAPoint(otherAg->getCenter(), unitVector, atom2, pointOnLClosestToAtom2);

		FLOAT a = euclideanDistance(pointOnLClosestToAtom1,agToPlace->getCenter());
		FLOAT b = euclideanDistance(pointOnLClosestToAtom2,otherAg->getCenter());
		FLOAT x = euclideanDistance(pointOnRClosestToAtom1,atom1);
		FLOAT e;
		FLOAT maxDist;
		if (NULL == constraints.m_pfGeneralMaxAtomicDistance)
			maxDist = minDistanceBetweenAtoms + 1;
		else
			maxDist = *constraints.m_pfGeneralMaxAtomicDistance;
		if (x > maxDist)
			// There is no way to do this problem the way we want to if x > maxDist.
			// So we'll set e to the minimum possible value and then increment it (see below).
			e = a + b;
		else {
			FLOAT y;
			if (minDistanceBetweenAtoms < x)
				y = Random::getFloat(x,maxDist);
			else
				y = Random::getFloat(minDistanceBetweenAtoms,maxDist);
			e = sqrt((y * y) - (x * x)) + a + b;
		}
		if (isnan(e))
		{
			printf("e = nan in the function StructuresTemplate::placeAtomGroupRelativeToAnother!\n");
			return PlaceAtomGroupRelativeToAnotherMethodFailed;
		}

		#if INIT_DEBUG == 2
			printf("pointOnRClosestToAtom1: ");
			AtomGroup::printPoint(pointOnRClosestToAtom1);
			printf("\n");

			printf("pointOnLClosestToAtom1: ");
			AtomGroup::printPoint(pointOnLClosestToAtom1);
			printf("\n");

			printf("pointOnLClosestToAtom2: ");
			AtomGroup::printPoint(pointOnLClosestToAtom2);
			printf("\n");

			printf("a = %lf, b = %lf, x = %lf, e = %lf, maxDist = %lf\n", a, b, x, e, maxDist);
		#endif

		while (true) {
			for (i = 0; i < 3; ++i) {
				vector[i] = unitVector[i] * e;
				center[i] = otherCenter[i] + vector[i];
			}
			if (isnan(center[0]))
			{
				printf("center[0] = nan in the function StructuresTemplate::placeAtomGroupRelativeToAnother!\n");
				return PlaceAtomGroupRelativeToAnotherMethodFailed;
			}
			agToPlace->setCenter(center);
			agToPlace->initRotationMatrix();
			agToPlace->localToGlobal();
			structure.updateAtomDistanceMatrix();
			#if INIT_DEBUG == 2
				printf("\nStructure with (e = %lf)\n", e);
				structure.print(Structure::PRINT_DISTANCE_MATRIX);
			#endif
			if (minDistancesOK(agToPlaceIndex, otherAgIndex, structure, constraints))
				break;
			e += 0.1;
		}
	}

	return Success;
}

void StructuresTemplate::getClosestAtoms(const AtomGroup &ag1,
		const AtomGroup &ag2, const Constraints &constraints,
		const FLOAT* &atom1, const FLOAT* &atom2, FLOAT &minDistBetween1And2) {
	const COORDINATE4* coordinates1 = ag1.getGlobalAtomCoordinates();
	const COORDINATE4* coordinates2 = ag2.getGlobalAtomCoordinates();

	FLOAT diff, dist, minDist;
	FLOAT smallestDist = 1e100; // some big number
	int closestIn1 = -1;
	int closestIn2 = -1;
	unsigned int i;

	for (unsigned int i1 = 0, n1 = ag1.getNumberOfAtoms(); i1 < n1; ++i1)
		for (unsigned int i2 = 0, n2 = ag2.getNumberOfAtoms(); i2 < n2; ++i2) {
			dist = 0;
			for (i = 0; i < 3; ++i) {
				diff = coordinates1[i1][i] - coordinates2[i2][i];
				dist += diff * diff;
			}
			dist = sqrt(dist);
			minDist = constraints.getMinDistance(ag1.getAtomicNumbers()[i1], ag2.getAtomicNumbers()[i2]);
			dist -= minDist;
			if (dist < smallestDist) {
				smallestDist = dist;
				minDistBetween1And2 = minDist;
				closestIn1 = i1;
				closestIn2 = i2;
			}
		}

	atom1 = coordinates1[closestIn1];
	atom2 = coordinates2[closestIn2];
}

void StructuresTemplate::closestPointFromALineToAPoint(
		const FLOAT* pointOnLine, const FLOAT* vectorAlongLine,
		const FLOAT* point, FLOAT* result) {
	FLOAT vectorLengthSquared;
	FLOAT u;

	vectorLengthSquared =
		vectorAlongLine[0] * vectorAlongLine[0] +
		vectorAlongLine[1] * vectorAlongLine[1] +
		vectorAlongLine[2] * vectorAlongLine[2];

	u = ((point[0] - pointOnLine[0]) * vectorAlongLine[0] +
		 (point[1] - pointOnLine[1]) * vectorAlongLine[1] +
		 (point[2] - pointOnLine[2]) * vectorAlongLine[2]) / vectorLengthSquared;

	result[0] = pointOnLine[0] + u * vectorAlongLine[0];
	result[1] = pointOnLine[1] + u * vectorAlongLine[1];
	result[2] = pointOnLine[2] + u * vectorAlongLine[2];
}

FLOAT StructuresTemplate::euclideanDistance(const FLOAT* point1, const FLOAT* point2) {
	COORDINATE3 diff;
	FLOAT answer = 0;
	FLOAT* diffPtrEnd = diff + 3;
	for (FLOAT* diffPtr = diff; diffPtr < diffPtrEnd; ++diffPtr) {
		*diffPtr = *(point1++) - *(point2++);
		answer += *diffPtr * *diffPtr;
	}
	return sqrt(answer);
}

bool StructuresTemplate::minDistancesOK(unsigned int ag1Index,
		unsigned int ag2Index, const Structure &structure,
		const Constraints &constraints) {
	const AtomGroup* atomGroups = structure.getAtomGroups();
	unsigned int startIndex1 = 0;
	unsigned int startIndex2 = 0;
	unsigned int i, j, n, m;
	unsigned int nAtomGroups = structure.getNumberOfAtomGroups();
	for (i = 0; i < nAtomGroups; ++i) {
		if (i < ag1Index)
			startIndex1 += atomGroups[i].getNumberOfAtoms();
		if (i < ag2Index)
			startIndex2 += atomGroups[i].getNumberOfAtoms();
	}
	const FLOAT* const* atomDistanceMatrix = structure.getAtomDistanceMatrix();
	const unsigned int* atomicNumbers = structure.getAtomicNumbers();
	for (i = startIndex1, n = startIndex1 + atomGroups[ag1Index].getNumberOfAtoms(); i < n; ++i)
		for (j = startIndex2, m = startIndex2 + atomGroups[ag2Index].getNumberOfAtoms(); j < m; ++j)
			if (atomDistanceMatrix[i][j] < constraints.getMinDistance(atomicNumbers[i], atomicNumbers[j]))
				return false;
	return true;
}
