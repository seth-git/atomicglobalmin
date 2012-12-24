
#ifndef __ATOM_GROUP_H_
#define __ATOM_GROUP_H_

#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdElementUtil.h"
#include "xsd/xsdTypeUtil.h"
#include "translation/strings.h"
#include "atomGroupTemplate.h"
#include "handbook/handbook.h"

#define CARTESIAN 0

class AtomGroup {
private:
	unsigned int m_iNumberOfAtoms;
	unsigned int* m_atomicNumbers;
	COORDINATE4* m_localPoints;
	COORDINATE4* m_globalPoints;

	COORDINATE3 m_centerOfMass;
	COORDINATE3 m_angles;
	COORDINATE3 m_sinAngles;
	COORDINATE3 m_cosAngles;
	FLOAT m_matrixX[MATRIX_SIZE][MATRIX_SIZE];
	FLOAT m_matrixY[MATRIX_SIZE][MATRIX_SIZE];
	FLOAT m_matrixZ[MATRIX_SIZE][MATRIX_SIZE];
	FLOAT m_matrixLocalToGlobal[MATRIX_SIZE][MATRIX_SIZE];

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
	void load(AtomGroupTemplate &atomGroupTemplate);
	bool load(TiXmlElement *pAtomGroupElem, const Strings* messages);
	bool save(TiXmlElement *pParentElem, const Strings* messages);
	
private:
	void cleanUp();
	void setLocalCenterOfMassToZero();
};

#endif

