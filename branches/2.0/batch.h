
#ifndef BATCH_H_
#define BATCH_H_

#include "action.h"
#include "structure.h"

class Batch : public Action {
public:
	unsigned int m_targetQueueSize;
	Batch(Input* input);
	~Batch();
	bool loadSetup(TiXmlElement *pSetupElem, const Strings* messages);
	bool saveSetup(TiXmlElement *pBatchElem, const Strings* messages);
	bool loadResume(TiXmlElement *pResumeElem, const Strings* messages);
	bool saveResume(TiXmlElement *pResumeElem, const Strings* messages);

	bool runMaster();
	bool runSlave();

private:
	void processResult(Structure* structure);

	static const unsigned int s_minOccurs[];
	static const bool s_setupAttReq[];
	static const char* s_setupAttDef[];
	static const unsigned int s_resumeMinOccurs[];
	static const unsigned int s_structuresMinOccurs[];
	static const unsigned int s_structuresMaxOccurs[];
};

#endif /* BATCH_H_ */
