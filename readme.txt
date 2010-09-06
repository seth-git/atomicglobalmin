Particle Swarm Optimization (PSO) Global Optimization Software Read Me

This is free software and may be modified and/or redistributed under the terms of the GNU General Public License.  Copyright 2007 Seth Call.

For a more detailed description of the program, see:
Call, S. T.; Boldyrev, A. I., Zubarev, D. Y. Global Minimum Structure Searches via Particle
Swarm Optimization.  J. Comput. Chem.  2007, 28, 1177-1186.

Table of Contents
I. Introduction
    A. Compiling the software
    B. Input, output, and resume files
    C. Running and restarting the program
    D. Summary output
        1. Particle swarm output
        2. Simulated annealing and basin hopping output
        3. Genetic algorithm output
    E. Obtaining results from output and resume files

II. Understanding and optimizing parameters
    A. Parameters common to all algorithms
    B. Particle swarm parameters
    C. Simulated annealing and basin hopping parameters

III. Running the program with Gaussian on a cluster of Linux computers
    A. Submitting the pso program to the PBS Scheduler
    B. Trouble Shooting

IV. Other useful tips
    A. Optimization Files
    B. Creating Gaussian .com files from a resume or optimization file
    C. Seeding the starting population with structures from previous runs
    D. Performing bond rotations using a deterministic, non-random search
    E. Transition State Searches

I. Introduction

This software searches for chemical structures that are global minima the potential energy surface.  It allows sets of one or more atoms with fixed geometries to be placed in molecules or groups and searches for minima by performing translation and rotation of molecules with respect to one another.  The program has four different random algorithms that can be used to perform searches: particle swarm optimization, enhanced simulated annealing, basin hoping, and a privative genetic algorithm.  This software can also perform a deterministic, non-random bond rotational search on a single molecule.

The software can run on one Linux computer or a cluster of Linux computers and will not run on Windows.  The use of multiple computers allows energy calculations for a group of candidate solutions to be performed at the same time.  The software can perform quantum energy calculations using Gaussian, though other quantum packages could be easily incorporated.  The program can also calculate energies using the Lennard Jones potential for testing purposes.  This software was written by Seth Call as part of a computer science masters thesis.

A. Compiling the software

This software was written in C++ and may only be compiled using the GNU g++ compiler on a Linux operating system.  The program uses the Message Passing Interface (MPI) library to run on multiple Linux computers.  To compile the program at the command line, navigate to the pso directory using the cd command, and type "make".

B. Input, output, and resume files

Each of the four algorithms, particle swarm optimization (PSO), basin hopping, simulated annealing, and the genetic algorithm has a different input file.  These were designed to be as self explanatory as possible.  Simulated annealing and basin hopping have a common input file, since they are similar algorithms.  Each time the program is run, it creates an output file, which contains the input parameters as well as summary information on the progress of the run.  The program also creates a resume file, which contains all information necessary for restarting a run that was stopped before it was completed.

C. Running and restarting the program

Four example input files are LJ7_PSO.inp, LJ7_Sim.inp, LJ7_BH.inp, and LJ7_GA.inp, for PSO, simulated annealing, and the genetic algorithm respectively.  To run the PSO input file type the following:

      ./pso LJ7_PSO.inp

Other input files are run similarly.  The global minimum for LJ7 has energy -16.505384.  To prematurely terminate a PSO or other type of run, press Ctrl-C.  To restart the PSO run after it has been stopped, type "./pso LJ7_PSO.res".  Restart other types of algorithms similarly.

D. Summary output

Summary information is written to the screen as the program runs.  This can be used to monitor the progress of the run.  The specific meaning of the different items of information are given below for the different algorithms.

     1. Particle swarm output

     The summary output from the PSO program looks like this:
     It: 110 Best Energy: -12.105384 Coord Vel Max,Avg: 0.200, 0.057 Angle Vel Max,Avg: 0.000, 0.000 Div: 0.070 Vis: 72.9%

     The meaning of each value is as follows:
     It: iteration number
     Best Energy: the energy value of the lowest energy structure
     Coord Vel Max,Avg: The coordinate velocity (the maximum and average respectively) for each atom or group of atoms
     Angle Vel Max,Avg: The angle velocity (the maximum and average respectively) for each group of atom (will be zero if there are no groups containing more than one atom)
     Div: The diversity in the population (typically between 0 and 0.5)
     Vis: The percentage visibility (the average percentage of candidate structures that can been seen by other structures, see paper)
     
     2. Simulated annealing and basin hopping output
     
     The summary output for simulated annealing and basin hopping looks like this:
     It: 30 Best Energy: -9.62310778 Temp: 777346.9 Num Pert: 4 Coord, Angle Pert: 0.3000, 20.0000 Accepted Pert: 87.1%

     The meaning of each value is as follows:
     It: iteration number
     Best Energy: the energy value of the lowest energy structure
     Temp: The current temperature
     Num Pert: The number of perturbations performed at one time
     Coord, Angle Pert: The amount by which coordinates and angles change respectively (coordinates are moved in a random direction by the specified amount)
     Accepted Pert: The percentage of accepted perturbations over the previous n iterations (see input file)
     
     3. Genetic algorithm output

     The summary output for the genetic algorithm looks like this:
     It: 60 Best Energy: -10.10563270 Converged: 100.0%

     The meaning of each value is as follows:
     It: iteration number
     Best Energy: the energy value of the lowest energy structure
     Converged: The percentage of calculations from which an energy value was successfully obtained.

E. Obtaining results from output and resume files

After normal termination of a run, the program writes a list of the best structures found to the output file.  To open the output file, type "vim LJ7_PSO.out".  When you are done, type ":q" and then the enter key.  To obtain preliminary results from the resume file, open the resume file using the vim command.  The top portion of the resume file contains parameters.  Following these are chemical structures in the current population.  Following these are the list of best structures found during the entire run.  To search for this list, type "/Best" and then the enter key.  When you are done viewing the list, type ":q" and then the enter key.  The program can also export the structures from the resume file to a temporary file in a more readable format.  To do this for the PSO resume file, type "./pso -t LJ7_PSO.res temp.txt".  When doing runs where multiple atoms are in groups that are moved and rotated as single units, note that chemical structures in the resume file are not stored in global (readable) coordinates.  Therefore, results must be written to a temporary file using the "-t" option as in the example above.


II. Understanding and optimizing parameters

This section discusses the parameters in the input file and techniques for setting them.  Knowing how to set parameters optimally can influence the performance of each algorithm.

A. Parameters common to all algorithms

Parameters common to all algorithms appear at the top and bottom of each input file.  An example of the parameters at the top of an input file for (H2O)OH- are as follows:

1   Particle Swarm Optimization Input File Version 1.2.1
2   
3   Energy function to use (Gaussian or Lennard Jones): Gaussian
4   Path to energy program: /path/to/g03/
5   Path to energy files: /path/to/gaussian/input/and/log/files/
6   Path to scratch directory (optional): /my/scratch/directory/
7   Search cube length, width, and height: 7.0
8   Number of linear structures: 0
9   Initialize linear structures in a box with the above cube length and a height and width of: 1.2
10  Number of planar structures: 0
11  Number of fragmented 3D structures: 0
12  Number of partially non-fragmented 3D structures: 0
13  Number of completely non-fragmented 3D structures: 32
14  General minimum atom distance (used if no value specified below): 0.7
15  Specific minimum atom distances (angstroms):
16  1 1 0.9
17  1 8 0.8
18  8 8 1.2
19  Maximum inter-atomic distance (angstroms): 3.0

The header on line 1 specifies the type of algorithm (particle swarm optimization, simulated annealing, etc.).  It contains the phrase "Input File" for input files and "Resume File" for resume files.  The header also contains a version number.  If you attempt to run an input file that belongs to an older version of the program, you may have to change or insert lines in the input file before it will run.  The program will specify when lines are missing or need to be changed.  The path to the energy program is on line 4.  The path to the energy files on line 5 is the directory where the ".log", ".com", ".dat", and ".chk" files are to be stored.  If using a cluster of Linux computers, this should be a directory accessible to all nodes.

When specifying the number of each structure type to initialize (lines 8-13), keep in mind that atoms are placed in groups each of which is moved and rotated as a "unit".  Each "unit" contains one or more atoms.  Line 9 indicates that linear structures are initialized so that all atoms are in a 3D box that is 1.2 x 1.2 x 7.0.  Planar structures are initialized so that centers of mass of each "unit" are on the same plane.  The fragmented, partially non-fragmented, and completely non-fragmented structures are described informally here.  For a more precise discussion, see the literature.  Fragmented structures contain "units" in random locations within the search cube. Partially non-fragmented structures are created such that every atom is within the maximum distance of at least one other atom.  These are called "partially" non-fragmented because two units may be isolated from the rest of the units (further than the maximum distance).  With completely non-fragmented structures, no two units can be isolated from other units.  For example, if you started at some unit and could only travel to other units within the maximum distance of that unit, you could travel to any unit.

The program allows one maximum distance between atoms (in this case 3 angstroms), and multiple minimum distances between different types of atoms.  In this case the minimum distance is 0.9 between two hydrogens, 0.8 between hydrogen and oxygen, and 1.2 between two oxygen atoms.

An example of the parameters at the end of the input file are given below:

43  Save this many of the best "different" structures: 1000
44  Consider 2 structures "different" if their RMS distance is greater or equal to (angstroms): 0.7
45  Save this many .log/energy output files from the list of best structures: 50
46  Output file name: /path/to/pso/files/OH3H2O_PSO.out
47  Resume file name (optional): /path/to/pso/files/OH3H2O_PSO.res
48  Write resume file after every set of this number of iterations (optional): 1
49  Print summary information after each set of this many iterations: 1
50  Charge: 0
51  Multiplicity: 1
52  Energy file header:
53  % chk=mychk
54  # B3LYP/6-311++G** opt=(calcfc) scfcon=6 scfcyc=300 optcyc=20 scf=direct
55  
56  Energy file footer (optional):
57  --link1--
58  % chk=mychk
59  # B3LYP/6-311++G** freq=noraman scfcon=6 scfcyc=300 optcyc=100
60  geom=check guess=read
61  
62  Number of unit types: 2
63  
64  Number of units of this type: 3
65  Format of this unit type: Cartesian
66  1 0.757540797781 0.0 0.587079636589
67  1 -0.757540797781 0.0 0.587079636589
68  8 0.0 0.0 0.0
69  
70  Number of units of this type: 1
71  Format of this unit type: Cartesian
72  1 0.0 0.0 1.064890
73  8 0.0 0.0 0.0

Line 43 specifies the size of the list of best structures that are saved during the course of the entire run.  This list is updated after every iteration.  Line 44 specifies that each structure in the list of best structures is different from every other structure in the list by a certain RMS distance (see article above).  Line 45 is the number of Gaussian .log files to save for the list of best structures.  These .log files are saved in a folder called bestSavedStructures located inside the directory where energy files are stored (see line 5 in the input file).  Lines 46-55 should be self explanatory.  Note that the program will automatically rename the checkpoint file for each structure in the population of candidate structures, so that there aren't naming conflicts.  The energy file footer (lines 56-60) is placed at the end of the Gaussian input file.  In this case, the footer specifies that a frequency calculation be performed.  Note that the energy file header and footer are not used when energies are calculated using the Lennard Jones potential.

Lines 62-73 are an example of how to specify groups of atoms, molecules, or "units".  The above consists of three water molecules and one hydroxide ion.  Lines 66-68 specifies the coordinates of water with two hydrogen atoms and one oxygen atom.  Lines 72-73 specify coordinates for hydroxide.  If you are dealing with single atoms that are moved separately form other components in the system, one atom can be specified for each "unit".  The "format of this unit type" can only be "Cartesian ".  At some point in the future, I may allow other types of input such as "Z-matrix."

The information in the following two sections contain detailed information about how simulated annealing and particle swarm algorithms run.  First time readers may choose to skip sections B and C.  The default parameters listed in these sections are likely adequate.

B. Particle swarm parameters

An example list of the parameters for the particle swarm optimization algorithm are given below.

21  Particle Swarm Optimization Parameters:
22  Start coordinate inertia (w): 0.95
23  End coordinate inertia (optional): 0.8
24  Reach end coordinate and angle inertias at iteration (optional): 10000
25  Coordinate individual minimum attraction (c2): 0.15
26  Coordinate population minimum attraction (c1): 0.15
27  Maximum Coordinate Velocity (Vmax, optional): 0.3
28  Start angle inertia (w): 0.95
29  End angle inertia (optional): 0.8
30  Angle individual minimum attraction (c2, deg): 15
31  Angle population minimum attraction (c1, deg): 15
32  Maximum Angle Velocity (Vmax, optional): 30
33  Starting RMS visibility distance (use 'auto' for automatic): auto
34  Increase the RMS visibility distance by this amount each iteration: 0.003
35  Switch to the repulsion phase when (1) diversity is below (optional): 0.15
36  And when (2) progress hasn't been made for this number of iterations (optional): 200
37  Switch to the attraction phase when diversity is above (optional): 0.2
38  Don't update individual best solutions within this RMS distance of the best seen by the population: 0.01
39  Maximum number of allowed iterations: 10000
40  Enforce minimum distance constraints on a copy of each structure rather than on the original: yes
41  Use energy value from local optimization: yes

If you are new to particle swarm, you can start with the parameters above, and adjust them as needed.  Because ab initio calculations can take a long time, it is best to use the Lennard Jones potential to test the parameters you have chosen.  Though this energy function behaves very differently, doing this will help you optimize parameters not strongly related to the energy function (i.e. visibility parameters).  After the parameters seem to work well with the Lennard Jones potential, try them with Gaussian and check for differences in how the algorithm performs.  This is particularly important if you are performing runs that take several weeks.

The w, c1, c2, and Vmax parameters (lines 22-32) are typical to particle swarm optimization, and a description of these is available in the literature on the Internet.  The parameters that most likely need to be changed are c1, c2, and Vmax (lines 25-27 and 30-32).  These affect how refined the search is.  Lowering these will favor local rather than global searches.

Parameters on lines 33-37 control the progress of the algorithm in the long term and are important.  These should be set so that at the beginning of a run each candidate structure can see and is influenced only by structures that are closely related.  In other words, the percentage visibility should start low, i.e. 0-1%, and increase slowly to 100%.  The percentage visibility can be observed as "Vis" in the summary output seen while the program runs.

The program now has an automatic method of assigning the starting RMS visibility distance (line 33).  If this option is turned on, the starting RMS visibility distance will be set to 80% of the smallest RMS distance between any two chemical structures in the population.  The time when this distance is set can vary, and before it is set, the RMS visibility distance (and percentage visibility) is zero.  The distance is set when one of three conditions is met:
      1. The best observed energy value hasn't changed for 70 iterations, and the average coordinate velocity (see summary output) is less than half of the maximum coordinate velocity (Vmax, line 27).
      2. Iteration 200 has been reached.
      3. The average velocity of coordinates is below 0.005.
Since the starting visibility distance can be hard to set, it is hoped that this automatic option will simplify setting the parameters.

Once the visibility distance reaches 100%, the diversity in the population will likely be low.  The degree of diversity in the population can be observed as "Div" in the summary output seen while the program runs.  When this gets low (see parameters on lines 35 and 36), the algorithm will temporarily switch to the repulsion phase.  When it switches back (see line 37), it will again start with a low visibility which will increase slowly to 100%.  Also, the memory of each individual solution is erased by replacing it with a structure from the list of best structures (see line 43).  For this reason, it is best to set line 43 to be equal to or larger than the total population size.  It is also a good idea to set the distance between structures in the list of best structures (line 44) to be slightly larger than the starting visibility distance (line 33).

The parameter on line 38 was designed to prevent the best structure seen by each individual from becoming too close to the best structure seen by the population.  I usually don't change it.

The parameter on line 39 is a maximum number of iterations allowed.  This is the only way the program is terminated automatically, though the program can be terminated manually at any time by pressing Control-C.

The parameter on line 40 deals with an automatic function that the program has for enforcing minimum distance constraints (see article above).  It could be said that enforcing the minimum distance constraints modifies the way the algorithm works.  Line 40 specifies that the program should make a copy of each structure before enforcing the minimum distance constraints and calculating the energy, so the progress of the algorithm is not modified.

This algorithm can also be used with local optimization (see line 41).  When local optimization is used, the energy of a structure is assigned to be the energy obtained through local optimization, but the positions of atoms and the progress of the PSO algorithm are not affected when this optimization is turned on.

C. Simulated annealing and basin hopping parameters

An example list of the simulated annealing parameters are given below.

21  Simulated Annealing Parameters:
22  Search only for non-fragmented structures: yes
23  Use energy value from local optimization(basin hopping): no
24  Search for transition states (random search with every perturbation accepted): no
25  Starting temperature or desired starting percentage of accepted transitions(use %): 85%
26  Boltzmann constant: 3.1669e-6
27  Minimum number of iterations before decreasing the temperature (N): 200
28  Decrease the temperature when the percentage of accepted transitions for the past N iterations is below: 100
29  Quenching factor(use 1 for basin hopping): 0.9995
30  Don't stop while the temperature is above this value: 400.0
31  Stop if the percentage of accepted transitions for the past N iterations is below: 1.0
32  Maximum number of allowed iterations: 100000
33  Starting number of perturbations per iteration: 3
34  Starting coordinate perturbation (angstroms): 0.25
35  Minimum coordinate perturbation (angstroms): 0.05
36  Starting angle perturbation (deg): 25
37  Minimum angle perturbation (deg): 3

This simulated annealing algorithm has the ability to perform non-fragmented searches, or in other words it can enforce minimum and maximum distances constraints through the course of the entire run.  This enables faster, more efficient searches.  To enable this feature, place yes on line 22.  Line 23 turns on basin hopping.  Basin hopping is very similar to simulated annealing except the temperature (line 25), number of perturbations to perform at a time (line 33), and perturbation amounts (34-37) do not change.  Also, when performing basin hopping, the energy value is obtained by making a copy of the original structure, performing optimization to the nearest local minimum, and assigning that energy value to the original structure.  By tradition, simulated annealing and basin hopping use only one structure and do not use a population of candidate structures, though this application will allow populations of solutions.  A population of solutions with these two algorithms is essentially a set of independent runs.  Using a population of solutions with these algorithms greatly increases the probability of finding the correct minimum and is highly recommended.

At each step, simulated annealing makes a number of random perturbations.  Each perturbation can move the center-of-mass for a group of atoms, a molecule, or a "unit" along a random vector the length of which is specified on lines 34 and 35.  Alternatively a perturbation can rotate one of three angles in a molecule or unit by a random amount (see lines 36 and 37).  Once a perturbation has been made, the energy is recalculated.  If the energy value is better, the perturbation is accepted. If it is worse, the perturbation is accepted with a certain probability defined as:

     p = e^(-change in energy / k * T)

where k is the Boltzmann constant in units of Hartrees per Kelvin (line 26) and T is the temperature in units of Kelvin (line 25).  At the start of a run, the temperature and the probability of accepting a bad perturbation are very high. The temperature T, the number of perturbations P, the coordinate perturbation amount C, and the angle perturbation amount A decrease as follows:

     T = T * quenching factor (line 24)
     P = P * SquareRoot(quenching factor)
     C = C * SquareRoot(quenching factor)
     A = A * SquareRoot(quenching factor)

At the end, the temperature and the probability of accepting bad perturbations are very low. Essentially, the algorithm is a slow transition between a random search and a downhill search.

Since the starting temperature (line 25) is sometimes time consuming to set optimally, this program now has an automatic method for setting this.  Instead, of specifying a starting temperature, you may specify a starting percentage of accepted transitions (see summary output displayed while the program is running).  Typical values range from 70-90%, and 85% works for many applications.  Note that if the percentage of accepted transitions is higher than 90%, it may take a long time for it to come down.

The quenching factor (line 29) controls the length of the run and is also important.  Set this so that the run will last long enough to produce good results but short enough to be reasonable.  Keep in mind the relationship: starting temperature * (quenching factor ^ number of iterations) = final temperature.  Typical values are between 0.999 and 0.99999.  I typically use 0.9995.

There are two ways a run can be terminated automatically.  The first is if the run exceeds a maximum number of iterations (line 32).  The second is if the percentage of accepted transitions is below a specified value (line 31) and if the temperature has dropped below a specified minimum value (line 30).  Generally, it is best if the percentage of accepted transition and the temperature are low toward the end of a run, though you may stop a run at any time.

Theoretically simulated annealing is guaranteed to find the global minimum if cooling proceeds slowly enough, though there is no guarantee that it can be done within a reasonable amount of time.  Because of the expensive nature of ab initio calculations, large numbers of calculations are sometimes not practical, though the theoretical guarantee is a plus.  Using a population of solutions is also very helpful in finding global minima.

It is often best to use the Lennard Jones potential for testing the parameters until they are in the ball park.  Though this energy function behaves very differently, doing this will help you find obvious errors in the input parameters and give you a rough idea of how the algorithm will proceed before you start using expensive ab initio calculations.  After the parameters seem to work well with the Lennard Jones potential, try them with Gaussian.  Check for differences in how the algorithm performs and make adjustments.  This is particularly important if you are performing runs that take several weeks.


III. Running the program on a cluster of computers

A. Submitting the pso program to the PBS Scheduler

This section describes how to run the program on a cluster of Linux computers with Gaussian.  Some familiarity with Gaussian and Linux is assumed.  It should also be noted that other quantum chemistry programs could be incorporated into the software by someone with C++ experience.

To use a cluster of computers, create a script file for submitting the main program to the PBS (Portable Batch System) scheduler. An example script file for doing this called "examplePBSScript" is in the "pso" directory.  Make a copy of this script and modify it as follows.

    1. Line 2 in the script file specifies the number of nodes you want to use and the number of processors per node (ppn) you want.  Set this appropriately.  If you specify multiple processors per node and if you want one Gaussian job to use all of the processors on a node, you may need to place this line in your Gaussian header (pso input file): "% nprocshared=number_of_processors_you_want".  The wall time is a maximum time limit you want to allow your job to run for.  Set this appropriately. The example file specifies 24 hours.
    2. Set the memory limit per node that you want on line 3.  If you don't need to set a memory limit, remove this line.
    3. Replace "job_name" on lines 4 and 5 with a job name of your choice.
    4. Line 16 specifies that the list of nodes allocated by the PBS scheduler be copped to a file called "nodes.txt" in the energy files directory (line 5 of the pso input file).  The pso application then reads this "nodes.txt" file and will start one energy calculation for each node/line in the file.  If you request multiple processors per node (line 2), remember that multiple Gaussian jobs will be run on each node using one processor per Gaussian job.
    5. Change lines 22 and 23 (the last two lines).  These lines call the pso program.  It is usually best to put the input, output, and resume files in a separate directory other than in the main pso directory.  NOTE: WHEN RESTARTING THE PROGRAM, DON'T FORGET TO CHANGE THE INPUT FILE NAME TO THE RESUME FILE NAME IN THE PBS SCRIPT FILE.

Also be sure to modify the pso input file appropriately.  Set the path to the energy program (line 4) to be the path to your Gaussian program.  Next on line 5, specify a directory where energy files (.com, .log, etc.) will be written.  You will need to create this directory, and it will need to be unique for every input file you intend to run simultaneously with the program.  Specify a scratch directory on line 6.  The pso program will create sub-directories in this directory where Gaussian scratch files will be written.  These sub directories have the name of the pso input or resume file.  The scratch directory can be on the hard drive of each local node or on a shared drive.  The pso program will delete any scratch files and directories it creates unless it is stopped prematurely (which it will be if the wall time is exceeded).  So, be sure to clean out your files in the scratch directories.

Make sure the names of your output and resume files are set appropriately in the input file.  Using full path names is likely necessary.  Also, modify the Gaussian header and optional footer appropriately.  The header will be placed at the start of every Gaussian input file.

Once you have created your script file and your input file, type the command qsub followed by the name of your script file.  To look at jobs in the queue type the command qstat.  To delete a job from the queue type, qdel job_number.

B. Trouble Shooting

The Gaussian scratch directory must be set by the pso program if you specified a scratch directory on line 6 of your input file.  The way this scratch directory is set can vary from system to system.  If you need to modify the command, it's listed in the mpi.cc file on line 47.  An alternate version of the command that has been commented out is listed on line 48.

The program that calls Gaussian is separate from the main pso program and is written using an MPI C++ library.  The command within the main pso program that calls this separate mpi program may need to be modified.  If you need to modify it, the command is in the energy.cc file on line 161.  A separate version of this command is on line 157.


IV. Other useful tips

The pso program has a number of different options that may used.  To view the full list of these options type ./pso.

A. Optimization Files

      When performing particle swarm optimization or simulated annealing, you may use single point energy calculations or perform optimization to the nearest local minimum.  If you use single point energy calculations during a run, it is beneficial afterward to optimize the list of best structures to their nearest local minima.  This is done by creating an optimization file from the resume file.  When using the -o option, specify a resume file, followed by the optimization file to be created, followed by the number of structures to transfer, followed by the number of structures to optimize at a time.  An example of how this is done is as follows:

      ./pso -o LJ7_PSO.res LJ7_PSO.opt 64 16 

This copies the best 64 structures from LJ7_PSO.res into the population of LJ7_PSO.opt and creates a new empty list of best structures in the LJ7_PSO.opt file.  To optimize the structures in LJ7_PSO.opt, type:

      ./pso LJ7_PSO.opt

This causes the pso program to iterate through the population in the optimization file.  During each iteration, the program optimizes the best 16 structures in the population, removes them from the population, and updates the list of best structures. Remember that the list of best structures does not contain duplicates, since each structure must be different than every other structure by the RMS distance specified in the optimization file.  After every 16 structures have been optimized, the LJ7_PSO.opt file is rewritten.  If you decide later you want to optimize more or less structures at a time than 16, this number is stored near the end of the optimization file and may be easily changed.

After optimizing a certain number of structures from the resume file, if you decide you want to optimize more structures, you can add them from the resume file.  An example of how to do this is as follows:

      ./pso -a LJ7_PSO.opt 30

This indicates you want to add 30 additional structures from the resume file.

B. Creating Gaussian .com files from a resume or optimization file

In a resume or optimization file, there is a population of solutions as well as a list of best structures seen so far during the run.  The pso program can create .com files form either of these two lists.  This is useful if you wish to perform frequency calculations on the list of best structures, or if you wish to view the chemical structures with Molden, GaussView, or another viewer.  An example of how to create .com files from the list of best structures is as follows:

      ./pso -c LJ7_PSO.opt /directory/in/which/to/put/com/files best

This would place the list of best structures in the specified directory with the name best1.com, best2.com, best3.com, etc. Energy values are written in the title line of each .com file.  Similarly, .com files can be created from the current population using the -p option.

C. Seeding the starting population with structures from previous runs

The results from one or more previous runs may be used to start a new run.  An example of how to do this is using the command:

      ./pso -s File.inp 200 File1.res File2.res File3.res

This command starts a run based on input parameters in File.inp.  Before doing so, it moves chemical structures from the list of best structures in the seed files (File1.res, File2.res, and File3.res) and places them in the starting population ensuring that each structure in the starting population is different from every other structure by the RMS distance specified in File.inp.  Any number of seed files may be used.  If there are not enough structures in the seed files to create a population of the size specified in File.inp, the structure with the lowest energy value will be duplicated to achieve the right population size.

If the seed files contain fewer of one type of unit or molecule, new molecules will be created and initialized with random positions and angles.  The number after the input file, in this case 200, is the number of iterations to perform freezing.  The coordinates of molecules or units read from the seed files will be frozen for this number of iterations, while the coordinates and angles of any randomly created molecules or units will be allowed to move.  Setting the number of iterations to perform freezing to zero turns freezing off.

D. Performing bond rotations using a deterministic, non-random search

The pso program can perform bond rotations on a single molecule in search of the lowest energy structure.  This is done with the -b and -m options.  The -b option produces a printout of bond locations and indicates which are 'rotatable'.  The -m option is used to create an optimization file with all possible bond rotational positions of the monomer.  You can then perform an optimization of the structures in the optimization file to obtain the results.  An example of how to use the -b option is as follows:

      ./pso -b Isoprene.inp

The input file used can be a simulated annealing, particle swarm, or genetic algorithm input file.  Use the -b option to ensure that bonds have been correctly identified.  If they have not, change the criteria used to identify bonds stored in the file bondLengths.txt.   The format of this file is atomic symbol, atomic symbol, bond type ('s' for single, 'd' for double, 't' for triple), minimum distance-maximum distance, where distances are in angstroms.

The file bondLengths.txt was generated from average bond lengths stored in the files averageSingleBondLengthsInAngstroms.txt and averageMultipleBondLengthsInAngstroms.txt using the Perl script updateBondLengths.pl.  These average bond lengths were taken from the Handbook of Chemistry and Physics.  Generally, the program allows bonds to be 15% larger or smaller than average bond lengths.

Once bonds have been correctly identified, use the -m option to generate an optimization file containing all possible bond rotational positions of the monomer.  When using the -m option, specify an input file, followed by the number of degrees to rotate bond angles, followed by an optimization file, followed by the number of structures to optimize at a time.
An example of how to do this is as follows:

      ./pso -m Isoprene.inp 30 Isoprene.opt 200

This creates the optimization file Isoprene.opt from the input file Isoprene.inp, using bond rotational angles of 30 degrees.  When the structures in Isoprene.opt are optimized, the file is re-written after every set of 200 structures have been optimized.  Not that in this case, structures don't necessarily have to be optimized even though they are stored in an 'optimization' file.  Single-point energy calculations can also be performed.  But structures in the population of Isoprene.opt are moved to the list of best structures in Isoprene.opt after each batch of 200 energy calculations are performed.

Also consider how many bond rotational positions are possible before generating an optimization file, to ensure the file created is not too large.  For example, if we rotate bonds by 30 degrees, that gives 360 / 30 = 12 possible rotational positions for each bond.  In Isoprene, there are 5 rotatable bonds.  That gives 12 to the power of 5 or 248,832 possible combinations.  Fortunately, one of the rotatable bonds in Isoprene contains a methyl group on one side, leaving only 12/3 = 4 rotational positions for that bond.  This reduces the number of possible combinations to 4*(12^4) = 82,944.  This program automatically detects methyl groups and also removes configurations that do not satisfy minimum inter-atomic distance constraints specified in the input file.  Also note that this program does not consider bonds in rings to be 'rotatable'.

E. Transition state searches

This program can also perform searches for transition states.  The method used is essentially a random search, but with allowed constraints on the maximum and minimum distances between molecules.  There are 2 methods for performing a transition state search.  The first way is to use a simulated annealing input file, and the second way is to place random structures in a simulated annealing optimization file using the -or option (see help.txt).  With both of these methods, make sure to put 'yes' for the option: 'Search for transition states (random search with every perturbation accepted)'.  When this option is turned on, the program looks for " 1 imaginary frequencies (negative Signs) " in the Gaussian .log file and will only save .log files with this phrase.  To summarize, using the a simulated annealing input file causes the program to treat the search as a simulated annealing search, except that every perturbation is accepted, so the search is essentially random.  When using the optimization file, every structure is randomly initialized, so new random structures are not perturbations of previous random structures.  This is the essential difference between the two methods.

