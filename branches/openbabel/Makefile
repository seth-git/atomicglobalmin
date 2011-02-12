#pso.make a makefile
CXXFLAGS=-g -Wall -I/usr/local/include/openbabel-2.0/ -l openbabel -L/usr/local/lib
CFLAGS=-g

pso: helper unit main.cc main.h myMpi.cc myMpi.h energy.o input.o energyProgram.o init.o argumentParser.o ring.o ringSet.o bond.o atom.o molecule.o moleculeSet.o gega.o typedef.h
	mpiCC $(CXXFLAGS) -o pso main.cc myMpi.cc energy.o input.o energyProgram.o init.o argumentParser.o ring.o ringSet.o bond.o atom.o molecule.o moleculeSet.o gega.o

helper: helper.o energy.o input.o energyProgram.o init.o argumentParser.o ring.o ringSet.o bond.o atom.o molecule.o moleculeSet.o gega.o typedef.h
	g++ $(CXXFLAGS) -o helper helper.o energy.o input.o energyProgram.o init.o argumentParser.o ring.o ringSet.o bond.o atom.o molecule.o moleculeSet.o gega.o

unit: unit.o energy.o input.o energyProgram.o init.o argumentParser.o ring.o ringSet.o bond.o atom.o molecule.o moleculeSet.o gega.o typedef.h
	g++ $(CXXFLAGS) -o unit unit.o energy.o input.o energyProgram.o init.o argumentParser.o ring.o ringSet.o bond.o atom.o molecule.o moleculeSet.o gega.o

energyProgram.o: energyProgram.h energyProgram.cc
#	g++ -c energyProgram.cc
#
input.o:  input.cc input.h typedef.h molecule.o moleculeSet.o energyProgram.o
#	g++ -c input.cc
#
init.o: init.cc init.h input.o energyProgram.o
#	g++ -c init.cc
#
argumentParser.o: argumentParser.cc argumentParser.h
#	g++ -c argumentParser.cc
#
bond.o:  bond.cc bond.h typedef.h atom.o
#       g++ -c bond.cc
#
atom.o:  atom.cc atom.h typedef.h
#	g++ -c atom.cc
#
ring.o:  ring.cc ring.h typedef.h atom.o
#	g++ -c ring.cc
#
ringSet.o:  ringSet.cc ringSet.h ring.o
#	g++ -c ring.cc
#
molecule.o:  molecule.cc molecule.h bond.o ringSet.o atom.o
#	g++ -c molecule.cc
#
moleculeSet.o:  moleculeSet.cc moleculeSet.h molecule.o bond.o ringSet.o atom.o
#	g++ -c moleculeSet.cc
energy.o: energy.cc energy.h moleculeSet.o input.o energyProgram.o
#       g++ -o energy.cc

gega.o: gega.cc gega.h moleculeSet.o input.o
#       g++ -o gega.cc

clean:  
	rm pso helper unit *.o

# END OF MAKE FILE

