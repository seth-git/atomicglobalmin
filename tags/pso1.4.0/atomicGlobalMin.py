#!/usr/local/bin/python

import string, sys
from cStringIO import StringIO
from decimal import *
import logging
import inspect

saveout = sys.stdout
myBuffer = StringIO()
sys.stdout = myBuffer
from cclib.parser import ccopen # Note this include prints stuff to the standard output that I don't want
sys.stdout = saveout

arguments = sys.argv
del arguments[0]

fileName = arguments[0]
del arguments[0]

myfile = ccopen(fileName)
myfile.logger.setLevel(logging.ERROR)
data = myfile.parse()
myDictionary = dict(inspect.getmembers(data))

notFoundMessage = 'not found'

for arg in arguments:
	if arg == 'scfenergies_last':
		if (not myDictionary.has_key('scfenergies')):
			print notFoundMessage
		else:
			print data.scfenergies[len(data.scfenergies)-1]
	elif arg == 'scfenergies_last_au':
		if (not myDictionary.has_key('scfenergies')):
			print notFoundMessage
		else:
			energyInEV = str(data.scfenergies[len(data.scfenergies)-1])
			hartreesPerEV = str(0.03674932453368173952251296638)
			energyInHartrees = Decimal(energyInEV) * Decimal(hartreesPerEV)
			print '%lf' % (energyInHartrees)
	elif arg == 'atomcoords_last':
		if (not myDictionary.has_key('atomcoords')):
			print notFoundMessage
		else:
			print data.atomcoords[len(data.atomcoords)-1]
	elif arg == 'isTransitionState':
		if (not myDictionary.has_key('vibfreqs')):
			print notFoundMessage
		elif ((len(data.vibfreqs) >= 2) and (data.vibfreqs[0] < 0) and (data.vibfreqs[1] > 0)):
			print 1
		else:
			print 0
	elif (not myDictionary.has_key(arg)):
		print notFoundMessage
	else:
		print myDictionary[arg]

