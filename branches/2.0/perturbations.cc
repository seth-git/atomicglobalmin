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
	clear();
}

void Perturbations::clear()
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

bool Perturbations::loadSetup(const rapidxml::xml_node<>* pPerturbationsElem, const Strings* messages)
{
	using namespace rapidxml;
	clear();
	const char* perturbationsElemNames[] = {messages->m_sxTranslationVector.c_str(), messages->m_sxRotationAngle.c_str()};
	XsdElementUtil perturbationsElemUtil(XSD_ALL, perturbationsElemNames, s_perturbationsMinOccurs);
	if (!perturbationsElemUtil.process(pPerturbationsElem))
		return false;
	const xml_node<>** perturbationsElements = perturbationsElemUtil.getAllElements();

	if (NULL != perturbationsElements[0]) {
		const char* translationVectorAttNames[] = {messages->m_sxStartLength.c_str(), messages->m_sxMinLength.c_str(), messages->m_sxProbability.c_str()};
		XsdAttributeUtil translationVectorAttUtil(translationVectorAttNames, s_translationVectorAttReq, s_translationVectorAttDef);
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
		XsdAttributeUtil rotationAngleAttUtil(rotationAngleAttNames, s_rotationAngleAttReq, s_rotationAngleAttDef);
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
		printf(messages->m_sProbabilityMustTotalOne.c_str(), pPerturbationsElem->name());
		return false;
	}

	if (!XsdTypeUtil::read1PosIntAtt(pPerturbationsElem, m_iStartingPerturbationsPerIteration, messages->m_sxNumberPerIteration.c_str(), true, "1"))
		return false;

	return true;
}

bool Perturbations::loadDefaults(unsigned int iStructures, bool bMoleculesPresent, Constraints* pConstraints)
{
	clear();

	m_pfTranslationVectorStartLength = new FLOAT;
	m_pfTranslationVectorMinLength = new FLOAT;
	m_pfTranslationVectorProbability = new FLOAT;

	const static FLOAT defaultStartTranslationVector = 0.4;
	if (pConstraints == NULL)
		*m_pfTranslationVectorStartLength = defaultStartTranslationVector;
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

bool Perturbations::saveSetup(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pParentElem, const Strings* messages)
{
	using namespace rapidxml;
	xml_node<>* perturbations = doc.allocate_node(node_element, messages->m_sxPerturbations.c_str());
	pParentElem->append_node(perturbations);

	if (m_pfTranslationVectorStartLength != NULL) {
		xml_node<>* translationVector = doc.allocate_node(node_element, messages->m_sxTranslationVector.c_str());
		perturbations->append_node(translationVector);
		XsdTypeUtil::setAttribute(doc, translationVector, messages->m_sxStartLength.c_str(), *m_pfTranslationVectorStartLength);
		if (*m_pfTranslationVectorMinLength != 0)
			XsdTypeUtil::setAttribute(doc, translationVector, messages->m_sxMinLength.c_str(), *m_pfTranslationVectorMinLength);
		XsdTypeUtil::setAttribute(doc, translationVector, messages->m_sxProbability.c_str(), *m_pfTranslationVectorProbability);
	}

	if (m_pfRotationStartRadians != NULL) {
		xml_node<>* rotationAngle = doc.allocate_node(node_element, messages->m_sxRotationAngle.c_str());
		perturbations->append_node(rotationAngle);
		XsdTypeUtil::setAttribute(doc, rotationAngle, messages->m_sxStartDegrees.c_str(), *m_pfRotationStartRadians * RAD_TO_DEG);
		if (*m_pfRotationMinRadians != 0)
			XsdTypeUtil::setAttribute(doc, rotationAngle, messages->m_sxMinDegrees.c_str(), *m_pfRotationMinRadians * RAD_TO_DEG);
		XsdTypeUtil::setAttribute(doc, rotationAngle, messages->m_sxProbability.c_str(), *m_pfRotationProbability);
	}

	if (m_iStartingPerturbationsPerIteration != 1)
		XsdTypeUtil::setAttribute(doc, perturbations, messages->m_sxNumberPerIteration.c_str(), m_iStartingPerturbationsPerIteration);

	return true;
}
