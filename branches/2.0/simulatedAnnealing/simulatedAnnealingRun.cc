
#include "simulatedAnnealingRun.h"
#include "simulatedAnnealing.h"

SimulatedAnnealingRun::SimulatedAnnealingRun(const SimulatedAnnealing* sharedData, Structure* pStructure)
{
	m_sharedData = sharedData;
	m_pStructure = pStructure;
}

void SimulatedAnnealingRun::init() {
	if (m_sharedData->m_pfStartingTemperature != NULL)
		m_fTemperature = *(m_sharedData->m_pfStartingTemperature);
	else
		m_fTemperature = 1000000;
	if (0 != m_sharedData->m_perturbations.m_fTranslationVectorProbability)
		m_fTranslationVectorLength = m_sharedData->m_perturbations.m_fTranslationVectorStartLength;
	if (0 != m_sharedData->m_perturbations.m_fRotationProbability)
		m_fRotationRadians = m_sharedData->m_perturbations.m_fRotationStartRadians;
	m_iIteration = 0;
	m_bRunComplete = false;
}

const unsigned int SimulatedAnnealingRun::s_resumeMinOccurs[] = {1,0,0,0,1,1,1,1,1};

bool SimulatedAnnealingRun::loadResume(const rapidxml::xml_node<>* pResumeElem,
		const Strings* messages) {
	using namespace rapidxml;

	const char * resumeElemNames[] = {
			messages->m_sxTemperature.c_str(),
			messages->m_sxTranslationVector.c_str(),
			messages->m_sxRotationAngle.c_str(),
			messages->m_sxPerturbationsPerIteration.c_str(),
			messages->m_sxIteration.c_str(),
			messages->m_sxTotalEnergyCalculations.c_str(),
			messages->m_sxElapsedSeconds.c_str(),
			messages->m_sxRunComplete.c_str(),
			messages->m_sxStructure.c_str()
	};
	XsdElementUtil resumeElementUtil(XSD_ALL, resumeElemNames, s_resumeMinOccurs);
	if (!resumeElementUtil.process(pResumeElem))
		return false;
	const xml_node<>** resumeElements = resumeElementUtil.getAllElements();

	if (!XsdTypeUtil::read1PosFloatAtt(resumeElements[0], m_fTemperature, messages->m_sxValue.c_str(), true, NULL))
		return false;
	if (0 != m_sharedData->m_perturbations.m_fTranslationVectorProbability) {
		if (NULL == resumeElements[1]) {
			printf("Under the %1$s element, the %2$s element is required when the %2$s probability is greater than zero.\n",
					messages->m_sxResume.c_str(), messages->m_sxTranslationVector.c_str());
			return false;
		}
		if (!XsdTypeUtil::read1PosFloatAtt(resumeElements[1], m_fTranslationVectorLength, messages->m_sxValue.c_str(), true, NULL))
			return false;
	}
	if (0 != m_sharedData->m_perturbations.m_fRotationProbability) {
		if (NULL == resumeElements[2]) {
			printf("Under the %1$s element, the %2$s element is required when the %2$s probability is greater than zero.\n",
					messages->m_sxResume.c_str(), messages->m_sxRotationAngle.c_str());
			return false;
		}
		if (!XsdTypeUtil::read1PosFloatAtt(resumeElements[2], m_fRotationRadians, messages->m_sxRadians.c_str(), true, NULL))
			return false;
	}
	if (1 != m_sharedData->m_perturbations.m_iStartingPerturbationsPerIteration) {
		if (NULL == resumeElements[3]) {
			printf("Under the %1$s element, the %2$s element is required when the %2$s is greater than one.\n",
					messages->m_sxResume.c_str(), messages->m_sxPerturbationsPerIteration.c_str());
			return false;
		}
		if (!XsdTypeUtil::read1PosFloatAtt(resumeElements[3], m_fPerturbationsPerIteration, messages->m_sxPerturbationsPerIteration.c_str(), true, NULL))
			return false;
	} else {
		m_fPerturbationsPerIteration = 1;
	}
	if (!XsdTypeUtil::read1NonNegativeIntAtt(resumeElements[4], m_iIteration, messages->m_sxIteration.c_str(), true, NULL))
		return false;
	if (!XsdTypeUtil::read1NonNegativeIntAtt(resumeElements[5], m_iEnergyCalculations, messages->m_sxTotalEnergyCalculations.c_str(), true, NULL))
		return false;
	if (!XsdTypeUtil::read1TimeT(resumeElements[6], m_tPrevElapsedSeconds, messages->m_sxElapsedSeconds.c_str(), true, NULL))
		return false;
	if (!XsdTypeUtil::read1BoolAtt(resumeElements[7], m_bRunComplete, messages->m_sxRunComplete.c_str(), true, NULL, messages))
		return false;
	if (!m_pStructure->load(resumeElements[8], messages))
		return false;

	return true;
}

bool SimulatedAnnealingRun::saveResume(rapidxml::xml_document<> &doc,
		rapidxml::xml_node<>* pSimElem, const Strings* messages) {
	using namespace rapidxml;
	xml_node<>* resume = doc.allocate_node(node_element, messages->m_sxResume.c_str());
	pSimElem->append_node(resume);

	xml_node<>* temperature = doc.allocate_node(node_element, messages->m_sxTemperature.c_str(), NULL, messages->m_sxTemperature.length());
	resume->append_node(temperature);
	XsdTypeUtil::setAttribute(doc, temperature, messages->m_sxValue.c_str(), m_fTemperature);

	if (0 != m_sharedData->m_perturbations.m_fTranslationVectorProbability) {
		xml_node<>* translationVector = doc.allocate_node(node_element, messages->m_sxTranslationVector.c_str(), NULL, messages->m_sxTranslationVector.length());
		resume->append_node(translationVector);
		XsdTypeUtil::setAttribute(doc, translationVector, messages->m_sxValue.c_str(), m_fTranslationVectorLength);
	}
	if (0 != m_sharedData->m_perturbations.m_fRotationProbability) {
		xml_node<>* rotationAngle = doc.allocate_node(node_element, messages->m_sxRotationAngle.c_str(), NULL, messages->m_sxRotationAngle.length());
		resume->append_node(rotationAngle);
		XsdTypeUtil::setAttribute(doc, rotationAngle, messages->m_sxRadians.c_str(), m_fRotationRadians);
	}
	if (1 != m_sharedData->m_perturbations.m_iStartingPerturbationsPerIteration) {
		xml_node<>* perturbationsPerIteration = doc.allocate_node(node_element, messages->m_sxPerturbationsPerIteration.c_str(), NULL, messages->m_sxPerturbationsPerIteration.length());
		resume->append_node(perturbationsPerIteration);
		XsdTypeUtil::setAttribute(doc, perturbationsPerIteration, messages->m_sxValue.c_str(), m_fPerturbationsPerIteration);
	}

	xml_node<>* iteration = doc.allocate_node(node_element, messages->m_sxIteration.c_str());
	resume->append_node(iteration);
	XsdTypeUtil::setAttribute(doc, iteration, messages->m_sxValue.c_str(), m_iIteration);

	xml_node<>* totalEnergyCalculations = doc.allocate_node(node_element, messages->m_sxTotalEnergyCalculations.c_str());
	resume->append_node(totalEnergyCalculations);
	XsdTypeUtil::setAttribute(doc, totalEnergyCalculations, messages->m_sxValue.c_str(), m_iEnergyCalculations);

	xml_node<>* elapsedSeconds = doc.allocate_node(node_element, messages->m_sxElapsedSeconds.c_str());
	resume->append_node(elapsedSeconds);
	XsdTypeUtil::setAttribute(doc, elapsedSeconds, messages->m_sxValue.c_str(), getTotalElapsedSeconds());

	xml_node<>* runComplete = doc.allocate_node(node_element, messages->m_sxRunComplete.c_str());
	resume->append_node(runComplete);
	XsdTypeUtil::setAttribute(doc, runComplete, messages->m_sxValue.c_str(), m_bRunComplete, messages);

	if (!m_pStructure->save(doc, resume, messages))
		return false;

	return true;
}

time_t SimulatedAnnealingRun::getTotalElapsedSeconds() {
	return m_tPrevElapsedSeconds + (time (NULL) - m_sharedData->m_tStartTime);
}
