#pso.make a makefile
CXXFLAGS=-g -Wall
CFLAGS=-g

pso:  mpi main.o input.o ring.o ringSet.o bond.o atom.o molecule.o moleculeSet.o energy.o gega.o typedef.h
	g++ $(CXXFLAGS) -o pso main.o input.o ring.o ringSet.o bond.o atom.o molecule.o moleculeSet.o energy.o gega.o

mpi:  mpi.cc energy.o atom.o bond.o ring.o ringSet.o molecule.o moleculeSet.o
	mpiCC $(CXXFLAGS) -o mpi mpi.cc energy.o atom.o bond.o ring.o ringSet.o molecule.o moleculeSet.o

main.o:  main.cc main.h input.h atom.h molecule.h moleculeSet.h energy.h
#	g++ -c main.cc
#
input.o:  input.cc input.h typedef.h
#	g++ -c input.cc
#
bond.o:  bond.cc bond.h atom.cc atom.h typedef.h
#       g++ -c bond.cc
#
atom.o:  atom.cc atom.h typedef.h
#	g++ -c atom.cc
#
ring.o:  ring.cc ring.h typedef.h
#	g++ -c ring.cc
#
ringSet.o:  ringSet.cc ringSet.h typedef.h
#	g++ -c ring.cc
#
molecule.o:  molecule.cc molecule.h typedef.h
#	g++ -c molecule.cc
#
moleculeSet.o:  moleculeSet.cc moleculeSet.h typedef.h
#	g++ -c moleculeSet.cc

energy.o: energy.cc energy.h moleculeSet.h
#       g++ -o energy.cc

gega.o: gega.cc gega.h energy.cc energy.h
#       g++ -o gega.cc

clean:  
	rm pso mpi atom.o bond.o ring.o ringSet.o energy.o  gega.o  input.o  main.o  molecule.o  moleculeSet.o mpi.o

# END OF MAKE FILE

