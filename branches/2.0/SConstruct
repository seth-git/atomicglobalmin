env = Environment(CCFLAGS='-Wall -Wno-unknown-pragmas -Wno-format -O3',LIBS='-lrt',CXX = 'mpiCC')
random = Split('random/mersenne.cpp random/random.cc')
tinyxml = Split('tinyxml/tinyxml.cpp tinyxml/tinyxmlparser.cpp tinyxml/tinyxmlerror.cpp tinyxml/tinystr.cpp')
src = [Glob('*.cc'), Glob('translation/*.cc'), Glob('xsd/*.cc'), Glob('handbook/*.cc'), Glob('matrix/*.cc'), Glob('externalEnergy/*.cc'), Glob('internalEnergy/*.cc')] + tinyxml + random
env.Program(target = 'agml', source = src)
