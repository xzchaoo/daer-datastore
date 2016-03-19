#ifndef XZC_ASSERT_H
#define XZC_ASSERT_H
//用于断言

#include <string>
#include <iostream>
#include <sstream>

#include "Exception.h"

namespace xzc{
	using namespace std;
//简单的断言 不相等就扔出异常或打印在控制台
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

//宏 用来添加 文件文件和行号
#define assertEquals(a,b) _assertEquals(__FILE__,__LINE__,"",a,b,true)
#define assertEquals3(a,b,msg) _assertEquals(__FILE__,__LINE__,msg,a,b,true)
#define assertEquals3_throw(a,b,msg)  _assertEquals(__FILE__,__LINE__,msg,a,b,false)

}
#endif