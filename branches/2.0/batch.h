/*
 * batch.h
 *
 *  Created on: Sep 13, 2013
 *      Author: sethcall
 */

#ifndef BATCH_H_
#define BATCH_H_

#include "action.h"
#include "structure.h"

class Batch : public Action {
public:
	Batch(Input* input);
	~Batch();
	bool loadSetup(TiXmlElement *pSetupElem, const Strings* messages);
	bool saveSetup(TiXmlElement *pBatchElem, const Strings* messages);
	bool loadResume(TiXmlElement *pResumeElem, const Strings* messages);
	bool saveResume(TiXmlElement *pResumeElem, const Strings* messages);
	bool run();

private:
	static const unsigned int s_minOccurs[];
	static const bool s_setupAttReq[];
	static const char* s_setupAttDef[];
	static const unsigned int s_resumeMinOccurs[];
	static const unsigned int s_structuresMinOccurs[];
	static const unsigned int s_structuresMaxOccurs[];

	void cleanUp();
};

#endif /* BATCH_H_ */
