
#include "util.h"

FLOAT** getDistanceMatrix(const Structure &structure, bool globalOrLocal) {
	unsigned int i, j, k;
	COORDINATE3 diff;
	FLOAT** matrix;
	matrix = new FLOAT*[structure.getNumberOfAtoms()];
	const COORDINATE4* const* coordinates;
	if (globalOrLocal)
		coordinates = structure.getAtomCoordinates();
	else
		coordinates = structure.getLocalAtomCoordinates();

	for (i = 0; i < structure.getNumberOfAtoms(); ++i)
		matrix[i] = new FLOAT[structure.getNumberOfAtoms()];

	for (i = 0; i < structure.getNumberOfAtoms(); ++i)
		for (j = 0; j < structure.getNumberOfAtoms(); ++j) {
			for (k = 0; i < 3; ++k)
				diff[k] = coordinates[i][k] - coordinates[j][k];
			matrix[i][j] = sqrt(diff[0]*diff[0] + diff[1]*diff[1] + diff[2]*diff[2]);
		}

	return matrix;
}

void deleteDistanceMatrix(FLOAT** matrix, unsigned int size) {
	for (unsigned int i = 0; i < size; ++i)
		delete[] matrix[i];
	delete matrix;
}

void printDistanceMatrix(FLOAT** matrix, unsigned int size) {
	std::cout << "Distance Matrix:" << std::endl;
	unsigned int i, j;
	for (i = 0; i < size; ++i) {
		for (j = 0; j < size; ++j)
			std::cout << "  " << matrix[i][j];
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

FLOAT minDistBetweenMolecules(FLOAT** atomDistMatrix, unsigned int startIndex1, unsigned int endIndex1,
		unsigned int startIndex2, unsigned int endIndex2) {
	FLOAT dist = 1e50;
	unsigned i, j;
	for (i = startIndex1; i < endIndex1; ++i)
		for (j = startIndex2; j < endIndex2; ++j)
			if (dist > atomDistMatrix[i][j])
				dist = atomDistMatrix[i][j];
	return dist;
}

bool** getFragmentationMatrix(const Structure &structure, FLOAT** atomDistMatrix, FLOAT maxDistance) {
	unsigned int molI, molJ;
	unsigned int atomI, nextAtomI;
	unsigned int atomJ, nextAtomJ;
	bool** matrix = new bool*[structure.getNumberOfAtomGroups()];
	const AtomGroup* atomGroups = structure.getAtomGroups();

	for (molI = 0; molI < structure.getNumberOfAtomGroups(); ++molI)
		matrix[molI] = new bool[structure.getNumberOfAtomGroups()];

	atomI = 0;
	atomJ = 0;
	for (molI = 0; molI < structure.getNumberOfAtomGroups(); ++molI) {
		nextAtomI = atomI + atomGroups[molI].getNumberOfAtoms();
		for (molJ = 0; molJ < structure.getNumberOfAtomGroups(); ++molJ) {
			nextAtomJ = atomJ + atomGroups[molJ].getNumberOfAtoms();
			if (molI == molJ)
				matrix[molI][molJ] = true;
			else
				matrix[molI][molJ] = minDistBetweenMolecules(atomDistMatrix, atomI, nextAtomI, atomJ, nextAtomJ) <= maxDistance;
			atomJ = nextAtomJ;
		}
		atomI = nextAtomI;
	}

	return matrix;
}

void deleteFragmentationMatrix(bool** matrix, unsigned int size) {
	for (unsigned int i = 0; i < size; ++i)
		delete[] matrix[i];
	delete matrix;
}

bool distancesWithinTolerance(FLOAT distance1, FLOAT distance2, FLOAT tolerance) {
	FLOAT diff = distance1 - distance2;
	if (diff < 0)
		diff = -diff;
	return diff <= tolerance;
}

// This verifies that rotation and translation were performed correctly.
// The correctness is verified by examining distances between atoms within atomGroups.
// These should be the same before and after translation and rotation.
bool transformationCheck(const Structure &structure) {
	const AtomGroup* atomGroups = structure.getAtomGroups();
	FLOAT** matrixBefore = getDistanceMatrix(structure, false);
	FLOAT** matrixAfter = getDistanceMatrix(structure, true);
	unsigned int moleculeI;
	unsigned int atomStart, atomEnd;
	unsigned int i, j;
	const FLOAT tolerance = 0.0001;

	atomStart = 0;
	for (moleculeI = 0; moleculeI < structure.getNumberOfAtomGroups(); ++moleculeI) {
		atomEnd = atomStart + atomGroups[moleculeI].getNumberOfAtoms();
		for (i = atomStart; i < atomEnd; ++i)
			for (j = atomStart; j < atomEnd; ++j)
				if (i != j && !distancesWithinTolerance(matrixBefore[i][j], matrixAfter[i][j], tolerance))
					return false;
		atomStart = atomEnd;
	}

	deleteDistanceMatrix(matrixBefore, structure.getNumberOfAtoms());
	deleteDistanceMatrix(matrixAfter, structure.getNumberOfAtoms());
	return true;
}

// This class determines if a graph is "connected" according to the graphy theory definition.
class Connectivity {
private:
	bool** m_adjacencyMatrix; // 2D array
	unsigned int m_iSize;

public:
	Connectivity (unsigned int size, bool** adjacencyMatrix) {
		m_iSize = size;
		m_adjacencyMatrix = adjacencyMatrix;
	}

	bool isConnected() {
		unsigned int numberVisited = 0;
		bool* visited = new bool[m_iSize];

		for (unsigned int i = 0; i < m_iSize; ++i)
			visited[i] = false;

		dfsearch(0, numberVisited, visited);

		delete[] visited;

		return numberVisited == m_iSize;
	}

private:
	void dfsearch(unsigned int toVisit, unsigned int &count, bool* visited) {
		visited[toVisit] = true;
		++count;
		for (unsigned int i = 0; i < m_iSize; ++i)
			if (!visited[i] && m_adjacencyMatrix[toVisit][i])
				dfsearch(i, count, visited);
	}
};

bool distanceConstraintsOK(const Structure &structure, FLOAT *pGeneralMin,
		std::map<unsigned int, std::map<unsigned int,FLOAT> > *pMinAtomicDistances, FLOAT *pMaxDist) {
	bool success = true;
	const unsigned int* atomicNumbers = structure.getAtomicNumbers();
	FLOAT** matrix = getDistanceMatrix(structure, true);
	unsigned int size =  structure.getNumberOfAtoms();
	unsigned int i, j;

	for (i = 0; i < size; ++i)
		for (j = 0; j < size; ++j)
			if (i != j) {
				if (pGeneralMin && matrix[i][j] < *pGeneralMin) {
					std::cout << " Atoms "
					     << Handbook::getAtomicSymbol(atomicNumbers[i]) << (i+1) << " and "
					     << Handbook::getAtomicSymbol(atomicNumbers[j]) << (j+1)
					     << " failed to meet the general minimum distance constraint of " << *pGeneralMin << std::endl;
					success = false;
				}
				if (pMinAtomicDistances && matrix[i][j] < (*pMinAtomicDistances)[atomicNumbers[i]][atomicNumbers[j]]) {
					std::cout << " Atoms "
					     << Handbook::getAtomicSymbol(atomicNumbers[i]) << (i+1) << " and "
					     << Handbook::getAtomicSymbol(atomicNumbers[j]) << (j+1)
					     << " failed to meet the specific minimum distance constraint of " << (*pMinAtomicDistances)[atomicNumbers[i]][atomicNumbers[j]] << std::endl;
					success = false;
				}
			}
	if (pMaxDist) {
		bool** fragmentationMatrix = getFragmentationMatrix(structure, matrix, *pMaxDist);
		Connectivity graph(structure.getNumberOfAtomGroups(), fragmentationMatrix);
		if (!graph.isConnected()) {
			std::cout << "Non-fragmentation check failed using max distance " << *pMaxDist << "." << std::endl;
			structure.print(Structure::PRINT_RADIANS);
			printDistanceMatrix(matrix, structure.getNumberOfAtoms());
			success = false;
		}
		deleteFragmentationMatrix(fragmentationMatrix, structure.getNumberOfAtomGroups());
	}
	deleteDistanceMatrix(matrix, structure.getNumberOfAtoms());
	return success;
}

bool structureInsideCube(const Structure &structure, FLOAT halfCubeLWH) {
	const COORDINATE4* const* coordinates = structure.getAtomCoordinates();
	const COORDINATE4* coordinate;

	unsigned int i, j, n;
	for (i = 0, n = structure.getNumberOfAtoms(); i < n; ++i) {
		coordinate = coordinates[i];
		for (j = 0; j < 3; ++j) {
			if (*coordinate[j] < -halfCubeLWH)
				return false;
			if (*coordinate[j] > halfCubeLWH)
				return false;
		}
	}
	return true;
}

void makeWaterSet(Structure &structure, unsigned int number) {
	const unsigned int iAtomGroupTemplates = 1;
	AtomGroupTemplate atomGroupTemplates[iAtomGroupTemplates];
	AtomGroupTemplate* water = &(atomGroupTemplates[0]);
	water->m_iNumber = number;
	water->m_iFormat = CARTESIAN;

	FLOAT* c;
	c = new FLOAT[3];
	c[0] = 0;
	c[1] = 0;
	c[2] = 0;
	water->m_coordinates.push_back(c);
	water->m_atomicNumbers.push_back(8);
	c = new FLOAT[3];
	c[0] = 0.757;
	c[1] = 0.586;
	c[2] = 0;
	water->m_coordinates.push_back(c);
	water->m_atomicNumbers.push_back(1);
	c = new FLOAT[3];
	c[0] = -0.757;
	c[1] = 0.586;
	c[2] = 0;
	water->m_coordinates.push_back(c);
	water->m_atomicNumbers.push_back(1);

	structure.setAtomGroups(iAtomGroupTemplates, atomGroupTemplates);
	structure.update();
}

bool floatsEqual(FLOAT a, FLOAT b) {
	return abs(a-b) <= FLOAT_ERROR;
}
