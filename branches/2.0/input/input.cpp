
#include "input.h"

void Input::cleanUp() {
	if (m_pXMLDocument != NULL) {
		delete m_pXMLDocument;
	}
}

void Input::load(const char* pFilename)
{
	cleanUp();
	m_pXMLDocument = new TiXmlDocument(pFilename);
	if (!m_pXMLDocument->LoadFile()) {
		printf("Failed to load file \"%s\"\n", pFilename);
		return;
	}
	printf("Opening %s...\n", pFilename);
	TiXmlHandle hDoc(m_pXMLDocument);
	TiXmlElement* pElem;
	
	pElem=hDoc.FirstChild("atom").Element();
	if (!pElem) return;
	pElem->QueryDoubleAttribute("x", &x);
	pElem->QueryDoubleAttribute("y", &y);
	pElem->QueryDoubleAttribute("z", &z);
	value = pElem->Attribute("value");
	printf("x: %lf, y: %lf, z: %lf, val: %s\n", x, y, z, value);
}

void Input::save(const char* pFilename)
{
	if (m_pXMLDocument == NULL) {
		return;
	}
	printf("Saving %s...\n", pFilename);
	m_pXMLDocument->SaveFile(pFilename);
}
