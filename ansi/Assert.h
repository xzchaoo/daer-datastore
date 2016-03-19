#ifndef XZC_ASSERT_H
#define XZC_ASSERT_H
//���ڶ���

#include <string>
#include <iostream>
#include <sstream>

#include "Exception.h"

namespace xzc{
	using namespace std;
//�򵥵Ķ��� ����Ⱦ��ӳ��쳣���ӡ�ڿ���̨
template<typename T,typename U>
void _assertEquals(const string& file, int line, const string& msg,const T& a,const U& b,bool printAtConsole){
	if(!(a==b)){
		ostringstream oss;
		oss<<"assert fail at: file:"<<file<<" line:"<<line<<" msg:"<<msg;
		if(printAtConsole){
			cerr<<oss.str()<<endl;
			exit(0);
		}
		throw Exception(oss.str());
	}
}

//�� ������� �ļ��ļ����к�
#define assertEquals(a,b) _assertEquals(__FILE__,__LINE__,"",a,b,true)
#define assertEquals3(a,b,msg) _assertEquals(__FILE__,__LINE__,msg,a,b,true)
#define assertEquals3_throw(a,b,msg)  _assertEquals(__FILE__,__LINE__,msg,a,b,false)

}
#endif