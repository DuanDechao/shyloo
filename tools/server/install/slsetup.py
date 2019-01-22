# -*- coding: utf-8 -*-

import os
import sys
appdir = os.path.dirname(os.path.abspath(__file__))

def addPath(relpath, pos = None):
	root = os.path.abspath(appdir + "/" + relpath)
	if root not in sys.path:
		if pos is None:
			sys.path.append(root)
		else:
			sys.path.insert(pos, root)
