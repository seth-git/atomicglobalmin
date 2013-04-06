
#ifndef __ATOM_GROUP_H_
#define __ATOM_GROUP_H_

#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdElementUtil.h"
#include "xsd/xsdTypeUtil.h"
#include "translation/strings.h"
#include "atomGroupTemplate.h"
#include "handbook/handbook.h"
#include <cmath>
#include "matrix/matrix.h"

#define CARTESIAN 0

class AtomGroup {
protected:
	unsigned int m_iNumberOfAtoms;
	unsigned int* m_atomicNumbers;
	COORDINATE4* m_localPoints;
	COORDINATE4* m_globalPoints;

	COORDINATE3 m_centerOfMass;
	COORDINATE3 m_angles;
	COORDINATE3 m_sinAngles;
	COORDINATE3 m_cosAngles;
	FLOAT m_rotationMatrixX[MATRIX_WIDTH][MATRIX_WIDTH];
	FLOAT m_rotationMatrixY[MATRIX_WIDTH][MATRIX_WIDTH];
	FLOAT m_rotationMatrixZ[MATRIX_WIDTH][MATRIX_WIDTH];
	FLOAT m_matrixLocalToGlobal[MATRIX_WIDTH][MATRIX_WIDTH];

	bool m_bFrozen; // If frozen, this indicates that coordinates shouldn't be updated until frozen is set to false

/*	vector<int>* m_bondedAtomIndices;
	vector<Bond*> m_bonds;
	vector<int> m_rotatableBonds; // index to m_bonds
	RingSet m_ringSet;
	FLOAT m_fBondRotationAngle;
	vector<int> m_rotationPositionMax;
	vector<int> m_rotationPosition; // values are: 0 - m_rotationPositionMax[i]
*/

public:
	AtomGroup();
	~AtomGroup();
	void setAtoms(const AtomGroupTemplate &atomGroupTemplate);
	void setAtoms(unsigned int numAtoms, const COORDINATE4* cartesianPoints,
			const unsigned int* atomicNumbers);

	bool load(TiXmlElement *pAtomGroupElem, const Strings* messages);
	bool save(TiXmlElement *pParentElem, const Strings* messages);

	void copy(AtomGroup &atomGroup);

	unsigned int getNumberOfAtoms() const { return m_iNumberOfAtoms; }

	const FLOAT* getCenter() const { return (const FLOAT*)m_centerOfMass; }
	void setCenter(COORDINATE3 centerOfMass) { memcpy(m_centerOfMass, centerOfMass, sizeof(COORDINATE3)); }

	FLOAT getAngleX() const { return m_angles[0]; }
	void setAngleX(FLOAT radians) { m_angles[0] = radians; }

	FLOAT getAngleY() const { return m_angles[1]; }
	void setAngleY(FLOAT radians) { m_angles[1] = radians; }

	FLOAT getAngleZ() const { return m_angles[2]; }
	void setAngleZ(FLOAT radians) { m_angles[2] = radians; }

	const COORDINATE4* getLocalAtomCoordinates() const { return m_localPoints; }
	const COORDINATE4* getGlobalAtomCoordinates() const { return m_globalPoints; }
	const unsigned int* getAtomicNumbers() const { return m_atomicNumbers; }

	void setFrozen(bool newValue) { m_bFrozen = newValue; }
	bool getFrozen() { return m_bFrozen; }

	/////////////////////////////////////////////////////////////////////
	// Purpose: This function creates the rotation matrix for a molecule.
	//    This rotation matrix can translate any point from coordinates local
	//    to the molecule to global coordinates.  This is accomplished simply by
	//    mutliplying a point local molecule by the rotation matrix.
	// Parameters: none
	// Returns: nothing
	void initRotationMatrix();

	/////////////////////////////////////////////////////////////////////
	// Purpose: This function recalulates the positions of individual
	//    atoms after the molecule has been rotated or moved.
	// Parameters: none
	// Returns: true if there were no errors
	bool localToGlobal();

private:
	void cleanUp();
	void setLocalCenterOfMassToZero();
};

#endif

