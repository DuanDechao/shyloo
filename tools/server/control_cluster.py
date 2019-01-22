# -*- coding: utf-8 -*-
#!/usr/bin/env python
import sys
from optparse import OptionParser
from pycommon import uid as uidmodule
from pycommon import cluster

def main():
	usage ="usage: %prog [options] arg"
	parser = OptionParser(usage)
	parser.add_option("-u", dest="uid", default = None, help="Specify the UID or username to work with")
	parser.add_option("-v", "--verbose", dest="verbose", action="store_true", help="Enable verbose debugging output")
	options, args = parser.parse_args()

	if options.uid == None:
		options.uid = uidmodule.getuid()
	
	command = "display"
	if args:
		command = args.pop(0)

	c = cluster.Cluster.get(uid = uidmodule.getuid(options.uid))

def runscript(procs, script = None, lockCells = False):
	pass

if __name__ == "__main__":
	sys.exit(main())

