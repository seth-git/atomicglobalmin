#include "perturbations.h"

Perturbations::Perturbations()
{
	m_pfTranslationVectorStartLength = NULL;
	m_pfTranslationVectorMinLength = NULL;
	m_pfTranslationVectorLength = NULL;
	m_pfTranslationVectorProbability = NULL;

	m_pfRotationStartRadians = NULL;
	m_pfRotationMinRadians = NULL;
	m_pfRotationRadians = NULL;
	m_pfRotationProbability = NULL;
}

Perturbations::~Perturbations()
{
	cleanUp();
}

void Perturbations::cleanUp()
{
	if (m_pfTranslationVectorStartLength != NULL) {
		delete m_pfTranslationVectorStartLength;
		m_pfTranslationVectorStartLength = NULL;
	}
	if (m_pfTranslationVectorMinLength != NULL) {
		delete m_pfTranslationVectorMinLength;
		m_pfTranslationVectorMinLength = NULL;
	}
	if (m_pfTranslationVectorLength != NULL) {
		delete m_pfTranslationVectorLength;
		m_pfTranslationVectorLength = NULL;
	}
	if (m_pfTranslationVectorProbability != NULL) {
		delete m_pfTranslationVectorProbability;
		m_pfTranslationVectorProbability = NULL;
	}

	if (m_pfRotationStartRadians != NULL) {
		delete m_pfRotationStartRadians;
		m_pfRotationStartRadians = NULL;
	}
	if (m_pfRotationMinRadians != NULL) {
		delete m_pfRotationMinRadians;
		m_pfRotationMinRadians = NULL;
	}
	if (m_pfRotationRadians != NULL) {
		delete m_pfRotationRadians;
		m_pfRotationRadians = NULL;
	}
	if (m_pfRotationProbability != NULL) {
		delete m_pfRotationProbability;
		m_pfRotationProbability = NULL;
	}
}

const unsigned int Perturbations::s_perturbationsMinOccurs[] = {0,0};

const bool Perturbations::s_translationVectorAttReq[] = {true,true,true};
const char* Perturbations::s_translationVectorAttDef[] = {"", "0", ""};

const bool Perturbations::s_rotationAngleAttReq[] = {true,true,true};
const char* Perturbations::s_rotationAngleAttDef[] = {"", "0", ""};

bool Perturbations::loadSetup(TiXmlElement *pPerturbationsElem, const Strings* messages)
{
	cleanUp();
	TiXmlHandle hPerturbations(0);
	hPerturbations=TiXmlHandle(pPerturbationsElem);
	const char* perturbationsElemNames[] = {messages->m_sxTranslationVector.c_str(), messages->m_sxRotationAngle.c_str()};
	XsdElementUtil perturbationsElemUtil(pPerturbationsElem->Value(), XSD_ALL, perturbationsElemNames, s_perturbationsMinOccurs);
	if (!perturbationsElemUtil.process(hPerturbations))
		return false;
	TiXmlElement** perturbationsElements = perturbationsElemUtil.getAllElements();

	if (NULL != perturbationsElements[0]) {
		const char* translationVectorAttNames[] = {messages->m_sxStartLength.c_str(), messages->m_sxMinLength.c_str(), messages->m_sxProbability.c_str()};
		XsdAttributeUtil translationVectorAttUtil(perturbationsElements[0]->Value(), translationVectorAttNames, s_translationVectorAttReq, s_translationVectorAttDef);
		if (!translationVectorAttUtil.process(perturbationsElements[0]))
			return false;
		const char** translationVectorAttValues = translationVectorAttUtil.getAllAttributes();

		m_pfTranslationVectorStartLength = new FLOAT;
		if (!XsdTypeUtil::getPositiveFloat(translationVectorAttValues[0], *m_pfTranslationVectorStartLength, translationVectorAttNames[0], perturbationsElements[0]))
			return false;

		m_pfTranslationVectorMinLength = new FLOAT;
		if (!XsdTypeUtil::getNonNegativeFloat(translationVectorAttValues[1], *m_pfTranslationVectorMinLength, translationVectorAttNames[1], perturbationsElements[0]))
			return false;

		m_pfTranslationVectorProbability = new FLOAT;
		if (!XsdTypeUtil::getPositiveFloat(translationVectorAttValues[2], *m_pfTranslationVectorProbability, translationVectorAttNames[2], perturbationsElements[0]))
			return false;
		if (!XsdTypeUtil::inRange(*m_pfTranslationVectorProbability, 0, 1, perturbationsElements[0], translationVectorAttNames[2]))
			return false;
	}

	if (NULL != perturbationsElements[1]) {
		const char* rotationAngleAttNames[] = {messages->m_sxStartDegrees.c_str(), messages->m_sxMinDegrees.c_str(), messages->m_sxProbability.c_str()};
		XsdAttributeUtil rotationAngleAttUtil(perturbationsElements[1]->Value(), rotationAngleAttNames, s_rotationAngleAttReq, s_rotationAngleAttDef);
		if (!rotationAngleAttUtil.process(perturbationsElements[1]))
			return false;
		const char** rotationAngleAttValues = rotationAngleAttUtil.getAllAttributes();

		m_pfRotationStartRadians = new FLOAT;
		if (!XsdTypeUtil::getPositiveFloat(rotationAngleAttValues[0], *m_pfRotationStartRadians, rotationAngleAttNames[0], perturbationsElements[1]))
			return false;
		if (!XsdTypeUtil::inRange(*m_pfRotationStartRadians, 0, 360, perturbationsElements[1], rotationAngleAttNames[0]))
			return false;
		*m_pfRotationStartRadians *= DEG_TO_RAD;

		m_pfRotationMinRadians = new FLOAT;
		if (!XsdTypeUtil::getNonNegativeFloat(rotationAngleAttValues[1], *m_pfRotationMinRadians, rotationAngleAttNames[1], perturbationsElements[1]))
			return false;
		if (!XsdTypeUtil::inRange(*m_pfRotationMinRadians, 0, 360, perturbationsElements[1], rotationAngleAttNames[1]))
			return false;
		*m_pfRotationMinRadians *= DEG_TO_RAD;

		m_pfRotationProbability = new FLOAT;
		if (!XsdTypeUtil::getPositiveFloat(rotationAngleAttValues[2], *m_pfRotationProbability, rotationAngleAttNames[2], perturbationsElements[1]))
			return false;
		if (!XsdTypeUtil::inRange(*m_pfRotationProbability, 0, 1, perturbationsElements[1], rotationAngleAttNames[2]))
			return false;
	}

	FLOAT fTotalProbability = 0;
	if (m_pfTranslationVectorProbability != NULL)
		fTotalProbability += *m_pfTranslationVectorProbability;
	if (m_pfRotationProbability != NULL)
		fTotalProbability += *m_pfRotationProbability;
	if (fTotalProbability != 1) {
		printf(messages->m_sProbabilityMustTotalOne.c_str(), pPerturbationsElem->Row());
		return false;
	}

	if (!XsdTypeUtil::readPosIntValueElement(pPerturbationsElem, m_iStartingPerturbationsPerIteration, messages->m_sxNumberPerIteration.c_str(), "1"))
		return false;

	return true;
}

bool Perturbations::loadDefaults(unsigned int iStructures, bool bMoleculesPresent, Constraints* pConstraints)
{
	cleanUp();

	m_pfTranslationVectorStartLength = new FLOAT;
	m_pfTranslationVectorMinLength = new FLOAT;
	m_pfTranslationVectorProbability = new FLOAT;

	const static FLOAT defaultStartTranslationVector = 0.4;
	if (pConstraints == NULL)
		*m_pfTranslationVectorStartLength = defaultStartTranslationVector;
	else if (pConstraints->m_pfGeneralMinAtomicDistance != NULL)
		*m_pfTranslationVectorStartLength = *(pConstraints->m_pfGeneralMinAtomicDistance) * 0.5;
	else {
		*m_pfTranslationVectorStartLength = pConstraints->getSmallestMinDistance();

		if (*m_pfTranslationVectorStartLength > 0)
			*m_pfTranslationVectorStartLength *= 0.5;
		else
			*m_pfTranslationVectorStartLength = defaultStartTranslationVector;
	}

	*m_pfTranslationVectorMinLength = *m_pfTranslationVectorStartLength * 0.1;
	if (*m_pfTranslationVectorMinLength < 0.1)
		*m_pfTranslationVectorMinLength = 0.1;

	if (!bMoleculesPresent) {
		*m_pfTranslationVectorProbability = 1;
	} else {
		m_pfRotationStartRadians = new FLOAT;
		m_pfRotationMinRadians = new FLOAT;
		m_pfRotationProbability = new FLOAT;

		*m_pfTranslationVectorProbability = 0.5;
		*m_pfRotationProbability = 0.5;

		*m_pfRotationStartRadians = 40.0 * DEG_TO_RAD;
		*m_pfRotationMinRadians = 4.0 * DEG_TO_RAD;
	}

	m_iStartingPerturbationsPerIteration = (iStructures * (((unsigned int)bMoleculesPresent)+1)) / 3;
	if (m_iStartingPerturbationsPerIteration < 1)
		m_iStartingPerturbationsPerIteration = 1;

	return true;
}

bool Perturbations::saveSetup(TiXmlElement *pParentElem, const Strings* messages)
{
	TiXmlElement* perturbations = new TiXmlElement(messages->m_sxPerturbations.c_str());
	pParentElem->LinkEndChild(perturbations);

	if (m_pfTranslationVectorStartLength != NULL) {
		TiXmlElement* translationVector = new TiXmlElement(messages->m_sxTranslationVector.c_str());
		perturbations->LinkEndChild(translationVector);
		translationVector->SetDoubleAttribute(messages->m_sxStartLength.c_str(), *m_pfTranslationVectorStartLength);
		if (*m_pfTranslationVectorMinLength != 0) {
			translationVector->SetDoubleAttribute(messages->m_sxMinLength.c_str(), *m_pfTranslationVectorMinLength);
		}
		translationVector->SetDoubleAttribute(messages->m_sxProbability.c_str(), *m_pfTranslationVectorProbability);
	}

	if (m_pfRotationStartRadians != NULL) {
		TiXmlElement* rotationAngle = new TiXmlElement(messages->m_sxRotationAngle.c_str());
		perturbations->LinkEndChild(rotationAngle);
		rotationAngle->SetDoubleAttribute(messages->m_sxStartDegrees.c_str(), *m_pfRotationStartRadians * RAD_TO_DEG);
		if (*m_pfRotationMinRadians != 0) {
			rotationAngle->SetDoubleAttribute(messages->m_sxMinDegrees.c_str(), *m_pfRotationMinRadians * RAD_TO_DEG);
		}
		rotationAngle->SetDoubleAttribute(messages->m_sxProbability.c_str(), *m_pfRotationProbability);
	}

	if (m_iStartingPerturbationsPerIteration != 1)
		perturbations->SetAttribute(messages->m_sxNumberPerIteration.c_str(), m_iStartingPerturbationsPerIteration);

	return true;
}
