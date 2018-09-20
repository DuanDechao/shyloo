#ifndef SL_SCRIPT_PICKLER_H
#define SL_SCRIPT_PICKLER_H
//#include "scriptobject.h"
#include "slmulti_sys.h"
#include "Python.h"
#include <string>
namespace sl{ namespace pyscript{

class Pickler
{						
public:	
	/** 
		���� cPicket.dumps 
	*/
	static std::string pickle(PyObject* pyobj);
	static std::string pickle(PyObject* pyobj, int8 protocol);

	/*���� cPicket.loads */
	static PyObject* unpickle(const std::string& str);

	/** 
		��ʼ��pickler 
	*/
	static bool initialize(void);
	static void finalise(void);
	
	/** 
		��ȡunpickle������ģ����� 
	*/
	static PyObject* getUnpickleFuncTableModule(void){ return pyPickleFuncTableModule_; }
	static PyObject* getUnpickleFunc(const char* funcName);

	static void registerUnpickleFunc(PyObject* pyFunc, const char* funcName);

private:
	static PyObject* picklerMethod_;						// cPicket.dumps����ָ��
	static PyObject* unPicklerMethod_;						// cPicket.loads����ָ��

	static PyObject* pyPickleFuncTableModule_;				// unpickle������ģ����� �����Զ������unpickle��������Ҫ�ڴ�ע��

	static bool	isInit;										// �Ƿ��Ѿ�����ʼ��
} ;

}
}

#endif // KBE_SCRIPT_PICKLER_H
