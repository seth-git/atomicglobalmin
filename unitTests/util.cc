////////////////////////////////////////////////////////////////////////////////
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "util.h"

FLOAT** getDistanceMatrix(MoleculeSet &moleculeSet, bool globalOrLocal) {
	unsigned int i, j;
	Point3D diff;
	FLOAT** matrix;
	matrix = new FLOAT*[moleculeSet.getNumberOfAtoms()];
	Atom const** atoms = moleculeSet.getAtoms();
	
	for (i = 0; i < (unsigned int)moleculeSet.getNumberOfAtoms(); ++i)
		matrix[i] = new FLOAT[moleculeSet.getNumberOfAtoms()];
	
	if (globalOrLocal) {
		for (i = 0; i < (unsigned int)moleculeSet.getNumberOfAtoms(); ++i)
			for (j = 0; j < (unsigned int)moleculeSet.getNumberOfAtoms(); ++j) {
				diff.x = atoms[i]->m_globalPoint.x - atoms[j]->m_globalPoint.x;
				diff.y = atoms[i]->m_globalPoint.y - atoms[j]->m_globalPoint.y;
				diff.z = atoms[i]->m_globalPoint.z - atoms[j]->m_globalPoint.z;
				matrix[i][j] = sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
			}
	} else {
		for (i = 0; i < (unsigned int)moleculeSet.getNumberOfAtoms(); ++i)
			for (j = 0; j < (unsigned int)moleculeSet.getNumberOfAtoms(); ++j) {
				diff.x = atoms[i]->m_localPoint.x - atoms[j]->m_localPoint.x;
				diff.y = atoms[i]->m_localPoint.y - atoms[j]->m_localPoint.y;
				diff.z = atoms[i]->m_localPoint.z - atoms[j]->m_localPoint.z;
				matrix[i][j] = sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
			}
	}
	
	return matrix;
}

void deleteDistanceMatrix(FLOAT** matrix, unsigned int size) {
	for (unsigned int i = 0; i < size; ++i)
		delete[] matrix[i];
	delete matrix;
}

void printDistanceMatrix(FLOAT** matrix, unsigned int size) {
	cout << "Distance Matrix:" << endl;
	unsigned int i, j;
	for (i = 0; i < size; ++i) {
		for (j = 0; j < size; ++j)
			cout << "  " << matrix[i][j];
		cout << endl;
	}
	cout << endl;
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


bool** getFragmentationMatrix(MoleculeSet &moleculeSet, FLOAT** atomDistMatrix, FLOAT maxDistance) {
	unsigned int molI, molJ;
	unsigned int atomI, nextAtomI;
	unsigned int atomJ, nextAtomJ;
	bool** matrix = new bool*[moleculeSet.getNumberOfMolecules()];
	const Molecule* molecules = moleculeSet.getMolecules();
	
	for (molI = 0; molI < (unsigned int)moleculeSet.getNumberOfMolecules(); ++molI)
		matrix[molI] = new bool[moleculeSet.getNumberOfMolecules()];
	
	atomI = 0;
	atomJ = 0;
	for (molI = 0; molI < (unsigned int)moleculeSet.getNumberOfMolecules(); ++molI) {
		nextAtomI = atomI + (unsigned int)molecules[molI].getNumberOfAtoms();
		for (molJ = 0; molJ < (unsigned int)moleculeSet.getNumberOfMolecules(); ++molJ) {
			nextAtomJ = atomJ + (unsigned int)molecules[molJ].getNumberOfAtoms();
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
// The correctness is verified by examining distances between atoms within molecules.
// These should be the same before and after translation and rotation.
bool transformationCheck(MoleculeSet &moleculeSet) {
	const Molecule* molecules = moleculeSet.getMolecules();
	FLOAT** matrixBefore = getDistanceMatrix(moleculeSet, false);
	FLOAT** matrixAfter = getDistanceMatrix(moleculeSet, true);
	unsigned int moleculeI;
	unsigned int atomStart, atomEnd;
	unsigned int i, j;
	const FLOAT tolerance = 0.0001;
	
	atomStart = 0;
	for (moleculeI = 0; moleculeI < (unsigned int)moleculeSet.getNumberOfMolecules(); ++moleculeI) {
		atomEnd = atomStart + (unsigned int)molecules[moleculeI].getNumberOfAtoms();
		for (i = atomStart; i < atomEnd; ++i)
			for (j = atomStart; j < atomEnd; ++j)
				if (i != j && !distancesWithinTolerance(matrixBefore[i][j], matrixAfter[i][j], tolerance))
					return false;
		atomStart = atomEnd;
	}
	
	deleteDistanceMatrix(matrixBefore, moleculeSet.getNumberOfAtoms());
	deleteDistanceMatrix(matrixAfter, moleculeSet.getNumberOfAtoms());
	return true;
}

bool partiallyFragmented(MoleculeSet &moleculeSet, bool** fragmentationMatrix, unsigned int numberOfMolecules, FLOAT maxDistance) {
	const Molecule* molecules = moleculeSet.getMolecules();
	bool withInMaxDist;
	unsigned int i, j;
	
	for (i = 0; i < numberOfMolecules; ++i) {
		if (molecules[i].getNumberOfAtoms() > 1)
			continue; // Assume the max distance contraints are satesfied within the molecule
		withInMaxDist = false;
		for (j = 0; j < numberOfMolecules; ++j)
			if (i != j && fragmentationMatrix[i][j]) {
				withInMaxDist = true;
				break;
			}
		if (!withInMaxDist) {
			cout << "Partial fragmentation check failed for molecule " << (i+1) << " using max distance " << maxDistance << "." << endl;
			return false;
		}
	}
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

bool distanceConstraintsOK(MoleculeSet &moleculeSet, FLOAT *pGeneralMin,
		map<unsigned int, map<unsigned int,FLOAT> > *pMinAtomicDistances, FLOAT *pMaxDist, unsigned int fragmentation) {
	bool success = true;
	Atom const** atoms = moleculeSet.getAtoms();
	FLOAT** matrix = getDistanceMatrix(moleculeSet, true);
	unsigned int size = (unsigned int) moleculeSet.getNumberOfAtoms();
	unsigned int i, j;
	
	for (i = 0; i < size; ++i)
		for (j = 0; j < size; ++j)
			if (i != j) {
				if (pGeneralMin && matrix[i][j] < *pGeneralMin) {
					cout << " Atoms "
					     << Atom::s_rgAtomcSymbols[atoms[i]->m_iAtomicNumber] << (i+1) << " and "
					     << Atom::s_rgAtomcSymbols[atoms[j]->m_iAtomicNumber] << (j+1)
					     << " failed to meet the general minimum distance constraint of " << *pGeneralMin << endl;
					success = false;
				}
				if (pMinAtomicDistances && matrix[i][j] < (*pMinAtomicDistances)[atoms[i]->m_iAtomicNumber][atoms[j]->m_iAtomicNumber]) {
					cout << " Atoms "
					     << Atom::s_rgAtomcSymbols[atoms[i]->m_iAtomicNumber] << (i+1) << " and "
					     << Atom::s_rgAtomcSymbols[atoms[j]->m_iAtomicNumber] << (j+1)
					     << " failed to meet the specific minimum distance constraint of " << (*pMinAtomicDistances)[atoms[i]->m_iAtomicNumber][atoms[j]->m_iAtomicNumber] << endl;
					success = false;
				}
			}
	if (pMaxDist) {
		bool** fragmentationMatrix = getFragmentationMatrix(moleculeSet, matrix, *pMaxDist);
		if (fragmentation == PARTIALLY_FRAGMENTED) {
			if (!partiallyFragmented(moleculeSet, fragmentationMatrix, moleculeSet.getNumberOfMolecules(), *pMaxDist)) {
				printDetailedInfo(moleculeSet, false);
				printDistanceMatrix(matrix, moleculeSet.getNumberOfAtoms());
				success = false;
			}
		} else if (fragmentation == NOT_FRAGMENTED) {
			Connectivity graph(moleculeSet.getNumberOfMolecules(), fragmentationMatrix);
			if (!graph.isConnected()) {
				cout << "Non-fragmentation check failed using max distance " << *pMaxDist << "." << endl;
				printDetailedInfo(moleculeSet, false);
				printDistanceMatrix(matrix, moleculeSet.getNumberOfAtoms());
				success = false;
			}
		}
		deleteFragmentationMatrix(fragmentationMatrix, moleculeSet.getNumberOfMolecules());
	}
	deleteDistanceMatrix(matrix, moleculeSet.getNumberOfAtoms());
	return success;
}

bool moleculeSetInsideCube(MoleculeSet &moleculeSet, Point3D &boxDimensions) {
	Molecule* moleculeArray = moleculeSet.getMolecules();
	
	const Atom* atomArray;
	unsigned int i, j;
	for (i = 0; i < (unsigned int)moleculeSet.getNumberOfMolecules(); ++i) {
		atomArray = moleculeArray[i].getAtoms();
		for (j = 0; j < (unsigned int)moleculeArray[i].getNumberOfAtoms(); ++j) {
			if (atomArray[j].m_globalPoint.x < 0)
				return false;
			if (atomArray[j].m_globalPoint.x > boxDimensions.x)
				return false;
			if (atomArray[j].m_globalPoint.y < 0)
				return false;
			if (atomArray[j].m_globalPoint.y > boxDimensions.y)
				return false;
			if (atomArray[j].m_globalPoint.z < 0)
				return false;
			if (atomArray[j].m_globalPoint.z > boxDimensions.z)
				return false;
		}
	}
	return true;
}

void printPoint(const Point3D &point) {
	cout << "(x,y,z) = (" << point.x << "," << point.y << "," << point.z << ")";
}

void printDetailedInfo(MoleculeSet &moleculeSet, bool printBondLengths) {
	const Molecule* moleculeArray = moleculeSet.getMolecules();
	const Atom* atomArray;
	unsigned int i, j, k;
	Point3D diff;
	FLOAT lengthL, lengthG;
	for (i = 0; i < (unsigned int)moleculeSet.getNumberOfMolecules(); ++i) {
		cout << "center";
		printPoint(moleculeArray[i].getCenter());
		cout << ", angles in rad(x,y,z) = (" << moleculeArray[i].getAngleX() << "," << moleculeArray[i].getAngleY() << "," << moleculeArray[i].getAngleZ() << ")";
		cout << endl;
		atomArray = moleculeArray[i].getAtoms();
		for (j = 0; j < (unsigned int)moleculeArray[i].getNumberOfAtoms(); ++j) {
			cout << Atom::s_rgAtomcSymbols[atomArray[j].m_iAtomicNumber] << (atomArray[j].m_iMoleculeSetIndex+1) << ": local";
			printPoint(atomArray[j].m_localPoint);
			cout << ", global";
			printPoint(atomArray[j].m_globalPoint);
			cout << endl;
		}
		if (printBondLengths) {
			for (j = 0; j < (unsigned int)moleculeArray[i].getNumberOfAtoms(); ++j) {
				for (k = j+1; k < (unsigned int)moleculeArray[i].getNumberOfAtoms(); ++k) {
					diff.x = atomArray[j].m_localPoint.x - atomArray[k].m_localPoint.x;
					diff.y = atomArray[j].m_localPoint.y - atomArray[k].m_localPoint.y;
					diff.z = atomArray[j].m_localPoint.z - atomArray[k].m_localPoint.z;
					lengthL = sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
					diff.x = atomArray[j].m_globalPoint.x - atomArray[k].m_globalPoint.x;
					diff.y = atomArray[j].m_globalPoint.y - atomArray[k].m_globalPoint.y;
					diff.z = atomArray[j].m_globalPoint.z - atomArray[k].m_globalPoint.z;
					lengthG = sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
					cout << "bond lengths between " << j+1 << " and " << k+1 << ": local = " << lengthL << ", global = " << lengthG << endl;
				}
			}
		}
	}
}

void makeWater(Molecule &molecule)
{
	vector<Point3D> points;
	vector<int> atomicNumbers;
	Point3D o, h1, h2;
	o.x = 0;
	o.y = 0;
	o.z = 0;
	h1.x = 0.757;
	h1.y = 0.586;
	h1.z = 0;
	h2.x = -0.757;
	h2.y = 0.586;
	h2.z = 0;
	
	points.push_back(o);
	atomicNumbers.push_back(8);
	
	points.push_back(h1);
	atomicNumbers.push_back(1);
	
	points.push_back(h2);
	atomicNumbers.push_back(1);

	molecule.makeFromCartesian(points, atomicNumbers);
	molecule.initRotationMatrix();
	molecule.localToGlobal();
}

void makeWaterSet(MoleculeSet &moleculeSet, Molecule &water, unsigned int number) {
	moleculeSet.setNumberOfMolecules(number);
	Molecule* moleculeArray = moleculeSet.getMolecules();
	for (unsigned int i = 0; i < (unsigned int)moleculeSet.getNumberOfMolecules(); ++i)
		moleculeArray[i].copy(water);
	moleculeSet.init();
}

void setupMinDistances(FLOAT generalMin, map<unsigned int, map<unsigned int,FLOAT> > &minAtomicDistances) {
	Atom::initMinAtomicDistances(generalMin);
	for (map<unsigned int, map<unsigned int, FLOAT> >::iterator i = minAtomicDistances.begin(); i != minAtomicDistances.end(); i++)
		for (map<unsigned int, FLOAT>::iterator j = i->second.begin(); j != i->second.end(); j++)
			Atom::setMinAtomicDistance(i->first, j->first, j->second);
}

