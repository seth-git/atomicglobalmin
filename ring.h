////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code managing a ring.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __RING_H__
#define __RING_H__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include "atom.h"

using namespace std;

class Ring
{
public:
	vector<int> m_ringAtomIndices; // indexes to ring atoms as contained in the molecule object
	
public:
	Ring(vector<int> &ringAtomIndices);
	Ring(Ring &otherRing);
	~Ring();
	
	bool equal(Ring &otherRing);
	void print(Atom *atoms);

private:
	////////////////////////////////////////////////////////////////////////////////
	// Purpose: This function puts the index numbers in m_ringAtomIndices in a standard
	//    format to avoid duplicate rings from being created.
	// Parameters: none
	// Returns: nothing
	void standardizeFormat();
};

#endif
