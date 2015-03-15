
#ifndef __PERTURBATIONS_H_
#define __PERTURBATIONS_H_

#include "../xsd/xsdAttributeUtil.h"
#include "../xsd/xsdElementUtil.h"
#include "../xsd/xsdTypeUtil.h"
#include "../translation/strings.h"
#include "../constraints.h"
#include "../structure.h"
#include "../structuresTemplate.h"
#include "../random/random.h"

class SAPerturbations {
public:
	unsigned int m_iStartingPerturbationsPerIteration;

	FLOAT m_fTranslationVectorStartLength;
	FLOAT m_fTranslationVectorMinLength;
	FLOAT m_fTranslationVectorProbability;

	FLOAT m_fRotationStartRadians;
	FLOAT m_fRotationMinRadians;
	FLOAT m_fRotationProbability;

	SAPerturbations();
	bool loadSetup(const rapidxml::xml_node<>* pPerturbationsElem);
	bool loadDefaults(unsigned int iStructures, bool bMoleculesPresent, Constraints* pConstraints);
	bool saveSetup(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pParentElem);

	/**************************************************************************
	 * This method performs a perturbation on a structure and assumes that when
	 * original and result are first passed in, they're copies of each other.
	 */
	bool perturb(const Constraints &constraints,
			FLOAT fTranslationVectorLength, FLOAT fRotationRadians,
			Structure &structure) const;

	static bool translate(FLOAT fVectorLength, Structure &structure);
	static bool rotate(FLOAT fRadianAngle, Structure &structure);

	static const unsigned int s_maxPerturbationTries;

private:
	static const unsigned int s_perturbationsMinOccurs[];

	static const bool s_translationVectorAttReq[];
	static const char* s_translationVectorAttDef[];

	static const bool s_rotationAngleAttReq[];
	static const char* s_rotationAngleAttDef[];
};

#endif
