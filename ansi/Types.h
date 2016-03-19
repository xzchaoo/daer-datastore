#ifndef XZC_TYPES_H_
#define XZC_TYPES_H_

//在这里定义了 byte 和 Variant这个类
#include <string>

#include "Ptr.h"
#include "Debug.h"

namespace xzc{
	using namespace std;

typedef unsigned char byte;

class Variant {
public:
	Variant(bool b){
		m_pb=ptr<byte>(new byte[sizeof(bool)],sizeof(bool));
		*(bool*)&*m_pb=b;
	}
	Variant(byte b){
		m_pb=ptr<byte>(new byte[sizeof(byte)],sizeof(byte));
		*(byte*)&*m_pb=b;
	}
	Variant(char c){
		m_pb=ptr<byte>(new byte[sizeof(char)],sizeof(char));
		*(char*)&*m_pb=c;
	}
	Variant(short s){
		m_pb=ptr<byte>(new byte[sizeof(short)],sizeof(short));
		*(short*)&*m_pb=s;
	}
	Variant(int i){
		m_pb=ptr<byte>(new byte[sizeof(int)],sizeof(int));
		*(int*)&*m_pb=i;
	}
	Variant(float f){
		m_pb=ptr<byte>(new byte[sizeof(float)],sizeof(float));
		*(float*)&*m_pb=f;
	}
	Variant(double d){
		m_pb=ptr<byte>(new byte[sizeof(double)],sizeof(double));
		*(double*)&*m_pb=d;
	}
	Variant(long l){
		m_pb=ptr<byte>(new byte[sizeof(long)],sizeof(long));
		*(long*)&*m_pb=l;
	}
	Variant(const string& s){
		const char* cs=s.c_str();
		int len=s.size();
		m_pb=ptr<byte>(new byte[sizeof(int)+len+1],sizeof(int)+len+1);
		*(int*)&*m_pb=len;
		for(int i=0;i<len;++i)
			m_pb[sizeof(int)+i]=cs[i];
		m_pb[sizeof(int)+len]=0;
	}
	Variant(ptr<byte> pb){
			m_pb=pb;
	}

	virtual ptr<byte> get()const{
		return m_pb;
	}
	virtual bool toBool()const{
			return *(bool*)&*m_pb;
	}
	virtual byte toByte()const{
			return *(byte*)&*m_pb;
	}
	virtual char toChar()const{
			return *(char*)&*m_pb;
	}
	virtual short toShort()const{
			return *(short*)&*m_pb;
	}
	virtual int toInt()const{
			return *(int*)&*m_pb;
	}
	virtual float toFloat()const{
			return *(float*)&*m_pb;
	}
	virtual double toDouble()const{
			return *(double*)&*m_pb;
	}
	virtual long toLong()const{
		return *(long*)&*m_pb;	
	}
	virtual string toString()const{
		int len=*(int*)&*m_pb;
		return string(&*m_pb+sizeof(int),&*m_pb+sizeof(int)+len);
	}

private:
	//用于保存实际的数据
	ptr<byte> m_pb;
};



}
#endif /* TYPES_H_ */


