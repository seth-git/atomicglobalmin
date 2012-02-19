
#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdio.h>
#include "../tinyxml/tinyxml.h"

using namespace std;

class Input {
	public:
		double x,y,z;
		const char *value;
		TiXmlDocument *m_pXMLDocument;

		Input() : x(0), y(0), z(0), m_pXMLDocument(NULL)
		{
		}

		~Input()
		{
			cleanUp();
		}

		void cleanUp();
		void save(const char* pFilename);
		void load(const char* pFilename);
};

#endif
