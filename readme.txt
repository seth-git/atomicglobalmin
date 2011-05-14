Atomic Global Minimum Locater Software

by Seth Call

Table of Contents

I. Introduction
    A. Getting Started
    B. Input, output, resume, and optimization files
    C. Running and restarting the program
    D. Obtaining results from output, resume, and optimization files
    E. Performing bond rotations using a deterministic, non-random search
    F. Configuring the program for quantum chemistry packages
    G. Developing new features
    H. References
    I. Disclaimer

II. Input file formats and optimization of random algorithm parameters
    A. Input parameters common to all algorithms
    B. Run output formats
        1. Particle swarm output
        2. Simulated annealing and basin hopping output
        3. Genetic algorithm output
    C. Particle swarm optimization input parameters
    D. Simulated annealing and basin hopping input parameters
    E. Genetic Algorithm

III. Running the program on a cluster of computers
    A. Submitting the program to the PBS Scheduler
    B. MPI options

IV. Other useful tips
    A. More about optimization files
    B. Creating quantum input files from a resume or optimization file
    C. Seeding the starting population with structures from previous runs
    D. Setting a wall time to ensure cleaning of scratch files
    E. Transition State Searches
    F. Trouble Shooting

I. Introduction

This software searches for chemical structures that are global minima on the potential energy surface.  It uses highly accurate ab initio methods and reduces the cost of these by using distributed computations on a Linux cluster. The software allows energy calculations to be performed using a wide variety of external quantum chemistry programs including ADF, GAMESS, GAMESS-UK, Gaussian, Jaguar, Molpro, and ORCA, as well as an internal Lennard Jones potential used for testing purposes.

This program can perform two different types of searches.  First, it searches for the global minimum of a set of molecules that each have fixed geometry using translation and rotation of molecules with respect to one another.  The program has four different random algorithms for this type of search: particle swarm optimization, enhanced simulated annealing, basin hoping, and a simple genetic algorithm. The second type of search is a bond rotational search for a single molecule.  This is a deterministic, non-random search using a user-specified bond rotational angle.

Finding accurate global minima even for small molecules can be challenging.  This software is an automated tool making it simple and easy to find such minima using the more accurate quantum methods.

A. Getting Started

While this program does not have a graphical user interface, it has a command line interface that is very simple to use.  Here are a few programs/information links you might find useful:

	Transfer files between Linux and Windows: http://www.winscp.com/
	Log into Linux from Windows: http://www.chiark.greenend.org.uk/~sgtatham/putty/
	Learn Unix/Linux: http://librenix.com/?inode=4052
	Edit Linux files: http://www.oregonwebradio.net/backup_fedora/tutorials/vim_li/quickstart.html

You will need to compile the C++ source code on a Linux computer or cluster.  While Windows is not supported, you may easily transfer results to and from your Windows computer.  Steps for installing this program are below:

1. Download the program file: pso1.5.0.tar.gz
2. Install g++ and MPI compilers
3. Compile the program

Step 1: You can download pso1.5.0.tar.gz file from the same place you downloaded this instruction file.  If you want to run the program on a remote Linux cluster, first download the program onto your local computer and then transfer the program to the Linux cluster.  If you use Windows, you can transfer the program to the Linux cluster using the WinSCP program (see above link).  If you are using Ubuntu or a Mac, open a terminal.  To open a terminal on Ubuntu, click on 'Applications', 'Accessories', 'Terminal'.  On a mac, navigate to your Applications folder, open Utilities, and double click on Terminal. Then, type these commands, pressing 'Enter' or 'Return' after each command:

	cd /directory/where/you/put/pso1.5.0.tar.gz
	scp  pso1.5.0.tar.gz your_user_name@your.linux.cluster:pso1.5.0.tar.gz

Step 2: If using a University Linux cluster, the g++ and MPI compilers should already be installed, so you can likely skip this step. If using a local Linux box, g++ should already be installed, but you will have to install the MPI compiler.  On Ubuntu Linux, you may install this by clicking on 'System' and 'Administration', then 'Synaptic Package Manager' and searching for mpi.  I installed the mpi-default-bin and mpi-default-dev packages.

Step 3: If you are running the program on a remote Linux cluster, you will need to log into the cluster with ssh.  To do this on Windows, use the putty program (see the above link).  To log in at the command line on Ubuntu or a Mac, type the following at the command line:

	ssh your_user_name@your.linux.cluster

Once logged in, navigate to the directory where you placed the program.  Change directories by typing: 'cd some_directory'.  To get out of the current directory type: 'cd ..'   .  To list files and directories in the current directory, type 'ls' or 'ls -al' (the later is a long list format).  When you have navigated to your program directory, type the following to decompress the program:

	gunzip pso1.5.0.tar.gz
	tar -xf pso1.5.0.tar

Then, navigate into the directory:

	cd pso

Then compile the program by typing:

	make

You may see warning messages.  If so, this is OK.  If you see error messages, this is NOT OK, and it likely means you do not have the right compilers installed (see step 2).

When you have successfully compiled the program, two executables will be created: pso and helper. The pso executable is an MPI application, while helper is not.  The helper application contains additional utilities.  A list of the options available with the pso and helper executables can be obtained by typing "./pso" or "./helper" respectively.

If using the program with ADF, GAMESS, GAMESS-UK, Jaguar, Molpro, or ORCA , please see configuration instructions in section F.

B. Input, output, resume, and optimization files

Each of the four algorithms, particle swarm optimization (PSO), basin hopping, simulated annealing, and the genetic algorithm has a different input file (file extension .inp).  These were designed to be as self explanatory as possible. Simulated annealing and basin hopping have a common input file, since they are similar algorithms.  Each time the program is run, it creates an output file (file extension .out), which contains the input parameters as well as summary information on the progress of the run.  The program also creates a resume file (file extension .res), which contains all information necessary for restarting a run that was stopped before it was completed.  An optimization file (file extension .opt) can be created from a resume file in order to perform a local geometry optimization on the list of best structures after the run has completed.

C. Running and restarting the program

Four example input files are LJ7_PSO.inp, LJ7_Sim.inp, LJ7_BH.inp, and LJ7_GA.inp, for PSO, simulated annealing, basin hopping, and the genetic algorithm respectively. To run PSO, type:

      ./pso LJ7_PSO.inp

Other input files are run similarly.  The LJ7_PSO.inp input file searches for the gobal minimum of 7 noble gas atoms (He, Ne, Ar, etc.) using the Lennard Jones potential.  The global minimum for LJ7 has energy -16.505384.  To prematurely terminate a PSO or other type of run, press Ctrl-C. To restart the PSO run after it has been stopped, type "./pso LJ7_PSO.res". Restart other types of algorithms similarly.

This type of search performs translation and rotation of rigid molecules in search of a global minimum.  It does not perform a bond rotational search.  This type of search must be performed separately as described in section E (although modifications to the program could combine a bond rotational search with a translational and rotational search).

To create a geometry optimization file from the LJ7_PSO.res resume file, type:

	./helper -o LJ7_PSO.res LJ7_PSO.opt 100 10

This creates an optimization file called LJ7_PSO.opt and transfers 100 structures to it from LJ7_PSO.res.  To run the optimization file type:

	./pso LJ7_PSO.opt

After each set of 10 structures have been optimized, the program will update the LJ7_PSO.opt file until all structures have been optimized.

Note that if you open an optimization file, it will say simulated annealing, particle swarm, or genetic algorithm at the top. This is because optimization files were originally designed to perform a geometry optimization on the list of best structures after a simulated annealing, particle swarm, or genetic algorithm run.  Now, they are more generally used to perform calculations on any list of chemical structures.  For example, this program uses optimization files to perform bond rotational searches.  Even though geometry optimization may or may not be used, the file type is still called an 'optimization' file.

D. Obtaining results from output, resume, and optimization files

After normal termination of a run, the program writes the best structure found to the output file.  To open the output file, type "vim LJ7_PSO.out".  When you are done, type ":q" and then the enter key. To obtain preliminary results from the resume file, open the resume file using the vim command.  The top portion of the resume file contains parameters. Following these are chemical structures in the current population.  Following these are the list of best structures found during the entire run.  To search for this list, type "/Best" and then the enter key. When you are done viewing the list, type ":q" and then the enter key.  The program can also export the structures from the resume or optimization file to a temporary file in a readable format.  Note: the coordinates of units/molecules should never be read directly from a resume or optimization file as these require translation and rotation.  To perform translation and rotation of the list of best structures into a readable format, type "./helper -t LJ7_PSO.res temp.txt".  This also works for optimization files.  Also note that the output files from your quantum chemistry program are saved.

E. Performing bond rotations using a deterministic, non-random search

The pso program can perform bond rotations on a single molecule in search of the lowest energy structure.  This is done with the -b and -m options.  The -b option produces a printout of bond locations and indicates which single bonds are 'rotatable'.  The -m option is used to create an optimization file with all possible bond rotational positions of the monomer.  You can then perform an optimization of the structures in the optimization file to obtain the results.  An example of how to use the -b option with the provided Isoprene.inp file is as follows:

      ./helper -b Isoprene.inp

Use the -b option to ensure that bonds have been correctly identified. Then, use the -m option to generate an optimization file containing all possible bond rotational positions of the monomer.  An example of how to do this is as follows:

      ./helper -m Isoprene.inp 30 Isoprene.opt 200

This creates the optimization file Isoprene.opt from the input file Isoprene.inp, using bond rotational angles of 30 degrees.  When the structures in Isoprene.opt are optimized, the file is re-written after every set of 200 structures have been optimized.

Before generating an optimization file, consider how many bond rotational positions are possible to ensure that the optimization file to be created will not be too large.  For this Isoprene Hydroxy Alkyl Peroxy Radical example, if we rotate bonds by 30 degrees, that gives 360 / 30 = 12 possible rotational positions for each bond.  In this molecule, there are 5 rotatable bonds. That gives 12 to the power of 5 or 248,832 possible combinations.  Fortunately, one of the rotatable bonds has a methyl group attached, leaving only 12/3 = 4 rotational positions for that case.  This reduces the number of possible combinations to 4*(12^4) = 82,944.  This program automatically detects methyl groups and also removes configurations that do not satisfy minimum inter-atomic distance constraints specified in the input file.  Also note that while this program does detect rings, it does not handle them.  Ring bonds are therefore labeled as non-rotatable.

Before doing a bond rotational search be sure to check that bonds are in the appropriate locations and are identified correctly as single, double, or triple using the -b option.  If they are not, manually change the criteria used to identify bonds in the file bondLengths.txt. The format of this file is atomic symbol, atomic symbol, bond type ('s' for single, 'd' for double, 't' for triple), minimum distance-maximum distance, where distances are in angstroms.

Note that the bondLengths.txt file was automatically generated from average bond lengths stored in the files averageSingleBondLengthsInAngstroms.txt and averageMultipleBondLengthsInAngstroms.txt using the Perl script updateBondLengths.pl.  These average bond lengths were taken from the Handbook of Chemistry and Physics.  The updateBondLengths.pl program allows bonds to be 15% larger or smaller than average bond lengths when generating bondLengths.txt.

F. Configuring the program for quantum chemistry packages

To support various quantum chemistry packages, it is necessary to create methods for writing input files and reading output files.  The more difficult challenge of reading output files has been solved for you by incorporating the cclib package into this software.  The cclib package can read output files from ADF, GAMESS, GAMESS-UK, Gaussian, Jaguar, Molpro, and ORCA.  Methods for creating input files are provided for GAMESS and Gaussian.

To install cclib, follow the instructions at cclib.sf.net.  Once installed, copy the atomicGlobalMin.py file from the pso directory to the directory where you installed cclib.  Then open the energyProgram.cc file inside the pso directory and set the cclibPythonScript variable to the full path of the atomicGlobalMin.py script inside the cclib directory.  Recompile the program by typing 'make'.  Test the cclib package by running unit tests by typing './unit'.  If you see “All tests passed!”, then cclib has been installed correctly.

Then, if necessary add your quantum chemistry package as follows:
1. Add a one-line define statement to energyProgram.h as follows:
   #define NAME_OF_YOUR_QUANTUM_CHEMISTRY_PROGRAM             UNIQUE_INTEGER

2. Modify energyProgram.cc as follows.  Modify the init function by copying the GAMESS or Gaussian program information and modifying it for your quantum chemistry package.  Note that the parameters you need to set are listed in the EnergyProgram function definition immediately beneath the init function.  Set the program name, executable path, input and output file extensions, and other basic information.  If you are using cclib, set the cclib flag to 'true'.  You may also list multiple output file types, with a file extension for each.  Please list first the output file extension of the file type you want to be read by the program.  If you list other file types, these will be saved with the results, but will not be read by this program.

3. Modify energy.cc as follows.  First, write a method for creating an input file for your quantum program.  To do this, you may copy and modify the createGamessInputFile method.  Make sure that your method is called correctly from the createInputFile method and that it is declared in energy.h.  Test your input file creation method as follows.  Create a resume file by typing './pso LJ7_Sim.inp' and stop the run by pressing control-C.  Open LJ7_Sim.res, change the energy function to the name of your quantum package, insert an energy file header, and set the charge and multiplicity.  Then type './helper -c LJ7_Sim.res test best'. This will create an input file in a directory called test which you can check for correctness.

Second, write a method in energy.cc for reading your quantum program's output file.  Make sure your method is called correctly from the readOutputFile method and that it is declared in energy.h.  If using cclib to read output files, this step can be skipped.

Third, check that your quantum program is called correctly in the doEnergyCalculation method.

Note: programs such as Gaussian require a scratch directory.  The scratch directory is specified on line 5 of your input file.  If using Gaussian, the pso program tells Gaussian where to put scratch files using the command "export GAUSS_SCRDIR = your_scratch_directory".  If you need to modify this command, or change it for another quantum chemistry package, it's listed in the energy.cc file in the two init functions.

G. Developing new features

If you create a method for writing a new input file type or reading a new output file type, please contribute your method so others can use it too. In fact if you have any new features you want, we encourage you to participate in their development.  Feel free to contact us by going to the website and clicking 'Support'.  Then, click 'creating a new item', select 'New Feature' or 'Question' for the category, enter a subject and description, select a file if you have one, and click 'Add Artifact'.

H. References

A few publications where the software has been used are below:

[1] Clark, J.; Call, S. T.; Austin, D.; Hansen, J. Computational Study of Isopren Hydroxyalkyl Peroxy Radical-Water complexes (C5H8(OH)O2-H2O). J. Phys. Chem. A, 2010, 114 (23), 6534–6541.

[2] Averkiev, B. B.; Call, S.; Boldyrev, A. I.; Wang, L. M.; Huang, W.; Wang, L. S. Photoelectron spectroscopy and ab initio study of the structure and bonding of Al7N- and Al7N. J. Phys. Chem. A. 2008, 112(9), 1873-1879.

[3] Call, S. T.; Boldyrev, A. I., Zubarev, D. Y. Global Minimum Structure Searches via Particle Swarm Optimization. J. Comput. Chem. 2007, 28, 1177-1186.

Note: If you wish to reference this software in a publication, please reference the web site, http://sourceforge.net/projects/atomicglobalmin/, as well as the publications above.

I. Disclaimer

Note that while developers of this product strive to write high quality software, bugs do happen occasionally.  If you find bugs, please report them.  Also, this software needs more unit tests.  If you can help write these by modifying unit.cc, that would be helpful.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

II. Input file formats and optimization of random algorithm parameters

This section discusses the parameters in the input file and techniques for setting them.  Knowing how to set parameters optimally can influence the performance of each algorithm.  While setting these parameters takes practice, examples are provided.  Also, some of the more important parameters can be set for you automatically by the program.

A. Input parameters common to all algorithms

Parameters common to all algorithms appear at the top and bottom of each input file.  An example of the parameters at the top of an input file for (H2O)3OH- are as follows:

1   Particle Swarm Optimization Input File Version 1.5.0
2   
3   Energy function to use: Gaussian
4   Path to energy files: /path/to/quantum/input/and/output/files/
5   Path to scratch directory (optional): /my/scratch/directory/
6   Search cube length, width, and height: 7.0
7   Number of linear structures: 0
8   Initialize linear structures in a box with the above cube length and a height and width of: 1.2
9  Number of planar structures: 0
10  Number of fragmented 3D structures: 0
11  Number of partially non-fragmented 3D structures: 0
12  Number of completely non-fragmented 3D structures: 64
13  General minimum atom distance (used if no value specified below): 0.7
14  Specific minimum atom distances (angstroms):
15  1 1 0.9
16  1 8 0.8
17  8 8 1.2
18  Maximum inter-atomic distance (angstroms): 3.0

The header on line 1 specifies the type of algorithm (particle swarm optimization, simulated annealing, etc.).  The phrases "Input File", "Resume File", and "Optimization File" indicate the file's purpose.  The header also contains a version number.  If you attempt to run an input file that belongs to an older version of the program, you may have to change or insert lines in the input file before it will run.  The program will specify when lines are missing or need to be changed.  The path to the energy files on line 4 is the directory where the input and output files for your quantum chemistry package are stored.  If using a cluster of Linux computers, this should be a directory accessible to all nodes.  The scratch directory (line 5) is where all scratch files are written.  When using Gaussian, always specify a scratch directory.  If running on a cluster, the scratch directory can be on a local hard drive to decrease network traffic.  Output files are copied from the scratch directory back to the energy files directory (line 4).

When specifying the number of each structure type to initialize (lines 7-12), keep in mind that atoms are placed in groups each of which is moved and rotated as a "unit" or molecule.  Each "unit" contains one or more atoms.  Line 8 indicates that linear structures are initialized so that all atoms are in a 3D box that is 1.2 x 1.2 x 7.0.  Planar structures are initialized so that centers of mass of each "unit" are on the same plane.  The fragmented, partially non-fragmented, and completely non-fragmented structures are described informally here.  For a more precise discussion, see reference 3 above.  Fragmented structures contain "units" in random locations within the search cube. Partially non-fragmented structures are created such that every atom is within the maximum distance of at least one other atom.  These are called "partially" non-fragmented because two units may be isolated from the rest of the units (further than the maximum distance).  With completely non-fragmented structures, no two units can be isolated from other units.  For example, if you started at some unit and could only travel to other units within the maximum distance of that unit, you could travel to any unit.

The program allows one maximum distance between atoms (in this case 3 angstroms), and multiple minimum distances between different types of atoms.  In this case the minimum distance is 0.9 between two hydrogens, 0.8 between hydrogen and oxygen, and 1.2 between two oxygen atoms.

An example of the parameters at the end of the input file are given below:

42  Save this many of the best "different" structures: 1000
43  Consider 2 structures "different" if their RMS distance is greater or equal to (angstroms): 0.7
44  Save this many quantum output files from the list of best structures: 50
45  Output file name: /path/to/pso/files/OH3H2O_PSO.out
46  Resume file name (optional): /path/to/pso/files/OH3H2O_PSO.res
47  Write resume file after every set of this number of iterations (optional): 1
48  Print summary information after each set of this many iterations: 1
49  Charge: -1
50  Multiplicity: 1
51  Energy file header:
52  % chk=mychk
53  # B3LYP/6-311++G** opt=(calcfc) scfcon=6 scfcyc=300 optcyc=20 scf=direct
54  
55  Energy file footer (optional):
56  --link1--
57  % chk=mychk
58  # B3LYP/6-311++G** freq=noraman scfcon=6 scfcyc=300 optcyc=100
59  geom=check guess=read
60  
61  Number of unit types: 2
62  
63  Number of units of this type: 3
64  Format of this unit type: Cartesian
65  1 0.757540797781 0.0 0.587079636589
66  1 -0.757540797781 0.0 0.587079636589
67  8 0.0 0.0 0.0
68  
69  Number of units of this type: 1
70  Format of this unit type: Cartesian
71  1 0.0 0.0 1.064890
72  8 0.0 0.0 0.0

Line 42 specifies the size of the list of best structures that are saved during the course of the entire run.  This list is updated after every iteration.  Line 43 specifies that each structure in the list of best structures is different from every other structure in the list by a certain RMS distance (see reference 3 above).  Line 44 is the number of chemical structures for which the quantum chemistry package output files will be saved.  These output files are saved in a folder called bestSavedStructures located inside the directory where energy files are stored (specified on line 4 of the input file).  Lines 45-53 should be self explanatory.  When using Gaussian, the program will automatically rename the checkpoint file for each structure in the population of candidate structures, so that there aren't naming conflicts.  The energy file footer (lines 55-59) is placed at the end of the quantum package input file.  In this case, the footer specifies that a frequency calculation be performed.  Note that the energy file header and footer are not used when energies are calculated using the Lennard Jones potential.

Lines 61-72 are an example of how to specify groups of atoms, molecules, or "units".  The above consists of three water molecules and one hydroxide ion. Lines 65-67 specifies the coordinates of water with two hydrogen atoms and one oxygen atom.  Lines 71-72 specify coordinates for hydroxide.  If you are dealing with single atoms that are moved separately form other components in the system, one atom can be specified for each "unit".  The "format of this unit type" can only be "Cartesian ".  At some point in the future, I may allow other types of input such as "Z-matrix."

B. Run output formats

As the program runs, it prints summary information to the screen.  As you modify the particle swarm or simulated annealing parameters, monitor the run's progress using this output.  The output meaning is described below.

     1. Particle swarm output

     The summary output from the PSO program looks like this:
       It: 110 Best Energy: -12.105384 Coord Vel Max,Avg: 0.200, 0.057 Angle Vel Max,Avg: 0.000, 0.000 Div: 0.070 Vis: 72.9%

     The meaning of each value is as follows:
       It: iteration number
       Best Energy: the energy value of the lowest energy structure
       Coord Vel Max,Avg: Coordinate velocity (the maximum and average respectively) for each atom or group of atoms
       Angle Vel Max,Avg: Angle velocity (the maximum and average respectively) for each group of atom (will be zero if
           there are no groups containing more than one atom)
       Div: The diversity in the population (typically between 0 and 0.5)
       Vis: The percentage visibility (average percentage of other solutions that is visible to each solution, see paper 3)
     
     2. Simulated annealing and basin hopping output
     
     The summary output for simulated annealing and basin hopping looks like this:
     It: 30 Best Energy: -9.62310778 Temp: 777346.9 Num Pert: 4 Coord, Angle Pert: 0.3000, 20.0000 Accepted Pert: 87.1%
     
     The meaning of each value is as follows:
       It: iteration number
       Best Energy: the energy value of the lowest energy structure
       Temp: The current temperature
       Num Pert: The number of perturbations performed at one time
       Coord, Angle Pert: The amount by which coordinates and angles change respectively (coordinates are moved
              in a random direction by the specified amount)
       Accepted Pert: The percentage of accepted perturbations over the previous n iterations (see input file)
     
     3. Genetic algorithm output

     The summary output for the genetic algorithm looks like this:
       It: 60 Best Energy: -10.10563270 Diversity: 0.05333658

     The meaning of each value is as follows:
       It: iteration number
       Best Energy: the energy value of the lowest energy structure
       Diversity: The diversity in the population (typically between 0 and 0.5)

When monitoring the course of an algorithm, check to see how often the best energy value changes and by how much.  For simulated annealing, the percentage of accepted transitions should start high (about 90%).  If this starts too high (i.e. 95%), however, it may take a very long time to come down.  For particle swarm optimization, the visibility should start low (i.e. 0%) and increase slowly to (100 %).  For particle swarm optimization and the genetic algorithm, the diversity indicates how different members of the population are from one another.  Populations must be sufficiently diverse in order to produce meaningful progress.  See the paper at the beginning of this document for the formulas used to calculate visibility and diversity.

C. Particle swarm optimization input parameters

An example list of the parameters for the particle swarm optimization algorithm are given below.

20  Particle Swarm Optimization Parameters:
21  Start coordinate inertia (w): 0.95
22  End coordinate inertia (optional): 0.8
23  Reach end coordinate and angle inertias at iteration (optional): 10000
24  Coordinate individual minimum attraction (c2): 0.15
25  Coordinate population minimum attraction (c1): 0.15
26  Maximum Coordinate Velocity (Vmax, optional): 0.3
27  Start angle inertia (w): 0.95
28  End angle inertia (optional): 0.8
29  Angle individual minimum attraction (c2, deg): 15
30  Angle population minimum attraction (c1, deg): 15
31  Maximum Angle Velocity (Vmax, optional): 30
32  Starting RMS visibility distance (use 'auto' for automatic): auto
33  Increase the RMS visibility distance by this amount each iteration: 0.003
34  Switch to the repulsion phase when (1) diversity is below (optional): 0.15
35  And when (2) progress hasn't been made for this number of iterations (optional): 200
36  Switch to the attraction phase when diversity is above (optional): 0.2
37  Don't update individual best solutions within this RMS distance of the best seen by the population: 0.01
38  Maximum number of allowed iterations: 10000
39  Enforce minimum distance constraints on a copy of each structure rather than on the original: yes
40  Use energy value from local optimization: yes

If you are new to particle swarm, you can start with the parameters above and adjust them as needed.  Because ab initio calculations can take a long time, it is best to use the Lennard Jones potential to test the parameters you have chosen.  Though this energy function behaves very differently, doing this will help you optimize parameters not strongly related to the energy function (i.e. visibility parameters).  After the parameters seem to work well with the Lennard Jones potential, try them with your quantum chemistry package and check for differences in how the algorithm performs.  While this may seem silly, it can save you from changing parameters in the middle of a run that can take weeks or months.

The w, c1, c2, and Vmax parameters (lines 21-31) are typical to particle swarm optimization, and a description of these is available in the literature on the Internet or in paper 3 above.  The parameters that most likely need to be changed are c1, c2, and Vmax (lines 24-26 and 29-31).  These affect how refined the search is.  Lowering these will favor local rather than global searches.

Parameters on lines 32-36 control the progress of the algorithm in the long term and are important.  These should be set so that at the beginning of a run each candidate structure can see and is influenced only by structures that are closely related.  In other words, the percentage visibility should start low, i.e. 0-1%, and increase slowly to 100%.  The percentage visibility can be observed as "Vis" in the summary output seen while the program runs.

The program also has an automatic method of assigning the starting RMS visibility distance (line 32).  If this option is turned on, the starting RMS visibility distance will be set to 80% of the smallest RMS distance between any two chemical structures in the population.  The time when this distance is set can vary, and before it is set, the RMS visibility distance (and percentage visibility) is zero.  The distance is set when one of three conditions is met:
      1. The best observed energy value hasn't changed for 70 iterations, and the average coordinate velocity (see summary output) is less than half of the maximum coordinate velocity (Vmax, line 26).
      2. Iteration 200 has been reached.
      3. The average velocity of coordinates is below 0.005.
Since the starting visibility distance can be hard to set, it is hoped that this automatic option will simplify setting the parameters.

Once the visibility distance reaches 100%, the diversity in the population will likely be low.  The degree of diversity in the population can be observed as "Div" in the summary output seen while the program runs.  When this gets low (see parameters on lines 34 and 35), the algorithm will temporarily switch to the repulsion phase.  When it switches back (see line 36), it will again start with a low visibility which will increase slowly to 100%.  Also, the memory of each individual solution is erased by replacing it with a structure from the list of best structures (see line 42).  For this reason, it is best to set line 42 to be equal to or larger than the total population size.  It is also a good idea to set the distance between structures in the list of best structures (line 43) to be slightly larger than the starting visibility distance (line 32).  Note that while entering the repulsion phase can reintroduce diversity, it is best if the algorithm can find a minimum without entering this phase (i.e. it should only be used as a backup).

The parameter on line 37 was designed to prevent the best structure seen by each individual from becoming too close to the best structure seen by the population.  I usually don't change it.

The parameter on line 38 is a maximum number of iterations allowed.  This is the only way the program is terminated automatically, though the program can be terminated manually at any time by pressing Control-C (or qdel if running on a Linux cluster).

The parameter on line 39 deals with an automatic function that the program has for enforcing minimum distance constraints (see paper 3 above).  It could be said that enforcing the minimum distance constraints modifies the way the algorithm works.  Line 39 specifies that the program should make a copy of each structure before enforcing the minimum distance constraints and calculating the energy, so the progress of the algorithm is not modified.

This algorithm can also be used with local optimization (see line 40).  When local optimization is used, the energy of a structure is assigned to be the energy obtained through local optimization, but the positions of atoms and the progress of the PSO algorithm are not affected when this optimization is turned on.  When using this option, a good approach is to limit the number of optimization steps rather that performing a full geometry optimization.  If a full geometry optimization is desired, this may be performed on the list of best structures after the run completes.

D. Simulated annealing and basin hopping input parameters

An example list of the simulated annealing parameters are given below.

20  Simulated Annealing Parameters:
21  Search only for non-fragmented structures: yes
22  Use energy value from local optimization(basin hopping): no
23  Search for transition states (random search with every perturbation accepted): no
24  Starting temperature or desired starting percentage of accepted transitions(use %): 85%
25  Boltzmann constant: 3.1669e-6
26  Minimum number of iterations before decreasing the temperature (N): 200
27  Decrease the temperature when the percentage of accepted transitions for the past N iterations is below: 100
28  Quenching factor(use 1 for basin hopping): 0.9995
29  Don't stop while the temperature is above this value: 400.0
30  Stop if the percentage of accepted transitions for the past N iterations is below: 1.0
31  Maximum number of allowed iterations: 100000
32  Starting number of perturbations per iteration: 3
33  Starting coordinate perturbation (angstroms): 0.25
34  Minimum coordinate perturbation (angstroms): 0.05
35  Starting angle perturbation (deg): 25
36  Minimum angle perturbation (deg): 3

This simulated annealing algorithm has the ability to perform non-fragmented searches, or in other words it can enforce minimum and maximum distance constraints through the course of the entire run.  This enables faster, more relevant searches.  To enable this feature, place yes on line 21.  Putting Yes for line 22, and setting the Quenching factor to 1 turns on Basin Hopping.  Basin hopping is very similar to simulated annealing except the temperature (line 24), number of perturbations to perform at a time (line 32), and perturbation amounts (33-36) do not change.  Also, when performing basin hopping, the energy value is obtained by making a copy of the original structure, performing optimization to the nearest local minimum, and assigning that energy value to the original structure.  By tradition, simulated annealing and basin hopping use only one structure and do not use a population of candidate structures, though this application will allow populations of solutions.  A population of solutions with these two algorithms is essentially a set of independent runs.  Using a population of solutions with these algorithms greatly increases the probability of finding the correct minimum and is highly recommended.

At each step, simulated annealing makes a number of random perturbations.  Each perturbation can move the center-of-mass for a group of atoms, a molecule, or a "unit" along a random vector the length of which is specified on lines 33 and 34.  Alternatively a perturbation can rotate one of three angles in a molecule or unit by a random amount (see lines 35 and 36).  Once a perturbation has been made, the energy is recalculated.  If the energy value is better, the perturbation is accepted. If it is worse, the perturbation is accepted with a certain probability defined as:

     p = e^(-ΔE/(k * T))

where ΔE is the change in energy in atomic units (hartrees), k is the Boltzmann constant in units of hartrees per kelvin (line 25), and T is the temperature in units of kelvin (line 24).  At the start of a run, the temperature and the probability of accepting a bad perturbation are very high. The temperature T, the number of perturbations P, the coordinate perturbation amount C, and the angle perturbation amount A decrease as follows:

     T = T x quenching factor (line 23)
     P = P x SquareRoot(quenching factor)
     C = C x SquareRoot(quenching factor)
     A = A x SquareRoot(quenching factor)

At the end, the temperature and the probability of accepting bad perturbations are very low. Essentially, the algorithm is a slow transition between a random search and a downhill search.

Since the starting temperature (line 24) is sometimes time consuming to set optimally, this program now has an automatic method for setting this.  Instead, of specifying a starting temperature, you may specify a starting percentage of accepted transitions (see summary output displayed while the program is running).  Typical values range from 70-90%, and 85% works for many applications.  Note that if the percentage of accepted transitions is higher than 90%, it may take a long time for it to come down.

The quenching factor (line 28) controls the length of the run and is also important.  Set this so that the run will last long enough to produce good results but short enough to be reasonable.  Keep in mind the relationship: starting temperature * (quenching factor ^ number of iterations) = final temperature.  Typical values are between 0.999 and 0.99999.  I typically use 0.9995.

There are two ways a run can be terminated automatically.  The first is if the run exceeds a maximum number of iterations (line 31).  The second is if the percentage of accepted transitions is below a specified value (line 30) and if the temperature has dropped below a specified minimum value (line 29).  Generally, it is best if the percentage of accepted transition and the temperature are low toward the end of a run, though you may stop a run at any time.

Theoretically simulated annealing is guaranteed to find the global minimum if cooling proceeds slowly enough, though there is no guarantee that it can be done within a reasonable amount of time.  Because of the expensive nature of ab initio calculations, large numbers of calculations are sometimes not practical, though the theoretical guarantee is a plus.  Using a population of solutions is also very helpful in finding global minima.

It is often best to use the Lennard Jones potential for testing the parameters until they are in the ball park.  Though this energy function behaves very differently, doing this will help you find obvious errors in the input parameters and give you a rough idea of how the algorithm will proceed before you start using expensive ab initio calculations.  After the parameters seem to work well with the Lennard Jones potential, try them with your quantum package.  Check for differences in how the algorithm performs and make adjustments.  This is particularly important if you are performing runs that take several weeks.

E. Genetic Algorithm

Genetic algorithms (GA) are an efficient means of searching for global minima.  While my focus has been on developing 'new' algorithms such as particle swarm optimization, GA algorithms are very important and widely used.  The genetic algorithm in this application is well designed, but it needs a new mating algorithm (see: the makeChild function in moleculeSet.cc).  If you are interested in doing this, please contact me.  If you want the most efficient algorithm, you might think about implementing a differential evolution algorithm.  In the mean time, use simulated annealing or particle swarm optimization.  Simulated annealing with a small population size, single point energy calculations, and the -i option is quite efficient.  Afterwards it's a good idea to perform local geometry optimization on the list of best structures using an optimization file.  This was my approach in reference 1 above.


III. Running the program on a cluster of computers

A. Submitting the program to the PBS Scheduler

This section describes how to run the program on a cluster of Linux computers with a quantum chemistry package.  Some familiarity with your quantum chemistry package and Linux is assumed.  It is also assumed that you have read the instructions for configuring quantum chemistry packages in section F.

Create a script file for submitting the main pso program to the PBS (Portable Batch System) scheduler. An example script file for doing this called "examplePBSScript" is in the "pso" directory.  Make a copy of this script and modify it as follows.

    1. Line 2 in the script file specifies the number of nodes you want to use and the number of processors per node (ppn) you want.  Make sure your population size is divisible by this number.  This application was designed to perform one energy calculation on each processor.  The wall time is a maximum time limit you want to allow your job to run for.  Set this appropriately. The example file specifies 24 hours.
    2. Line 3 is where you set the memory limit per node that you want.  Also set the memory limit in the Gamess or Gaussian header in the pso input file.
    3. Line 16 specifies that the list of nodes allocated by the PBS scheduler be copped to a file called "nodes.txt" in the energy files directory (line 4 of the pso input file).
    4. Change lines 22 and 23 (the last two lines).  These lines call the pso program.  It is best to put the input, output, and resume files in a separate directory that is backed up and not in the main pso directory.    NOTE: WHEN RESTARTING THE PROGRAM, DON'T FORGET TO CHANGE THE INPUT FILE NAME TO THE RESUME FILE NAME IN THE PBS SCRIPT FILE.

Also be sure to modify the pso input file appropriately.  On line 3 for the energy function parameter, specify the name of your quantum chemistry package.  On line 4, specify the full path of a directory where you want output files from your quantum chemistry package to be written.  You will need to create this directory, and it will need to be unique for every input file you want to run simultaneously with the program.  Specify the full path to the scratch directory on line 5.  The pso program will create sub-directories in this directory where scratch files will be written.  These sub directories have the name of the pso input or resume file.  The scratch directory can be on the hard drive of each local node or on a shared drive.  The pso program will delete scratch files it creates if you use the -walltime option (see section IV, D).

Make sure the names of your output and resume files are set appropriately in the input file.  Use full path names.  Also, modify the header and optional footer to your quantum package appropriately.  The header will be placed at the start of every quantum input file.

Once you have created your script file and your input file and checked them twice, type the command qsub followed by the name of your script file.  To look at jobs in the queue, type the command qstat.  To look at only the jobs you have submitted, type 'qstat | grep your_user_name'.  To delete a job from the queue type: 'qdel job_number'.

Note: Though this application was generally designed to run one quantum calculation per processor, it does have some built in functionality to run single quantum calculations on multiple processors.  Though I have not done this in a few years, you may talk to your system administrator and get help doing this if desired.  The pso application reads the "nodes.txt" file and formerly started one energy calculation for each node/line in the file.  Using mpirun instead of mpiexec can be helpful in this case.  If you specify multiple processors per node and if you want one Gaussian job to use all of the processors on a node, you may need to place this line in your Gaussian header (pso input file): "% nprocshared=number_of_processors_you_want".

B. MPI Options

When calculating energies for a large population of candidate structures, the program allows options to improve efficiency.  By default, each processor will calculate the same number of quantum jobs.  If for example, you have a population of 64 structures and 16 processors, each processor will calculate energies for exactly 4 structures. This means that by default, there is no communication between processors, except at the start of each new batch of calculations.

Since the time required for different quantum jobs varies, a better approach is to set aside one processor to act as a master that then deals out quantum tasks to the other slave processors.  This can be achieved by specifying the -m option. For example, if you have a population of 64 structures and you want about 4 quantum jobs per processor, you would now need 17 processors, one master and 16 slaves.  This can improve efficiency since it allows load balancing to compensate for quantum jobs that take a long time.

When using a population of structures with simulated annealing, each member of the population is essentially an independent simulated annealing run, but energy calculations for the entire population must still be performed in synchronized batches.  This means that all energy calculations for a particular iteration must be completed first.  Next, the iteration number increases and the temperature decreases (both of these variables are the same for all runs).  Next, perturbations are made on each structure, and a new batch of energy calculations is started.  This is inefficient because of the great differences in the amount of time required for different quantum energy calculations.  To overcome this, the -i option can be used with simulated annealing to allow all independent runs to run separately rather than in a synchronized manner.  It does this by creating separate input, output, and resume files for each population member.  For example, if my input file were named LJ7_Sim.inp, this option would create a directory called LJ7_SimRuns and create input files in this directory named LJ7_Sim_1.inp, LJ7_Sim_2.inp, etc.  Resume and output files would also be placed here.  This option additionally creates a separate list of best structures for each run.  For example if my energy files directory (input file line 4) were called energyFiles, the -i option would create Run1, Run2, etc. directories inside this directory, each with it's own bestSavedStructures directory.

Also when using the -i option, the program has a method for periodically merging the separate lists of best structures into one master list.  For example, if my energy files directory were called energyFiles, the program will create a sub directory called bestSavedStructures and will copy structures from the bestSavedStructures directory in each of the Run1, Run2, etc. directories.  This method also creates a master ouput and resume file.  The method is run when ever you restart the run, and when the run completely finishes.  You may also run the method manually by typing "./helper -u YourInputFile.inp".

When using the -i option and you must resume an unfinished run, do not change the input file to a resume file.  Since each population member has its own resume file, the program is smart enough to find each of these resume files and restart the run.  While the ./helper -u method does create a master resume file, this is only used to store the list of best structures and/or create an optimization file.

IV. Other useful tips

A. More about optimization files

When performing particle swarm optimization or simulated annealing, you may use single point energy calculations or perform optimization to the nearest local minimum.  If you use single point energy calculations during a run, it is beneficial afterward to optimize the list of best structures to their nearest local minima.  This is done by creating an optimization file from the resume file.  When using the -o option, specify a resume file, followed by the optimization file to be created, followed by the number of structures to transfer, followed by the number of structures to optimize at a time.  An example of how this is done is as follows:

      ./helper -o LJ7_PSO.res LJ7_PSO.opt 64 16 

This copies the best 64 structures from LJ7_PSO.res into the population of LJ7_PSO.opt and creates a new empty list of best structures in the LJ7_PSO.opt file.  To optimize the structures in LJ7_PSO.opt, type:

      ./pso LJ7_PSO.opt

This causes the pso program to iterate through the population in the optimization file.  During each iteration, the program optimizes the best 16 structures in the population, removes them from the population, and updates the list of best structures. Remember that the list of best structures does not contain duplicates, since each structure must be different than every other structure by the RMS distance specified in the optimization file.  After every 16 structures have been optimized, the LJ7_PSO.opt file is rewritten.  If you decide later you want to optimize more or less structures at a time than 16, this number is stored near the end of the optimization file and may be easily changed.

After optimizing a certain number of structures from the resume file, if you decide you want to optimize more structures, you can add them from the resume file.  An example of how to do this is as follows:

      ./helper -a LJ7_PSO.opt 30

This indicates you want to add 30 additional structures from the resume file.

B. Creating quantum input files from a resume or optimization file

In a resume or optimization file, there is a population of solutions as well as a list of best structures seen so far during the run.  The pso program can create quantum (Gamess, Gaussian, etc.) input files form either of these two lists.  This is useful if you wish to perform frequency calculations on the list of best structures, or if you wish to view the chemical structures with Molden, GaussView, or another viewer.  An example of how to create input files from the list of best structures is as follows:

      ./helper -c LJ7_PSO.opt /directory/in/which/to/put/com/files best

If using Gaussian, this would place the list of best structures in the specified directory with the name best1.com, best2.com, best3.com, etc. Energy values are written in the title line of each .com file.  Similarly, .com files can be created from the current population using the -p option.

C. Seeding the starting population with structures from previous runs

The results from one or more previous runs may be used to start a new run.  An example of how to do this is using the command:

      ./pso -s File1.res File2.res File3.res 200 File.inp

This command starts a run based on input parameters in File.inp.  Before doing so, it moves chemical structures from the list of best structures in the seed files (File1.res, File2.res, and File3.res) and places them in the starting population ensuring that each structure in the starting population is different from every other structure by the RMS distance specified in File.inp.  Any number of seed files may be used.  If there are not enough structures in the seed files to create a population of the size specified in File.inp, the structure with the lowest energy value will be duplicated to achieve the right population size.

If the seed files contain fewer of one type of unit or molecule, new molecules will be created and initialized with random positions and angles.  The number after the input file, in this case 200, is the number of iterations to perform freezing.  The coordinates of molecules or units read from the seed files will be frozen for this number of iterations, while the coordinates and angles of any randomly created molecules or units will be allowed to move.  Setting the number of iterations to perform freezing to zero turns freezing off.

Note: when using multiple options, specify the -s option last, as it will assume that every following argument is a seed file.

D. Setting a wall time to ensure cleaning of scratch files

During a typical run, the program must be restarted several times.  When a program is terminated by the scheduler before the run is finished, this leaves files in the scratch directory. This is a problem if you want the scratch directory to be on the node's local hard drive rather than on a network drive.  Using the local hard drives frees up network bandwidth, so it would be nice if the program would watch the clock, clean up the scratch files, and die gracefully, rather than being killed by the PBS scheduler. The program can do this as follows:

	./pso -walltime HH:MM:SS File.inp

This tells the program to terminate before the specified duration of time has elapsed (hours, minutes, and seconds).  The format of the wall time is the same as the PBS scheduler, so you can simply copy and paste it.  Note: if you kill the PBS job using the qdel command, the scratch files will NOT be deleted, so don't do this.  If you need to stop a run before the wall time is up, use the cd command to navigate to the energy files directory (line 4 in the input file), and type 'touch stop'.  This will create a blank file called stop.  At the end of the run's next iteration, the program will check for the existence of this file.  If it's there, it will stop the run and clean up the scratch files correctly.

E. Transition state searches

This program can also perform searches for transition states.  The method used is essentially a random search, but with allowed constraints on the maximum and minimum distances between molecules. There are 2 methods for performing a transition state search.  The first way is to use a simulated annealing input file, and the second way is to place random structures in a simulated annealing optimization file using the -or option (./helper -or).  With both of these methods, make sure to put 'yes' for the option: 'Search for transition states (random search with every perturbation accepted)'.  To summarize, using a simulated annealing input file causes the program to treat the search as a simulated annealing search, except that every perturbation is accepted, so the search is essentially random.  When using the optimization file, every structure is randomly initialized, so new random structures are not perturbations of previous random structures.  This is the essential difference between the two methods.

F. Trouble Shooting

If you encounter unexpected behavior from the application, here are some things you can do.  First, check this manual to ensure the software is not performing as designed.  Second, there are a few unit tests that you can run.  In particular, these can help check that the cclib package is installed correctly. To run the unit tests, navigate (cd) to the main program directory and type './unit'.  Third, turn on additional error messages by setting the PRINT_CATCH_MESSAGES constant to 'true' at the top of input.h. This may provide additional information to help identify the problem.  If you believe the problem is related to MPI, you can also turn on the PRINT_MPI_MESSAGES constant at the top of myMpi.h.  If you find a message that seems related to your unexpected behavior, search for the error in the code to learn more about what the application was doing when it generated the error.  If you find an genuine error, please submit a bug report with a list of steps for reproducing it.

