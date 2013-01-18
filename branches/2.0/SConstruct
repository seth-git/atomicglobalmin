env = Environment(CCFLAGS='-Wall -Wno-unknown-pragmas -Wno-format -O3')
tinyxml = Split('tinyxml/tinyxml.cpp tinyxml/tinyxmlparser.cpp tinyxml/tinyxmlerror.cpp tinyxml/tinystr.cpp')
src = [Glob('*.cc'), Glob('translation/*.cc'), Glob('xsd/*.cc'), Glob('handbook/*.cc'), Glob('matrix/*.cc'), Glob('externalEnergy/*.cc')] + tinyxml
env.Program(target = 'agml', source = src)
