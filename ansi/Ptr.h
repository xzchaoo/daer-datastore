#ifndef XZC_PTR_H_
#define XZC_PTR_H_

//提供一个简单的智能指针

#include "Exception.h"

namespace xzc{
	using namespace std;

//一个简单的智能指针
//安全性
template<typename T>
class ptr{
	//友元声明 注意是U
	template<typename U>
	friend class ptr;
public:
	//默认构造函数
	ptr() :
			m_t(0), m_use(new int(1)), m_isArray(false),m_length(0) {
	}
	//接受一个指针 默认不是数组
	ptr(T* t) :
			m_t(t), m_use(new int(1)), m_isArray(false),m_length(1) {
	}
	//接受一个指针, 指示是不是数组
	ptr(T* t, int len) :
			m_t(t), m_use(new int(1)), m_isArray(true),m_length(len) {
	}
	//复制构造函数 注意是U
	template<typename U>
	ptr(const ptr<U>& o) :
			m_t(o.m_t), m_use(o.m_use), m_isArray(o.m_isArray),m_length(o.m_length) {
		++*m_use;
	}
	ptr(const ptr<T>& o) :
			m_t(o.m_t), m_use(o.m_use), m_isArray(o.m_isArray),m_length(o.m_length) {
		++*m_use;
	}
	//赋值
	template<typename U>
	ptr<T>& operator=(const ptr<U>&o) {
		if ((void*)this == (void*)&o)
			return *this;
		dec();
		copyField(o);
		++*m_use;
		return *this;
	}
	ptr<T>& operator=(const ptr<T>&o) {
		if (this == &o)
			return *this;
		dec();
		copyField(o);
		++*m_use;
		return *this;
	}
	//重载 *  -> []
	T& operator*() {
		return *m_t;
	}
	const T& operator*() const {
		return *m_t;
	}
	T* operator->() {
		return m_t;
	}
	const T* operator->() const {
		return m_t;
	}
	T& operator[](int index) {
		return m_t[index];
	}
	const T& operator[](int index) const {
		return m_t[index];
	}
	//重载== !=
	template<typename U>
	bool operator==(const ptr<U>& o) const{
		return (void*)m_t == (void*)o.m_t;// && m_use == o.m_use;
	}
	template<typename U>
	bool operator!=(const ptr<U>& o) const{
		return !(*this==o);
	}
	~ptr() {
		dec();
	}
	//返回数组对应的长度
	int getLength()const{
		return m_length;
	}
	//是否为NULL
	bool isNull()const{
		return m_t==0;
	}
	//dynamic_cast类型转换
	template<typename U>
	ptr<U> as(){
		U* u=dynamic_cast<U*>(m_t);
		if(u==0&&m_t!=0){
			throw Exception("ClassCastException");
		}
		return ptr<U>(u,m_use,m_isArray,m_length);
	}
	//强制性的类型转换 可能有危险
	template<typename U>
	ptr<U> forceAs(){
		return ptr<U>((U*)m_t,m_use,m_isArray,m_length);
	}
private:
	//用于辅助类型转换时用的构造函数
	ptr(T* t, int* use, bool isArray, int length):m_t(t),m_use(use),m_isArray(isArray),m_length(length){
		++*m_use;
	}
	//复制构造函数和赋值函数时用的 用来复制字段
	template<typename U>
	void copyField(const ptr<U>& o){
		this->m_t =o.m_t;
		this->m_use = o.m_use;
		this->m_isArray = o.m_isArray;
		this->m_length=o.m_length;
	}
	//减少use 并 根据情况 delete对应的数据
	void dec() {
		if (--*m_use == 0) {
			delete m_use;
			if (m_isArray)
				delete[] m_t;
			else
				delete m_t;
		}
	}
	T* m_t;
	int* m_use;
	bool m_isArray;
	int m_length;
};



}
#endif /* PTR_H_ */
