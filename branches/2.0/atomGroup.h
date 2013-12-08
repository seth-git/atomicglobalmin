
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
	FLOAT m_translationMatrix[MATRIX_WIDTH][MATRIX_WIDTH];
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
	void setCenter(COORDINATE3 centerOfMass) { memcpy(m_centerOfMass, centerOfMass, SIZEOF_COORDINATE3); }

	const FLOAT* getAngles() const { return m_angles; }
	void setAngles(COORDINATE3 angles) { memcpy(m_angles, angles, SIZEOF_COORDINATE3); }

	const COORDINATE4* getLocalAtomCoordinates() const { return m_localPoints; }
	const COORDINATE4* getGlobalAtomCoordinates() const { return m_globalPoints; }
	const unsigned int* getAtomicNumbers() const { return m_atomicNumbers; }

	void setFrozen(bool newValue) { m_bFrozen = newValue; }
	bool getFrozen() { return m_bFrozen; }

	/////////////////////////////////////////////////////////////////////
	// Purpose: This function initializes the rotation and translation matrix.
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

	static void getCenterOfMass(unsigned int iNumberOfAtoms,
			const unsigned int* atomicNumbers, const COORDINATE4* points,
			COORDINATE3 &centerOfMass);

	static void printPoint(const FLOAT* point);

	void print() const { print(false, false); }

	void print(bool radiansOrDegrees, bool printLocalCoordinates) const;

private:
	static const unsigned int s_atomGroupMinOccurs[];
	static const unsigned int s_atomGroupMaxOccurs[];
	static const bool s_atomAttReq[];
	static const char* s_atomAttDef[];
	static const bool s_coordinateAttReq[];
	static const char* s_coordinateAttDef[];
	void cleanUp();
};

#endif

