#ifndef XZC_EXCEPTION_H_
#define XZC_EXCEPTION_H_

//�򵥵��쳣��

#include <string>

namespace xzc{
	using namespace std;
	//һ���򵥵��쳣��
class Exception {
public:
	Exception() {
	}
	Exception(const string& msg):m_msg(msg) {
	}
	string getMessage() const {
		return m_msg;
	}
private:
	string m_msg;
};


}
#endif /* EXCEPTION_H_ */
