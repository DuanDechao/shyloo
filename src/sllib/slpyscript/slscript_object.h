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
	/*��������*/																			\
	static PyObject* _tp_new(PyTypeObject* type, PyObject* args, PyObject* kwds){			\
		return CLASS::tp_new(type, args, kwds);												\
	}																						\
																							\
	/*��ȡ�ű���������Ի򷽷�*/															\
	static PyObject* _tp_getattro(PyObject* self, PyObject* name){							\
		return static_cast<CLASS*>(self)->onScriptGetAttribute(name);						\
	}																						\
																							\
	/*���ö�������Ժͷ���*/																\
	static int _tp_setattro(PyObject* self, PyObject* name, PyObject* value){				\
		return (value != NULL) ?															\
			static_cast<CLASS*>(self)->onScriptSetAttribute(name, value) :					\
			static_cast<CLASS*>(self)->onScriptDelAttribute(name);							\
	}																						\
																							\
	/*�����ʼ������*/																		\
	static int _tp_init(PyObject* self, PyObject * args, PyObject* kwds){					\
		return static_cast<CLASS*>(self)->onScriptInit(self, args, kwds);					\
	}																						\
																							\
public:																						\
	/*��װ�̽ű�ģ���еķ����ͳ�Ա����б�*/												\
	static PyMethodDef*		s_##CLASS##_pScriptMethods;										\
	static PyMemberDef*		s_##CLASS##_pScriptMembers;										\
	static PyGetSetDef*		s_##CLASS##_pScriptGetseters;									\
																							\
	/*ģ��Ҫ��¶���ű��ķ����ͳ�Ա�����뵽�����ָ���б���*/								\
	static PyMethodDef		s_##CLASS##_scriptMethods[];									\
	static PyMemberDef		s_##CLASS##_scriptMembers[];									\
	static PyGetSetDef		s_##CLASS##_scriptGetSeters[];									\
																							\
	static bool				s_##CLASS##_pyInstalled;										\
																							\
	/*getset��ֻ������*/																	\
	static int __py_readonly_descr(PyObject* self, PyObject* value, void* closure){			\
		PyErr_Format(PyExc_TypeError, "this attribute %s.%s is read only",					\
		self != NULL ? self->ob_type->tp_name : #CLASS,										\
		closure != NULL ? (char*)closure : "unknown");										\
		PyErr_PrintEx(0);																	\
		return 0;																			\
	}																						\
																							\
    /*getset��ֻд����*/																	\
	static int __py_writeonly__descr(PyObject* self, PyObject* value, void* closure){		\
		PyErr_Format(PyExc_TypeError, "this attribute %s.%s is write only",					\
		self != NULL ? self->ob_type->tp_name : #CLASS,										\
		closure != NULL ? (char*)closure : "unknown");										\
		PyErr_PrintEx(0);																	\
		return 0;																			\
	}																						\
																							\
	/*��ýű����*/																		\
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
	/*���㱩¶�����ĸ���*/																	\
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
	/*�������м̳�ģ�鱩¶�ĳ�Ա����*/														\
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
	/*�������м̳�ģ��ı�¶getset����*/													\
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
	/*���и��༰��ǰģ�鱩¶�ĳ�Ա�ͷ�����װ������Ҫ����ű����б���*/						\
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
		SUPERCLASS::setupScriptMethodAndAttribute(pPymdf, pMdf, pGsdf);						\
	}																						\
																							\
	/*��װ��ǰ�ű�ģ��*/																	\
	static void installScript(PyObject* mod, const char* name){								\
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
		CLASS::onInstallScript(mod);														\
		if (PyType_Ready(&s_objectType) < 0){												\
			ECHO_ERROR("PyType_Ready(" #CLASS ") is error");								\
			PyErr_Print();																	\
			return;																			\
		}																					\
																							\
		if (mod){																			\
			Py_INCREF(&s_objectType);														\
			if (PyModule_AddObject(mod, name, (PyObject*)&s_objectType) < 0){				\
				ECHO_ERROR("PyModule_AddObject(%s) is error!", name);						\
			}																				\
		}																					\
																							\
		SCRIPT_ERROR_CHECK();																\
		s_##CLASS##_pyInstalled = true;														\
																							\
		ScriptObject::s_allObjectTypes[name] = &s_objectType;								\
	}																						\
																							\
	/*ж�d��ǰ�ű�ģ��*/																	\
	static void uninstallScript(void){														\
		SAFE_RELEASE_ARRAY(s_##CLASS##_pScriptMethods);										\
		SAFE_RELEASE_ARRAY(s_##CLASS##_pScriptMembers);										\
		SAFE_RELEASE_ARRAY(s_##CLASS##_pScriptGetseters);									\
		CLASS::onUninstallScript();															\
																							\
		if (s_##CLASS##_pyInstalled)														\
			Py_DECREF(&s_objectType);														\
	}																						\

#define SCRIPT_INIT(CLASS, CALL, SEQ, MAP, ITER, ITERNEXT)									\
	PyTypeObject CLASS::s_objectType =														\
	{																						\
		PyVarObject_HEAD_INIT(&PyType_Type, 0)												\
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
		(initproc)CLASS::_tp_init,						/*tp_init			*/				\
		0,												/*tp_alloc			*/				\
		CLASS::_tp_new,									/*tp_new			*/				\
		PyObject_GC_Del,								/*tp_free			*/				\
	};																						\

class ScriptObject :public PyObject{
	SCRIPT_OBJECT_HEADER(ScriptObject, ScriptObject)

public:
	ScriptObject(PyTypeObject* pyType, bool isInitialised = false);
	~ScriptObject();

	//���е��_��e
	typedef std::unordered_map<std::string, PyTypeObject*> SCRIPTOBJECT_TYPES;
	static  SCRIPTOBJECT_TYPES s_allObjectTypes;
	static PyTypeObject* getScriptObjectType(const std::string& name);

	//�_����������Ӌ��
	void incRef() const { Py_INCREF((PyObject*)this); }
	void decRef() const { Py_DECREF((PyObject*)this); }
	int refCount() const { return (int)(((PyObject*)this)->ob_refcnt); }

	PyObject* tp_repr();
	PyObject* tp_str();

	//�ű����󴴽�����
	static PyObject* tp_new(PyTypeObject* type, PyObject* args, PyObject* kwds);


	//�ű������ȡ���Ի򷽷�
	PyObject* onScriptGetAttribute(PyObject* attr);

	//�ű������������Ի򷽷�
	int onScriptSetAttribute(PyObject* attr, PyObject* value);

	//�ű�����ɾ��һ������
	int onScriptDelAttribute(PyObject* attr);

	//�ű������ʼ��
	int onScriptInit(PyObject* self, PyObject* args, PyObject* kwds) { return 0; }

	//��ȡ�����������
	const char* scriptName() const { return ob_type->tp_name; }

	//�ű�����װʱ
	static void onInstallScript(PyObject* mod) {}

	//�ű���ж��ʱ
	static void onUninstallScript() {}
};

}
}
#endif
