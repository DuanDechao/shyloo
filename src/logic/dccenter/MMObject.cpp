#include "MMObject.h"

bool MMObject::setPropInt8(const int32 prop, const int8 value){
	return m_poProp->setValue(prop, PROP::DTYPE_INT8, (const char*)&value, sizeof(int8));
}
bool MMObject::setPropInt16(const int32 prop, const int16 value){
	return m_poProp->setValue(prop, PROP::DTYPE_INT16, (const char*)&value, sizeof(int16));
}
bool MMObject::setPropInt32(const int32 prop, const int32 value){
	return m_poProp->setValue(prop, PROP::DTYPE_INT32, (const char*)&value, sizeof(int32));
}
bool MMObject::setPropInt64(const int32 prop, const int64 value){
	return m_poProp->setValue(prop, PROP::DTYPE_INT64, (const char*)&value, sizeof(int64));
}
bool MMObject::setPropFloat(const int32 prop, const float value){
	return m_poProp->setValue(prop, PROP::DTYPE_FLOAT, (const char*)&value, sizeof(float));
}
bool MMObject::setPropString(const int32 prop, const char* value){
	int32 size = strlen(value) + 1;
	return m_poProp->setValue(prop, PROP::DTYPE_STRING, value, size);
}

int8 MMObject::getPropInt8(const int32 prop) const{
	return *(int8*)m_poProp->getValue(prop, PROP::DTYPE_INT8);
}
int16 MMObject::getPropInt16(const int32 prop) const{
	return *(int16*)m_poProp->getValue(prop, PROP::DTYPE_INT16);
}
int32 MMObject::getPropInt32(const int32 prop) const{
	return *(int32*)m_poProp->getValue(prop, PROP::DTYPE_INT32);
}
int64 MMObject::getPropInt64(const int32 prop) const{
	return *(int64*)m_poProp->getValue(prop, PROP::DTYPE_INT64);
}
float MMObject::getPropFloat(const int32 prop) const{
	return *(float*)m_poProp->getValue(prop, PROP::DTYPE_FLOAT);
}
const char* MMObject::getPropString(const int32 prop) const{
	return m_poProp->getValue(prop, PROP::DTYPE_STRING);
}
