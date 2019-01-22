# -*- coding: utf-8 -*-
import re
import threading
class ExposedMetaClass(type):
	def __init__(cls, *args, **kw):
		type.__init__(cls, *args, **kw)
		print "ssssssssssssssssss", cls.__name__
		cls.s_exposedMethods = []
		for superClass in cls.__mro__:
			for name, method in superClass.__dict__.items():
				if hasattr(method, "exposedLabel"):
					cls.s_exposedMethods.append(name)


class Exposed(object):
	__metaclass__ = ExposedMetaClass

	@staticmethod
	def expose(label = None, args = [], precond = lambda self: True):
		def inner(f, label = label, args = args, precond = precond):
			f.exposedPrecond = precond
			f.exposedArgs = args

			if label is None:
				label = re.sub("[A-Z]", lambda m : " " + m.group(0), f.__name__)
				label = re.sub("^[a-z]", lambda m : m.group(0).upper(), label)
			
			f.exposedLabel = label
			return f
	
		return inner

	def getExposedMethods(self):
		methods = []
		for funcName in self.s_exposedMethods:
			func = getattr(self, funcName)
			if func.exposedPrecond(self):
				methods.append((func.__name__, func.exposedLabel, func.exposedArgs))
		return methods

	def getExposedID(self):
		pass


class Cluster(Exposed):

	OBJECT_CACHE = []
	OBJECT_CACHE_LOCK = threading.RLock()
    def __init__(self, **kw):
		Exposed.__init__(self)

		if kw.has_key("view"):
			self.view = kw["view"]
		else:
			self.view = "machine"

		self.ctime = time.time()
		self.kw = kw
		self.thread = threading.currentThread()
		
		self.OBJECT_CACHE_LOCK.acquire()
		self.expungeCache()
		self.OBJECT_CACHE.append(self)
		self.OBJECT_CACHE_LOCK.release()
	
	@classmethod
	def expungeCache(self):
		for obj in self.OBJECT_CACHE[:]:
			if time.time() - obj.ctime > 1.0:
				self.OBJECT_CACHE.remove(obj)

	@classmethod
	def get(self, **kw):
		try:
			self.OBJECT_CACHE_LOCK.acquire()
			self.expungeCache()

			for obj in self.OBJECT_CACHE:
				if obj.kw == kw and obj.thread == threading.currentThread():
					return obj

			return Cluster(**kw)
		
		finally:
			self.OBJECT_CACHE_LOCK.release()
	
	def refresh_(self, clearUsers = False):
		self.procs = set()


    @Exposed.expose(args = [("ddc", "fancy")])
    def exposeTestFunc(self):
        pass
