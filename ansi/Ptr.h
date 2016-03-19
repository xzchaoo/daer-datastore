#ifndef XZC_PTR_H_
#define XZC_PTR_H_

//�ṩһ���򵥵�����ָ��

#include "Exception.h"

namespace xzc{
	using namespace std;

//һ���򵥵�����ָ��
//��ȫ��
template<typename T>
class ptr{
	//��Ԫ���� ע����U
	template<typename U>
	friend class ptr;
public:
	//Ĭ�Ϲ��캯��
	ptr() :
			m_t(0), m_use(new int(1)), m_isArray(false),m_length(0) {
	}
	//����һ��ָ�� Ĭ�ϲ�������
	ptr(T* t) :
			m_t(t), m_use(new int(1)), m_isArray(false),m_length(1) {
	}
	//����һ��ָ��, ָʾ�ǲ�������
	ptr(T* t, int len) :
			m_t(t), m_use(new int(1)), m_isArray(true),m_length(len) {
	}
	//���ƹ��캯�� ע����U
	template<typename U>
	ptr(const ptr<U>& o) :
			m_t(o.m_t), m_use(o.m_use), m_isArray(o.m_isArray),m_length(o.m_length) {
		++*m_use;
	}
	ptr(const ptr<T>& o) :
			m_t(o.m_t), m_use(o.m_use), m_isArray(o.m_isArray),m_length(o.m_length) {
		++*m_use;
	}
	//��ֵ
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
	//���� *  -> []
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
	//����== !=
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
	//���������Ӧ�ĳ���
	int getLength()const{
		return m_length;
	}
	//�Ƿ�ΪNULL
	bool isNull()const{
		return m_t==0;
	}
	//dynamic_cast����ת��
	template<typename U>
	ptr<U> as(){
		U* u=dynamic_cast<U*>(m_t);
		if(u==0&&m_t!=0){
			throw Exception("ClassCastException");
		}
		return ptr<U>(u,m_use,m_isArray,m_length);
	}
	//ǿ���Ե�����ת�� ������Σ��
	template<typename U>
	ptr<U> forceAs(){
		return ptr<U>((U*)m_t,m_use,m_isArray,m_length);
	}
private:
	//���ڸ�������ת��ʱ�õĹ��캯��
	ptr(T* t, int* use, bool isArray, int length):m_t(t),m_use(use),m_isArray(isArray),m_length(length){
		++*m_use;
	}
	//���ƹ��캯���͸�ֵ����ʱ�õ� ���������ֶ�
	template<typename U>
	void copyField(const ptr<U>& o){
		this->m_t =o.m_t;
		this->m_use = o.m_use;
		this->m_isArray = o.m_isArray;
		this->m_length=o.m_length;
	}
	//����use �� ������� delete��Ӧ������
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
