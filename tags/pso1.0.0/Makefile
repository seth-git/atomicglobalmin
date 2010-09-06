#pso.make a makefile
CXXFLAGS=-g -Wall
CFLAGS=-g

pso :  main.o input.o atom.o molecule.o moleculeSet.o energy.o gega.o
	g++ $(CXXFLAGS) -o pso main.o input.o atom.o molecule.o moleculeSet.o energy.o gega.o -lpthread

main.o:  main.cc main.h input.h atom.h molecule.h moleculeSet.h energy.h
#	g++ -c main.cc
#
input.o:  input.cc input.h typedef.h
#	g++ -c input.cc
#
atom.o:  atom.cc atom.h typedef.h
#	g++ -c atom.cc
#
molecule.o:  molecule.cc molecule.h typedef.h
#	g++ -c molecule.cc
#
moleculeSet.o:  moleculeSet.cc moleculeSet.h typedef.h
#	g++ -c moleculeSet.cc

energy.o: energy.cc energy.h typedef.h
#       g++ -o energy.cc

gega.o: gega.cc gega.h typedef.h
#       g++ -o gega.cc

clean:  
	rm pso *.o

# END OF MAKE FILE


