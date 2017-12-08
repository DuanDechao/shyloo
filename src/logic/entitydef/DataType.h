#ifndef SL_LOGIC_ENTITY_DEF_DATA_TYPE_H
#define SL_LOGIC_ENTITY_DEF_DATA_TYPE_H
#include "slmulti_sys.h"
#include "slscript_object.h"
typedef uint8 DATATYPE_UID;
class DataType{
public:
	DataType(DATATYPE_UID uid = 0);
	virtual ~DataType(){}

	virtual bool isSameType(PyObject* pyValue) = 0;

	inline DATATYPE_UID id() const { return _id; }

private:
	DATATYPE_UID	_id;
};
#endif
