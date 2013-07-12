
#ifndef __CONSTRAINTS_H__
#define __CONSTRAINTS_H__

#include <stdio.h>
#include <string>
#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdElementUtil.h"
#include "xsd/xsdTypeUtil.h"
#include "translation/strings.h"

class Structure; // Forward declaration

#define MIN_DIST_ARRAY_SIZE  MAX_ATOMIC_NUMBERS+1
#define SIZEOF_MIN_DIST_ARRAY sizeof(FLOAT) * MIN_DIST_ARRAY_SIZE * MIN_DIST_ARRAY_SIZE
typedef FLOAT MinDistArray[MIN_DIST_ARRAY_SIZE];

class Constraints {
public:
	std::string m_sName;
	Constraints* m_pBase;
	FLOAT* m_pfCubeLWH; // cube length, width, and height
	FLOAT* m_pfHalfCubeLWH; // half cube length, width, and height
	FLOAT* m_pfGeneralMaxAtomicDistance;

	Constraints()
	{
		m_pBase = NULL;
		m_pfCubeLWH = NULL;
		m_pfHalfCubeLWH = NULL;
		m_pfGeneralMinAtomicDistance = NULL;
		m_pfGeneralMaxAtomicDistance = NULL;
		m_rgMinAtomicDistances = NULL;
	}

	~Constraints()
	{
		cleanUp();
	}

	bool load(TiXmlElement *pConstraintsElem, const Strings* messages, std::map<std::string,Constraints*> &constraintsMap);
	bool save(TiXmlElement *pConstraintsElem, const Strings* messages) const;

	void copy(const Constraints &other);

	FLOAT getSmallestMinDistance() const; // Returns 0 if there are no specific minimum distances

	/**********************************************************************
	 * Purpose: This method combines two sets of constraints, assigning the
	 *    result to this object. Where there are differences, the more
	 *    restrictive constraint is chosen.
	 * Parameters: other - the constraints to combine with this set.
	 * Returns: nothing
	 */
	void combineConstraints(const Constraints &other);

	FLOAT getMinDistance(unsigned int atomicNumber1, unsigned int atomicNumber2) const;

	bool hasContainerOrMaxDist() const { return NULL != m_pfCubeLWH || NULL != m_pfGeneralMaxAtomicDistance; }

	bool minDistancesOK(
			const std::map<unsigned int, bool> &atomGroupsInitialized,
			const Structure &structure) const;

	bool minDistancesOK(const Structure &structure) const;

	bool maxDistancesOK(
			const std::map<unsigned int, bool> &atomGroupsInitialized,
			const Structure &structure) const;

	bool maxDistancesOK(const Structure &structure) const;

	static void depthFirstSearch(unsigned int toVisit,
			unsigned int &visitedCount, bool* visited,
			const bool* const * adjacencyMatrix, unsigned int matrixSize);

	bool centerInContainer(
			const std::map<unsigned int, bool> &atomGroupsInitialized,
			Structure &structure) const;

	bool centerInContainer(Structure &structure) const;

private:
	FLOAT* m_pfGeneralMinAtomicDistance;
	MinDistArray* m_rgMinAtomicDistances; // 2D array, insert two atomic numbers, and get the minimum distance back (will be -1 if there is no value)
	std::map<unsigned int, std::map<unsigned int,FLOAT> > m_mapMinAtomicDistances; // 2D map (similar to m_rgMinAtomicDistances, except this is used for faster writing of the constraints to a file)

	static const unsigned int s_minOccurs[];

	static const unsigned int s_distMinOccurs[];
	static const unsigned int s_distMaxOccurs[];

	static const bool         s_required[];
	static const char*        s_defaultValues[];

	static const bool         s_minRequired[];
	static const char*        s_minDefaultValues[];

	bool addMinDist(TiXmlElement *pElem, unsigned int &timesReadGeneralMin, const Strings* messages);

	void cleanUp();
	bool specificMinDistNotInBase() const;
};

#endif
