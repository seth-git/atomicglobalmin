
#include "structure.h"

Structure::Structure() {
	m_iNumberOfAtomGroups = 0;
	m_atomGroups = NULL;
	m_iNumberOfAtoms = 0;
	m_atomCoordinates = NULL;
	m_atomicNumbers = NULL;
}

Structure::~Structure() {
	clear();
}

void Structure::clear() {
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

void Structure::setAtoms(unsigned int numAtoms,
		const COORDINATE4 *cartesianPoints, const unsigned int* atomicNumbers) {
	if (numAtoms != m_iNumberOfAtoms) {
		clear();
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

void Structure::setAtomGroups(unsigned int numAtomGroupTemplates, const AtomGroupTemplate* atomGroupTemplates) {
	unsigned int i, j, k, l;
	const AtomGroupTemplate* agtemp;
	const AtomGroup* ag;

	unsigned int iNumberOfAtomGroups = 0;
	unsigned int iNumberOfAtoms = 0;
	for (i = 0; i < numAtomGroupTemplates; ++i) {
		agtemp = &atomGroupTemplates[i];
		iNumberOfAtomGroups += agtemp->m_iNumber;
		iNumberOfAtoms += agtemp->m_iNumber * agtemp->m_atomicNumbers.size();
	}
	AtomGroup* atomGroups = new AtomGroup[iNumberOfAtomGroups];

	if (atomsMatch(numAtomGroupTemplates, atomGroupTemplates)) {
		// Copy the coordinates
		COORDINATE4 localAtomCoordinates[m_iNumberOfAtoms];
		for (i = 0, j = 0; i < m_iNumberOfAtomGroups; ++i) {
			ag = &m_atomGroups[i];
			memcpy(&localAtomCoordinates[j], ag->getLocalAtomCoordinates(), sizeof(COORDINATE4) * ag->getNumberOfAtoms());
			j += ag->getNumberOfAtoms();
		}

		for (i = 0, k = 0, l = 0; i < numAtomGroupTemplates; ++i) {
			agtemp = &atomGroupTemplates[i];
			for (j = 1; j <= agtemp->m_iNumber; ++j) {
				atomGroups[k++].setAtoms(agtemp->m_atomicNumbers.size(), &localAtomCoordinates[l], &m_atomicNumbers[l]);
				l += agtemp->m_atomicNumbers.size();
			}
		}
	} else {
		for (i = 0, k = 0; i < numAtomGroupTemplates; ++i) {
			agtemp = &atomGroupTemplates[i];
			for (j = 1; j <= agtemp->m_iNumber; ++j)
				atomGroups[k++].setAtoms(*agtemp);
		}
	}

	clear();
	m_iNumberOfAtomGroups = iNumberOfAtomGroups;
	m_iNumberOfAtoms = iNumberOfAtoms;
	m_atomGroups = atomGroups;
	initCoordinateRefs();
}

bool Structure::atomsMatch(unsigned int numAtomGroupTemplates,
		const AtomGroupTemplate* atomGroupTemplates) {

	unsigned int groupi, groupj, groupn;
	const AtomGroupTemplate* templatei;
	unsigned int i;

	unsigned int totalAtoms = 0;
	unsigned int atomi = 0;
	for (groupi = 0; groupi < numAtomGroupTemplates; ++groupi) {
		templatei = &atomGroupTemplates[groupi];
		for (groupj = 0, groupn = templatei->m_iNumber; groupj < groupn; ++groupj) {
			totalAtoms += groupn;
			if (totalAtoms > m_iNumberOfAtoms)
				return false;
			for (i = 0; i < templatei->m_atomicNumbers.size(); ++i)
				if (m_atomicNumbers[atomi++] != templatei->m_atomicNumbers[i])
					return false;
		}
	}
	if (totalAtoms != m_iNumberOfAtoms)
		return false;
	return true;
}

void Structure::insertAtomGroup(AtomGroupTemplate &atomGroupTemplate, unsigned int index) {
	unsigned int iNumberOfAtomGroups = m_iNumberOfAtomGroups + 1;
	AtomGroup* atomGroups = new AtomGroup[iNumberOfAtomGroups];
	unsigned int iNumberOfAtoms = m_iNumberOfAtoms+atomGroupTemplate.m_atomicNumbers.size();

	unsigned int i;
	for (i = 0; i < index; ++i)
		atomGroups[i].copy(m_atomGroups[i]);
	atomGroups[index].setAtoms(atomGroupTemplate);
	for (i = index+1; i < iNumberOfAtomGroups; ++i)
		atomGroups[i].copy(m_atomGroups[i-1]);

	clear();
	m_iNumberOfAtomGroups = iNumberOfAtomGroups;
	m_iNumberOfAtoms = iNumberOfAtoms;
	m_atomGroups = atomGroups;
	initCoordinateRefs();
}

void Structure::deleteAtomGroup(unsigned int index) {
	unsigned int iNumberOfAtomGroups = m_iNumberOfAtomGroups - 1;
	AtomGroup* atomGroups = new AtomGroup[iNumberOfAtomGroups];
	unsigned int iNumberOfAtoms = m_iNumberOfAtoms - m_atomGroups[index].getNumberOfAtoms();

	unsigned int i;
	for (i = 0; i < index; ++i)
		atomGroups[i].copy(m_atomGroups[i]);
	for (i = index; i < iNumberOfAtomGroups; ++i)
		atomGroups[i].copy(m_atomGroups[i+1]);

	clear();
	m_iNumberOfAtomGroups = iNumberOfAtomGroups;
	m_iNumberOfAtoms = iNumberOfAtoms;
	m_atomGroups = atomGroups;
	initCoordinateRefs();
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

bool Structure::save(TiXmlElement *pParentElem, const Strings* messages) const {
	return true;
}

void Structure::copy(Structure &structure) {
	clear();

	m_iNumberOfAtomGroups = structure.m_iNumberOfAtomGroups;
	m_atomGroups = new AtomGroup[m_iNumberOfAtomGroups];
	for (unsigned int i = 0; i < m_iNumberOfAtomGroups; ++i)
		m_atomGroups[i].copy(structure.m_atomGroups[i]);

	m_iNumberOfAtoms = structure.m_iNumberOfAtoms;
	initCoordinateRefs();

	m_energy = structure.m_energy;
	m_bIsTransitionState = structure.m_bIsTransitionState;
}
