
#include "structure.h"

Structure::Structure() {
	m_iNumberOfAtomGroups = 0;
	m_atomGroups = NULL;
	m_iNumberOfAtoms = 0;
	m_atomCoordinates = NULL;
	m_atomicNumbers = NULL;
}

Structure::~Structure() {
	cleanUp();
}

void Structure::cleanUp() {
	m_iNumberOfAtomGroups = 0;
	if (m_atomGroups != NULL) {
		delete[] m_atomGroups;
		m_atomGroups = NULL;
	}
	m_iNumberOfAtoms = 0;
	if (m_atomCoordinates != NULL) {
		delete[] m_atomCoordinates;
		m_atomCoordinates = NULL;
	}
	if (m_atomicNumbers != NULL) {
		delete[] m_atomicNumbers;
		m_atomicNumbers = NULL;
	}
}

void Structure::setAtoms(unsigned int numAtomGroupTemplates, const AtomGroupTemplate* atomGroupTemplates) {
	unsigned int i, j, k;

	cleanUp();
	m_iNumberOfAtomGroups = 0;
	m_iNumberOfAtoms = 0;
	for (i = 0; i < numAtomGroupTemplates; ++i) {
		m_iNumberOfAtomGroups += atomGroupTemplates[i].m_iNumber;
		m_iNumberOfAtoms += atomGroupTemplates[i].m_iNumber * atomGroupTemplates[i].m_atomicNumbers.size();
	}

	k = 0;
	m_atomGroups = new AtomGroup[m_iNumberOfAtomGroups];
	for (i = 0; i < numAtomGroupTemplates; ++i)
		for (j = 1; j <= atomGroupTemplates[i].m_iNumber; ++j)
			m_atomGroups[k++].setAtoms(atomGroupTemplates[i]);
	initCoordinateRefs();
}

void Structure::setAtoms(unsigned int numAtoms,
		const COORDINATE3 *cartesianPoints, const unsigned int* atomicNumbers) {
	if (numAtoms != m_iNumberOfAtoms) {
		cleanUp();
		m_iNumberOfAtoms = numAtoms;
		m_iNumberOfAtomGroups = 1;
		m_atomGroups = new AtomGroup[m_iNumberOfAtomGroups];
		m_atomGroups[0].setAtoms(numAtoms, cartesianPoints, atomicNumbers);
		initCoordinateRefs();
	} else {
		unsigned int i, j;
		j = 0;
		for (i = 0; i < m_iNumberOfAtomGroups; ++i) {
			m_atomGroups[i].setAtoms(m_atomGroups[i].getNumberOfAtoms(), &cartesianPoints[j], &atomicNumbers[j]);
			j += m_atomGroups[i].getNumberOfAtoms();
		}
	}
}

void Structure::initCoordinateRefs() {
	unsigned int i, j, k;
	if (m_atomCoordinates != NULL)
		delete[] m_atomCoordinates;
	m_atomCoordinates = new const COORDINATE4*[m_iNumberOfAtoms];
	if (m_atomicNumbers != NULL)
		delete[] m_atomicNumbers;
	m_atomicNumbers = new unsigned int[m_iNumberOfAtoms];
	const COORDINATE4* coordinates;
	k = 0;
	for (i = 0; i < m_iNumberOfAtomGroups; ++i) {
		memcpy(&m_atomicNumbers[k], m_atomGroups[i].getAtomicNumbers(), sizeof(unsigned int)*m_atomGroups[i].getNumberOfAtoms());
		coordinates = m_atomGroups[i].getGlobalAtomCoordinates();
		for (j = 0; j < m_atomGroups[i].getNumberOfAtoms(); ++j)
			m_atomCoordinates[k++] = &coordinates[j];
	}
}

bool Structure::load(TiXmlElement *pStructureElem, const Strings* messages) {
	return true;
}

bool Structure::save(TiXmlElement *pParentElem, const Strings* messages) {
	return true;
}
