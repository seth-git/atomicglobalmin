////////////////////////////////////////////////////////////////////////////////
// Purpose: This file manages a ring.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "ring.h"

Ring::Ring (vector<int> &ringAtomIndices)
{
	int i;
	
	m_ringAtomIndices.clear();
	for (i = 0; i < (signed int)ringAtomIndices.size(); ++i)
		m_ringAtomIndices.push_back(ringAtomIndices[i]);
	standardizeFormat();
}

Ring::Ring (Ring &otherRing)
{
	int i;
	
	m_ringAtomIndices.clear();
	for (i = 0; i < (signed int)otherRing.m_ringAtomIndices.size(); ++i)
		m_ringAtomIndices.push_back(otherRing.m_ringAtomIndices[i]);
}

Ring::~Ring ()
{
	m_ringAtomIndices.clear();
}

void Ring::print(Atom *atoms)
{
	if (m_ringAtomIndices.size() < 1)
		return;
	cout << (m_ringAtomIndices[0]+1) << "(" << Atom::s_rgAtomcSymbols[atoms[m_ringAtomIndices[0]].m_iAtomicNumber] << ")";
	for (int i = 1; i < (signed int)m_ringAtomIndices.size(); ++i)
		cout << ", " << (m_ringAtomIndices[i]+1) << "(" << Atom::s_rgAtomcSymbols[atoms[m_ringAtomIndices[i]].m_iAtomicNumber] << ")";
	cout << endl;
}

bool Ring::equal(Ring &otherRing)
{
	if (m_ringAtomIndices.size() != otherRing.m_ringAtomIndices.size())
		return false;
	for (int i = 0; i < (signed int)m_ringAtomIndices.size(); ++i)
		if (m_ringAtomIndices[i] != otherRing.m_ringAtomIndices[i])
			return false;
	return true;
}

void Ring::standardizeFormat()
{
	vector<int> newList;
	int i;
	int min = m_ringAtomIndices[0];
	int minPosition = 0;

	// Make sure the smallest index is first in the list
	for (i = 1; i < (signed int)m_ringAtomIndices.size(); ++i)
		if (m_ringAtomIndices[i] < min) {
			min = m_ringAtomIndices[i];
			minPosition = i;
		}
	for (i = minPosition; i < (signed int)m_ringAtomIndices.size(); ++i)
		newList.push_back(m_ringAtomIndices[i]);
	for (i = 0; i < minPosition; ++i)
		newList.push_back(m_ringAtomIndices[i]);
	
	// Make sure the smallest index next to min is second in the list
	if ((newList.size() >= 3) && (newList[1] > newList[newList.size()-1])) {
		int end = (newList.size() - 1)/2;
		int temp;
		for (i = 1; i <= end; ++i) {
			// Swap i and size-i
			temp = newList[i];
			newList[i] = newList[newList.size()-i];
			newList[newList.size()-i] = temp;
		}
	}
	
	// Copy the list back
	for (i = 0; i < (signed int)m_ringAtomIndices.size(); ++i)
		m_ringAtomIndices[i] = newList[i];
	newList.clear();
}

