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
	unsigned int* m_atomGroupIndices; // array giving the starting atom index of each atom group
	unsigned int m_iNumberOfAtoms;
	const COORDINATE4** m_atomCoordinates; // array with each element pointing to a COORDINATE4
	const COORDINATE4** m_localAtomCoordinates; // array with each element pointing to a COORDINATE4 (untranslated and unrotated)
	unsigned int* m_atomicNumbers;
	FLOAT** m_atomDistanceMatrix;
	FLOAT** m_atomGroupDistanceMatrix;

	FLOAT m_energy;
	bool m_bIsTransitionState;

public:
	Structure();
	~Structure();
	bool load(TiXmlElement *pStructureElem, const Strings* messages);
	bool save(TiXmlElement *pParentElem, const Strings* messages) const;

	void setAtoms(unsigned int numAtoms, const COORDINATE4 *cartesianPoints,
			const unsigned int* atomicNumbers);

	/**************************************************************************
	 * Purpose: This method assigns atom groupings to the structure.  When this
	 *    method is called, if the structure has (1) one atom group, (2) the
	 *    same total number of atoms as the atom group templates, and (3) the
	 *    same atomic numbers as the atom group templates, this method enforces
	 *    the grouping while preserving the coordinates.  Otherwise, the
	 *    coordinates will be erased.
	 * Parameters: numAtomGroupTemplates - the size of atomGroupTemplates
	 *             atomGroupTemplates - an array with the replacement grouping
	 * Returns: nothing
	 *************************************************************************/
	void setAtomGroups(unsigned int numAtomGroupTemplates,
			const AtomGroupTemplate* atomGroupTemplates);

	/**************************************************************************
	 * Purpose: This method inserts an atom group at the specified index.
	 * Parameters: atomGroupTemplate - the template of the atom group to insert
	 *             index - the position of the new atom group
	 * Returns: nothing
	 *************************************************************************/
	void insertAtomGroup(AtomGroupTemplate &atomGroupTemplate, unsigned int index);

	/**************************************************************************
	 * Purpose: This method deletes an atom group at the specified index.
	 * Parameters: index - the position of the atom group to be removed
	 * Returns: nothing
	 *************************************************************************/
	void deleteAtomGroup(unsigned int index);

	void copy(Structure &structure);

	unsigned int getNumberOfAtomGroups() const { return m_iNumberOfAtomGroups; }
	const AtomGroup* getAtomGroups() const { return m_atomGroups; }
	AtomGroup* getAtomGroup(unsigned int index) { return &(m_atomGroups[index]); } // be careful with this method
	unsigned int getNumberOfAtoms() const { return m_iNumberOfAtoms; }
	const unsigned int* getAtomicNumbers() const { return m_atomicNumbers; }
	const COORDINATE4* const* getAtomCoordinates() const { return (const COORDINATE4* const*) m_atomCoordinates; }
	const COORDINATE4* const* getLocalAtomCoordinates() const { return (const COORDINATE4* const*) m_localAtomCoordinates; } // Untranslated and unrotated
	const FLOAT* const* getAtomDistanceMatrix() const { return (const FLOAT* const*)m_atomDistanceMatrix; }
	const FLOAT* const* getAtomGroupDistanceMatrix() const { return (const FLOAT* const*)m_atomGroupDistanceMatrix; }

	bool getIsTransitionState() const { return m_bIsTransitionState; }

	void setIsTransitionState(bool isTransitionState) { m_bIsTransitionState = isTransitionState; }

	FLOAT getEnergy() const { return m_energy; }

	void setEnergy(FLOAT energy) { m_energy = energy; }

	void clear();

	/**************************************************************************
	 * Purpose: This method applies translation and rotation operations,
	 *    updating the m_atomCoordinates.
	 */
	void applyOperations();

	void updateAtomDistanceMatrix();

	FLOAT findClosestDistance(unsigned int iAtomGroup1, unsigned int iAtomGroup2);

	void updateAtomGroupDistanceMatrix();

	void update() {
		applyOperations();
		updateAtomDistanceMatrix();
		updateAtomGroupDistanceMatrix();
	}

	/**************************************************************************
	 * Purpose: This returns the starting atom index of each atom group
	 * Parameters: none
	 * Returns: an array of size getNumberOfAtomGroups() where each value is
	 *    an index for getAtomicNumbers() or getAtomCoordinates() for a group.
	 *************************************************************************/
	const unsigned int* getAtomGroupIndices() { return m_atomGroupIndices; }

	static unsigned int PRINT_RADIANS;
	static unsigned int PRINT_ENERGY;
	static unsigned int PRINT_LOCAL_COORDINATES;
	static unsigned int PRINT_BOND_LENGTHS;
	static unsigned int PRINT_DISTANCE_MATRIX;

	void print() const { print(PRINT_ENERGY); }

	void print(unsigned int flags) const;

	static void printDistanceMatrix(const FLOAT* const * matrix,
			const unsigned int* atomicNumbers, unsigned int size);

private:
	void initCoordinateRefs();
	bool atomsMatch(unsigned int numAtomGroupTemplates,
			const AtomGroupTemplate* atomGroupTemplates);
};

#endif /* STRUCTURE_H_ */
