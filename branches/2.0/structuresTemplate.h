
#ifndef __STRUCTURES_TEMPLATE_H_
#define __STRUCTURES_TEMPLATE_H_

#include "atomGroupTemplate.h"
#include "constraints.h"
#include "seed.h"
#include "structure.h"
#include "random/random.h"
#include <list>

// 0 = OFF, 1 = DEBUG, 2 = DEBUG VERBOSE
#define INIT_DEBUG 0

class StructuresTemplate {
public:
	enum Type { Linear, Planar, ThreeD };
	enum InitResult {
		Success,
		MissingContainerOrMaxDist,
		MinDistanceProblem,
		MaxDistanceProblem,
		WontFitInContainer,
		PlaceAtomGroupRelativeToAnotherMethodFailed
	};

	unsigned int m_iAtomGroupTemplates; // size of m_atomGroupTemplates (will be zero if there are none)
	AtomGroupTemplate* m_atomGroupTemplates;

	Constraints* m_pConstraints;
	
	unsigned int m_iLinear;
	Constraints* m_pLinearConstraints;
	unsigned int m_iPlanar;
	Constraints* m_pPlanarConstraints;
	unsigned int m_i3D;
	Constraints* m_p3DConstraints;
	
	FLOAT* m_bondRotationalSearchAngle; // Stored in radians
	
	Seed* m_pSeed;
	
	StructuresTemplate();
	~StructuresTemplate();
	bool load(TiXmlElement *pStructuresTemplateElem, std::map<std::string,Constraints*> &constraintsMap, const Strings* messages);
	bool save(TiXmlElement *pParentElem, const Strings* messages);

	bool initializeStructures(std::list<Structure*> &structures, const Constraints* pActionConstraints);

	/**************************************************************************
	 * Purpose: This method performs random initialization by performing
	 *    translation and rotation of atom groups inside a structure.
	 * Parameters: structure - the chemical structure to initialize
	 *             constraints - min and max distances, container, etc.
	 *             type - linear, planar, or 3D
	 * Returns: true if the structure was initialized within the constraints
	 *          false otherwise
	 */
	static bool initializeStructure(Structure &structure,
			const Constraints &constraints, Type type) {
		unsigned int iTries, iGroupTries;
		InitResult result = initializeStructure(structure, constraints, type,
				defaultMaxInitializationTries, iTries,
				defaultMaxGroupInitializationTries, iGroupTries);
		if (Success != result)
			printInitFailureMessage(result);
		return result == Success;
	}

	static unsigned int defaultMaxInitializationTries;
	static unsigned int defaultMaxGroupInitializationTries;

	/**************************************************************************
	 * Purpose: This method performs random initialization by performing
	 *    translation and rotation of atom groups inside a structure.
	 * Parameters: structure - the chemical structure to initialize
	 *             constraints - min and max distances, container, etc.
	 *             type - linear, planar, or 3D
	 *             iMaxTries - the maximum number of attempts
	 *             iTries - the actual number of attempts
	 *             iMaxGroupTries - the maximum number of attempts for each
	 *                              atom group
	 *             iGroupTries - the highest number of attempts used to
	 *                           initialize an atom group
	 * Returns: an INIT_FAILURE_REASON enum value
	 */
	static InitResult initializeStructure(Structure &structure,
			const Constraints &constraints, Type type, unsigned int iMaxTries,
			unsigned int &iTries, unsigned int iMaxGroupTries,
			unsigned int &iGroupTries);

	static InitResult initializeAtomGroup(Structure &structure,
			unsigned int index, Type type,
			const Constraints &constraints);

	static InitResult initializeAtomGroup(Structure &structure,
			unsigned int index, Type type, int prevIndex,
			const Constraints &constraints, unsigned int iMaxTries,
			unsigned int &iTries,
			std::map<unsigned int, bool> &atomGroupsInitialized);

	static InitResult placeAtomGroupRelativeToAnother(
			unsigned int agToPlaceIndex, unsigned int otherAgIndex,
			Structure &structure, const FLOAT* unitVector,
			const Constraints &constraints);

	static void getVectorInDirection(FLOAT angleX, FLOAT angleY, FLOAT length,
			COORDINATE4 &result);

	static FLOAT euclideanDistance(const FLOAT* point1, const FLOAT* point2);

	static void getClosestAtoms(const AtomGroup &ag1, const AtomGroup &ag2,
			const Constraints &constraints, const FLOAT* &atom1,
			const FLOAT* &atom2, FLOAT &minDistBetween1And2);

	static void closestPointFromALineToAPoint(const FLOAT* pointOnLine,
			const FLOAT* vectorAlongLine, const FLOAT* point, FLOAT* result);

	/**********************************************************************
	 * Purpose: This checks the minimum distance constraints between two
	 *    atom groups.
	 * Parameters: ag1Index and ag2Index - indexes of the two atom groups
	 *             structure - the structure containing the two atom groups
	 *             constraints - contains the minimum distances
	 * Returns: true of the distance constraints are satisfied
	 */
	static bool minDistancesOK(unsigned int ag1Index, unsigned int ag2Index,
			const Structure &structure, const Constraints &constraints);

	static void printInitFailureMessage(InitResult result);

protected:
	void clear();
	bool readInitializationType(TiXmlElement *pElem, std::map<std::string,
			Constraints*> &constraintsMap, unsigned int &numberOfThisType,
			Constraints* &pConstraints, const Strings* messages);

	unsigned int checkCompatabilityWithGroups(const Structure &structure,
			unsigned int &firstDiffTemplateIndex,
			unsigned int &firstDiffStructureIndex, bool &firstDiffMissing);

	unsigned int checkCompatabilityWithoutGroups(const Structure &structure,
			unsigned int &firstDiffTemplateIndex,
			unsigned int &firstDiffStructureIndex, bool &firstDiffMissing);

	bool ensureCompatibile(Structure &structure, unsigned int structureNumber,
			const Constraints &constraints);

	static const bool         s_attRequired[];
	static const char*        s_attDefaults[];

	static const unsigned int s_minOccurs[];
	static const unsigned int s_popTempMinOccurs[];
	static const unsigned int s_popTempMaxOccurs[];
	
	static const bool s_initTypeAttRequired[];
	static const char* s_initTypeAttDefaults[];
};

#endif
