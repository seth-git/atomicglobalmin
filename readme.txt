Atomic Global Minimum Locator 1.6.2 Release Notes

Release date: September 8th, 2012

This software searches the potential energy surface of small to medium size atomic systems for global minima using quantum ab initio techniques. It performs bond rotations and molecule translations and rotations on a Linux cluster with MPI.

Version 1.6.2
	- Fixed a small bug relating to the initialization of linear, planar, and 3D structures.

Version 1.6.1
	- Fixed bug for the -i option.

Version 1.6.0
	- This version adds support for multiple languages.  Currently, only English is available, but messages can be much more easily translated into other languages either manually or via Google translate.  See the language support section of the documentation.
	- This version offers documentation in a different format.  The old pdf user manual has been replaced with web-based documentation, which has an added printer-friendly format.  To load this documentation in a web browser, double click the index.html file inside the docs folder.

Version 1.5.2
	- Fixed a small bug relating to the initialization of linear, planar, and 3D structures.  Also removed unused variables that were causing compiler warnings.

Version 1.5.1
	- Updated the documentation.  No program or code changes.

Version 1.5.0
	- This release includes implicit support for GAMESS, including an input file generation method, and some documentation updates.  This version also includes bug fixes for the simulated annealing -i option.  In particular the program now will not restart completed runs, if the user accidentally tries to do this or if the user is performing multiple separate runs using the -i option and some but not all of the runs have completed.  Also when using the -i option, the program will now always update the main output file from the individual output files when the run completes or shortly before the wall time is reached.  This new version also includes general refactoring to ensure clean program termination.

For release notes on older versions, see the subversion log.

