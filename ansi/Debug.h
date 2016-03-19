#ifndef XZC_DEBUG_H
#define XZC_DEBUG_H

//���ڵ��� ��ʱ �������ͳ��

#include <iostream>
#include <string>


namespace xzc{
	using namespace std;
//�򵥷�װ�� cout
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

//��Ӧ3�����
extern Xout info;
extern Xout warn;
extern Xout debug;


//����ͳ�� ������private�̳��� ������ͳ�ƶ������������
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

//���ڼ�ʱ
void timeBegin();
long timeEnd();
long timeEnd(string msg);

}
#endif