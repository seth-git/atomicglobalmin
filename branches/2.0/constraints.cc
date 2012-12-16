#include "constraints.h"

//const char*      Constraints::s_elementNames[]     = {"cube", "atomicDistances"};
const unsigned int Constraints::s_minOccurs[] = { 0, 0 };

//const char*      Constraints::s_distElementNames[] = {"min"        , "max"};
const unsigned int Constraints::s_distMinOccurs[] = { 0, 0 };
const unsigned int Constraints::s_distMaxOccurs[] = { XSD_UNLIMITED, 1 };

//const char* Constraints::s_attributeNames[] = {"name", "base"};
const bool    Constraints::s_required[]       = { true, false };
const char*   Constraints::s_defaultValues[]  = { "", "" };

//const char* Constraints::s_minAttributeNames[] = {"Value", "z1" , "z2"};
const bool    Constraints::s_minRequired[]       = { true, false, false };
const char*   Constraints::s_minDefaultValues[]  = { "", "", "" };

void Constraints::cleanUp() {
	m_sName = "";
	m_pBase = NULL;
	if (m_pfCubeLWH != NULL) {
		delete m_pfCubeLWH;
		m_pfCubeLWH = NULL;
	}
	if (m_pfGeneralMinAtomicDistance != NULL) {
		delete m_pfGeneralMinAtomicDistance;
		m_pfGeneralMinAtomicDistance = NULL;
	}
	if (m_pfGeneralMaxAtomicDistance != NULL) {
		delete m_pfGeneralMaxAtomicDistance;
		m_pfGeneralMaxAtomicDistance = NULL;
	}
	if (m_rgMinAtomicDistances != NULL) {
		delete[] m_rgMinAtomicDistances;
		m_rgMinAtomicDistances = NULL;
		m_mapMinAtomicDistances.clear();
	}
}

bool Constraints::load(TiXmlElement *pConstraintsElem, const Strings* messages,
                       std::map<std::string, Constraints*> &constraintsMap) {
	const Strings* messagesDL = Strings::instance();
	const char* elementNames[] = { messages->m_sxCube.c_str(), messages->m_sxAtomicDistances.c_str() };
	const char* distElementNames[] = { messages->m_sxMin.c_str(), messages->m_sxMax.c_str() };
	XsdElementUtil constraintUtil(pConstraintsElem->Value(), XSD_ALL, elementNames, s_minOccurs);
	TiXmlHandle handle(0);
	TiXmlElement** constraintElements;

	cleanUp();

	const char* valueAttNames[] = { messages->m_sxName.c_str(), messages->m_sxBase.c_str() };
	const char** values;
	XsdAttributeUtil valueUtil(pConstraintsElem->Value(), valueAttNames, s_required, s_defaultValues);
	if (!valueUtil.process(pConstraintsElem)) {
		return false;
	}
	values = valueUtil.getAllAttributes();

	if (values[1] != NULL) {
		Constraints* other = constraintsMap[values[1]];
		if (other == NULL) {
			printf(messagesDL->m_sConstraintNameMisMatch.c_str(), pConstraintsElem->Row(), messages->m_sxBase.c_str(), values[1]);
			return false;
		}
		copy(*other);
		m_pBase = other;
	}
	m_sName = values[0];

	handle = TiXmlHandle(pConstraintsElem);
	if (!constraintUtil.process(handle)) {
		return false;
	}
	constraintElements = constraintUtil.getAllElements();

	if (constraintElements[0] != NULL) {
		if (!m_pfCubeLWH)
			m_pfCubeLWH = new FLOAT;
		if (!XsdTypeUtil::readPosFloatValueElement(constraintElements[0], *m_pfCubeLWH, messages->m_sxSize.c_str()))
			return false;
	}

	if (constraintElements[1] != NULL) {
		XsdElementUtil distUtil(pConstraintsElem->Value(), XSD_SEQUENCE, distElementNames, s_distMinOccurs, s_distMaxOccurs);
		TiXmlHandle handle(0);
		std::vector<TiXmlElement*>* distElements;
		unsigned int i;

		handle = TiXmlHandle(constraintElements[1]);
		if (!distUtil.process(handle)) {
			return false;
		}
		distElements = distUtil.getSequenceElements();
		unsigned int timesReadGeneralMin = 0;
		for (i = 0; i < distElements[0].size(); ++i) {
			if (!addMinDist(distElements[0][i], timesReadGeneralMin, messages)) {
				return false;
			}
		}
		for (i = 0; i < distElements[1].size(); ++i) { // Should be only one
			if (!m_pfGeneralMaxAtomicDistance)
				m_pfGeneralMaxAtomicDistance = new FLOAT;
			if (!XsdTypeUtil::readPosFloatValueElement(distElements[1][i], *m_pfGeneralMaxAtomicDistance, messages)) {
				return false;
			}
		}
	}

	return true;
}

bool Constraints::addMinDist(TiXmlElement *pElem, unsigned int &timesReadGeneralMin, const Strings* messages) {
	const char* minAttributeNames[] = {messages->m_sxValue.c_str(), messages->m_sxZ1.c_str(), messages->m_sxZ2.c_str() };
	const char** values;
	const Strings* messagesDL = Strings::instance();

	XsdAttributeUtil attUtil(pElem->Value(), minAttributeNames, s_minRequired, s_minDefaultValues);
	if (!attUtil.process(pElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();

	if ((values[1] == NULL && values[2] != NULL) || (values[1] != NULL && values[2] == NULL)) {
		printf(messagesDL->m_sErrorZ1Z2.c_str(), pElem->Row(), pElem->Value(), minAttributeNames[1], minAttributeNames[2]);
		return false;
	}

	if (values[1] == NULL) {
		if (timesReadGeneralMin >= 1) {
			printf(messagesDL->m_sErrorOneGeneralMin.c_str(), pElem->Row(), pElem->Value(), minAttributeNames[1], minAttributeNames[2]);
			return false;
		}
		if (!m_pfGeneralMinAtomicDistance)
			m_pfGeneralMinAtomicDistance = new FLOAT;
		XsdTypeUtil::getPositiveFloat(values[0], *m_pfGeneralMinAtomicDistance, minAttributeNames[0], pElem);
		++timesReadGeneralMin;
	} else {
		unsigned int i, j;
		if (m_rgMinAtomicDistances == NULL) {
			m_rgMinAtomicDistances = new MinDistArray[MIN_DIST_ARRAY_SIZE];
			for (i = 1; i <= MAX_ATOMIC_NUMBERS; ++i)
				for (j = 1; j <= MAX_ATOMIC_NUMBERS; ++j)
					m_rgMinAtomicDistances[i][j] = -1;
		}
		if (!XsdTypeUtil::getAtomicNumber(values[1], i, pElem->Row(), minAttributeNames[1], pElem->Value())) {
			return false;
		}
		if (!XsdTypeUtil::getAtomicNumber(values[2], j, pElem->Row(), minAttributeNames[2], pElem->Value())) {
			return false;
		}
		if (m_rgMinAtomicDistances[i][j] != -1) {
			printf(messagesDL->m_sErrorDuplicateMinDist.c_str(), pElem->Row(), pElem->Value(), minAttributeNames[1], i, minAttributeNames[2], j);
			return false;
		}
		if (!XsdTypeUtil::getPositiveFloat(values[0], m_rgMinAtomicDistances[i][j], minAttributeNames[0], pElem)) {
			return false;
		}
		m_rgMinAtomicDistances[j][i] = m_rgMinAtomicDistances[i][j];
		m_mapMinAtomicDistances[i][j] = m_rgMinAtomicDistances[i][j];
	}
	return true;
}

bool Constraints::save(TiXmlElement *pConstraintsElem, const Strings* messages) {
	pConstraintsElem->SetAttribute(messages->m_sxName.c_str(), m_sName.c_str());
	if (m_pBase)
		pConstraintsElem->SetAttribute(messages->m_sxBase.c_str(), m_pBase->m_sName.c_str());
	if (m_pfCubeLWH && (!m_pBase || !m_pBase->m_pfCubeLWH || *m_pfCubeLWH != *(m_pBase->m_pfCubeLWH))) {
		TiXmlElement* cube = new TiXmlElement(messages->m_sxCube.c_str());
		cube->SetDoubleAttribute(messages->m_sxSize.c_str(), *m_pfCubeLWH);
		pConstraintsElem->LinkEndChild(cube);
	}

	bool writeGeneralMin = m_pfGeneralMinAtomicDistance && (!m_pBase || !m_pBase->m_pfGeneralMinAtomicDistance || *m_pfGeneralMinAtomicDistance != *(m_pBase->m_pfGeneralMinAtomicDistance));
	bool writeGeneralMax = m_pfGeneralMaxAtomicDistance && (!m_pBase || !m_pBase->m_pfGeneralMaxAtomicDistance || *m_pfGeneralMaxAtomicDistance != *(m_pBase->m_pfGeneralMaxAtomicDistance));
	bool writeSpecificMins = specificMinDistNotInBase();

	if (writeGeneralMin || writeGeneralMax || writeSpecificMins) {
		TiXmlElement* atomicDistances = new TiXmlElement(messages->m_sxAtomicDistances.c_str());
		if (writeGeneralMin) {
			TiXmlElement* generalMin = new TiXmlElement(messages->m_sxMin.c_str());
			generalMin->SetDoubleAttribute(messages->m_sxValue.c_str(), *m_pfGeneralMinAtomicDistance);
			atomicDistances->LinkEndChild(generalMin);
		}
		if (writeSpecificMins) {
			TiXmlElement* min;
			for (std::map<unsigned int, std::map<unsigned int, FLOAT> >::iterator i = m_mapMinAtomicDistances.begin(); i != m_mapMinAtomicDistances.end(); i++)
				for (std::map<unsigned int, FLOAT>::iterator j = i->second.begin(); j != i->second.end(); j++)
					if (!m_pBase || j->second != m_pBase->m_rgMinAtomicDistances[i->first][j->first]) {
						min = new TiXmlElement(messages->m_sxMin.c_str());
						min->SetDoubleAttribute(messages->m_sxValue.c_str(), j->second);
						min->SetAttribute(messages->m_sxZ1.c_str(), i->first);
						min->SetAttribute(messages->m_sxZ2.c_str(), j->first);
						atomicDistances->LinkEndChild(min);
					}
		}
		if (writeGeneralMax) {
			TiXmlElement* generalMax = new TiXmlElement(messages->m_sxMax.c_str());
			generalMax->SetDoubleAttribute(messages->m_sxValue.c_str(), *m_pfGeneralMaxAtomicDistance);
			atomicDistances->LinkEndChild(generalMax);
		}
		pConstraintsElem->LinkEndChild(atomicDistances);
	}
	return true;
}

bool Constraints::specificMinDistNotInBase() {
	if (!m_rgMinAtomicDistances)
		return false;

	if (!m_pBase || !m_pBase->m_rgMinAtomicDistances)
		return true;

	for (std::map<unsigned int, std::map<unsigned int, FLOAT> >::iterator i = m_mapMinAtomicDistances.begin(); i != m_mapMinAtomicDistances.end(); i++)
		for (std::map<unsigned int, FLOAT>::iterator j = i->second.begin(); j != i->second.end(); j++)
			if (j->second != m_pBase->m_rgMinAtomicDistances[i->first][j->first])
				return true;

	return false;
}

void Constraints::copy(Constraints &other) {
	cleanUp();
	m_sName = other.m_sName;
	m_pBase = other.m_pBase;
	if (other.m_pfCubeLWH != NULL) {
		m_pfCubeLWH = new FLOAT;
		*m_pfCubeLWH = *other.m_pfCubeLWH;
	}
	
	if (other.m_pfGeneralMinAtomicDistance != NULL) {
		m_pfGeneralMinAtomicDistance = new FLOAT;
		*m_pfGeneralMinAtomicDistance = *other.m_pfGeneralMinAtomicDistance;
	}
	
	if (other.m_pfGeneralMaxAtomicDistance != NULL) {
		m_pfGeneralMaxAtomicDistance = new FLOAT;
		*m_pfGeneralMaxAtomicDistance = *other.m_pfGeneralMaxAtomicDistance;
	}
	
	if (other.m_rgMinAtomicDistances) {
		m_rgMinAtomicDistances = new MinDistArray[MIN_DIST_ARRAY_SIZE];
		memcpy(m_rgMinAtomicDistances, other.m_rgMinAtomicDistances, sizeof(FLOAT) * MIN_DIST_ARRAY_SIZE * MIN_DIST_ARRAY_SIZE);

		for (std::map<unsigned int, std::map<unsigned int, FLOAT> >::iterator i = other.m_mapMinAtomicDistances.begin(); i != other.m_mapMinAtomicDistances.end(); i++)
			for (std::map<unsigned int, FLOAT>::iterator j = i->second.begin(); j != i->second.end(); j++)
				m_mapMinAtomicDistances[i->first][j->first] = j->second;
	}
}

FLOAT Constraints::getSmallestMinDistance() {
	const static FLOAT start = 1000.0;
	FLOAT smallestMin = start;
	FLOAT min;
	for (std::map<unsigned int, std::map<unsigned int, FLOAT> >::iterator i = m_mapMinAtomicDistances.begin(); i != m_mapMinAtomicDistances.end(); i++)
		for (std::map<unsigned int, FLOAT>::iterator j = i->second.begin(); j != i->second.end(); j++) {
			min = j->second;
			if (min < smallestMin)
				smallestMin = min;
		}
	if (smallestMin == start)
		return 0;
	else
		return smallestMin;
}
