
#include "atomGroup.h"

AtomGroup::AtomGroup()
{
	m_iNumberOfAtoms = 0;
	m_atomicNumbers = NULL;
	m_localPoints = NULL;
	m_globalPoints = NULL;
}

AtomGroup::~AtomGroup()
{
	clear();
}

void AtomGroup::clear()
{
	m_iNumberOfAtoms = 0;
	if (m_atomicNumbers != NULL) {
		delete[] m_atomicNumbers;
		m_atomicNumbers = NULL;
	}
	if (m_localPoints != NULL) {
		delete[] m_localPoints;
		m_localPoints = NULL;
	}
	if (m_globalPoints != NULL) {
		delete[] m_globalPoints;
		m_globalPoints = NULL;
	}
	std::fill_n(m_centerOfMass, 3, 0);
	std::fill_n(m_angles, 3, 0);
	m_bFrozen = false;
}

void AtomGroup::setAtoms(const AtomGroupTemplate &agTemplate)
{
	unsigned int i, j;

	clear();
	m_iNumberOfAtoms = agTemplate.m_atomicNumbers.size();
	m_atomicNumbers = new unsigned int[m_iNumberOfAtoms];
	m_localPoints = new COORDINATE4[m_iNumberOfAtoms];
	m_globalPoints = new COORDINATE4[m_iNumberOfAtoms];

	for (i = 0; i < m_iNumberOfAtoms; ++i)
	{
		m_atomicNumbers[i] = agTemplate.m_atomicNumbers[i];
		memcpy(m_localPoints[i], agTemplate.m_coordinates[i], SIZEOF_COORDINATE3);
		m_localPoints[i][3] = 1;
	}

	COORDINATE3 centerOfMass;
	getCenterOfMass(m_iNumberOfAtoms, m_atomicNumbers, m_localPoints, centerOfMass);
	for (i = 0; i < m_iNumberOfAtoms; ++i)
		for (j = 0; j < 3; ++j)
			m_localPoints[i][j] -= centerOfMass[j];

	initRotationMatrix();
	localToGlobal();
}

void AtomGroup::getCenterOfMass(unsigned int iNumberOfAtoms,
		const unsigned int* atomicNumbers, const COORDINATE4* points,
		COORDINATE3 &centerOfMass) {
	centerOfMass[0] = 0;
	centerOfMass[1] = 0;
	centerOfMass[2] = 0;
	FLOAT totalMass = 0;
	unsigned int i, j;

	for (i = 0; i < iNumberOfAtoms; ++i) {
		FLOAT mass = Handbook::getMass(atomicNumbers[i]);
		for (j = 0; j < 3; ++j)
			centerOfMass[j] += mass * points[i][j];
		totalMass += mass;
	}
	for (j = 0; j < 3; ++j)
		centerOfMass[j] /= totalMass;
}

void AtomGroup::setAtoms(unsigned int numAtoms, const COORDINATE4* cartesianPoints,
		const unsigned int* atomicNumbers) {
	unsigned int i, j;

	if (numAtoms != m_iNumberOfAtoms) {
		clear();
		m_iNumberOfAtoms = numAtoms;
		m_atomicNumbers = new unsigned int[m_iNumberOfAtoms];
		m_localPoints = new COORDINATE4[m_iNumberOfAtoms];
		m_globalPoints = new COORDINATE4[m_iNumberOfAtoms];
	}

	memcpy(m_atomicNumbers, atomicNumbers, sizeof(unsigned int) * m_iNumberOfAtoms);
	memcpy(m_globalPoints, cartesianPoints, SIZEOF_COORDINATE4 * m_iNumberOfAtoms);
	memcpy(m_localPoints, cartesianPoints, SIZEOF_COORDINATE4 * m_iNumberOfAtoms);

	getCenterOfMass(m_iNumberOfAtoms, m_atomicNumbers, m_localPoints, m_centerOfMass);
	for (i = 0; i < m_iNumberOfAtoms; ++i)
		for (j = 0; j < 3; ++j)
			m_localPoints[i][j] -= m_centerOfMass[j];

	m_angles[0] = 0;
	m_angles[1] = 0;
	m_angles[2] = 0;

	initRotationMatrix();
}

const unsigned int AtomGroup::s_atomGroupMinOccurs[] = {0,0,1};
const unsigned int AtomGroup::s_atomGroupMaxOccurs[] = {1,1,XSD_UNLIMITED};

const bool AtomGroup::s_atomAttReq[] = {true,true,true,true};
const char* AtomGroup::s_atomAttDef[] = {"", "", "", ""};

const bool AtomGroup::s_coordinateAttReq[] = {true,true,true};
const char* AtomGroup::s_coordinateAttDef[] = {"", "", ""};

bool AtomGroup::load(const rapidxml::xml_node<>* pAtomGroupElem)
{
	using namespace rapidxml;
	using namespace strings;
	clear();

	if (!XsdTypeUtil::read1BoolAtt(pAtomGroupElem, m_bFrozen, xFrozen, false, "false"))
		return false;

	const char* atomGroupElemNames[] = {xTranslation, xRadianAngles, xAtom};
	XsdElementUtil atomGroupElemUtil(XSD_SEQUENCE, atomGroupElemNames, s_atomGroupMinOccurs, s_atomGroupMaxOccurs);
	if (!atomGroupElemUtil.process(pAtomGroupElem)) {
		return false;
	}
	std::vector<const xml_node<>*>* atomGroupElements = atomGroupElemUtil.getSequenceElements();

	m_iNumberOfAtoms = atomGroupElements[2].size();
	m_atomicNumbers = new unsigned int[m_iNumberOfAtoms];
	m_localPoints = new COORDINATE4[m_iNumberOfAtoms];
	m_globalPoints = new COORDINATE4[m_iNumberOfAtoms];

	const char* atomAttNames[] = {xBigZ, xX, xY, xZ};

	if (m_iNumberOfAtoms == 1) {
		std::fill_n(m_localPoints[0], 3, 0);
		m_localPoints[0][3] = 1;

		const xml_node<>* pAtomElem = atomGroupElements[2][0];
		XsdAttributeUtil atomAttUtil(atomAttNames, s_atomAttReq, s_atomAttDef);
		if (!atomAttUtil.process(pAtomElem))
			return false;
		const char** atomAttValues = atomAttUtil.getAllAttributes();

		unsigned int attNum = 0;
		if (!XsdTypeUtil::getAtomicNumber(atomAttValues[attNum], m_atomicNumbers[attNum], atomAttNames[attNum], pAtomElem->name()))
			return false;
		do {
			++attNum;
			if (!XsdTypeUtil::getFloat(atomAttValues[attNum], m_centerOfMass[attNum-1], atomAttNames[attNum], pAtomElem))
				return false;
		} while (attNum < 3);
	} else if (m_iNumberOfAtoms > 1) {
		const char* coordinateAttNames[] = {xX, xY, xZ};
		unsigned int i;
		if (1 == atomGroupElements[0].size()) {
			const xml_node<>* pTranslationElem = atomGroupElements[0][0];
			XsdAttributeUtil translationAttUtil(coordinateAttNames, s_coordinateAttReq, s_coordinateAttDef);
			if (!translationAttUtil.process(pTranslationElem))
				return false;
			const char** attValues = translationAttUtil.getAllAttributes();
			for (i = 0; i < 3; ++i)
				if (!XsdTypeUtil::getFloat(attValues[i], m_centerOfMass[i], coordinateAttNames[i], pTranslationElem))
					return false;
		} else {
			std::fill_n(m_centerOfMass, 3, 0);
		}
		if (1 == atomGroupElements[1].size()) {
			const xml_node<>* pRadianAnglesElem = atomGroupElements[1][0];
			XsdAttributeUtil angleAttUtil(coordinateAttNames, s_coordinateAttReq, s_coordinateAttDef);
			if (!angleAttUtil.process(pRadianAnglesElem))
				return false;
			const char** attValues = angleAttUtil.getAllAttributes();
			for (i = 0; i < 3; ++i)
				if (!XsdTypeUtil::getFloat(attValues[i], m_angles[i], coordinateAttNames[i], pRadianAnglesElem))
					return false;
		} else {
			std::fill_n(m_angles, 3, 0);
		}
		std::vector<const xml_node<>*>* atoms = &(atomGroupElements[2]);
		for (unsigned int i = 0; i < m_iNumberOfAtoms; ++i) {
			const xml_node<>* pAtomElem = (*atoms)[i];
			XsdAttributeUtil atomAttUtil(atomAttNames, s_atomAttReq, s_atomAttDef);
			if (!atomAttUtil.process(pAtomElem))
				return false;
			const char** atomAttValues = atomAttUtil.getAllAttributes();
			unsigned int attNum = 0;
			if (!XsdTypeUtil::getAtomicNumber(atomAttValues[attNum], m_atomicNumbers[i], atomAttNames[attNum], pAtomElem->name()))
				return false;
			do {
				++attNum;
				if (!XsdTypeUtil::getFloat(atomAttValues[attNum], m_localPoints[i][attNum-1], atomAttNames[attNum], pAtomElem))
					return false;
			} while (attNum < 3);
			m_localPoints[i][3] = 1;
		}
	}

	// Note: Assume local coordinates have been translated so the center of mass is zero.

	initRotationMatrix();
	localToGlobal();

	return true;
}

bool AtomGroup::save(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pParentElem) {
	using namespace rapidxml;
	using namespace strings;
	xml_node<>* atomGroup = doc.allocate_node(node_element, xAtomGroup);
	pParentElem->append_node(atomGroup);

	if (m_bFrozen)
		atomGroup->append_attribute(doc.allocate_attribute(xFrozen, pTrue));

	if (m_iNumberOfAtoms == 1) {
		xml_node<>* atom = doc.allocate_node(node_element, xAtom);
		atomGroup->append_node(atom);
		XsdTypeUtil::setAttribute(doc, atom, xBigZ, m_atomicNumbers[0]);
		XsdTypeUtil::setAttribute(doc, atom, xX, m_centerOfMass[0]);
		XsdTypeUtil::setAttribute(doc, atom, xY, m_centerOfMass[1]);
		XsdTypeUtil::setAttribute(doc, atom, xZ, m_centerOfMass[2]);
	} else if (m_iNumberOfAtoms > 1) {
		xml_node<>* translation = doc.allocate_node(node_element, xTranslation);
		atomGroup->append_node(translation);
		XsdTypeUtil::setAttribute(doc, translation, xX, m_centerOfMass[0]);
		XsdTypeUtil::setAttribute(doc, translation, xY, m_centerOfMass[1]);
		XsdTypeUtil::setAttribute(doc, translation, xZ, m_centerOfMass[2]);

		xml_node<>* radianAngles = doc.allocate_node(node_element, xRadianAngles);
		atomGroup->append_node(radianAngles);
		XsdTypeUtil::setAttribute(doc, radianAngles, xX, m_angles[0]);
		XsdTypeUtil::setAttribute(doc, radianAngles, xY, m_angles[1]);
		XsdTypeUtil::setAttribute(doc, radianAngles, xZ, m_angles[2]);

		FLOAT* temp;
		for (unsigned int i = 0; i < m_iNumberOfAtoms; ++i) {
			xml_node<>* atom = doc.allocate_node(node_element, xAtom);
			atomGroup->append_node(atom);
			XsdTypeUtil::setAttribute(doc, atom, xBigZ, m_atomicNumbers[i]);
			temp = m_localPoints[i];
			XsdTypeUtil::setAttribute(doc, atom, xX, temp[0]);
			XsdTypeUtil::setAttribute(doc, atom, xY, temp[1]);
			XsdTypeUtil::setAttribute(doc, atom, xZ, temp[2]);
		}
	}

	return true;
}

void AtomGroup::copy(AtomGroup &atomGroup) {
	clear();

	m_iNumberOfAtoms = atomGroup.m_iNumberOfAtoms;
	m_atomicNumbers = new unsigned int[m_iNumberOfAtoms];
	memcpy(m_atomicNumbers, atomGroup.m_atomicNumbers, sizeof(unsigned int) * m_iNumberOfAtoms);
	m_localPoints = new COORDINATE4[m_iNumberOfAtoms];
	memcpy(m_localPoints, atomGroup.m_localPoints, SIZEOF_COORDINATE4 * m_iNumberOfAtoms);
	m_globalPoints = new COORDINATE4[m_iNumberOfAtoms];
	memcpy(m_globalPoints, atomGroup.m_globalPoints, SIZEOF_COORDINATE4 * m_iNumberOfAtoms);

	memcpy(m_centerOfMass, atomGroup.m_centerOfMass, SIZEOF_COORDINATE3);
	memcpy(m_angles, atomGroup.m_angles, SIZEOF_COORDINATE3);
	memcpy(m_sinAngles, atomGroup.m_sinAngles, SIZEOF_COORDINATE3);
	memcpy(m_cosAngles, atomGroup.m_cosAngles, SIZEOF_COORDINATE3);

	memcpy(m_rotationMatrixX, atomGroup.m_rotationMatrixX, SIZEOF_MATRIX);
	memcpy(m_rotationMatrixY, atomGroup.m_rotationMatrixY, SIZEOF_MATRIX);
	memcpy(m_rotationMatrixZ, atomGroup.m_rotationMatrixZ, SIZEOF_MATRIX);
	memcpy(m_matrixLocalToGlobal, atomGroup.m_matrixLocalToGlobal, SIZEOF_MATRIX);

	m_bFrozen = atomGroup.m_bFrozen;
}

void AtomGroup::initRotationMatrix()
{
	FLOAT temp[MATRIX_WIDTH][MATRIX_WIDTH];

	for (unsigned int i = 0; i < 3; ++i) {
		m_sinAngles[i] = sin(m_angles[i]);
		m_cosAngles[i] = cos(m_angles[i]);
	}

	memcpy(m_rotationMatrixX, IDENTITY_MATRIX, SIZEOF_MATRIX);
	memcpy(m_rotationMatrixY, IDENTITY_MATRIX, SIZEOF_MATRIX);
	memcpy(m_rotationMatrixZ, IDENTITY_MATRIX, SIZEOF_MATRIX);

	// Note that the column index comes before the row index (matrix[col][row])
	m_rotationMatrixX[1][1] = m_cosAngles[0];
	m_rotationMatrixX[1][2] = -m_sinAngles[0];
	m_rotationMatrixX[2][1] = m_sinAngles[0];
	m_rotationMatrixX[2][2] = m_cosAngles[0];

	m_rotationMatrixY[0][0] = m_cosAngles[1];
	m_rotationMatrixY[0][2] = m_sinAngles[1];
	m_rotationMatrixY[2][0] = -m_sinAngles[1];
	m_rotationMatrixY[2][2] = m_cosAngles[1];

	m_rotationMatrixZ[0][0] = m_cosAngles[2];
	m_rotationMatrixZ[0][1] = -m_sinAngles[2];
	m_rotationMatrixZ[1][0] = m_sinAngles[2];
	m_rotationMatrixZ[1][1] = m_cosAngles[2];

	Matrix::matrixMultiplyMatrix(m_rotationMatrixX,m_rotationMatrixY,temp);
	Matrix::matrixMultiplyMatrix(temp,m_rotationMatrixZ,m_matrixLocalToGlobal);

	// Note: when translation is the last operation, the next three commands
	// have the same effect as multiplying by a translation matrix.
	m_matrixLocalToGlobal[0][3] = m_centerOfMass[0];
	m_matrixLocalToGlobal[1][3] = m_centerOfMass[1];
	m_matrixLocalToGlobal[2][3] = m_centerOfMass[2];
}

bool AtomGroup::localToGlobal()
{
	unsigned int i;
	for (i = 0; i < m_iNumberOfAtoms; ++i)
		Matrix::matrixMultiplyPoint(m_matrixLocalToGlobal, m_localPoints[i], m_globalPoints[i]);
//	for (i = 0; i < m_rotatableBonds.size(); ++i)
//		if (!performBondRotation(i))
//			return false;
	return true;
}

void AtomGroup::printPoint(const FLOAT* point) {
	printf("%11.7lf %11.7lf %11.7lf", point[0], point[1], point[2]);
}

void AtomGroup::print(bool radiansOrDegrees, bool printLocalCoordinates) const {
	if (m_iNumberOfAtoms > 1) {
		printf("Group Center: ");
		printPoint(m_centerOfMass);
		if (radiansOrDegrees) {
			printf(" Angles(rad): ");
			printPoint(m_angles);
		} else {
			COORDINATE3 angles;
			angles[0] = m_angles[0] * RAD_TO_DEG;
			angles[1] = m_angles[1] * RAD_TO_DEG;
			angles[2] = m_angles[2] * RAD_TO_DEG;
			printf(" Angles(deg): ");
			printPoint(angles);
		}
		printf("\n");
		for (unsigned int atomIndex = 0; atomIndex < m_iNumberOfAtoms; ++atomIndex) {
			printf("%2u %2s ", (atomIndex+1), Handbook::getAtomicSymbol(m_atomicNumbers[atomIndex]));
			if (printLocalCoordinates) {
				printf("Local: ");
				printPoint(m_localPoints[atomIndex]);
				printf(", Global: ");
			}
			printPoint(m_globalPoints[atomIndex]);
			printf("\n");
		}
	} else if (1 == m_iNumberOfAtoms){
		printf("Single Atom: %2s ", Handbook::getAtomicSymbol(m_atomicNumbers[0]));
		if (printLocalCoordinates) {
			printf("Local: ");
			printPoint(m_localPoints[0]);
			printf(", Global: ");
		}
		printPoint(m_globalPoints[0]);
		printf("\n");
	}
}
