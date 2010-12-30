////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code managing a set of unique rings.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __RINGSET_H__
#define __RINGSET_H__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include "ring.h"

using namespace std;

class RingSet
{
public:
	vector<Ring*> m_rings;
	
public:
	RingSet();
	~RingSet();
	
	
	void copy(RingSet &otherRingSet);
	
	void addRing(vector<int> &ringAtomIndices);
	void print(Atom *atoms);

private:
	void cleanUp();
};

#endif
