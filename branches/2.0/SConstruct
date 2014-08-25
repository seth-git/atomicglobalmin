env = Environment(CCFLAGS='-g -Wall -O3',LIBS='-lrt',CXX = 'mpiCC')
random = Split('random/mersenne.cpp random/random.cc')
src = [Glob('*.cc'), Glob('translation/*.cc'), Glob('xsd/*.cc'), Glob('handbook/*.cc'), Glob('matrix/*.cc'), Glob('externalEnergy/*.cc'), Glob('internalEnergy/*.cc'), Glob('simulatedAnnealing/*.cc')] + random
env.Program(target = 'agml', source = src)
