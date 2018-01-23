#ifndef __SL_PY_MACROS_H__
#define __SL_PY_MACROS_H__
namespace sl{
namespace script{


#define S_Return { Py_INCREF(Py_None); return Py_None; }
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

/* 靠靠靠get靠 */
#define DECLARE_PY_GET_METHOD(MNAME)                                                \
    PyObject* MNAME();                                                              \
    static PyObject* __pyget_##MNAME(PyObject *self, void *closure)                 \
    {                                                                               \
        return static_cast<ThisClass*>(self)->MNAME();                              \
    }                                                                               \


/* 靠靠靠set靠 */
#define DECLARE_PY_SET_METHOD(MNAME)                                                \
    int MNAME(PyObject *value);                                                     \
    static int __pyset_##MNAME(PyObject *self,                                      \
                                    PyObject *value, void *closure)                 \
    {                                                                               \
        return static_cast<ThisClass*>(self)->MNAME(value);                         \
    }                                                                               \

/* 靠靠靠getset靠 */
#define DECLARE_PY_GETSET_METHOD(GETNAME, SETNAME)                                  \
    DECLARE_PY_GET_METHOD(GETNAME)                                                  \
    DECLARE_PY_SET_METHOD(SETNAME)                                                  \


#define PY_METHOD_ARG_PyObject_ptr                      PyObject *
#define PY_METHOD_ARG_PyObject_ptr_PYARGTYPE            "O"

#define DECLARE_PY_MOTHOD_ARG1(FUNCNAME, ARG_TYPE1)                                                                                 \
    PyObject* FUNCNAME(PY_METHOD_ARG_##ARG_TYPE1);                                                                                  \
    static PyObject* __py_##FUNCNAME(PyObject* self, PyObject* args, PyObject* kwds)                                                \
    {                                                                                                                               \
        PY_METHOD_ARG_##ARG_TYPE1 arg1;                                                                                             \
                                                                                                                                    \
        const uint8 argsSize = 1;                                                                                                   \
        uint16 currargsSize = (uint16)PyTuple_Size(args);                                                                           \
        ThisClass* pobj = static_cast<ThisClass*>(self);                                                                            \
                                                                                                                                    \
        if(currargsSize == argsSize)                                                                                                \
        {                                                                                                                           \
            if(!PyArg_ParseTuple(args, PY_METHOD_ARG_##ARG_TYPE1##_PYARGTYPE,                                                       \
                                        &arg1))                                                                                     \
            {                                                                                                                       \
                PyErr_Format(PyExc_TypeError, "%s: args(%s) is error!\n", __FUNCTION__, #ARG_TYPE1);                                \
                PyErr_PrintEx(0);                                                                                                   \
                S_Return;                                                                                                           \
            }                                                                                                                       \
        }                                                                                                                           \
        else                                                                                                                        \
        {                                                                                                                           \
            PyErr_Format(PyExc_AssertionError, "%s: args require %d args(%s), gived %d! is script[%s].\n",                          \
                __FUNCTION__, argsSize, #ARG_TYPE1, currargsSize, pobj->scriptName());                                              \
                                                                                                                                    \
            PyErr_PrintEx(0);                                                                                                       \
                                                                                                                                    \
            S_Return;                                                                                                               \
        }                                                                                                                           \
                                                                                                                                    \
        return pobj->FUNCNAME(arg1);                                                                                                \
    }                                                                                                                               \
                                                                                                                                    \

#define SCRIPT_OBJECT_CALL_ARGS0(OBJ, METHOT_NAME)                                                      \
{                                                                                                       \
    if(static_cast<PyObject*>(OBJ) == NULL)                                                             \
    {                                                                                                   \
        ECHO_ERROR("call " #METHOT_NAME ", OBJ is NULL!");                                              \
    }                                                                                                   \
    else                                                                                                \
    {                                                                                                   \
        if(PyObject_HasAttrString(OBJ, METHOT_NAME))                                                    \
        {                                                                                               \
            PyObject* pyResult = PyObject_CallMethod((OBJ), (METHOT_NAME),                              \
                                                                const_cast<char*>(""));                 \
            if(pyResult != NULL)                                                                        \
                Py_DECREF(pyResult);                                                                    \
            else                                                                                        \
                PyErr_PrintEx(0);                                                                       \
        }                                                                                               \
    }                                                                                                   \
}                                                                                                       \

}
}
#endif
