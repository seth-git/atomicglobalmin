#include "saPerturbations.h"
#include <cmath>

SAPerturbations::SAPerturbations()
{
}

const unsigned int SAPerturbations::s_perturbationsMinOccurs[] = {0,0};

const bool SAPerturbations::s_translationVectorAttReq[] = {true,true,true};
const char* SAPerturbations::s_translationVectorAttDef[] = {"", "0", ""};

const bool SAPerturbations::s_rotationAngleAttReq[] = {true,true,true};
const char* SAPerturbations::s_rotationAngleAttDef[] = {"", "0", ""};

bool SAPerturbations::loadSetup(const rapidxml::xml_node<>* pPerturbationsElem)
{
	using namespace rapidxml;
	using namespace strings;
	const char* perturbationsElemNames[] = {xTranslationVector, xRotationAngle};
	XsdElementUtil perturbationsElemUtil(XSD_ALL, perturbationsElemNames, s_perturbationsMinOccurs);
	if (!perturbationsElemUtil.process(pPerturbationsElem))
		return false;
	const xml_node<>** perturbationsElements = perturbationsElemUtil.getAllElements();

	if (NULL != perturbationsElements[0]) {
		const char* translationVectorAttNames[] = {xStartLength, xMinLength, xProbability};
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
		const char* rotationAngleAttNames[] = {xStartDegrees, xMinDegrees, xProbability};
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
		printf(ProbabilityMustTotalOne, pPerturbationsElem->name());
		return false;
	}

	if (!XsdTypeUtil::read1PosIntAtt(pPerturbationsElem, m_iStartingPerturbationsPerIteration, xNumberPerIteration, true, "1"))
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

bool SAPerturbations::saveSetup(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pParentElem)
{
	using namespace rapidxml;
	using namespace strings;
	xml_node<>* perturbations = doc.allocate_node(node_element, xPerturbations);
	pParentElem->append_node(perturbations);

	if (m_fTranslationVectorProbability != 0) {
		xml_node<>* translationVector = doc.allocate_node(node_element, xTranslationVector);
		perturbations->append_node(translationVector);
		XsdTypeUtil::setAttribute(doc, translationVector, xStartLength, m_fTranslationVectorStartLength);
		if (m_fTranslationVectorMinLength != 0)
			XsdTypeUtil::setAttribute(doc, translationVector, xMinLength, m_fTranslationVectorMinLength);
		XsdTypeUtil::setAttribute(doc, translationVector, xProbability, m_fTranslationVectorProbability);
	}

	if (m_fRotationProbability != 0) {
		xml_node<>* rotationAngle = doc.allocate_node(node_element, xRotationAngle);
		perturbations->append_node(rotationAngle);
		XsdTypeUtil::setAttribute(doc, rotationAngle, xStartDegrees, m_fRotationStartRadians * RAD_TO_DEG);
		if (m_fRotationMinRadians != 0)
			XsdTypeUtil::setAttribute(doc, rotationAngle, xMinDegrees, m_fRotationMinRadians * RAD_TO_DEG);
		XsdTypeUtil::setAttribute(doc, rotationAngle, xProbability, m_fRotationProbability);
	}

	if (m_iStartingPerturbationsPerIteration != 1)
		XsdTypeUtil::setAttribute(doc, perturbations, xNumberPerIteration, m_iStartingPerturbationsPerIteration);

	return true;
}

const unsigned int SAPerturbations::s_maxPerturbationTries = 500;

bool SAPerturbations::perturb(const Constraints &constraints,
		FLOAT fTranslationVectorLength, FLOAT fRotationRadians,
		Structure &structure) const {
	Structure original;
	FLOAT random;
	unsigned int count = 0;

	original.copy(structure);
	do {
		try {
			random = Random::getFloat(0,1);
			if (random <= m_fTranslationVectorProbability) {
				if (!translate(fTranslationVectorLength, structure))
					return false;
				if (constraints.validate(structure))
					return true;
				else
					throw 1;
			} else
				random -= m_fTranslationVectorProbability;
			if (random <= m_fRotationProbability) {
				if (!rotate(fRotationRadians, structure))
					return false;
				if (constraints.validate(structure))
					return true;
				else
					throw 2;
			} else
				random -= m_fRotationProbability;
		} catch (unsigned int e) {
			structure.copy(original);
			++count;
		}
	} while (count < s_maxPerturbationTries);

	return false;
}

bool SAPerturbations::translate(FLOAT fVectorLength, Structure &structure) {
	unsigned int i;
	unsigned int iNumberOfAtomGroups = structure.getNumberOfAtomGroups();
	const AtomGroup* atomGroups = structure.getAtomGroups();
	std::vector<unsigned int> translatable(iNumberOfAtomGroups);

	for (i = 0; i < iNumberOfAtomGroups; ++i)
		if (!atomGroups[i].getFrozen())
			translatable.push_back(i);
	if (0 == translatable.size()) {
		printf("Error: There are no translatable atom groups in SAPerturbations::translate.");
		return false;
	}

	AtomGroup* atomGroup = structure.getAtomGroup(translatable[Random::getInt(0, translatable.size()-1)]);
	FLOAT angleX = Random::getFloat(-PIE_OVER_2,PIE_OVER_2);
	FLOAT angleY = Random::getFloat(0,PIE_X_2);
	COORDINATE4 vector;
	StructuresTemplate::getVectorInDirection(angleX, angleY, fVectorLength, vector);
	const FLOAT* center = atomGroup->getCenter();
	COORDINATE3 newCenter = {
		center[0] + vector[0],
		center[1] + vector[1],
		center[2] + vector[2]
	};
	atomGroup->setCenter(newCenter);
	atomGroup->initRotationMatrix();
	atomGroup->localToGlobal();
	structure.updateAtomDistanceMatrix();
	return true;
}

bool SAPerturbations::rotate(FLOAT fRadianAngle, Structure &structure) {
	unsigned int i;
	unsigned int iNumberOfAtomGroups = structure.getNumberOfAtomGroups();
	const AtomGroup* atomGroups = structure.getAtomGroups();
	std::vector<unsigned int> rotatable(iNumberOfAtomGroups);

	for (i = 0; i < iNumberOfAtomGroups; ++i)
		if (atomGroups[i].getNumberOfAtoms() > 1 && !atomGroups[i].getFrozen())
			rotatable.push_back(i);
	if (0 == rotatable.size()) {
		printf("Error: There are no rotatable atom groups in SAPerturbations::rotate. Exiting.");
		return false;
	}

	AtomGroup* atomGroup = structure.getAtomGroup(rotatable[Random::getInt(0, rotatable.size()-1)]);
	FLOAT angles[3];
	memcpy(angles, atomGroup->getAngles(), SIZEOF_COORDINATE3);
	i = Random::getInt(0,2);
	if (Random::getInt(0,1))
		angles[i] -= fRadianAngle;
	else
		angles[i] += fRadianAngle;
	if (1 == i) {
		if (std::abs(angles[i]) > PIE_OVER_2) {
			if (angles[i] > 0)
				angles[i] = PIE_OVER_2 - (angles[i] - PIE_OVER_2);
			else
				angles[i] = -PIE_OVER_2 + (-PIE_OVER_2 - angles[i]);
			angles[0] += PIE;
			if (angles[0] >= PIE_X_2)
				angles[0] -= PIE_X_2;
			angles[2] += PIE;
			if (angles[2] >= PIE_X_2)
				angles[2] -= PIE_X_2;
		}
	} else {
		if (angles[i] < 0)
			angles[i] += PIE_X_2;
		else if (angles[i] >= PIE_X_2)
			angles[i] -= PIE_X_2;
	}
	atomGroup->setAngles(angles);
	atomGroup->initRotationMatrix();
	atomGroup->localToGlobal();
	structure.updateAtomDistanceMatrix();
	return true;
}


