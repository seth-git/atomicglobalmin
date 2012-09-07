////////////////////////////////////////////////////////////////////////////////
// Purpose: This file manages a set of unique rings.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "ringSet.h"

RingSet::RingSet(void)
{
}

RingSet::~RingSet(void)
{
	cleanUp();
}

void RingSet::cleanUp(void)
{
	for (int i = 0; i < (signed int)m_rings.size(); ++i)
		delete m_rings[i];
	m_rings.clear();
}

void RingSet::copy(RingSet &otherRingSet)
{
	cleanUp();
	
	for (int i = 0; i < (signed int)otherRingSet.m_rings.size(); ++i)
		m_rings.push_back(new Ring(*otherRingSet.m_rings[i]));
}

void RingSet::addRing(vector<int> &ringAtomIndices)
{
	Ring* ring = new Ring(ringAtomIndices);
	
	for (int i = 0; i < (signed int)m_rings.size(); ++i)
		if (m_rings[i]->equal(*ring)) {
			delete ring;
			return;
		}
	m_rings.push_back(ring);
}

void RingSet::print(Atom *atoms)
{
	for (int i = 0; i < (signed int)m_rings.size(); ++i) {
		cout << "Ring " << (i+1) << ": ";
		m_rings[i]->print(atoms);
	}
}

