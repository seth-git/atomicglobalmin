#pso.make a makefile
CXXFLAGS=-g -Wall -Wno-format-security
CFLAGS=-g -Wno-format-security

pso: helper unit main.cc main.h myMpi.cc myMpi.h energy.o strings.o input.o energyProgram.o init.o argumentParser.o ring.o ringSet.o bond.o atom.o molecule.o moleculeSet.o gega.o typedef.h
	mpiCC $(CXXFLAGS) -o pso main.cc myMpi.cc energy.o strings.o input.o energyProgram.o init.o argumentParser.o ring.o ringSet.o bond.o atom.o molecule.o moleculeSet.o gega.o

helper: helper.o energy.o strings.o input.o energyProgram.o init.o argumentParser.o ring.o ringSet.o bond.o atom.o molecule.o moleculeSet.o gega.o typedef.h
	g++ $(CXXFLAGS) -o helper helper.o energy.o strings.o input.o energyProgram.o init.o argumentParser.o ring.o ringSet.o bond.o atom.o molecule.o moleculeSet.o gega.o

unit: unitTests/unit.o unitTests/util.o energy.o strings.o input.o energyProgram.o init.o argumentParser.o ring.o ringSet.o bond.o atom.o molecule.o moleculeSet.o gega.o typedef.h
	g++ $(CXXFLAGS) -o unit unitTests/util.cc unitTests/unit.cc energy.o strings.o input.o energyProgram.o init.o argumentParser.o ring.o ringSet.o bond.o atom.o molecule.o moleculeSet.o gega.o

helper.o: helper.h helper.cc

unitTests/unit.o: unitTests/unit.h unitTests/unit.cc unitTests/util.o
#	g++ -c energyProgram.cc
#
unitTests/util.o: unitTests/util.h unitTests/util.cc
#	g++ -c energyProgram.cc
#

energyProgram.o: energyProgram.h energyProgram.cc
#	g++ -c energyProgram.cc
#
strings.o: strings.h strings.cc
#	g++ -c strings.cc
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
	rm pso helper unit *.o unitTests/*.o

# END OF MAKE FILE

