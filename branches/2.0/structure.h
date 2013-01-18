/*
 * structure.h
 *
 *  Created on: Dec 26, 2012
 *      Author: sethcall
 */

#ifndef STRUCTURE_H_
#define STRUCTURE_H_

#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdElementUtil.h"
#include "xsd/xsdTypeUtil.h"
#include "translation/strings.h"
#include "atomGroup.h"

class Structure {
protected:
	unsigned int m_iNumberOfAtomGroups;
	AtomGroup* m_atomGroups;
	unsigned int m_iNumberOfAtoms;
	const COORDINATE4** m_atomCoordinates;
	unsigned int* m_atomicNumbers;

	FLOAT m_energy;
	bool m_bIsTransitionState;

public:
	Structure();
	~Structure();
	bool load(TiXmlElement *pStructureElem, const Strings* messages);
	bool save(TiXmlElement *pParentElem, const Strings* messages);

	void setAtoms(unsigned int numAtomGroupTemplates, const AtomGroupTemplate* atomGroupTemplates);
	void setAtoms(unsigned int numAtoms, const COORDINATE3 *cartesianPoints,
			const unsigned int* atomicNumbers);

	unsigned int getNumberOfAtomGroups() { return m_iNumberOfAtomGroups; }
	unsigned int getNumberOfAtoms() { return m_iNumberOfAtoms; }
	const unsigned int* getAtomicNumbers() { return m_atomicNumbers; }
	const COORDINATE4** getAtomCoordinates() { return m_atomCoordinates; }

    bool getIsTransitionState() const { return m_bIsTransitionState; }

    void setIsTransitionState(bool isTransitionState) { m_bIsTransitionState = isTransitionState; }

    FLOAT getEnergy() const { return m_energy; }

    void setEnergy(FLOAT energy) { m_energy = energy; }

private:
	void cleanUp();
	void initCoordinateRefs();

};

#endif /* STRUCTURE_H_ */
