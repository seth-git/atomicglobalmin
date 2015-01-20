
#ifndef BATCH_H_
#define BATCH_H_

#include "action.h"
#include "structure.h"

class Batch : public Action {
public:
	Batch(Input* input);
	~Batch();
	bool loadSetup(const rapidxml::xml_node<>* pSetupElem);
	bool saveSetup(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pBatchElem);
	bool loadResume(const rapidxml::xml_node<>* pResumeElem);
	bool saveResume(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pBatchElem);

	bool runMaster();
	bool runSlave();

	// These methods are public for testing purposes
	void processResult(Structure* structure);

protected:
	void getInitialAssignments(std::map<int,Structure*>* assignments, std::list<int> &queue, std::map<int,Structure*> &structureMap);

	static const int DIE_TAG;
	static const int WORK_TAG;
	static const int ENERGY_CAL_FAILURE_TAG;
	static const int NO_MORE_STRUCTURES_TAG;
	static const int FINISH_TAG;

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
