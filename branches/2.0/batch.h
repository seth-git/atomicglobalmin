
#ifndef BATCH_H_
#define BATCH_H_

#include "action.h"
#include "structure.h"

class Batch : public Action {
public:
	unsigned int m_targetQueueSize;
	Batch(Input* input);
	~Batch();
	bool loadSetup(const rapidxml::xml_node<>* pSetupElem);
	bool saveSetup(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pBatchElem);
	bool loadResume(const rapidxml::xml_node<>* pResumeElem);
	bool saveResume(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pBatchElem);

	bool runMaster();
	bool runSlave();

protected:
	void getInitialAssignments(std::list<Structure*> &unassigned, std::map<int,Structure*>* assignments, unsigned int &iAssignments);
	void getInitialAssignment(std::list<int> &queue);
	void processResult(Structure* structure);

	static const char* s_elementNames[];
	static const unsigned int s_minOccurs[];
	static const char* s_setupAttNames[];
	static const bool s_setupAttReq[];
	static const char* s_setupAttDef[];
	static const unsigned int s_resumeMinOccurs[];
	static const unsigned int s_structuresMinOccurs[];
	static const unsigned int s_structuresMaxOccurs[];
};

#endif /* BATCH_H_ */
