
#include "batch.h"
#include "input.h"

Batch::Batch(Input* input) : Action(input)
{
	m_iStructures = 0;
	m_structures = NULL;
}

Batch::~Batch()
{
	cleanUp();
}

void Batch::cleanUp()
{
	m_iStructures = 0;
	if (m_structures != NULL) {
		delete m_structures;
		m_structures = NULL;
	}
}

//const char*      Batch::s_elementNames[] = {"structuresTemplate"};
const unsigned int Batch::s_minOccurs[]    = {1                   };

const bool Batch::s_setupAttReq[] = {true,true};
const char* Batch::s_setupAttDef[] = {"1", "200"};

bool Batch::loadSetup(TiXmlElement *pSetupElem, const Strings* messages)
{
	cleanUp();
	const char* elementNames[] = {messages->m_sxStructuresTemplate.c_str()};
	XsdElementUtil setupUtil(pSetupElem->Value(), XSD_ALL, elementNames, s_minOccurs);
	TiXmlHandle handle(0);
	handle=TiXmlHandle(pSetupElem);
	if (!setupUtil.process(handle))
		return false;
	TiXmlElement** setupElements = setupUtil.getAllElements();

	if (!m_structuresTemplate.load(setupElements[0], m_constraintsMap, messages))
		return false;

	return true;
}

bool Batch::saveSetup(TiXmlElement *pBatchElem, const Strings* messages)
{
	TiXmlElement* setup = new TiXmlElement(messages->m_sxSetup.c_str());
	pBatchElem->LinkEndChild(setup);

	return m_structuresTemplate.save(setup, messages);
}

const unsigned int Batch::s_resumeMinOccurs[] = {1, 1, 1};
const unsigned int Batch::s_structuresMinOccurs[] = {1};
const unsigned int Batch::s_structuresMaxOccurs[] = {XSD_UNLIMITED};

bool Batch::loadResume(TiXmlElement *pResumeElem, const Strings* messages)
{
	if (pResumeElem == NULL) {
		if (!m_structuresTemplate.initializeStructures(m_iStructures, m_structures, m_pConstraints))
			return false;
		m_tElapsedSeconds = 0;
	} else {
		TiXmlHandle hResume(0);
		hResume=TiXmlHandle(pResumeElem);
		const char* resumeElemNames[] = {messages->m_sxTotalEnergyCalculations.c_str(), messages->m_sxElapsedSeconds.c_str(), messages->m_sxStructures.c_str()};
		XsdElementUtil resumeElemUtil(pResumeElem->Value(), XSD_ALL, resumeElemNames, s_resumeMinOccurs);
		if (!resumeElemUtil.process(hResume)) {
			return false;
		}
		TiXmlElement** resumeElements = resumeElemUtil.getAllElements();
		if (!XsdTypeUtil::read1NonNegativeIntAtt(resumeElements[0], m_iEnergyCalculations, messages->m_sxValue.c_str(), true, NULL))
			return false;
		if (!XsdTypeUtil::read1TimeT(resumeElements[1], m_tElapsedSeconds, messages->m_sxValue.c_str(), true, NULL))
			return false;

		TiXmlHandle hStructures(0);
		hStructures=TiXmlHandle(resumeElements[2]);
		const char* structuresElemNames[] = {messages->m_sxStructure.c_str()};
		XsdElementUtil structuresElemUtil(resumeElements[2]->Value(), XSD_SEQUENCE, structuresElemNames, s_structuresMinOccurs, s_structuresMaxOccurs);
		if (!structuresElemUtil.process(hStructures)) {
			return false;
		}
		std::vector<TiXmlElement*>* structuresElements = structuresElemUtil.getSequenceElements();
		std::vector<TiXmlElement*>* v = &(structuresElements[0]);
		m_iStructures = v->size();
		m_structures = new Structure[m_iStructures];
		unsigned int count = 0;
		for (std::vector<TiXmlElement*>::iterator it = v->begin(); it != v->end(); it++)
			if (!m_structures[count++].load(*it, messages))
				return false;
	}
	return true;
}

bool Batch::saveResume(TiXmlElement *pBatchElem, const Strings* messages)
{
	TiXmlElement* resume = new TiXmlElement(messages->m_sxResume.c_str());
	pBatchElem->LinkEndChild(resume);

	TiXmlElement* totalEnergyCalculations = new TiXmlElement(messages->m_sxTotalEnergyCalculations.c_str());
	totalEnergyCalculations->SetAttribute(messages->m_sxValue.c_str(), m_iEnergyCalculations);
	resume->LinkEndChild(totalEnergyCalculations);
	
	TiXmlElement* elapsedSeconds = new TiXmlElement(messages->m_sxElapsedSeconds.c_str());
	XsdTypeUtil::writeTimeT(m_tElapsedSeconds, elapsedSeconds, messages->m_sxValue.c_str());
	resume->LinkEndChild(elapsedSeconds);

	TiXmlElement* structures = new TiXmlElement(messages->m_sxStructures.c_str());
	resume->LinkEndChild(structures);
	for (unsigned int i = 0; i < m_iStructures; ++i)
		if (!m_structures[i].save(structures, messages))
			return false;

	return true;
}

bool Batch::run()
{
	if (!Action::run())
		return false;

	for (unsigned int i = 0; i < m_iStructures; ++i) {
		if (!m_pEnergy->execute(m_structures[i]))
			return false;
		++m_iEnergyCalculations;
	}

	m_pInput->save();
	return true;
}
