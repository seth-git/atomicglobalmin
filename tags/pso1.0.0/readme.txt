Particle Swarm Optimization (PSO) Global Optimization Software Read Me

For a more detailed description, see:
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
    F. Seeding the input

II. Understanding and optimizing parameters
    A. Parameters common to all algorithms
    B. Particle swarm parameters
    C. Simulated annealing and basin hopping parameters

III. Running the program on a cluster of computers
    A. Method 1: Submitting the main program to the queue
    B. Method 2: Automatically submitting Gaussian jobs to the queue
    C. Other useful tips

I. Introduction

The software can run on one Linux computer or a cluster of Linux computers and will not run on Windows.  The program can interface with the PBS (Portable Batch System), a common Linux job scheduling system.  The use of multiple computers allows energy calculations for a group of candidate solutions to be performed at the same time.  The software can perform energy calculations using Gaussian, though other quantum packages could be easily incorporated.  The program can also calculate energies using the Lennard Jones potential for testing purposes.  This software was written by Seth Call as part of a computer science masters thesis.

A. Compiling the software

This software was written in C++ and may only be compiled using the GNU g++ compiler on a Linux operating system.  To compile the program at the command line, navigate to the pso directory using the cd command, and type "make".

B. Input, output, and resume files

The software has four different algorithms: particle swarm optimization (PSO), basin hopping, simulated annealing, and a primitive genetic algorithm.  Each algorithm has an input file that was designed to be as self explanatory as possible.  Simulated annealing and basin hopping have a common input file, since they are similar algorithms.  Each time the program is run, it creates an output file, which contains the input parameters as well as summary information on the progress of the run.  The program also creates a resume file, which contains all information necessary for restarting a run that was stopped before it was completed.

C. Running and restarting the program
Three example input files are inputLJ7_PSO.txt, inputLJ7_Sim.txt, and inputLJ7_GA.txt, for PSO, simulated annealing, and the genetic algorithm respectively.  To run the PSO input file type the following:

./pso inputLJ7_PSO.txt

Other input files are run similarly.  The global minimum for LJ7 has energy -16.505384.  To prematurely terminate a PSO or other type of run, press Ctrl-C.  To restart the PSO run after it has been stopped, type "./pso resumeLJ7_PSO.txt".  Restart other types of runs similarly.

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
     It: 200 Best Energy: -7.06211868 Temp: 4997.5 Coord, Angle Pert: 0.2999, 29.9925 Accepted Trans: 71.0% Converged: 100.0%

     The meaning of each value is as follows:
     It: iteration number
     Best Energy: the energy value of the lowest energy structure
     Temp: The current temperature
     Coord, Angle Pert: The amount by which coordinates and angles change respectively (coordinates are moved in a random direction by the specified amount)
     Accepted Trans: The percentage of accepted transitions over the previous n iterations (see input file)
     Converged: The percentage of calculations from which an energy value was successfully obtained.
     
     3. Genetic algorithm output

     The summary output for the genetic algorithm looks like this:
     It: 60 Best Energy: -10.10563270 Converged: 100.0%

     The meaning of each value is as follows:
     It: iteration number
     Best Energy: the energy value of the lowest energy structure
     Converged: The percentage of calculations from which an energy value was successfully obtained.
E. Obtaining results from output and resume files

After normal termination of a run, the program writes a list of the best structures found to the output file.  To open the output file, type "vim outputLJ7_PSO.txt".  When you are done, type ":q" and then type the enter key.  To obtain preliminary results from the resume file, open the resume file using the vim command.  The top portion of the resume file contains parameters.  Following these are chemical structures in the current population.  Following these are the list of best structures found during the entire run.  To search for this list, type "/Best" and then the enter key.  When you are done viewing the list, type ":q" and then type the enter key.  The program can also export the structures from the resume file to a temporary file in a more readable format.  To do this for the PSO resume file, type "./pso -t resumeLJ7_PSO.txt temp.txt".  When doing runs where multiple atoms are in groups that are moved and rotated as single units, note that chemical structures in the resume file are not stored in global (readable) coordinates.  Therefore, results must be written to a temporary file using the "-t" option as in the example above.

F. Seeding the input

The results from one or more previous runs may be used to start a new run.  An example of how to do this is using the command:

./pso -s resumeFile1.txt resumeFile2.txt  resumeFile3.txt inputFile.txt

This command starts a run based on input parameters in inputFile.txt.  Before doing so, it moves chemical structures from the seed files (resumeFile1.txt, resumeFile2.txt, and resumeFile3.txt) and places them in the starting population and in the list of best structures for the new run.  Any number of seed files may be used.


II. Understanding and optimizing parameters

This section describes the parameters in the input file and discusses techniques for setting them so that results are optimal.  Knowing how to set parameters optimally can have a great influence on the performance of each algorithm.

A. Parameters common to all algorithms

Parameters common to all algorithms appear at the top and bottom of each input file.  An example of the parameters at the top of an input file for (H2O)OH- are as follows:

1   Particle Swarm Optimization Input File Version 1.0
2
3   Energy function to use (Gaussian or Lennard Jones): Gaussian
4   Path to energy program: /path/to/g03/
5   Path to energy files: /path/to/gaussian/input/and/log/files/for/this/pso/input/file/
6   Search cube length, width, and height: 7.0
7   Number of linear structures: 0
8   Initialize linear structures in a box with the above cube length and a height and width of: 1.2
9   Number of planar structures: 0
10  Number of fragmented 3D structures: 20
11  Number of partially non-fragmented 3D structures: 20
12  Number of completely non-fragmented 3D structures: 60
13  General minimum atom distance (used if no value specified below): 0.7
14  Specific minimum atom distances (angstroms):
15  1 1 0.9
16  1 8 0.8
17  8 8 1.2
18  Maximum inter-atomic distance (angstroms): 3.0

This section contains notes on some of these parameters.  The header on line 1 specifies the type of algorithm (particle swarm optimization, simulated annealing, etc.).  It contains the phrase "Input File" for input files and "Resume File" for resume files.  The header also contains a version number.  If you attempt to run an input file that belongs to an older version of the program, you may have to insert lines into the input file before it will run.  The program will specify when lines are missing.  The path to the energy program is on line 4.  The path to the energy files on line 5 is the directory where the ".log", ".com", ".dat", and ".chk" files are to be stored.  If using a cluster nodes, this should be a directory accessible to all nodes.

When specifying the number of each structure type to initialize (lines 7-12), keep in mind that atoms are placed in groups each of which is moved and rotated as a unit.  Each "unit" contains one or more atoms.  Line 8 indicates that linear structures are initialized so that all atoms are in a 3D box that is 1.2 x 1.2 x 7.0.  Planar structures are initialized so that centers of mass of each "unit" are on the same plane.  The fragmented, partially non-fragmented, and completely non-fragmented structures are described informally here.  For a more precise discussion, see the literature.  Fragmented structures contain "units" in random locations within the search cube. Partially non-fragmented structures are created such that every atom is within the maximum distance of, at least, one other atom.  These are called "partially" non-fragmented because two units may be isolated from the rest of the units (further than the maximum distance).  With completely non-fragmented structures, no two units can be isolated from other units.  For example, if you started at some unit and could only travel to other units within the maximum distance of that unit, you could travel to any unit.

The program allows one maximum distance between atoms (in this case 3 angstroms), and multiple minimum distances between different types of atoms.  In this case the minimum distance is 0.9 between two hydrogens, 0.8 between hydrogen and oxygen, and 1.2 between two oxygen atoms.

An example of the parameters at the end of the input file are given below:

42  Save this many of the best "different" structures: 500
43  Consider 2 structures "different" if their RMS distance is greater or equal to (angstroms): 0.7
44  Output file name: /path/to/pso/files/outputOH3H2O_PSO.txt
45  Resume file name (optional): /path/to/pso/files/resumeOH3H2O_PSO.txt
46  Print summary information after each set of this many iterations: 1
47  Cluster node names file (put LOCAL for local nodes): nodes.txt
48  Job queue template file (used instead of the node names file, optional):
49  Number of jobs submitted to the job queue at a time:
50  Every other iteration, use the wave function from the previous iteration: no
51  Charge: -1
52  Multiplicity: 1
53  Energy file header:
54  $ rungauss
55  % mem=800MB
56  % nprocshared=4
57  # B3LYP/6-31++G** scf=tight guess=huckel maxdisk=250000000
58  
59  Number of unit types: 2
60  							
61  Number of units of this type: 3
62  Format of this unit type: Cartesian
63  1 0.595144 0.0 0.753501
64  1 -0.595144 0.0 0.753501
65  8 0.0 0.0 0.0
66  
67  Number of units of this type: 1
68  Format of this unit type: Cartesian
69  1 0.0 0.0 1.064890
70  8 0.0 0.0 0.0

Line 42 specifies the size of the list of best structures that are saved during the course of the entire run.  This list is updated after every iteration.  Line 43 specifies that each structure in the list of best structures is different from every other structure in the list by a certain RMS distance (see article above).  Lines 47-49 are parameters for running the program on a cluster of computers.  These are described in section III.  Line 50 was an experiment to see if energy calculations could be performed more quickly by using wave functions from previous iterations.  This was tested in particular with simulated annealing.  Initial findings showed that this option did not produce significant speed increases and some times hampered the progress of the algorithm.  I haven't used it in a long time.  Lines 51 and 52 are the charge and multiplicity.  Lines 54-57 contain the header to the gaussian input file.  This header is not used when energies are calculated using the Lennard Jones potential.

Lines 59-70 are an example of how to specify groups of atoms, molecules, or "units".  The above consists of three water molecules and one hydroxide ion.  Lines 63-59 specifies the coordinates of water with two hydrogen atoms and one oxygen atom.  Lines 69-70 specify coordinates for hydroxide.  If you are dealing with single atoms that are moved separately form other components in the system, one atom can be specified for each "unit".  The "format of this unit type" can only be "Cartesian ".  At some point in the future, I may allow other types of input such as "Z-matrix."


B. Particle swarm parameters

An example list of the parameters for the particle swarm optimization algorithm are given below.

19  Start coordinate inertia (w): 0.95
20  End coordinate inertia (optional): 0.8
21  Reach end coordinate and angle inertias at iteration (optional): 10000
22  Coordinate individual minimum attraction (c2): 0.1
23  Coordinate population minimum attraction (c1): 0.1
24  Maximum Coordinate Velocity (Vmax, optional): 0.2
25  Start angle inertia (w): 0.95
26  End angle inertia (optional): 0.8
27  Angle individual minimum attraction (c2, deg): 15
28  Angle population minimum attraction (c1, deg): 15
29  Maximum Angle Velocity (Vmax, optional): 30
30  Starting visibility RMS distance (optional): 0.01
31  Increase the visibility RMS distance by this amount each iteration (optional): 0.004
32  Initialize structures in pairs where each is a near copy of the other (gives non-zero initial visibility): no
33  Switch to the repulsion phase when (1) diversity is below (optional): 0.05
34  And when (2) progress hasn't been made for this number of iterations (optional): 40
35  Switch to the attraction phase when diversity is above (optional): 0.2
36  When switching to the attraction phase, replace this number of individual best solutions with population best solutions: 200
37  Don't update individual best solutions within this RMS distance of the best seen by the population: 0.005
38  Maximum number of allowed iterations: 800
39  Enforce minimum distance constraints on a copy of each structure rather than on the original: no
40  Use local optimization (energy value only): no

If you are new to particle swarm, you can start with the parameters above, and adjust them as needed.  Because ab initio calculations can take a long time, it is best to use the Lennard Jones potential to test the parameters you have chosen.  Though this energy function behaves very differently, doing this will help you optimize parameters not strongly related to the energy function (i.e. visibility parameters).  After the parameters seem to work well with the Lennard Jones potential, try them with Gaussian and check for differences in how the algorithm performs.  This is particularly important if you are performing runs that take several weeks.

The w, c1, c2, and Vmax parameters (lines 19-28) are typical to particle swarm optimization, and a description of these is available in the literature on the Internet.  The parameters that most likely need to be changed are c1, c2, and Vmax (lines 22-24 and 27-29).  These affect how refined the search is.  Lowering these will favor local rather than global searches.

Parameters on lines 30, 31, 33-36 control the progress of the algorithm in the long term and are also important.  These should be set so that at the beginning of a run each candidate structure can see and is influenced only by structures that are closely related.  In other words, the percentage visibility should start low (i.e. 0-1%) and increase slowly to 100%.  Once it reaches 100%, the diversity in the population will likely be low.  The degree of diversity in the population can be observed as "Div" in the summary output seen while the program runs.  When this gets low (see parameters on lines 33 and 34), the algorithm will temporarily switch to the repulsion phase.  When it switches back (see line 35), it will again start with a low visibility which will increase slowly to 100%.  Also, the memory of each individual solution is erased (see line 36) by replacing it with a structure from the list of best structures (see line 42).  It is usually best to set line 36 to be the same as the total population size.

The parameter on line 32 was an experimental initialization option that was designed to create some initial visibility and movement at the beginning of each run.  After making the parameter, I started randomly initializing the velocities of coordinates and angles, so this option was no longer necessary.  It is usually best to leave this option off.

The parameter on line 37 was designed to prevent the best structure seen by each individual from becoming too close to the best structure seen by the population.  I usually don't change it.

The parameter on line 38 is a maximum number of iterations allowed.  This is the only way the program is terminated automatically, though the program can be terminated manually at any time by pressing Control-C.

The parameter on line 39 deals with an automatic function that the program has to enforce minimum distance constraints (see article above).  It could be said that enforcing the minimum distance constraints modifies the way the algorithm works.  Line 39 specifies that the program should make a copy of each structure before enforcing the minimum distance constraints and calculating the energy, so the progress of the algorithm is not modified.

This algorithm can also be used with local optimization (see line 40).  When local optimization is used, the energy of a structure is assigned to be the energy obtained through local optimization, but the positions of atoms and the progress of the PSO algorithm are not affected when this optimization is turned on.


C. Simulated annealing and basin hopping parameters

An example list of the simulated annealing parameters are given below.

18  Search only for non-fragmented structures: yes
19  Perform basin hopping search: no
20  Starting temperature: 10000.0
21  Scaling factor (substituent for Boltzmann constant): 400.0
22  Minimum number of iterations before decreasing the temperature (N): 100
23  Decrease the temperature when the percentage of accepted transitions for the past N iterations is below: 100
24  Quenching factor: 0.99999
25  Don't stop while the temperature is above this value: 200.0
26  Stop if the accepted transitions is below this percentage for each of five consecutive iterations: 0.05
27  Maximum number of allowed iterations: 500000
28  Starting coordinate perturbation (angstroms): 0.3
29  Minimum coordinate perturbation (angstroms): 0.05
30  Starting angle perturbation (deg): 30
31  Minimum angle perturbation (deg): 5

This simulated annealing algorithm has the ability to perform non-fragmented searches, or in other words it can enforce minimum and maximum distances constraints through the course of the entire run.  This enables faster, more efficient searches.  To enable this feature, place yes on line 18.  Line 19 turns on basin hopping.  Basin hopping is very similar to simulated annealing except the temperature does not decrease, but stays constant.  Also, when performing basin hopping, the energy value is obtained by making a copy of the original structure, performing optimization to the nearest local minimum, and assigning that energy value to the original structure.  Simulated annealing and basin hopping typically use only one structure and do not use a population of candidate structures, though this application will allow populations of solutions.  A population of solutions with this algorithm is essentially a set of independent runs.

At each step, simulated annealing makes a random perturbation.  This perturbation can move the center-of-mass for a group of atoms, molecule, or "unit" along a random vector (line 28).  Alternatively a perturbation can rotate one of three angles in a molecule or unit by a random amount (line 30).  Once a transition has been made, the energy is recalculated.  If the energy value is better, the perturbation is accepted. If it is worse, the perturbation is accepted with a certain probability defined as:

	p = e^(-change in energy / k * T)

where k is the scaling factor (line 21) and T is the temperature (line 20).  At the start of a run, the temperature and the probability of accepting a bad perturbation are very high. The temperature T and the perturbation amount A decrease as follows:

	T = T * quenching factor (line 24)
	A = A * SquareRoot(quenching factor)

At the end, the temperature and the probability of accepting bad perturbations are very low. Essentially, the algorithm is a slow transition between a random search and a downhill search.  Theoretically simulated annealing is guaranteed to find the global minimum, though there is no guarantee that it can be done within a reasonable amount time.  Because of the expensive nature of ab initio calculations, large numbers of calculations are sometimes not practical, though the theoretical guarantee is a plus.  Using a population of solutions is also helpful in many cases.

When setting the parameters, it is often best to use the Lennard Jones potential for testing the parameters until they are in the ball park.  Though this energy function behaves very differently, doing this will help you get a rough idea of how the algorithm will proceed before you start using expensive ab initio calculations which.  After the parameters seem to work well with the Lennard Jones potential, try them with Gaussian.  Check for differences in how the algorithm performs and make adjustments.  This is particularly important if you are performing runs that take several weeks.

I usually set the temperature at some high value such as 10,000.  I then set the scaling factor so that the percentage of accepted transitions (displayed while the program is running) is high 80-85%.  If the percentage of accepted transitions is higher than 90%, it may take a long time for it to come down.  I then set the quenching factor so that the run will last long enough to produce good results but short enough to be reasonable.  Typical values are between 0.999 and 0.99999.

There are two ways a run can be terminated automatically.  The first is if the run exceeds a maximum number of iterations (line 27).  The second is if the percentage of accepted transitions is below a specified value for each of five consecutive iterations (line 26) and if the temperature has dropped below a specified minimum value (line 25).  Generally, it is best if the percentage of accepted transition and the temperature are low toward the end of a run.


III. Running the program on a cluster of computers

This section describes how to run the program on a cluster of computers using the PBS (Portable Batch System) to interface with the cluster scheduler and start Gaussian jobs.  Some familiarity with Gaussian and Linux is assumed.  It should also be noted that other quantum chemistry programs could be incorporated into the software by someone with C++ experience, though only Gaussian is supported at this time.

There are two ways to run the program on a cluster of computers:
1. Submit the program to the queue, then have the program read node names from a file, log into these nodes, and start jobs on each node.
2. Run the program at the command line manually and have the program submit Gaussian jobs to the queue.

The first option requires maintaining an rsh or ssh connection, which limits the number of nodes that can be used at a time.  The second option requires polling the queue every 30 seconds to check for the completion of jobs.  It has the potential advantage, however, that more jobs can be run at a time.  Note that at some point in the future, the program may be modified to incorporate mpi library calls allowing any number of nodes to be used without polling the queuing system.


A. Method 1: Submitting the main program to the queue

To use this option, make sure that lines 48 and 4 are blank (line numbers are specific to the example in the previous section):

47  Cluster node names file (put LOCAL for local nodes): nodes.txt
48  Job queue template file (used instead of the node names file, optional):
49  Number of jobs submitted to the job queue at a time:

Next, submit the main program to the job queue using a script file.  An example script file for doing this called "psoExampleScript" is in the "scripts" directory inside the "pso" directory.  Make a copy of this script and modify it as follows.

    1. Line 2 in the script file specifies the number of nodes you want to use and the number of processors per node (ppn) you want.  Set this appropriately.  If you specify multiple processors per node and if you want one Gaussian job to use all of the processors on a node, you may need to place this line in your Gaussian header (pso input file): "% nprocshared=number_of_processors_you_want".  Some Gaussian installations may not require you to do this.  The wall time is a maximum time limit you want to allow your job to run for.  Set this appropriately. The example file specifies 24 hours.
    #PBS -l mem=1GB
    2. Set the memory limit per node that you want on line 3.
    3. Replace "job_name" on lines 4 and 5 with a job name of your choice.  
    4. Lines 15-20 obtain the list of allocated nodes form the queue and write one node per line in the "node.txt" file.  The pso application then reads this "nodes.txt" file and will start one energy calculation for each node/line in the file.  If you request multiple processors per node (line 2) select one of the following two options:
        A. If you want to run multiple Gaussian jobs on each node using one processor per job, remove the # symbol from the start of line 17 in the script file.  Also make sure there is a # symbol at the start of line 20 in the script file.
        B. If you want to run one Gaussian job on each node using all processors on the node, remove the # symbol from the start of line 20 in the script file.  Also make sure there is a # symbol at the start of line 17 in the script file.
    If you do not wish to use multiple processors per node, use option B above.  Also note, if the names of nodes on your cluster do not begin with "node00", you will need to change lines 17 and 20 accordingly.  Also, if you want to use ssh instead of rsh, you will need to change lines 17 and 20 accordingly.
    5. Change line 24 (the last line).  This calls the program with the input file using full path names.  It is usually best to put the input, output, and resume files in a separate directory other than the main pso directory.  NOTE: WHEN RESTARTING THE PROGRAM, DON.T FORGET TO CHANGE THE INPUT FILE NAME TO THE RESUME FILE NAME.


B. Method 2: Automatically submitting Gaussian jobs to the queue

To use this option, specify a template script file the program can use on line 48 in the pso input file (see below).  An example script file for doing this called "gaussianJobScript" is in the "scripts" directory inside the "pso" directory.  Make a copy of this script and place the name of this script file on line 48 in the input file (line 48 is used in this example only).  Also place the number of jobs to submit to the queue at a time on line 49.

47  Cluster node names file (put LOCAL for local nodes): nodes.txt
48  Job queue template file (used instead of the node names file, optional): myGaussinJobScript
49  Number of jobs submitted to the job queue at a time: 10

To modify your copy of "gaussianJobScript", do the following.  Specify one node and the number of processors per node (ppn) you want on line 2 of the script file.  Specify the amount of memory you want on line 3.  Specify a job name on line 4 (all jobs will have the same name).  This line also appears in the file:

$ENERGY_PROGRAM_GOES_HERE < $INPUT_FILE_NAME_GOES_HERE > $OUTPUT_FILE_NAME_GOES_HERE

The pso application will replace these three values, so DO NOT CHANGE THE ABOVE LINE.  Once you.ve modified your copy of the "gaussianJobScript" file, run the pso program from the command line as follows:

cd /path/to/pso
./pso /path/to/input/file/inputFile.txt

The program will perform energy calculations by submitting jobs to the queue.


C. Other useful tips

Also be sure to modify the pso input file appropriately.  Set the path to the energy program (line 4) to be the path to your Gaussian program.  Note that in doing so, you may need to specify that the program set the scratch directory each time it calls Gaussian.  The following is an example of how to do this:

Path to energy program: env SCRDIR=/some/scratch/directory /path/to/g03/

Next on line 5, specify a directory where energy files (.com, .dat, .log, etc.) will be written.  You will need to create this directory, and it will need to be unique for every input file you intend to run simultaneously with the program.  Make sure the names of your output and resume files are set appropriately in the input file.  Using full path names may be necessary.  Also, modify the Gaussian header appropriately.  This header will be placed at the start of every Gaussian input file.

Also, the program may have problems finding the file: periodic_table.csv.  You can either specify a full path name for this file or copy it to your home directory (assuming you are starting the program from your home directory).  Also, if you are submitting the main program to the queue, don.t forget to replace the name of the input file with the name of the resume file.

