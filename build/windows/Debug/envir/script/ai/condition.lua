local load = load
local error = error
_ENV = {}

local funcs = {}
function get_func(str)
	if funcs[str] ~= nil then
		return funcs[str]
	end
	
	local chunk = load("return "..str, "get_func", "t", _ENV)
	local func = chunk()
	if func ~= nil then
		funcs[str] = func
	end	
	return func
end

function call(str, object)
	local func = get_func(str)
	if func == nil then
		error("can't load func"..str)
	end
	return func(object)
end

function IsNumber(num)
	return function(object)
		return num == 3
	end
end

function no_(func)
	return function(object)
		return not func(object)
	end
end

function and_(func1, func2)
	return function(object)
		return func1(object) and func2(object)
	end
end

function or_(func1, func2)
	return function(object)
		return func1(object) or func2(object)
	end
end

return _ENV