
#include "structuresTemplate.h"

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
	cleanUp();
}

void StructuresTemplate::cleanUp()
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
	cleanUp();
	
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
		if (!readInitializationType(ptElements[1], constraintsMap, m_iLinear, &m_pLinearConstraints, messages))
			return false;
	
	if (ptElements[2] != NULL)
		if (!readInitializationType(ptElements[2], constraintsMap, m_iPlanar, &m_pPlanarConstraints, messages))
			return false;
	
	if (ptElements[3] != NULL)
		if (!readInitializationType(ptElements[3], constraintsMap, m_i3D, &m_p3DConstraints, messages))
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

bool StructuresTemplate::readInitializationType(TiXmlElement *pElem, std::map<std::string,Constraints*> &constraintsMap, unsigned int &numberOfThisType, Constraints** pConstraints, const Strings* messages) {
	const char** values;
	const char* attributeNames[] = {messages->m_sxNumber.c_str(), messages->m_sxConstraints.c_str()};
	XsdAttributeUtil attUtil(pElem->Value(), attributeNames, s_initTypeAttRequired, s_initTypeAttDefaults);
	if (!attUtil.process(pElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();
	
	if (!XsdTypeUtil::getPositiveInt(values[0], numberOfThisType, attributeNames[0], pElem)) {
		return false;
	}
	
	if (values[1] != NULL) {
		(*pConstraints) = constraintsMap[values[1]];
		if ((*pConstraints) == NULL) {
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

/**************************************************************************
 * Purpose: This method examines differences between the template and the
 *    structure.  A "difference" means two atom groups have differing
 *    numbers of atoms or different atomic numbers.
 * Parameters:
 *    structure - the structure
 *    firstDiffTemplateIndex - If a difference is found, this will be the
 *       index in the template of the first different atom group.
 *    firstDiffStructureIndex - If a difference is found, this will be the
 *       index in the structure of the first different atom group.
 *    firstDiffMissing - If true, firstDiffTemplateIndex will be an index
 *       in the template of the first atom group that's missing in the
 *       structure; firstDiffStructureIndex will be where to insert the
 *       atom group.  If false, firstDiffStructureIndex will be an index to
 *       the extra atom group that should be removed from the structure.
 * Returns: the number of different atom groups
 *************************************************************************/
unsigned int StructuresTemplate::checkCompatabilityWithGroups(const Structure &structure,
		unsigned int &firstDiffTemplateIndex, unsigned int &firstDiffStructureIndex,
		bool &firstDiffMissing) {

	firstDiffTemplateIndex = -1;
	firstDiffStructureIndex = -1;

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
				firstDiffStructureIndex = sgroup;
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
 *       index in the template of the first different atom group.
 *    firstDiffStructureIndex - If a difference is found, this will be the
 *       index in the structure of the first different atom group.
 *    firstDiffMissing - If true, firstDiffTemplateIndex will be an index
 *       in the template of the first atom group that's missing in the
 *       structure; firstDiffStructureIndex will be where to insert the
 *       atom group.  If false, firstDiffStructureIndex will be an index to
 *       the extra atom group that should be removed from the structure.
 * Returns: the number of different atom groups
 *************************************************************************/
unsigned int StructuresTemplate::checkCompatabilityWithoutGroups(const Structure &structure,
		unsigned int &firstDiffTemplateIndex, unsigned int &firstDiffStructureIndex,
		bool &firstDiffMissing) {

	firstDiffTemplateIndex = -1;
	firstDiffStructureIndex = -1;

	if (structure.getNumberOfAtomGroups() != 1) {
		return 1000000; // Some big number indicating they're incompatible
	}

	const unsigned int* atomicNumbers = structure.getAtomicNumbers();
	unsigned int n = structure.getNumberOfAtoms();
	unsigned int structIndex = 0;
	unsigned int tgroup = 0;
	unsigned int tgroup_num = m_atomGroupTemplates[tgroup].m_iNumber;
	unsigned int tgroupi = 0;
	bool match;
	unsigned int numDifferences = 0;
	while (true) {
		match = structIndex < n
				&& m_atomGroupTemplates[tgroup].atomicNumbersMatch(
						&(atomicNumbers[structIndex]), n-structIndex);
		if ((!match && tgroupi < tgroup_num) ||
				(match && tgroupi == tgroup_num)) {
			++numDifferences;
			if (numDifferences == 1) {
				firstDiffTemplateIndex = tgroup;
				firstDiffStructureIndex = structIndex;
				firstDiffMissing = !match;
			}
		}
		if (match)
			structIndex += m_atomGroupTemplates[tgroup].m_atomicNumbers.size();
		++tgroupi;
		if ((!match && tgroupi == tgroup_num) ||
				tgroupi > tgroup_num) {
			++tgroup;
			if (tgroup >= m_iAtomGroupTemplates) {
				// If we have groups in the structure that are of a different
				// type than those in the template, they're incompatible.
				if (structIndex < n)
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
 * Purpose: This method checks the structure for compatability with the
 *    template.  If the structure has the same atom groups, they're
 *    compatible. If the structure has the same atom groups as the
 *    template, except one less or one more of a particular atom group,
 *    this method will add or delete as needed.
 * Parameters:
 *    structure - the structure
 *    pActionConstraints - an array of constraints with which newly added
 *       molecules must comply
 * Returns: true if the structure is or was made to be compatible with the
 *    template, and if the structure meets the constraints.
 *************************************************************************/
bool StructuresTemplate::ensureCompatibile(Structure &structure,
		const Constraints* pActionConstraints) {
	unsigned int numDifferences;
	unsigned int firstDiffTemplateIndex;
	unsigned int firstDiffStructureIndex;
	bool firstDiffMissing;

	numDifferences = checkCompatabilityWithGroups(structure,
			firstDiffTemplateIndex, firstDiffStructureIndex, firstDiffMissing);

	if (numDifferences == 0) {
		return true;
	} else if (numDifferences == 1) {
		if (firstDiffMissing) {
			structure.insertAtomGroup(m_atomGroupTemplates[firstDiffTemplateIndex], firstDiffStructureIndex);

			// Todo: initialize the atom group according to the constraints
		} else {
			structure.deleteAtomGroup(firstDiffStructureIndex);
		}
		return true;
	}

	numDifferences = checkCompatabilityWithoutGroups(structure,
			firstDiffTemplateIndex, firstDiffStructureIndex, firstDiffMissing);

	if (numDifferences == 0) {
		structure.setAtomGroups(m_iAtomGroupTemplates, m_atomGroupTemplates);
		return true;
	} else if (numDifferences == 1) {
		AtomGroupTemplate* agt = &m_atomGroupTemplates[firstDiffTemplateIndex];
		if (firstDiffMissing) {
			--agt->m_iNumber;
			structure.setAtomGroups(m_iAtomGroupTemplates, m_atomGroupTemplates);
			++agt->m_iNumber;
			structure.insertAtomGroup(*agt, firstDiffStructureIndex);

			// Todo: initialize the atom group according to the constraints
		} else {
			++agt->m_iNumber;
			structure.setAtomGroups(m_iAtomGroupTemplates, m_atomGroupTemplates);
			--agt->m_iNumber;
			structure.deleteAtomGroup(firstDiffStructureIndex);
		}
		return true;
	}
	return false;
}

bool StructuresTemplate::initializeStructures(unsigned int &numStructures,
		Structure* &structures, const Constraints* pActionConstraints) {
	unsigned int i, j;
	std::vector<const Constraints*> constraints;
	if (NULL != pActionConstraints)
		constraints.push_back(pActionConstraints);
	if (NULL != m_pConstraints)
		constraints.push_back(m_pConstraints);

	bool success = true;
	std::vector<Structure*> seededStructures;
	try {
		if (m_pSeed != NULL)
			if (!m_pSeed->readStructures(seededStructures, constraints))
				throw "";

		numStructures = seededStructures.size();
		structures = new Structure[numStructures];
		for (i = 0, j = 0; i < seededStructures.size(); ++i, ++j) {
			structures[j].copy(*seededStructures[i]);
			if (!ensureCompatibile(structures[j], pActionConstraints))
				return false;
		}
	} catch (const char* message) {
		success = false;
	}

	for (i = 0; i < seededStructures.size(); ++i)
		delete seededStructures[i];
	seededStructures.clear();

	return success;
}
