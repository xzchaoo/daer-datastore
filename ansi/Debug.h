#ifndef XZC_DEBUG_H
#define XZC_DEBUG_H

//用于调试 计时 对象个数统计

#include <iostream>
#include <string>


namespace xzc{
	using namespace std;
//简单封装了 cout
struct Xout{
	Xout(string tag,bool b,ostream& out):m_tag(tag),m_show(b),m_out(out){}
	template<typename T>
	Xout& operator<<(const T& t){
		if(this->m_show)
			m_out<<t;
		return *this;
	}
	Xout& operator<<(ostream& (*__pf)(ostream&)){
		if(this->m_show)
			__pf(m_out);
		return*this;
	}
	ostream& m_out;
	bool m_show;
	string m_tag;
};

//对应3个输出
extern Xout info;
extern Xout warn;
extern Xout debug;


//用于统计 子类用private继承它 可以起到统计对象个数的作用
//debug
class CountObject{
public:
	CountObject(){++objectCount;}
	CountObject(const CountObject&){++objectCount;}
	virtual ~CountObject(){--objectCount;}
	static int getObjectCount(){
		return objectCount;
	}
private:
	static int objectCount;
};

//用于计时
void timeBegin();
long timeEnd();
long timeEnd(string msg);

}
#endif