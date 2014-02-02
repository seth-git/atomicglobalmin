
#include "batch.h"
#include "input.h"

Batch::Batch(Input* input) : Action(input)
{
}

Batch::~Batch()
{
	clear();
}

//const char*      Batch::s_elementNames[] = {"structuresTemplate"};
const unsigned int Batch::s_minOccurs[]    = {1                   };

const bool Batch::s_setupAttReq[] =  {true, true};
const char* Batch::s_setupAttDef[] = {"1" , "2"};

bool Batch::loadSetup(TiXmlElement *pSetupElem, const Strings* messages)
{
	const char* setupAttNames[] = {messages->m_sxSaveFrequency.c_str(), messages->m_sxQueueSize.c_str()};
	XsdAttributeUtil setupAttUtil(pSetupElem->Value(), setupAttNames, s_setupAttReq, s_setupAttDef);
	if (!setupAttUtil.process(pSetupElem))
		return false;
	const char** setupAttValues = setupAttUtil.getAllAttributes();
	if (!XsdTypeUtil::getPositiveInt(setupAttValues[0], m_iSaveFrequency, setupAttNames[0], pSetupElem))
		return false;
	if (!XsdTypeUtil::getPositiveInt(setupAttValues[1], m_targetQueueSize, setupAttNames[1], pSetupElem))
		return false;

	const char* elementNames[] = {messages->m_sxStructuresTemplate.c_str()};
	XsdElementUtil setupUtil(pSetupElem->Value(), XSD_ALL, elementNames, s_minOccurs);
	TiXmlHandle handle(pSetupElem);
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

	if (1 != m_iSaveFrequency)
		setup->SetAttribute(messages->m_sxSaveFrequency.c_str(), m_iSaveFrequency);

	if (2 != m_targetQueueSize)
		setup->SetAttribute(messages->m_sxQueueSize.c_str(), m_targetQueueSize);

	return m_structuresTemplate.save(setup, messages);
}

const unsigned int Batch::s_resumeMinOccurs[] = {1, 1, 1, 1};
const unsigned int Batch::s_structuresMinOccurs[] = {1};
const unsigned int Batch::s_structuresMaxOccurs[] = {XSD_UNLIMITED};

bool Batch::loadResume(TiXmlElement *pResumeElem, const Strings* messages)
{
	if (pResumeElem == NULL) {
		if (!m_structuresTemplate.initializeStructures(m_structures, m_pConstraints))
			return false;
		m_tPrevElapsedSeconds = 0;
		m_bRunComplete = false;
	} else {
		TiXmlHandle hResume(pResumeElem);
		const char* resumeElemNames[] = {messages->m_sxTotalEnergyCalculations.c_str(), messages->m_sxElapsedSeconds.c_str(), messages->m_sxRunComplete.c_str(), messages->m_sxStructures.c_str()};
		XsdElementUtil resumeElemUtil(pResumeElem->Value(), XSD_ALL, resumeElemNames, s_resumeMinOccurs);
		if (!resumeElemUtil.process(hResume))
			return false;
		TiXmlElement** resumeElements = resumeElemUtil.getAllElements();
		if (!XsdTypeUtil::read1NonNegativeIntAtt(resumeElements[0], m_iEnergyCalculations, messages->m_sxValue.c_str(), true, NULL))
			return false;
		if (!XsdTypeUtil::read1TimeT(resumeElements[1], m_tPrevElapsedSeconds, messages->m_sxValue.c_str(), true, NULL))
			return false;
		if (!XsdTypeUtil::read1BoolAtt(resumeElements[2], m_bRunComplete, messages->m_sxValue.c_str(), true, NULL, messages))
			return false;

		TiXmlHandle hStructures(resumeElements[3]);
		const char* structuresElemNames[] = {messages->m_sxStructure.c_str()};
		XsdElementUtil structuresElemUtil(resumeElements[3]->Value(), XSD_SEQUENCE, structuresElemNames, s_structuresMinOccurs, s_structuresMaxOccurs);
		if (!structuresElemUtil.process(hStructures))
			return false;
		std::vector<TiXmlElement*>* structuresElements = structuresElemUtil.getSequenceElements();
		std::vector<TiXmlElement*>* v = &(structuresElements[0]);
		for (std::vector<TiXmlElement*>::iterator it = v->begin(); it != v->end(); it++) {
			Structure* pStructure = new Structure();
			m_structures.push_back(pStructure);
			if (!pStructure->load(*it, messages))
				return false;
		}
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
	XsdTypeUtil::writeTimeT(getTotalElapsedSeconds(), elapsedSeconds, messages->m_sxValue.c_str());
	resume->LinkEndChild(elapsedSeconds);

	TiXmlElement* runComplete = new TiXmlElement(messages->m_sxRunComplete.c_str());
	XsdTypeUtil::writeBool(m_bRunComplete, runComplete, messages->m_sxValue.c_str(), messages);
	resume->LinkEndChild(runComplete);

	TiXmlElement* structures = new TiXmlElement(messages->m_sxStructures.c_str());
	resume->LinkEndChild(structures);
	for (std::list<Structure*>::iterator it = m_structures.begin(); it != m_structures.end(); it++)
		if (!(*it)->save(structures, messages))
			return false;

	return true;
}

bool Batch::runMaster() {
	if (!Action::run())
		return false;

	unsigned int queueSize;
	unsigned int i, j;

	queueSize = m_structures.size() / m_iMpiProcesses;
	if (m_structures.size() % m_iMpiProcesses > 0)
		++queueSize;
	if (queueSize > m_targetQueueSize)
		queueSize = m_targetQueueSize;

	unsigned int numberToAssign = queueSize * (m_iMpiProcesses-1);
	if (numberToAssign > m_structures.size())
		numberToAssign = m_structures.size();

	if (m_structures.size() >= m_iMpiProcesses && numberToAssign == m_structures.size())
		--numberToAssign; // Reserve one for the master

	if (PRINT_MPI_MESSAGES)
		printf("Target queue size: %d, Queue size: %d, Number to assign: %d.\n", m_targetQueueSize, queueSize, numberToAssign);

	std::list<Structure*> unassigned;
	for (std::list<Structure*>::iterator it = m_structures.begin(); it != m_structures.end(); ++it)
		unassigned.push_back(*it);

	Structure* pStructure;
	std::map<int,Structure*> assignments[m_iMpiProcesses];
	unsigned int iAssignments = 0;
	int id;
	j = 1;
	for (i = 1; i <= numberToAssign; ++i) {
		pStructure = unassigned.front();
		unassigned.pop_front();
		id = pStructure->getId();
		if (PRINT_MPI_MESSAGES)
			printf("Sending slave %d structure %d.\n", j, id);
		MPI_Send(&id, 1, MPI_INT, j, WORK_TAG, MPI_COMM_WORLD);
		assignments[j][id] = pStructure;
		++iAssignments;

		++j;
		if (j >= m_iMpiProcesses)
			j = 1;
	}

	char* xml;
	bool success;
	MPI_Status status;
	unsigned int iEnergyCalcFailures = 0;
	unsigned int iFailedProcesses = 0;
	unsigned int iSaveCount = 0;
	do {
		if (unassigned.size() > 0) {
			pStructure = unassigned.front();
			unassigned.pop_front();
			if (PRINT_MPI_MESSAGES)
				printf("Master performing calculations on %d.\n", pStructure->getId());
			if (m_pEnergy->execute(*pStructure)) {
				iEnergyCalcFailures = 0;
				processResult(pStructure);
			} else {
				++iEnergyCalcFailures;
				unassigned.push_back(pStructure);
				if (iEnergyCalcFailures > s_iMaxEnergyCalcFailures) {
					if (PRINT_MPI_MESSAGES)
						printf("Master failed to perform %d energy calculations in a row. Sending die signal to other processes and exiting.\n", iEnergyCalcFailures);
					for (unsigned int i = 1; i < m_iMpiProcesses; ++i)
						MPI_Send(0, 0, MPI_INT, i, DIE_TAG, MPI_COMM_WORLD);
					return false;
				}
			}
		}
		while (iAssignments > 0 && MpiUtil::receiveString(MPI_ANY_SOURCE, xml, status, unassigned.size() == 0)) {
			if (status.MPI_TAG == WORK_TAG) {
				pStructure = new Structure();
				success = pStructure->loadStr(xml);
				delete[] xml;
				if (!success) {
					delete pStructure;
					return false;
				}
				if (PRINT_MPI_MESSAGES)
					printf("Master received structure %d from process %d.\n", pStructure->getId(), status.MPI_SOURCE);
				processResult(pStructure);
				assignments[status.MPI_SOURCE].erase(pStructure->getId());
				--iAssignments;
			} else {
				id = atoi(xml);
				if (PRINT_MPI_MESSAGES)
					printf("Master received non work message %d from process %d regarding structure %d.\n", status.MPI_TAG, status.MPI_SOURCE, pStructure->getId());
				delete[] xml;
				std::map<int,Structure*>* pAssignments = &(assignments[status.MPI_SOURCE]);
				if (status.MPI_TAG == DIE_TAG) {
					++iFailedProcesses;
					FLOAT percentFailed = (FLOAT)iFailedProcesses / (FLOAT)m_iMpiProcesses;
					if (percentFailed > s_fMaxMPIProcessFailures) {
						if (PRINT_MPI_MESSAGES)
							printf("More than %0.0lf% of processes failed. Sending die signal to other processes and exiting.\n", s_fMaxMPIProcessFailures * 100);
						for (unsigned int i = 1; i < m_iMpiProcesses; ++i)
							MPI_Send(0, 0, MPI_INT, i, DIE_TAG, MPI_COMM_WORLD);
						return false;
					} else {
						iAssignments -= pAssignments->size();
						for (std::map<int,Structure*>::iterator it = pAssignments->begin(); it != pAssignments->end(); ++it)
							unassigned.push_back(it->second);
						pAssignments->clear();
					}
					continue;
				} else {
					--iAssignments;
					unassigned.push_back((*pAssignments)[id]);
					pAssignments->erase(id);
				}
			}
			if (unassigned.size() > 0) {
				pStructure = unassigned.front();
				unassigned.pop_front();
				id = pStructure->getId();
				if (PRINT_MPI_MESSAGES)
					printf("Sending slave %d structure %d.\n", status.MPI_SOURCE, id);
				assignments[status.MPI_SOURCE][id] = pStructure;
				++iAssignments;
				MPI_Send(&id, 1, MPI_INT, status.MPI_SOURCE, WORK_TAG, MPI_COMM_WORLD);
			} else {
				if (PRINT_MPI_MESSAGES)
					printf("There are no more structures. Sending slave %d the finish tag.\n", status.MPI_SOURCE);
				MPI_Send(0, 0, MPI_INT, status.MPI_SOURCE, FINISH_TAG, MPI_COMM_WORLD);
			}
		}
		m_bRunComplete = unassigned.size() == 0 && iAssignments == 0;
		if (++iSaveCount >= m_iSaveFrequency || m_bRunComplete) {
			m_pInput->save();
			iSaveCount = 0;
		}
	} while (!m_bRunComplete);

	return true;
}

void Batch::processResult(Structure* structure) {
	for (std::list<Structure*>::iterator it = m_structures.begin(); it != m_structures.end(); it++) {
		if ((*it)->getId() == structure->getId()) {
			if (*it != structure)
				delete *it;
			m_structures.erase(it);
			break;
		}
	}
	updateResults(structure);
	++m_iEnergyCalculations;
}

bool Batch::runSlave() {
	if (!Action::run())
		return false;

	std::map<int,Structure*> structureMap;
	for (std::list<Structure*>::iterator it = m_structures.begin(); it != m_structures.end(); it++)
		structureMap[(*it)->getId()] = *it;

	int id;
	MPI_Status status;
	Structure* pStructure;
	unsigned int iEnergyCalcFailures = 0;
	std::string temp;

	while (true) {
		if (PRINT_MPI_MESSAGES)
			printf("Slave %d waiting for a structure.\n", m_iMpiRank);
		MPI_Recv(&id, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		if (status.MPI_TAG != WORK_TAG) {
			if (PRINT_MPI_MESSAGES)
				printf("Slave %d received non-work message %d. Exiting.\n", m_iMpiRank, status.MPI_TAG);
			break;
		}
		if (PRINT_MPI_MESSAGES)
			printf("Slave %d received structure %d.\n", m_iMpiRank, id);
		pStructure = structureMap[id];
		if (m_pEnergy->execute(*pStructure)) {
			iEnergyCalcFailures = 0;
			std::string xml;
			pStructure->save(xml);
			if (PRINT_MPI_MESSAGES)
				printf("Slave %d reporting energy calculation results on structure %d.\n", m_iMpiRank, pStructure->getId());
		    MPI_Send((void*)xml.c_str(), xml.length()+1, MPI_CHAR, 0, WORK_TAG, MPI_COMM_WORLD);
		} else {
			++iEnergyCalcFailures;
			temp = id;
			if (iEnergyCalcFailures <= s_iMaxEnergyCalcFailures) {
				if (PRINT_MPI_MESSAGES)
					printf("Slave %d sending failure message to master.\n", m_iMpiRank);
				MPI_Send((void*)temp.c_str(), temp.length()+1, MPI_CHAR, 0, FAILURE_TAG, MPI_COMM_WORLD);
			} else {
				if (PRINT_MPI_MESSAGES)
					printf("Slave %d sending die message to master and exiting.\n", m_iMpiRank);
				MPI_Send((void*)temp.c_str(), temp.length()+1, MPI_CHAR, 0, DIE_TAG, MPI_COMM_WORLD);
				return false;
			}
		}
	}

	return true;
}
