////////////////////////////////////////////////////////////////////////////////
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "unit.h"

bool memoryTest()
{
	MoleculeSet* pMoleculeSet;
	
	cout << "Testing memory deletion of MoleculeSet objects...";

	pMoleculeSet = new MoleculeSet();
	pMoleculeSet->setOutputEnergyFile(".", "best", 1, "com", 0, false);
	pMoleculeSet->setOutputEnergyFile(".", "best", 1, "com", 1, false);
	delete pMoleculeSet;

	pMoleculeSet = new MoleculeSet();
	pMoleculeSet->setOutputEnergyFile(".", "best", 1, "com", 1, false);
	pMoleculeSet->setOutputEnergyFile(".", "best", 1, "com", 0, false);
	delete pMoleculeSet;

	pMoleculeSet = new MoleculeSet();
	pMoleculeSet->setOutputEnergyFile(".", "shouldNeverExist", 1, "bogusExtension", 3, true);
	pMoleculeSet->setOutputEnergyFile(".", "shouldNeverExist", 1, "bogusExtension", 29, true);
	delete pMoleculeSet;
	
	cout << " Passed!" << endl;
	
	return true;
}

int main(int argc, char *argv[])
{
	if (!memoryTest())
		return 1;
	cout << "All Tests Passed!" << endl;
}

