
#include "batch.h"
#include "input.h"
#include "xsd/mpiUtils.h"

Batch::Batch(Input* input) : Action(input)
{
}

Batch::~Batch()
{
	// Note: clear is called from the Action class
}

const char*     Batch::s_elementNames[] = {strings::xStructuresTemplate};
const unsigned int Batch::s_minOccurs[] = {1};

const char* Batch::s_setupAttNames[] = {strings::xSaveFrequency};
const bool Batch::s_setupAttReq[] =  {true};
const char* Batch::s_setupAttDef[] = {"1"};

bool Batch::loadSetup(const rapidxml::xml_node<>* pSetupElem)
{
	XsdAttributeUtil setupAttUtil(s_setupAttNames, s_setupAttReq, s_setupAttDef);
	if (!setupAttUtil.process(pSetupElem))
		return false;
	const char** setupAttValues = setupAttUtil.getAllAttributes();
	if (!XsdTypeUtil::getPositiveInt(setupAttValues[0], m_iSaveFrequency, s_setupAttNames[0], pSetupElem))
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
		if (!m_structuresTemplate.initializeStructures(m_structures, m_pConstraints, false))
			return false;
		for (std::list<Structure*>::iterator it = m_structures.begin(); it != m_structures.end(); ++it) {
			std::string id = ExternalEnergy::ToString((*it)->getId());
			(*it)->m_sFilePrefix = pStructure + id;
		}
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

const int Batch::DIE_TAG = 1;
const int Batch::WORK_TAG = 2;
const int Batch::FINISH_TAG = 3;

bool Batch::runMaster() {
	if (m_bRunComplete) {
		puts(strings::RunCompleted);
		return true;
	}
	if (!Action::run())
		return false;

	std::map<int,Structure*> assignments[m_iMpiProcesses];
	std::list<int> queue;
	std::map<int,Structure*> structureMap;
	getInitialAssignments(assignments, queue, structureMap);

	char* xml;
	bool success;
	MPI_Status status;
	unsigned int iEnergyCalcFailures = 0;
	unsigned int iFailedProcesses = 0;
	unsigned int iStructuresToSave = 0;
	MpiUtil mpiUtil;
	Structure* pStructure;
	int id;
	bool bTimeToQuit = false;
	bool bNotifiedSlavesOfTimeToQuit = false;
	bool sentFinish[m_iMpiProcesses];
	bool finished[m_iMpiProcesses];
	// Initialize to false (false == 0)
	memset(sentFinish, 0, sizeof(sentFinish));
	memset(finished, 0, sizeof(finished));
	unsigned int iSlavesFinished = 0;
	bool bBlockingReceive;
	do {
		if (!bTimeToQuit && !queue.empty()) {
			pStructure = structureMap[queue.front()];
			queue.pop_front();
			#if MPI_DEBUG
				printf("Master performing calculations on %d.\n", pStructure->getId());
			#endif
			if (m_pEnergy->execute(*pStructure)) {
				iEnergyCalcFailures = 0;
				processResult(pStructure);
				++iStructuresToSave;
			} else {
				++iEnergyCalcFailures;
				queue.push_back(pStructure->getId());
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

		while (!(iSlavesFinished >= m_iMpiProcesses-1 && (m_structures.empty() || bTimeToQuit))) {
			if (!bTimeToQuit)
				bTimeToQuit = calculateTimeToQuit();
			if (1 == m_iMpiProcesses)
				break;
			bBlockingReceive = (!bTimeToQuit && queue.empty() && iStructuresToSave < m_iSaveFrequency) ||
					           (bTimeToQuit && bNotifiedSlavesOfTimeToQuit && 0 == iStructuresToSave);
			#if MPI_DEBUG
				if (bBlockingReceive)
					printf("Master waiting for a message.\n");
				else
					printf("Master checking for messages.\n");
			#endif
			if (!MpiUtil::receiveString(MPI_ANY_SOURCE, xml, status, bBlockingReceive))
				break;
			switch (status.MPI_TAG) {
			case WORK_TAG:
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
				++iStructuresToSave;

				if (!bTimeToQuit && assignments[status.MPI_SOURCE].empty()) {
					if (!queue.empty()) {
						pStructure = structureMap[queue.front()];
						queue.pop_front();
						id = pStructure->getId();
						assignments[status.MPI_SOURCE][id] = pStructure;
						#if MPI_DEBUG
							printf("Sending slave %d structure %d.\n", status.MPI_SOURCE, id);
						#endif
						MPI_Send(&id, 1, MPI_INT, status.MPI_SOURCE, WORK_TAG, MPI_COMM_WORLD);
					} else if (!finished[status.MPI_SOURCE] && !sentFinish[status.MPI_SOURCE]) {
						#if MPI_DEBUG
							printf("Sending finish message to slave %d...\n", status.MPI_SOURCE);
						#endif
						MPI_Send(0, 0, MPI_INT, status.MPI_SOURCE, FINISH_TAG, MPI_COMM_WORLD);
						sentFinish[status.MPI_SOURCE] = true;
					}
				}

				break;
			case FINISH_TAG:
				finished[status.MPI_SOURCE] = true;
				++iSlavesFinished;
				if (!sentFinish[status.MPI_SOURCE])
					bTimeToQuit = true;
				#if MPI_DEBUG
					printf("Master received finish signal from slave %d.\n", status.MPI_SOURCE);
				#endif
				break;
			case DIE_TAG:
				#if MPI_DEBUG
					printf("Master received die message from process %d.\n", status.MPI_SOURCE);
				#endif
				finished[status.MPI_SOURCE] = true;
				++iSlavesFinished;
				++iFailedProcesses;
				FLOAT percentFailed = (FLOAT)iFailedProcesses / (FLOAT)m_iMpiProcesses;
				if (percentFailed > s_fMaxMPIProcessFailures) {
					#if MPI_DEBUG
						printf("More than %0.0lf%% of processes failed. Sending die signal to other processes and exiting.\n", s_fMaxMPIProcessFailures * 100);
					#endif
					for (unsigned int i = 1; i < m_iMpiProcesses; ++i)
						if (!finished[i])
							MPI_Send(0, 0, MPI_INT, i, DIE_TAG, MPI_COMM_WORLD);
					return false;
				} else {
					std::map<int,Structure*>* pAssignments = &(assignments[status.MPI_SOURCE]);
					for (std::map<int,Structure*>::iterator it = pAssignments->begin(); it != pAssignments->end(); ++it) {
						assignments[0][it->first] = it->second;
						queue.push_back(it->first);
					}
					pAssignments->clear();
				}
				break;
			}
		}

		m_bRunComplete = m_structures.empty();
		if ((!bTimeToQuit && iStructuresToSave >= m_iSaveFrequency) ||
				((bTimeToQuit || m_bRunComplete) && iStructuresToSave > 0)) {
			renameResultsFiles();
			if (!m_pInput->save())
				return false;
			iStructuresToSave = 0;
		}

		if (bTimeToQuit && !bNotifiedSlavesOfTimeToQuit) {
			for (int i = 1; i < (signed int)m_iMpiProcesses; ++i)
				if (!finished[i] && !sentFinish[i]) {
					#if MPI_DEBUG
						printf("Sending finish message to slave %d...\n", i);
					#endif
					MPI_Send(0, 0, MPI_INT, i, FINISH_TAG, MPI_COMM_WORLD);
					sentFinish[i] = true;
				}
			bNotifiedSlavesOfTimeToQuit = true;
		}
	} while (!(iSlavesFinished >= m_iMpiProcesses-1 && (m_bRunComplete || bTimeToQuit)));

	return true;
}

bool Batch::runSlave() {
	if (m_bRunComplete)
		return true;
	if (!Action::run())
		return false;

	int id;
	MPI_Status status;
	Structure* pStructure;
	unsigned int iEnergyCalcFailures = 0;
	int messageReceived = 0;
	bool finish = false;
	std::map<int,Structure*> assignments[m_iMpiProcesses];
	std::list<int> queue;
	std::map<int,Structure*> structureMap;
	getInitialAssignments(assignments, queue, structureMap);

	while (true) {
		do {
			if (queue.empty()) {
				finish = calculateTimeToQuit();
				if (finish)
					messageReceived = 0;
				else {
					#if MPI_DEBUG
						printf("Slave %d waiting for a message.\n", m_iMpiRank);
					#endif
					messageReceived = MpiUtil::receiveInt(0, &id, status, true);
				}
			} else {
				#if MPI_DEBUG
					printf("Slave %d checking for a message.\n", m_iMpiRank);
				#endif
				messageReceived = MpiUtil::receiveInt(0, &id, status, false);
			}
			if (messageReceived) {
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
				case FINISH_TAG:
					#if MPI_DEBUG
						printf("Slave %d received finish message.\n", m_iMpiRank);
					#endif
					finish = true;
					break;
				default:
					#if MPI_DEBUG
						printf("Slave %d received non-work message %d. Exiting.\n", m_iMpiRank, status.MPI_TAG);
					#endif
					return false;
				}
			}
		} while (messageReceived && !finish);
		if (!finish)
			finish = calculateTimeToQuit();
		if (finish) {
			#if MPI_DEBUG
				printf("Slave %d sending finish message to the master and exiting.\n", m_iMpiRank);
			#endif
			MPI_Send(NULL, 0, MPI_CHAR, 0, FINISH_TAG, MPI_COMM_WORLD);
			break;
		}
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
			#if MPI_DEBUG
				printf("Slave %d failed to calculate energy for structure %d.\n", m_iMpiRank, id);
			#endif
			++iEnergyCalcFailures;
			queue.push_back(id);
			if (iEnergyCalcFailures > s_iMaxEnergyCalcFailures) {
				#if MPI_DEBUG
					printf("Slave %d sending die message to master.\n", m_iMpiRank);
				#endif
				MPI_Send(0, 0, MPI_CHAR, 0, DIE_TAG, MPI_COMM_WORLD);
				return false;
			}
		}
	}

	return true;
}

void Batch::getInitialAssignments(std::map<int,Structure*>* assignments, std::list<int> &queue, std::map<int,Structure*> &structureMap) {
	Structure* pStructure;
	unsigned int i = m_iMpiProcesses-1;
	for (std::list<Structure*>::iterator it = m_structures.begin(); it != m_structures.end(); ++it) {
		pStructure = *it;
		assignments[i][pStructure->getId()] = pStructure;
		if (i == 0)
			i = m_iMpiProcesses-1;
		else
			--i;
	}
	std::map<int,Structure*>* pAssignments = &(assignments[m_iMpiRank]);
	for (std::map<int,Structure*>::const_iterator it = pAssignments->begin(); it != pAssignments->end(); ++it)
		queue.push_back(it->second->getId());
	pAssignments->clear();
	for (std::list<Structure*>::iterator it = m_structures.begin(); it != m_structures.end(); it++)
		structureMap[(*it)->getId()] = *it;
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
