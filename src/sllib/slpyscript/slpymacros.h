#ifndef __SL_PY_MACROS_H__
#define __SL_PY_MACROS_H__
namespace sl{
namespace script{

//输出当前脚本产生的错误消息
#define SCRIPT_ERROR_CHECK()																	\
{																								\
	if (PyErr_Occurred()){																		\
		PyErr_PrintEx(0);																		\
	}																							\
}

#define S_INCREF(pyObj)																			\
	if (pyObj){																					\
		Py_INCREF(pyObj);																		\
	}																							\

#define S_DECREF(pyObj)																			\
	if (pyObj){																					\
		Py_DECREF(pyObj);																		\
	}																							\

	// python的对象释放
#define S_RELEASE(pyObj)																		\
	if (pyObj){																					\
		Py_DECREF(pyObj);																		\
		pyObj = NULL;																			\
	}																							\

//定义暴露给脚本的方法宏
#define SCRIPT_METHOD_DECLARE_BEGIN(CLASS)												bool CLASS::s_##CLASS##_pyInstalled = false; PyMethodDef CLASS::s_##CLASS##_scriptMethods[] = {
#define SCRIPT_METHOD_DECLARE(METHOD_NAME, METHOD_FUNC, FLAGS, DOC)						{ METHOD_NAME, (PyCFunction)&__py_##METHOD_FUNC, FLAGS, DOC },
#define SCRIPT_METHOD_DECLARE_END()														{ 0, 0, 0, 0 }};

//定义暴露给脚本的属性宏
#define SCRIPT_MEMBER_DECLARE_BEGIN(CLASS)												PyMemberDef CLASS::s_##CLASS##_scriptMembers[] = {
#define SCRIPT_MEMBER_DECLARE(MEMBER_NAME, MEMBER_REF, MEMBER_TYPE, FLAGS, DOC)			{ const_cast<char*>(MEMBER_NAME), MEMBER_TYPE, offsetof(ThisClass, MEMBER_REF), FLAGS, DOC },
#define SCRIPT_MEMBER_DECLARE_END()														{ 0, 0, 0, 0, 0 }};

//定义暴露给脚本的getset属性宏
#define SCRIPT_GETSET_DECLARE_BEGIN(CLASS)												PyGetSetDef CLASS::s_##CLASS##_scriptGetSeters[] = {
#define SCRIPT_GETSET_DECLARE(NAME, GET, SET, DOC, CLOSURE)								{const_cast<char*>(NAME), (getter)__pyget_##GET, (setter)__pyset_##SET, DOC, CLOSURE},
#define SCRIPT_GET_DECLARE(NAME, GET, DOC, CLOSURE)										{const_cast<char*>(NAME), (getter)__pyget_##GET, (setter)__py_readonly_descr, DOC, const_cast<char*>(NAME)},
#define SCRIPT_SET_DECLARE(NAME, SET, DOC, CLOSURE)										{const_cast<char*>(NAME), (getter)__py_writeonly__descr, (setter)__pyset_##SET, DOC, const_cast<char*>(NAME)},
#define SCRIPT_GETSET_DECLARE_END()														{0, 0, 0, 0, 0}};

}
}
#endif
