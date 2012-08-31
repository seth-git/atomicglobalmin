
#ifndef __ACTION_H_
#define __ACTION_H_

#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdElementUtil.h"
#include "xsd/xsdTypeUtil.h"
#include "translation/strings.h"

class IAction {
	public:
		virtual bool load(TiXmlElement *pActionElem, const Strings* messages) = 0;
		virtual bool save(TiXmlElement *pActionElem, const Strings* messages) = 0;
		virtual bool run() = 0;
};

#endif
