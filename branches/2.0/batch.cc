
#include "batch.h"
#include "input.h"

Batch::Batch(Input* input) : Action(input)
{
	m_targetQueueSize = 0;
}

Batch::~Batch()
{
	// Note: clear is called from the Action class
}

const char*     Batch::s_elementNames[] = {strings::xStructuresTemplate};
const unsigned int Batch::s_minOccurs[] = {1};

const char* Batch::s_setupAttNames[] = {strings::xSaveFrequency, strings::xQueueSize};
const bool Batch::s_setupAttReq[] =  {true, true};
const char* Batch::s_setupAttDef[] = {"1" , "3"};

bool Batch::loadSetup(const rapidxml::xml_node<>* pSetupElem)
{
	XsdAttributeUtil setupAttUtil(s_setupAttNames, s_setupAttReq, s_setupAttDef);
	if (!setupAttUtil.process(pSetupElem))
		return false;
	const char** setupAttValues = setupAttUtil.getAllAttributes();
	if (!XsdTypeUtil::getPositiveInt(setupAttValues[0], m_iSaveFrequency, s_setupAttNames[0], pSetupElem))
		return false;
	if (!XsdTypeUtil::getPositiveInt(setupAttValues[1], m_targetQueueSize, s_setupAttNames[1], pSetupElem))
		return false;

	XsdElementUtil setupUtil(XSD_ALL, s_elementNames, s_minOccurs);
	if (!setupUtil.process(pSetupElem))
		return false;
	const rapidxml::xml_node<>** setupElements = setupUtil.getAllElements();

	if (!m_structuresTemplate.load(setupElements[0], m_constraintsMap))
		return false;

	return true;
}

bool Batch::saveSetup(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pBatchElem)
{
	using namespace rapidxml;
	using namespace strings;
	xml_node<>* setup = doc.allocate_node(node_element, xSetup);
	pBatchElem->append_node(setup);

	if (1 != m_iSaveFrequency)
		XsdTypeUtil::setAttribute(doc, setup, xSaveFrequency, m_iSaveFrequency);

	if (3 != m_targetQueueSize)
		XsdTypeUtil::setAttribute(doc, setup, xQueueSize, m_targetQueueSize);

	return m_structuresTemplate.save(doc, setup);
}

const unsigned int Batch::s_resumeMinOccurs[] = {1, 1, 1, 1};
const unsigned int Batch::s_structuresMinOccurs[] = {1};
const unsigned int Batch::s_structuresMaxOccurs[] = {XSD_UNLIMITED};

bool Batch::loadResume(const rapidxml::xml_node<>* pResumeElem)
{
	using namespace rapidxml;
	using namespace strings;
	if (pResumeElem == NULL) {
		if (!m_structuresTemplate.initializeStructures(m_structures, m_pConstraints))
			return false;
		m_tPrevElapsedSeconds = 0;
		m_bRunComplete = false;
	} else {
		const char* resumeElemNames[] = {xTotalEnergyCalculations, xElapsedSeconds, xRunComplete, xStructures};
		XsdElementUtil resumeElemUtil(XSD_ALL, resumeElemNames, s_resumeMinOccurs);
		if (!resumeElemUtil.process(pResumeElem))
			return false;
		const xml_node<>** resumeElements = resumeElemUtil.getAllElements();
		if (!XsdTypeUtil::read1NonNegativeIntAtt(resumeElements[0], m_iEnergyCalculations, xValue, true, NULL))
			return false;
		if (!XsdTypeUtil::read1TimeT(resumeElements[1], m_tPrevElapsedSeconds, xValue, true, NULL))
			return false;
		if (!XsdTypeUtil::read1BoolAtt(resumeElements[2], m_bRunComplete, xValue, true, NULL))
			return false;

		const char* structuresElemNames[] = {xStructure};
		XsdElementUtil structuresElemUtil(XSD_SEQUENCE, structuresElemNames, s_structuresMinOccurs, s_structuresMaxOccurs);
		if (!structuresElemUtil.process(resumeElements[3]))
			return false;
		std::vector<const xml_node<>*>* structuresElements = structuresElemUtil.getSequenceElements();
		std::vector<const xml_node<>*>* v = &(structuresElements[0]);
		for (std::vector<const xml_node<>*>::iterator it = v->begin(); it != v->end(); it++) {
			Structure* pStructure = new Structure();
			m_structures.push_back(pStructure);
			if (!pStructure->load(*it))
				return false;
		}
	}
	return true;
}

bool Batch::saveResume(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pBatchElem)
{
	using namespace rapidxml;
	using namespace strings;
	xml_node<>* resume = doc.allocate_node(node_element, xResume);
	pBatchElem->append_node(resume);

	xml_node<>* totalEnergyCalculations = doc.allocate_node(node_element, xTotalEnergyCalculations);
	XsdTypeUtil::setAttribute(doc, totalEnergyCalculations, xValue, m_iEnergyCalculations);
	resume->append_node(totalEnergyCalculations);
	
	xml_node<>* elapsedSeconds = doc.allocate_node(node_element, xElapsedSeconds);
	XsdTypeUtil::setAttribute(doc, elapsedSeconds, xValue, getTotalElapsedSeconds());
	resume->append_node(elapsedSeconds);

	xml_node<>* runComplete = doc.allocate_node(node_element, xRunComplete);
	XsdTypeUtil::setAttribute(doc, runComplete, xValue, m_bRunComplete);
	resume->append_node(runComplete);

	xml_node<>* structures = doc.allocate_node(node_element, xStructures);
	resume->append_node(structures);
	for (std::list<Structure*>::iterator it = m_structures.begin(); it != m_structures.end(); it++)
		if (!(*it)->save(doc, structures))
			return false;

	return true;
}

bool Batch::runMaster() {
	if (m_bRunComplete) {
		puts("This run has already been completed.");
		return true;
	}
	if (!Action::run())
		return false;

	std::list<Structure*> unassigned;
	std::map<int,Structure*> assignments[m_iMpiProcesses];
	unsigned int iAssignments;
	getInitialAssignments(unassigned, assignments, iAssignments);

	char* xml;
	bool success;
	MPI_Status status;
	unsigned int iEnergyCalcFailures = 0;
	unsigned int iFailedProcesses = 0;
	unsigned int iSaveCount = 0;
	std::map<int,bool> sentFinishMessage;
	MpiUtil mpiUtil;
	Structure* pStructure;
	int id;
	do {
		if (!unassigned.empty()) {
			pStructure = unassigned.front();
			unassigned.pop_front();
			#if MPI_DEBUG
				printf("Master performing calculations on %d.\n", pStructure->getId());
			#endif
			if (m_pEnergy->execute(*pStructure)) {
				iEnergyCalcFailures = 0;
				processResult(pStructure);
			} else {
				++iEnergyCalcFailures;
				unassigned.push_back(pStructure);
				if (iEnergyCalcFailures > s_iMaxEnergyCalcFailures) {
					#if MPI_DEBUG
						printf("Master failed to perform %d energy calculations in a row. Sending die signal to other processes and exiting.\n", iEnergyCalcFailures);
					#endif
					for (unsigned int i = 1; i < m_iMpiProcesses; ++i)
						MPI_Send(0, 0, MPI_INT, i, DIE_TAG, MPI_COMM_WORLD);
					return false;
				}
			}
		}

		mpiUtil.completeNonBlockingSends();

		#if MPI_DEBUG
			printf("Master checking for messages.\n");
		#endif
		while (iAssignments > 0 && MpiUtil::receiveString(MPI_ANY_SOURCE, xml, status, unassigned.empty())) {
			if (status.MPI_TAG == WORK_TAG) {
				pStructure = new Structure();
				success = pStructure->loadStr(xml);
				delete[] xml;
				if (!success) {
					delete pStructure;
					return false;
				}
				#if MPI_DEBUG
					printf("Master received structure %d from process %d.\n", pStructure->getId(), status.MPI_SOURCE);
				#endif
				processResult(pStructure);
				assignments[status.MPI_SOURCE].erase(pStructure->getId());
				--iAssignments;
			} else {
				id = atoi(xml);
				#if MPI_DEBUG
					printf("Master received non-work message %d from process %d regarding structure %d.\n", status.MPI_TAG, status.MPI_SOURCE, id);
				#endif
				delete[] xml;
				std::map<int,Structure*>* pAssignments = &(assignments[status.MPI_SOURCE]);
				if (status.MPI_TAG == DIE_TAG) {
					++iFailedProcesses;
					FLOAT percentFailed = (FLOAT)iFailedProcesses / (FLOAT)m_iMpiProcesses;
					if (percentFailed > s_fMaxMPIProcessFailures) {
						#if MPI_DEBUG
							printf("More than %0.0lf%% of processes failed. Sending die signal to other processes and exiting.\n", s_fMaxMPIProcessFailures * 100);
						#endif
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
			if (!unassigned.empty()) {
				pStructure = unassigned.front();
				unassigned.pop_front();
				id = pStructure->getId();
				#if MPI_DEBUG
					printf("Sending slave %d structure %d.\n", status.MPI_SOURCE, id);
				#endif
				assignments[status.MPI_SOURCE][id] = pStructure;
				++iAssignments;

				mpiUtil.nonBlockingSend(&id, 1, status.MPI_SOURCE, WORK_TAG);
			} else if (sentFinishMessage.find(status.MPI_SOURCE) == sentFinishMessage.end()) {
				#if MPI_DEBUG
					printf("There are no more structures. Sending slave %d the finish tag.\n", status.MPI_SOURCE);
				#endif

				mpiUtil.nonBlockingSend(0, 0, status.MPI_SOURCE, FINISH_TAG);
				sentFinishMessage[status.MPI_SOURCE] = true;
			}
		}
		m_bRunComplete = unassigned.empty() && iAssignments == 0;
		if (++iSaveCount >= m_iSaveFrequency || m_bRunComplete) {
			if (!m_pInput->save())
				return false;
			iSaveCount = 0;
		}
	} while (!m_bRunComplete);

	renameResultsFiles();
	return true;
}

bool Batch::runSlave() {
	if (m_bRunComplete)
		return true;
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
	MPI_Request request;
	int messageReceived = 0;
	bool initiateReceive = true;
	bool receivedFinishMessage = false;
	std::list<int> queue;

	getInitialAssignment(queue);

	while (true) {
		do {
			if (initiateReceive) {
				MPI_Irecv(&id, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
				initiateReceive = false;
			}
			if (queue.empty() && !receivedFinishMessage) {
				#if MPI_DEBUG
					printf("Slave %d waiting for a message.\n", m_iMpiRank);
				#endif
				MPI_Wait(&request,&status);
				messageReceived = 1;
			} else {
				#if MPI_DEBUG
					printf("Slave %d checking for a message.\n", m_iMpiRank);
				#endif
				MPI_Test(&request, &messageReceived, &status);
			}
			if (messageReceived) {
				initiateReceive = true;
				switch(status.MPI_TAG) {
				case WORK_TAG:
					queue.push_back(id);
					#if MPI_DEBUG
						printf("Slave %d received structure %d.\n", m_iMpiRank, id);
					#endif
					break;
				case DIE_TAG:
					#if MPI_DEBUG
						printf("Slave %d received die message. Exiting.\n", m_iMpiRank);
					#endif
					return false;
				case WALL_TIME_TAG:
					#if MPI_DEBUG
						printf("Slave %d received wall-time message. Exiting.\n", m_iMpiRank);
					#endif
					return true;
				case FINISH_TAG:
					#if MPI_DEBUG
						printf("Slave %d received finish message.\n", m_iMpiRank);
					#endif
					receivedFinishMessage = true;
					break;
				default:
					#if MPI_DEBUG
						printf("Slave %d received non-work message %d. Exiting.\n", m_iMpiRank, status.MPI_TAG);
					#endif
					return false;
				}
			#if MPI_DEBUG
			} else {
				printf("Slave %d did not receive a message.\n", m_iMpiRank);
			#endif
			}
		} while (messageReceived && !receivedFinishMessage);
		if (queue.empty())
			break;
		id = queue.front();
		queue.pop_front();
		pStructure = structureMap[id];
		#if MPI_DEBUG
			printf("Slave %d performing calculations on structure %d.\n", m_iMpiRank, pStructure->getId());
		#endif
		if (m_pEnergy->execute(*pStructure)) {
			iEnergyCalcFailures = 0;
			std::string xml;
			pStructure->save(xml);
			#if MPI_DEBUG
				printf("Slave %d reporting energy calculation results on structure %d.\n", m_iMpiRank, pStructure->getId());
			#endif
		    MPI_Send((void*)xml.c_str(), xml.length()+1, MPI_CHAR, 0, WORK_TAG, MPI_COMM_WORLD);
		} else {
			++iEnergyCalcFailures;
			temp = id;
			if (iEnergyCalcFailures <= s_iMaxEnergyCalcFailures) {
				#if MPI_DEBUG
					printf("Slave %d sending energy calculation failure message to master.\n", m_iMpiRank);
				#endif
				MPI_Send((void*)temp.c_str(), temp.length()+1, MPI_CHAR, 0, ENERGY_CAL_FAILURE_TAG, MPI_COMM_WORLD);
			} else {
				#if MPI_DEBUG
					printf("Slave %d sending die message to master and exiting.\n", m_iMpiRank);
				#endif
				MPI_Send((void*)temp.c_str(), temp.length()+1, MPI_CHAR, 0, DIE_TAG, MPI_COMM_WORLD);
				return false;
			}
		}
	}

	return true;
}

void Batch::getInitialAssignments(std::list<Structure*> &unassigned, std::map<int,Structure*>* assignments, unsigned int &iAssignments) {
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

	#if MPI_DEBUG
		if (m_iMpiRank == 0)
			printf("Target queue size: %d, Queue size: %d, Number to assign: %d.\n", m_targetQueueSize, queueSize, numberToAssign);
	#endif

	for (std::list<Structure*>::iterator it = m_structures.begin(); it != m_structures.end(); ++it)
		unassigned.push_back(*it);

	Structure* pStructure;
	iAssignments = 0;
	int id;
	j = 1;
	for (i = 1; i <= numberToAssign; ++i) {
		pStructure = unassigned.front();
		unassigned.pop_front();
		id = pStructure->getId();

		assignments[j][id] = pStructure;
		++iAssignments;

		++j;
		if (j >= m_iMpiProcesses)
			j = 1;
	}
}

void Batch::getInitialAssignment(std::list<int> &queue) {
	std::list<Structure*> unassigned;
	std::map<int,Structure*> assignments[m_iMpiProcesses];
	unsigned int iAssignments;
	getInitialAssignments(unassigned, assignments, iAssignments);
	std::map<int,Structure*>* pAssignments = &(assignments[m_iMpiRank]);
	for (std::map<int,Structure*>::const_iterator it = pAssignments->begin(); it != pAssignments->end(); ++it) {
		queue.push_back(it->second->getId());
	}
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
