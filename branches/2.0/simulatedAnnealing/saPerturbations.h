
#ifndef __PERTURBATIONS_H_
#define __PERTURBATIONS_H_

#include "../xsd/xsdAttributeUtil.h"
#include "../xsd/xsdElementUtil.h"
#include "../xsd/xsdTypeUtil.h"
#include "../translation/strings.h"
#include "../constraints.h"

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

private:
	static const unsigned int s_perturbationsMinOccurs[];

	static const bool s_translationVectorAttReq[];
	static const char* s_translationVectorAttDef[];

	static const bool s_rotationAngleAttReq[];
	static const char* s_rotationAngleAttDef[];
};

#endif
