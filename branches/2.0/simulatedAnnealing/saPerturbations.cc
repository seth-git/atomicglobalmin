#include "saPerturbations.h"

SAPerturbations::SAPerturbations()
{
}

const unsigned int SAPerturbations::s_perturbationsMinOccurs[] = {0,0};

const bool SAPerturbations::s_translationVectorAttReq[] = {true,true,true};
const char* SAPerturbations::s_translationVectorAttDef[] = {"", "0", ""};

const bool SAPerturbations::s_rotationAngleAttReq[] = {true,true,true};
const char* SAPerturbations::s_rotationAngleAttDef[] = {"", "0", ""};

bool SAPerturbations::loadSetup(const rapidxml::xml_node<>* pPerturbationsElem, const Strings* messages)
{
	using namespace rapidxml;
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

		if (!XsdTypeUtil::getPositiveFloat(translationVectorAttValues[0], m_fTranslationVectorStartLength, translationVectorAttNames[0], perturbationsElements[0]))
			return false;

		if (!XsdTypeUtil::getNonNegativeFloat(translationVectorAttValues[1], m_fTranslationVectorMinLength, translationVectorAttNames[1], perturbationsElements[0]))
			return false;

		if (!XsdTypeUtil::getPositiveFloat(translationVectorAttValues[2], m_fTranslationVectorProbability, translationVectorAttNames[2], perturbationsElements[0]))
			return false;
		if (!XsdTypeUtil::inRange(m_fTranslationVectorProbability, 0, 1, perturbationsElements[0], translationVectorAttNames[2]))
			return false;
	} else {
		m_fTranslationVectorProbability = 0;
	}

	if (NULL != perturbationsElements[1]) {
		const char* rotationAngleAttNames[] = {messages->m_sxStartDegrees.c_str(), messages->m_sxMinDegrees.c_str(), messages->m_sxProbability.c_str()};
		XsdAttributeUtil rotationAngleAttUtil(rotationAngleAttNames, s_rotationAngleAttReq, s_rotationAngleAttDef);
		if (!rotationAngleAttUtil.process(perturbationsElements[1]))
			return false;
		const char** rotationAngleAttValues = rotationAngleAttUtil.getAllAttributes();

		if (!XsdTypeUtil::getPositiveFloat(rotationAngleAttValues[0], m_fRotationStartRadians, rotationAngleAttNames[0], perturbationsElements[1]))
			return false;
		if (!XsdTypeUtil::inRange(m_fRotationStartRadians, 0, 360, perturbationsElements[1], rotationAngleAttNames[0]))
			return false;
		m_fRotationStartRadians *= DEG_TO_RAD;

		if (!XsdTypeUtil::getNonNegativeFloat(rotationAngleAttValues[1], m_fRotationMinRadians, rotationAngleAttNames[1], perturbationsElements[1]))
			return false;
		if (!XsdTypeUtil::inRange(m_fRotationMinRadians, 0, 360, perturbationsElements[1], rotationAngleAttNames[1]))
			return false;
		m_fRotationMinRadians *= DEG_TO_RAD;

		if (!XsdTypeUtil::getPositiveFloat(rotationAngleAttValues[2], m_fRotationProbability, rotationAngleAttNames[2], perturbationsElements[1]))
			return false;
		if (!XsdTypeUtil::inRange(m_fRotationProbability, 0, 1, perturbationsElements[1], rotationAngleAttNames[2]))
			return false;
	} else {
		m_fRotationProbability = 0;
	}

	FLOAT fTotalProbability = 0;
	fTotalProbability += m_fTranslationVectorProbability;
	fTotalProbability += m_fRotationProbability;
	if (fTotalProbability != 1) {
		printf(messages->m_sProbabilityMustTotalOne.c_str(), pPerturbationsElem->name());
		return false;
	}

	if (!XsdTypeUtil::read1PosIntAtt(pPerturbationsElem, m_iStartingPerturbationsPerIteration, messages->m_sxNumberPerIteration.c_str(), true, "1"))
		return false;

	return true;
}

bool SAPerturbations::loadDefaults(unsigned int iStructures, bool bMoleculesPresent, Constraints* pConstraints)
{
	const static FLOAT defaultStartTranslationVector = 0.4;
	if (pConstraints == NULL)
		m_fTranslationVectorStartLength = defaultStartTranslationVector;
	else {
		m_fTranslationVectorStartLength = pConstraints->getSmallestMinDistance();

		if (m_fTranslationVectorStartLength > 0)
			m_fTranslationVectorStartLength *= 0.5;
		else
			m_fTranslationVectorStartLength = defaultStartTranslationVector;
	}

	m_fTranslationVectorMinLength = m_fTranslationVectorStartLength * 0.1;
	if (m_fTranslationVectorMinLength < 0.1)
		m_fTranslationVectorMinLength = 0.1;

	if (!bMoleculesPresent) {
		m_fTranslationVectorProbability = 1;
	} else {
		m_fTranslationVectorProbability = 0.5;
		m_fRotationProbability = 0.5;

		m_fRotationStartRadians = 40.0 * DEG_TO_RAD;
		m_fRotationMinRadians = 4.0 * DEG_TO_RAD;
	}

	m_iStartingPerturbationsPerIteration = (iStructures * (((unsigned int)bMoleculesPresent)+1)) / 3;
	if (m_iStartingPerturbationsPerIteration < 1)
		m_iStartingPerturbationsPerIteration = 1;

	return true;
}

bool SAPerturbations::saveSetup(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pParentElem, const Strings* messages)
{
	using namespace rapidxml;
	xml_node<>* perturbations = doc.allocate_node(node_element, messages->m_sxPerturbations.c_str());
	pParentElem->append_node(perturbations);

	if (m_fTranslationVectorProbability != 0) {
		xml_node<>* translationVector = doc.allocate_node(node_element, messages->m_sxTranslationVector.c_str());
		perturbations->append_node(translationVector);
		XsdTypeUtil::setAttribute(doc, translationVector, messages->m_sxStartLength.c_str(), m_fTranslationVectorStartLength);
		if (m_fTranslationVectorMinLength != 0)
			XsdTypeUtil::setAttribute(doc, translationVector, messages->m_sxMinLength.c_str(), m_fTranslationVectorMinLength);
		XsdTypeUtil::setAttribute(doc, translationVector, messages->m_sxProbability.c_str(), m_fTranslationVectorProbability);
	}

	if (m_fRotationProbability != 0) {
		xml_node<>* rotationAngle = doc.allocate_node(node_element, messages->m_sxRotationAngle.c_str());
		perturbations->append_node(rotationAngle);
		XsdTypeUtil::setAttribute(doc, rotationAngle, messages->m_sxStartDegrees.c_str(), m_fRotationStartRadians * RAD_TO_DEG);
		if (m_fRotationMinRadians != 0)
			XsdTypeUtil::setAttribute(doc, rotationAngle, messages->m_sxMinDegrees.c_str(), m_fRotationMinRadians * RAD_TO_DEG);
		XsdTypeUtil::setAttribute(doc, rotationAngle, messages->m_sxProbability.c_str(), m_fRotationProbability);
	}

	if (m_iStartingPerturbationsPerIteration != 1)
		XsdTypeUtil::setAttribute(doc, perturbations, messages->m_sxNumberPerIteration.c_str(), m_iStartingPerturbationsPerIteration);

	return true;
}
