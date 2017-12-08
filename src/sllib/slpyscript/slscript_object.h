#ifndef SL_LIB_SCRIPT_OBJECT_H
#define SL_LIB_SCRIPT_OBJECT_H
#include "Include/Python.h"
#include <unordered_map>
#include "slmulti_sys.h"
#include "slpymacros.h"
namespace sl {
namespace script{

#define SCRIPT_OBJECT_HEADER(CLASS, SUPERCLASS)												\
	SCRIPT_HREADER_BASE(CLASS, SUPERCLASS);													\
	static void _tp_dealloc(PyObject* self){												\
		CLASS::s_objectType.tp_free(self);													\
	}																						\

#define BASE_SCRIPT_HEADER(CLASS, SUPERCLASS)												\
	SCRIPT_HREADER_BASE(CLASS, SUPERCLASS);													\
	static void _tp_dealloc(PyObject* self){												\
		static_cast<CLASS*>(self)->~CLASS();												\
		CLASS::s_objectType.tp_free(self);													\
	}																						\

#define SCRIPT_HREADER_BASE(CLASS, SUPERCLASS)												\
	static PyTypeObject		s_objectType;													\
	typedef CLASS ThisClass;																\
	static PyObject* _tp_repr(PyObject* self){												\
		return static_cast<CLASS*>(self)->tp_repr();										\
	}																						\
																							\
	static PyObject* _tp_str(PyObject* self){												\
		return static_cast<CLASS*>(self)->tp_str();											\
	}																						\
																							\
	/*创建对象*/																			\
	static PyObject* _tp_new(PyTypeObject* type, PyObject* args, PyObject* kwds){			\
		return CLASS::tp_new(type, args, kwds);												\
	}																						\
																							\
	/*获取脚本对象的属性或方法*/															\
	static PyObject* _tp_getattro(PyObject* self, PyObject* name){							\
		return static_cast<CLASS*>(self)->onScriptGetAttribute(name);						\
	}																						\
																							\
	/*设置对象的属性和方法*/																\
	static int _tp_setattro(PyObject* self, PyObject* name, PyObject* value){				\
		return (value != NULL) ?															\
			static_cast<CLASS*>(self)->onScriptSetAttribute(name, value) :					\
			static_cast<CLASS*>(self)->onScriptDelAttribute(name);							\
	}																						\
																							\
	/*请求初始化对象*/																		\
	static int _tp_init(PyObject* self, PyObject * args, PyObject* kwds){					\
		return static_cast<CLASS*>(self)->onScriptInit(self, args, kwds);					\
	}																						\
																							\
public:																						\
	/*安装教脚本模块中的方法和成员存放列表*/												\
	static PyMethodDef*		s_##CLASS##_pScriptMethods;										\
	static PyMemberDef*		s_##CLASS##_pScriptMembers;										\
	static PyGetSetDef*		s_##CLASS##_pScriptGetseters;									\
																							\
	/*模块要暴露给脚本的方法和成员，导入到上面的指针列表中*/								\
	static PyMethodDef		s_##CLASS##_scriptMethods[];									\
	static PyMemberDef		s_##CLASS##_scriptMembers[];									\
	static PyGetSetDef		s_##CLASS##_scriptGetSeters[];									\
																							\
	static bool				s_##CLASS##_pyInstalled;										\
																							\
	/*getset的只读属性*/																	\
	static int __py_readonly_descr(PyObject* self, PyObject* value, void* closure){			\
		PyErr_Format(PyExc_TypeError, "this attribute %s.%s is read only",					\
		self != NULL ? self->ob_type->tp_name : #CLASS,										\
		closure != NULL ? (char*)closure : "unknown");										\
		PyErr_PrintEx(0);																	\
		return 0;																			\
	}																						\
																							\
    /*getset的只写属性*/																	\
	static int __py_writeonly__descr(PyObject* self, PyObject* value, void* closure){		\
		PyErr_Format(PyExc_TypeError, "this attribute %s.%s is write only",					\
		self != NULL ? self->ob_type->tp_name : #CLASS,										\
		closure != NULL ? (char*)closure : "unknown");										\
		PyErr_PrintEx(0);																	\
		return 0;																			\
	}																						\
																							\
	/*获得脚本类别*/																		\
	static PyTypeObject* getScriptType(void){												\
		return &s_objectType;																\
	}																						\
																							\
	static PyTypeObject* getBaseScriptType(void){											\
		if (strcmp("ScriptObject", #SUPERCLASS) == 0)										\
			return 0;																		\
		return SUPERCLASS::getScriptType();													\
	}																						\
																							\
	static long calcDictOffset(void){														\
		return 0;																			\
	}																						\
																							\
	/*计算暴露方法的个数*/																	\
	static int calcTotalMethodCount(void){													\
		int iLen = 0;																		\
		while (true){																		\
			PyMethodDef* pmt = &s_##CLASS##_scriptMethods[iLen];							\
			if (!pmt->ml_doc && !pmt->ml_flags && !pmt->ml_meth && !pmt->ml_name)			\
				break;																		\
			iLen++;																			\
		}																					\
																							\
		if (strcmp(#CLASS, #SUPERCLASS) == 0){											\
			return iLen;																	\
		}																					\
																							\
		return SUPERCLASS::calcTotalMethodCount() + iLen;									\
	}																						\
																							\
	/*计算所有继承模块暴露的成员个数*/														\
	static int calcTotalMemberCount(void){													\
		int iLen = 0;																		\
		while (true){																		\
			PyMemberDef* pmd = &s_##CLASS##_scriptMembers[iLen];							\
			if (!pmd->doc && !pmd->flags && !pmd->type && !pmd->name && !pmd->offset)		\
				break;																		\
			iLen++;																			\
		}																					\
																							\
		if (strcmp(#CLASS, #SUPERCLASS) == 0){											\
			return iLen;																	\
		}																					\
																							\
		return SUPERCLASS::calcTotalMemberCount() + iLen;									\
	}																						\
																							\
	/*计算所有继承模块的暴露getset个数*/													\
	static int calcTotalGetSetCount(void){													\
		int iLen = 0;																		\
		while (true){																		\
			PyGetSetDef* pgs = &s_##CLASS##_scriptGetSeters[iLen];							\
			if (!pgs->doc && !pgs->get && !pgs->set && !pgs->name && !pgs->closure)			\
				break;																		\
			iLen++;																			\
		}																					\
																							\
		if (strcmp(#CLASS, #SUPERCLASS) == 0){											\
			return iLen;																	\
		}																					\
																							\
		return SUPERCLASS::calcTotalGetSetCount() + iLen;									\
	}																						\
																							\
	/*所有父类及当前模块暴露的成员和方法安装到最终要导入脚本的列表中*/						\
	static void setupScriptMethodAndAttribute(PyMethodDef* pPymdf, PyMemberDef* pMdf,		\
	PyGetSetDef* pGsdf)																		\
	{																						\
		PyMethodDef* pmf = NULL;															\
		PyMemberDef* pmd = NULL;															\
		PyGetSetDef* pgs = NULL;															\
																							\
		int i = 0;																			\
		while (true){																		\
			pmf = &s_##CLASS##_scriptMethods[i];											\
			if (!pmf->ml_doc && !pmf->ml_flags && !pmf->ml_meth && !pmf->ml_name)			\
				break;																		\
			i++;																			\
			*(pPymdf++) = *pmf;																\
		}																					\
																							\
		i = 0;																				\
		while (true){																		\
			pmd = &s_##CLASS##_scriptMembers[i];											\
			if (!pmd->doc && !pmd->flags && !pmd->type && !pmd->name && !pmd->offset)		\
				break;																		\
			i++;																			\
			*(pMdf++) = *pmd;																\
		}																					\
																							\
		i = 0;																				\
		while (true){																		\
			pgs = &s_##CLASS##_scriptGetSeters[i];											\
			if (!pgs->doc && !pgs->get && !pgs->set && !pgs->name && !pgs->closure)			\
				break;																		\
			i++;																			\
			*(pGsdf++) = *pgs;																\
		}																					\
																							\
		if (strcmp(#CLASS, #SUPERCLASS) == 0){											\
			*(pPymdf) = *pmf;																\
			*(pMdf) = *pmd;																	\
			*(pGsdf) = *pgs;																\
			return;																			\
		}																					\
																							\
																							\
		SUPERCLASS::setupScriptMethodAndAttribute(pPymdf, pMdf, pGsdf);						\
	}																						\
																							\
	/*安装当前脚本模块*/																	\
	static void refreshObjectType(const char* name){										\
		int iMethodCount = CLASS::calcTotalMethodCount();									\
		int iMemberCount = CLASS::calcTotalMemberCount();									\
		int iGetsetCount = CLASS::calcTotalGetSetCount();									\
																							\
		s_##CLASS##_pScriptMethods = NEW PyMethodDef[iMethodCount + 2];						\
		s_##CLASS##_pScriptMembers = NEW PyMemberDef[iMemberCount + 2];						\
		s_##CLASS##_pScriptGetseters = NEW PyGetSetDef[iGetsetCount + 2];					\
																							\
		setupScriptMethodAndAttribute(s_##CLASS##_pScriptMethods,							\
		s_##CLASS##_pScriptMembers, s_##CLASS##_pScriptGetseters);							\
																							\
		s_objectType.tp_methods = s_##CLASS##_pScriptMethods;								\
		s_objectType.tp_members = s_##CLASS##_pScriptMembers;								\
		s_objectType.tp_getset = s_##CLASS##_pScriptGetseters;								\
																							\
		s_##CLASS##_pyInstalled = true;														\
																							\
		ScriptObject::s_allObjectTypes[name] = &s_objectType;								\
	}																						\
																							\
	/*卸載当前脚本模块*/																	\
	static void uninstallScript(void){														\
		SAFE_RELEASE_ARRAY(s_##CLASS##_pScriptMethods);										\
		SAFE_RELEASE_ARRAY(s_##CLASS##_pScriptMembers);										\
		SAFE_RELEASE_ARRAY(s_##CLASS##_pScriptGetseters);									\
		CLASS::onUninstallScript();															\
																							\
		if (s_##CLASS##_pyInstalled)														\
			Py_DECREF(&s_objectType);														\
	}																						\

/** 这个宏正式的初始化一个脚本模块， 将一些必要的信息填充到python的type对象中
*/
#define SCRIPT_INIT(CLASS, CALL, SEQ, MAP, ITER, ITERNEXT)									\
	TEMPLATE_SCRIPT_INIT(;, CLASS, CLASS, CALL, SEQ, MAP, ITER, ITERNEXT)					\


#define TEMPLATE_SCRIPT_INIT(TEMPLATE_HEADER, TEMPLATE_CLASS,								\
	CLASS, CALL, SEQ, MAP, ITER, ITERNEXT)													\
	TEMPLATE_HEADER PyMethodDef* TEMPLATE_CLASS::s_##CLASS##_pScriptMethods = NULL;			\
	TEMPLATE_HEADER PyMemberDef* TEMPLATE_CLASS::s_##CLASS##_pScriptMembers = NULL;			\
	TEMPLATE_HEADER PyGetSetDef* TEMPLATE_CLASS::s_##CLASS##_pScriptGetseters = NULL;		\
																							\
	TEMPLATE_HEADER																			\
	PyTypeObject TEMPLATE_CLASS::s_objectType =												\
	{																						\
		PyVarObject_HEAD_INIT(&PyType_Type, 0)												\
		#CLASS,													/* tp_name            */	\
		sizeof(TEMPLATE_CLASS),									/* tp_basicsize       */	\
		0,														/* tp_itemsize        */	\
		(destructor)TEMPLATE_CLASS::_tp_dealloc,				/* tp_dealloc         */	\
		0,														/* tp_print           */	\
		0,														/* tp_getattr         */	\
		0,														/* tp_setattr         */	\
		0,														/* tp_compare         */	\
		TEMPLATE_CLASS::_tp_repr,								/* tp_repr            */	\
		0,														/* tp_as_number       */	\
		SEQ,													/* tp_as_sequence     */	\
		MAP,													/* tp_as_mapping      */	\
		0,														/* tp_hash            */	\
		CALL,													/* tp_call            */	\
		TEMPLATE_CLASS::_tp_str,								/* tp_str             */	\
		(getattrofunc)CLASS::_tp_getattro,						/* tp_getattro        */	\
		(setattrofunc)CLASS::_tp_setattro,						/* tp_setattro        */	\
		0,														/* tp_as_buffer       */	\
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,				/* tp_flags           */	\
		"KBEngine::" #CLASS " objects.",						/* tp_doc             */	\
		0,														/* tp_traverse        */	\
		0,														/* tp_clear           */	\
		0,														/* tp_richcompare     */	\
		0,														/* tp_weaklistoffset  */	\
		ITER,													/* tp_iter            */	\
		ITERNEXT,												/* tp_iternext        */	\
		0,														/* tp_methods         */	\
		0,														/* tp_members         */	\
		0,														/* tp_getset          */	\
		TEMPLATE_CLASS::getBaseScriptType(),					/* tp_base            */	\
		0,														/* tp_dict            */	\
		0,														/* tp_descr_get       */	\
		0,														/* tp_descr_set       */	\
		TEMPLATE_CLASS::calcDictOffset(),						/* tp_dictoffset      */	\
		(initproc)TEMPLATE_CLASS::_tp_init,						/* tp_init            */	\
		0,														/* tp_alloc           */	\
		TEMPLATE_CLASS::_tp_new,								/* tp_new             */	\
		PyObject_GC_Del,										/* tp_free            */	\
	};																						\

#define BASE_SCRIPT_INIT(CLASS, CALL, SEQ, MAP, ITER, ITERNEXT)								\
	PyMethodDef* CLASS::s_##CLASS##_pScriptMethods = NULL;									\
	PyMemberDef* CLASS::s_##CLASS##_pScriptMembers = NULL;									\
	PyGetSetDef* CLASS::s_##CLASS##_pScriptGetseters = NULL;								\
																							\
	PyTypeObject CLASS::s_objectType =														\
	{																						\
		PyVarObject_HEAD_INIT(NULL, 0)														\
		#CLASS,																				\
		sizeof(CLASS),									/*tp_basesize		*/				\
		0,												/*tp_itemsize		*/				\
		(destructor)CLASS::_tp_dealloc,					/*tp_dealloc		*/				\
		0,												/*tp_print			*/				\
		0,												/*tp_getattr		*/				\
		0,												/*tp_setattr		*/				\
		0,												/*tp_compare		*/				\
		CLASS::_tp_repr,								/*tp_repr			*/				\
		0,												/*tp_as_number		*/				\
		SEQ,											/*tp_as_sequence    */				\
		MAP,											/*tp_as_mapping     */				\
		0,												/*tp_hash			*/				\
		CALL,											/*tp_call			*/				\
		CLASS::_tp_str,									/*tp_str			*/				\
		(getattrofunc)CLASS::_tp_getattro,				/*tp_getattro		*/				\
		(setattrofunc)CLASS::_tp_setattro,				/*tp_setattro		*/				\
		0,												/*tp_as_buffer		*/				\
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,		/*tp_flags			*/				\
		"shyloo::" #CLASS " object.",					/*tp_doc			*/				\
		0,												/*tp_traverse		*/				\
		0,												/*tp_clear			*/				\
		0,												/*tp_richcompare	*/				\
		0,												/*tp_weaklistoffset	*/				\
		ITER,											/*tp_iter			*/				\
		ITERNEXT,										/*tp_iternext		*/				\
		0,												/*tp_methods		*/				\
		0,												/*tp_members		*/				\
		0,												/*tp_getset			*/				\
		CLASS::getBaseScriptType(),						/*tp_base			*/				\
		0,												/*tp_dict			*/				\
		0,												/*tp_descr_get		*/				\
		0,												/*tp_descr_set		*/				\
		CLASS::calcDictOffset(),						/*tp_dictoffset		*/				\
		0,												/*tp_init			*/				\
		0,												/*tp_alloc			*/				\
		0,												/*tp_new			*/				\
		PyObject_GC_Del,								/*tp_free			*/				\
		0,												/* tp_is_gc           */			\
		0,												/* tp_bases           */			\
		0,												/* tp_mro             */			\
		0,												/* tp_cache           */			\
		0,												/* tp_subclasses      */			\
		0,												/* tp_weaklist        */			\
		0,												/* tp_del			  */			\
	};																						\


class ScriptObject :public PyObject{
	SCRIPT_OBJECT_HEADER(ScriptObject, ScriptObject)

public:
	ScriptObject(PyTypeObject* pyType, bool isInitialised = false);
	~ScriptObject();

	//所有的腳本類別
	typedef std::unordered_map<std::string, PyTypeObject*> SCRIPTOBJECT_TYPES;
	static  SCRIPTOBJECT_TYPES s_allObjectTypes;
	static PyTypeObject* getScriptObjectType(const std::string& name);

	//腳本對象引用計數
	void incRef() const { Py_INCREF((PyObject*)this); }
	void decRef() const { Py_DECREF((PyObject*)this); }
	int refCount() const { return (int)(((PyObject*)this)->ob_refcnt); }

	PyObject* tp_repr();
	PyObject* tp_str();

	//脚本请求创建对象
	static PyObject* tp_new(PyTypeObject* type, PyObject* args, PyObject* kwds);


	//脚本请求获取属性或方法
	PyObject* onScriptGetAttribute(PyObject* attr);

	//脚本请求设置属性或方法
	int onScriptSetAttribute(PyObject* attr, PyObject* value);

	//脚本请求删除一个属性
	int onScriptDelAttribute(PyObject* attr);

	//脚本请求初始化
	int onScriptInit(PyObject* self, PyObject* args, PyObject* kwds) { return 0; }

	//获取对象类别名称
	const char* scriptName() const { return ob_type->tp_name; }

	//脚本被安装时
	static void onInstallScript(PyObject* mod) {}

	//脚本被卸载时
	static void onUninstallScript() {}
};

}
}
#endif
