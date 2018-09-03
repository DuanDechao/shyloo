/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2017 KBEngine.

KBEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

KBEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef KBE_PY_MEMORYSTREAM_H
#define KBE_PY_MEMORYSTREAM_H

#include "scriptobject.h"
#include "slmemorystream.h"

namespace sl{ namespace pyscript{

class PyMemoryStream : public ScriptObject
{		
	/** ���໯ ��һЩpy�������������� */
	INSTANCE_SCRIPT_HREADER(PyMemoryStream, ScriptObject)
public:	
	static PySequenceMethods seqMethods;

	PyMemoryStream(bool readonly = false);
	PyMemoryStream(std::string& strInitData, bool readonly = false);
	PyMemoryStream(PyObject* pyBytesInitData, bool readonly = false);
	PyMemoryStream(MemoryStream* streamInitData, bool readonly = false);

	PyMemoryStream(PyTypeObject* pyType, bool isInitialised = false, bool readonly = false);
	virtual ~PyMemoryStream();


	/**
	֧��pickler ����
	*/
	static PyObject* __py_reduce_ex__(PyObject* self, PyObject* protocol);

	/**
	unpickle����
	*/
	static PyObject* __unpickle__(PyObject* self, PyObject* args);

	/**
	�ű�����װʱ������
	*/
	static void onInstallScript(PyObject* mod);

	static PyObject* py_new();

	/**
	��ʼ���̶��ֵ�
	*/
	void initialize(std::string strDictInitData);
	void initialize(PyObject* pyDictInitData);
	void initialize(MemoryStream* streamInitData);

	inline MemoryStream& stream(){return stream_;}
	inline PyObject* pyBytes(){
		if(stream_.size() == 0){
			return PyBytes_FromString("");
		}
	
		return PyBytes_FromStringAndSize((char*)stream_.data(), stream_.size());
	}
	inline int length(void) const{ return (int)stream_.size();}
	inline bool readonly() const{return readonly_; }


	void addToStream(MemoryStream* mstream);

	void createFromStream(MemoryStream* mstream);

	/** 
		��ö�������� 
	*/
	PyObject* tp_repr();
	PyObject* tp_str();

	static PyObject* __py_append(PyObject* self, PyObject* args, PyObject* kwargs);	
	static PyObject* __py_pop(PyObject* self, PyObject* args, PyObject* kwargs);	
	
	static Py_ssize_t seq_length(PyObject* self);
	static PyObject* __py_bytes(PyObject* self, PyObject* args, PyObject* kwargs);	
	
	static PyObject* __py_rpos(PyObject* self, PyObject* args, PyObject* kwargs);
	static PyObject* __py_wpos(PyObject* self, PyObject* args, PyObject* kwargs);

	static PyObject* __py_fill(PyObject* self, PyObject* args, PyObject* kwargs);

protected:
	MemoryStream stream_;
	bool readonly_;
} ;

}
}
#endif // KBE_PY_MEMORYSTREAM_H
