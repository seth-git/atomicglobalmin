
#include "constraints.h"

//const char*      Constraints::s_elementNames[]     = {"cube", "atomicDistances"};
const unsigned int Constraints::s_minOccurs[]        = {0     , 0                };

//const char*      Constraints::s_distElementNames[] = {"min"        , "max"};
const unsigned int Constraints::s_distMinOccurs[]    = {0            , 0    };
const unsigned int Constraints::s_distMaxOccurs[]    = {XSD_UNLIMITED, 1    };

//const char* Constraints::s_attributeNames[] = {"Value", "z1" , "z2"};
const bool  Constraints::s_required[]         = {true   , false, false };
const char* Constraints::s_defaultValues[]    = {""     , ""   , ""};

void Constraints::cleanUp() {
	unsigned int i;
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
		for (i = 1; i <= MAX_ATOMIC_NUMBERS; ++i) {
			delete[] m_rgMinAtomicDistances[i];
		}
		delete[] m_rgMinAtomicDistances;
		m_rgMinAtomicDistances = NULL;
	}
}

bool Constraints::load(TiXmlElement *pElem, const Strings* messages)
{
	const char* elementNames[] = {messages->m_sxCube.c_str(), messages->m_sxAtomicDistances.c_str()};
	const char* distElementNames[] = {messages->m_sxMin.c_str(), messages->m_sxMax.c_str()};
	XsdElementUtil constraintUtil(pElem->Value(), XSD_ALL, elementNames, s_minOccurs);
	TiXmlHandle handle(0);
	TiXmlElement** constraintElements;

	cleanUp();
	
	if (!XsdTypeUtil::readStrValueElement(pElem, m_sName, messages->m_sxName.c_str())) {
		return false;
	}

	handle=TiXmlHandle(pElem);
	if (!constraintUtil.process(handle)) {
		return false;
	}
	constraintElements = constraintUtil.getAllElements();

	if (constraintElements[0] != NULL) {
		m_pfCubeLWH = new FLOAT;
		XsdTypeUtil::readPosFloatValueElement(constraintElements[0], *m_pfCubeLWH, messages->m_sxSize.c_str());
	}

	if (constraintElements[1] != NULL) {
		XsdElementUtil distUtil(pElem->Value(), XSD_SEQUENCE, distElementNames, s_distMinOccurs, s_distMaxOccurs);
		TiXmlHandle handle(0);
		std::vector<TiXmlElement*>* distElements;
		unsigned int i;
		
		handle=TiXmlHandle(constraintElements[1]);
		if (!distUtil.process(handle)) {
			return false;
		}
		distElements = distUtil.getSequenceElements();
		for (i = 0; i < distElements[0].size(); ++i) {
			if (!addMinDist(distElements[0][i], messages)) {
				return false;
			}
		}
		for (i = 0; i < distElements[1].size(); ++i) { // Should be only one
			m_pfGeneralMaxAtomicDistance = new FLOAT;
			if (!XsdTypeUtil::readPosFloatValueElement(distElements[1][i], *m_pfGeneralMaxAtomicDistance, messages)) {
				return false;
			}
		}
	}
	
	return true;
}

bool Constraints::addMinDist(TiXmlElement *pElem, const Strings* messages)
{
	const char* attributeNames[] = {messages->m_sxValue.c_str(), messages->m_sxZ1.c_str() , messages->m_sxZ2.c_str()};
	const char** values;
	const Strings* messagesDL = Strings::instance();
	
	XsdAttributeUtil attUtil(pElem->Value(), attributeNames, s_required, s_defaultValues);
	if (!attUtil.process(pElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();
	
	if ((values[1] == NULL && values[2] != NULL) ||
	    (values[1] != NULL && values[2] == NULL)) {
		printf(messagesDL->m_sErrorZ1Z2.c_str(), pElem->Row(), pElem->Value(), attributeNames[1], attributeNames[2]);
		return false;
	}
	
	if (values[1] == NULL) {
		if (m_pfGeneralMinAtomicDistance != NULL) {
			printf(messagesDL->m_sErrorOneGeneralMin.c_str(), pElem->Row(), pElem->Value(), attributeNames[1], attributeNames[2]);
			return false;
		}
		m_pfGeneralMinAtomicDistance = new FLOAT;
		XsdTypeUtil::getPositiveFloat(values[0], *m_pfGeneralMinAtomicDistance, attributeNames[0], pElem);
	} else {
		unsigned int i, j;
		if (m_rgMinAtomicDistances == NULL) {
			m_rgMinAtomicDistances = new FLOAT*[MAX_ATOMIC_NUMBERS+1];
			m_rgMinAtomicDistances[0] = NULL;
			for (i = 1; i <= MAX_ATOMIC_NUMBERS; ++i) {
				m_rgMinAtomicDistances[i] = new FLOAT[MAX_ATOMIC_NUMBERS+1];
				for (j = 1; j <= MAX_ATOMIC_NUMBERS; ++j) {
					m_rgMinAtomicDistances[i][j] = -1;
				}
			}
		}
		if (!XsdTypeUtil::getPositiveInt(values[1], i, attributeNames[1], pElem)) {
			return false;
		}
		if (i > MAX_ATOMIC_NUMBERS) {
			printf(messagesDL->m_sErrorAtomicNumOverMax.c_str(), pElem->Row(), attributeNames[1], pElem->Value(), i, MAX_ATOMIC_NUMBERS);
			return false;
		}
		if (!XsdTypeUtil::getPositiveInt(values[2], j, attributeNames[2], pElem)) {
			return false;
		}
		if (j > MAX_ATOMIC_NUMBERS) {
			printf(messagesDL->m_sErrorAtomicNumOverMax.c_str(), pElem->Row(), attributeNames[2], pElem->Value(), j, MAX_ATOMIC_NUMBERS);
			return false;
		}
		if (m_rgMinAtomicDistances[i][j] != -1) {
			printf(messagesDL->m_sErrorDuplicateMinDist.c_str(), pElem->Row(), pElem->Value(), attributeNames[1], i, attributeNames[2], j);
			return false;
		}
		if (!XsdTypeUtil::getPositiveFloat(values[0], m_rgMinAtomicDistances[i][j], attributeNames[0], pElem)) {
			return false;
		}
		m_rgMinAtomicDistances[j][i] = m_rgMinAtomicDistances[i][j];
	}
	return true;
}

void Constraints::save(const Strings* messages)
{
//  These are commented out to prevent a compiler warning.  They are needed.
//	const char* elementNames[] = {messages->m_sxCube.c_str(), messages->m_sxAtomicDistances.c_str()};
//	const char* distElementNames[] = {messages->m_sxMin.c_str(), messages->m_sxMax.c_str()};
//	const char* attributeNames[] = {messages->m_sxValue.c_str(), messages->m_sxZ1.c_str() , messages->m_sxZ2.c_str()};
}
