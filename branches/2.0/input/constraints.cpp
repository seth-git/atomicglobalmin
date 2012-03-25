
#include "constraints.h"

bool Constraints::load(TiXmlElement *pElem)
{
	static const std::string elementNames[] = {"cube", "atomicDistances"};
	static const unsigned int   minOccurs[] = {0     , 0                };
	XsdElementUtil constraintUtil(pElem->Value(), XSD_ALL, elementNames, 2, minOccurs, NULL);
	TiXmlHandle handle(0);
	TiXmlElement** constraintElements;

	cleanUp();

	static const std::string name = "name";
	if (!XsdTypeUtil::readStrValueElement(pElem, m_sName, &name)) {
		return false;
	}

	handle=TiXmlHandle(pElem);
	if (!constraintUtil.process(handle)) {
		return false;
	}
	constraintElements = constraintUtil.getAllElements();

	static const std::string size = "size";
	if (constraintElements[0] != NULL) {
		m_pfCubeLWH = new FLOAT;
		XsdTypeUtil::readPosFloatValueElement(constraintElements[0], *m_pfCubeLWH, &size);
	}

	if (constraintElements[1] != NULL) {
		static const std::string distElementNames[] = {"min"        , "max"};
		static const unsigned int   distMinOccurs[] = {0            , 0    };
		static const unsigned int   distMaxOccurs[] = {XSD_UNLIMITED, 1    };
		XsdElementUtil distUtil(pElem->Value(), XSD_SEQUENCE, distElementNames, 2, distMinOccurs, distMaxOccurs);
		TiXmlHandle handle(0);
		vector<TiXmlElement*>* distElements;
		unsigned int i;
		
		handle=TiXmlHandle(constraintElements[1]);
		if (!distUtil.process(handle)) {
			return false;
		}
		distElements = distUtil.getSequenceElements();
		for (i = 0; i < distElements[0].size(); ++i) {
			if (!addMinDist(distElements[0][i])) {
				return false;
			}
		}
		for (i = 0; i < distElements[1].size(); ++i) { // Should be only one
			m_pfGeneralMaxAtomicDistance = new FLOAT;
			if (!XsdTypeUtil::readPosFloatValueElement(distElements[1][i], *m_pfGeneralMaxAtomicDistance)) {
				return false;
			}
		}
	}
	
	return true;
}

bool Constraints::addMinDist(TiXmlElement *pElem)
{
	static const std::string attributeNames[] = {"value", "z1" , "z2"};
	static const bool        required[]       = {true   , false, false };
	static const std::string defaultValues[]  = {""     , ""   , ""};
	const char** values;
	
	XsdAttributeUtil attUtil(pElem->Value(), attributeNames, 3, required, defaultValues);
	if (!attUtil.process(pElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();

	if ((values[1] == NULL && values[2] != NULL) ||
	    (values[1] != NULL && values[2] == NULL)) {
		printf("In the element '%s', '%s' and '%s' must both be included or both must be excluded.\n", pElem->Value(), attributeNames[1].c_str(), attributeNames[2].c_str());
		return false;
	}

	if (values[1] == NULL) {
		if (m_pfGeneralMinAtomicDistance != NULL) {
			printf("Only one element '%s' can be a general minimum distance, not having '%s' and '%s'.\n", pElem->Value(), attributeNames[1].c_str(), attributeNames[2].c_str());
			return false;
		}
		m_pfGeneralMinAtomicDistance = new FLOAT;
		XsdTypeUtil::getPositiveFloat(values[0], *m_pfGeneralMinAtomicDistance, attributeNames[0].c_str(), pElem->Value());
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
		if (!XsdTypeUtil::getPositiveInt(values[1], i, attributeNames[1].c_str(), pElem->Value())) {
			return false;
		}
		if (i > MAX_ATOMIC_NUMBERS) {
			printf("In a constraint, an atomic number '%s' in the element '%s' is %u, which is greater than the maximum of %u.\n",
			       attributeNames[1].c_str(), pElem->Value(), i, MAX_ATOMIC_NUMBERS);
			return false;
		}
		if (!XsdTypeUtil::getPositiveInt(values[2], j, attributeNames[2].c_str(), pElem->Value())) {
			return false;
		}
		if (j > MAX_ATOMIC_NUMBERS) {
			printf("In a constraint, an atomic number '%s' in the element '%s' is %u, which is greater than the maximum of %u.\n",
			       attributeNames[2].c_str(), pElem->Value(), j, MAX_ATOMIC_NUMBERS);
			return false;
		}
		if (m_rgMinAtomicDistances[i][j] != -1) {
			printf("In a constraint, the minimum distance('%s') between %s=%u and %s=%u has already been set.\n",
			       pElem->Value(), attributeNames[1].c_str(), i, attributeNames[2].c_str(), j);
			return false;
		}
		if (!XsdTypeUtil::getPositiveFloat(values[0], m_rgMinAtomicDistances[i][j], attributeNames[0].c_str(), pElem->Value())) {
			return false;
		}
		m_rgMinAtomicDistances[j][i] = m_rgMinAtomicDistances[i][j];
	}
	return true;
}

void Constraints::save()
{
}
