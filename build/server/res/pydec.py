def _func(f, index, paramT, omitdd):
	print ('error:', paramT, omitdd, f)
	f()

def func0(index, paramT, omitd = False):
	return lambda f: _func(f, index, paramT, omitd)
	
def func1(index, paramT, omit = False):
	a = func0(index, paramT, omitd = omit)
	return a

def fancy_decroe(f):
	f()

def salesgirl(method):
	return method
	