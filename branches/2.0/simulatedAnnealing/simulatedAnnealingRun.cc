
#include "simulatedAnnealingRun.h"
#include "simulatedAnnealing.h"

SimulatedAnnealingRun::SimulatedAnnealingRun(const SimulatedAnnealing* sharedData, Structure* pStructure)
{
	m_sharedData = sharedData;
	m_pStructure = pStructure;
	m_fTranslationVectorLength = 0;
	m_fRotationRadians = 0;
	m_iIteration = 0;
	m_fPerturbationsPerIteration = 1;
	m_fTemperature = 0;
	m_bRunComplete = false;
	m_tPrevElapsedSeconds = 0;
	m_iEnergyCalculations = 0;
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

const char* SimulatedAnnealingRun::s_resumeElemNames[] = {
		strings::xTemperature,
		strings::xTranslationVector,
		strings::xRotationAngle,
		strings::xPerturbationsPerIteration,
		strings::xIteration,
		strings::xTotalEnergyCalculations,
		strings::xElapsedSeconds,
		strings::xRunComplete,
		strings::xStructure
};
const unsigned int SimulatedAnnealingRun::s_resumeMinOccurs[] = {1,0,0,0,1,1,1,1,1};

bool SimulatedAnnealingRun::loadResume(const rapidxml::xml_node<>* pResumeElem) {
	using namespace rapidxml;
	using namespace strings;

	XsdElementUtil resumeElementUtil(XSD_ALL, s_resumeElemNames, s_resumeMinOccurs);
	if (!resumeElementUtil.process(pResumeElem))
		return false;
	const xml_node<>** resumeElements = resumeElementUtil.getAllElements();

	if (!XsdTypeUtil::read1PosFloatAtt(resumeElements[0], m_fTemperature, xValue, true, NULL))
		return false;
	if (0 != m_sharedData->m_perturbations.m_fTranslationVectorProbability) {
		if (NULL == resumeElements[1]) {
			printf("Under the %1$s element, the %2$s element is required when the %2$s probability is greater than zero.\n",
					xResume, xTranslationVector);
			return false;
		}
		if (!XsdTypeUtil::read1PosFloatAtt(resumeElements[1], m_fTranslationVectorLength, xValue, true, NULL))
			return false;
	}
	if (0 != m_sharedData->m_perturbations.m_fRotationProbability) {
		if (NULL == resumeElements[2]) {
			printf("Under the %1$s element, the %2$s element is required when the %2$s probability is greater than zero.\n",
					xResume, xRotationAngle);
			return false;
		}
		if (!XsdTypeUtil::read1PosFloatAtt(resumeElements[2], m_fRotationRadians, xRadians, true, NULL))
			return false;
	}
	if (1 != m_sharedData->m_perturbations.m_iStartingPerturbationsPerIteration) {
		if (NULL == resumeElements[3]) {
			printf("Under the %1$s element, the %2$s element is required when the %2$s is greater than one.\n",
					xResume, xPerturbationsPerIteration);
			return false;
		}
		if (!XsdTypeUtil::read1PosFloatAtt(resumeElements[3], m_fPerturbationsPerIteration, xPerturbationsPerIteration, true, NULL))
			return false;
	} else {
		m_fPerturbationsPerIteration = 1;
	}
	if (!XsdTypeUtil::read1NonNegativeIntAtt(resumeElements[4], m_iIteration, xValue, true, NULL))
		return false;
	if (!XsdTypeUtil::read1NonNegativeIntAtt(resumeElements[5], m_iEnergyCalculations, xValue, true, NULL))
		return false;
	if (!XsdTypeUtil::read1TimeT(resumeElements[6], m_tPrevElapsedSeconds, xValue, true, NULL))
		return false;
	if (!XsdTypeUtil::read1BoolAtt(resumeElements[7], m_bRunComplete, xValue, true, NULL))
		return false;
	if (!m_pStructure->load(resumeElements[8]))
		return false;

	return true;
}

bool SimulatedAnnealingRun::saveResume(rapidxml::xml_document<> &doc,
		rapidxml::xml_node<>* pSimElem) {
	using namespace rapidxml;
	using namespace strings;
	xml_node<>* resume = doc.allocate_node(node_element, xResume);
	pSimElem->append_node(resume);

	xml_node<>* temperature = doc.allocate_node(node_element, xTemperature);
	resume->append_node(temperature);
	XsdTypeUtil::setAttribute(doc, temperature, xValue, m_fTemperature);

	if (0 != m_sharedData->m_perturbations.m_fTranslationVectorProbability) {
		xml_node<>* translationVector = doc.allocate_node(node_element, xTranslationVector);
		resume->append_node(translationVector);
		XsdTypeUtil::setAttribute(doc, translationVector, xValue, m_fTranslationVectorLength);
	}
	if (0 != m_sharedData->m_perturbations.m_fRotationProbability) {
		xml_node<>* rotationAngle = doc.allocate_node(node_element, xRotationAngle);
		resume->append_node(rotationAngle);
		XsdTypeUtil::setAttribute(doc, rotationAngle, xRadians, m_fRotationRadians);
	}
	if (1 != m_sharedData->m_perturbations.m_iStartingPerturbationsPerIteration) {
		xml_node<>* perturbationsPerIteration = doc.allocate_node(node_element, xPerturbationsPerIteration);
		resume->append_node(perturbationsPerIteration);
		XsdTypeUtil::setAttribute(doc, perturbationsPerIteration, xValue, m_fPerturbationsPerIteration);
	}

	xml_node<>* iteration = doc.allocate_node(node_element, xIteration);
	resume->append_node(iteration);
	XsdTypeUtil::setAttribute(doc, iteration, xValue, m_iIteration);

	xml_node<>* totalEnergyCalculations = doc.allocate_node(node_element, xTotalEnergyCalculations);
	resume->append_node(totalEnergyCalculations);
	XsdTypeUtil::setAttribute(doc, totalEnergyCalculations, xValue, m_iEnergyCalculations);

	xml_node<>* elapsedSeconds = doc.allocate_node(node_element, xElapsedSeconds);
	resume->append_node(elapsedSeconds);
	XsdTypeUtil::setAttribute(doc, elapsedSeconds, xValue, getTotalElapsedSeconds());

	xml_node<>* runComplete = doc.allocate_node(node_element, xRunComplete);
	resume->append_node(runComplete);
	XsdTypeUtil::setAttribute(doc, runComplete, xValue, m_bRunComplete);

	if (!m_pStructure->save(doc, resume))
		return false;

	return true;
}

time_t SimulatedAnnealingRun::getTotalElapsedSeconds() {
	return m_tPrevElapsedSeconds + (time (NULL) - m_sharedData->m_tStartTime);
}

bool SimulatedAnnealingRun::performIteration() {
	const Constraints* pConstraints = m_sharedData->m_pConstraints;
	Structure saved, temp;
	unsigned int count = (unsigned int)m_fPerturbationsPerIteration;

	saved.copy(*m_pStructure);
	while (count) {
		if (!m_sharedData->m_perturbations.perturb(*pConstraints,
				m_fTranslationVectorLength, m_fRotationRadians, *m_pStructure))
			return false;
		--count;
	}

	return true;
}
