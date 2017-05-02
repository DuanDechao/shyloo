function reload(m)
	package.loaded[m] = nil
	require(m)
end

function call(m, f, ...)
	local M = require(m)
	assert(M, "can not find module")
	
	local func = M[f]
	assert(func, "can not find function")
	
	return func(...)
end