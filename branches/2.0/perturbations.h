
#ifndef __PERTURBATIONS_H_
#define __PERTURBATIONS_H_

#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdElementUtil.h"
#include "xsd/xsdTypeUtil.h"
#include "translation/strings.h"
#include "constraints.h"

class Perturbations {
public:
	unsigned int m_iStartingPerturbationsPerIteration;
	FLOAT m_fPerturbationsPerIteration;

	FLOAT* m_pfTranslationVectorStartLength;
	FLOAT* m_pfTranslationVectorMinLength;
	FLOAT* m_pfTranslationVectorLength;
	FLOAT* m_pfTranslationVectorProbability;

	FLOAT* m_pfRotationStartRadians;
	FLOAT* m_pfRotationMinRadians;
	FLOAT* m_pfRotationRadians;
	FLOAT* m_pfRotationProbability;

	Perturbations();
	~Perturbations();
	bool loadSetup(const rapidxml::xml_node<>* pPerturbationsElem, const Strings* messages);
	bool loadDefaults(unsigned int iStructures, bool bMoleculesPresent, Constraints* pConstraints);
	bool saveSetup(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pParentElem, const Strings* messages);

private:
	void clear();

	static const unsigned int s_perturbationsMinOccurs[];

	static const bool s_translationVectorAttReq[];
	static const char* s_translationVectorAttDef[];

	static const bool s_rotationAngleAttReq[];
	static const char* s_rotationAngleAttDef[];
};

#endif
