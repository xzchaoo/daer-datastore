#ifndef XZC_UTILS_H_
#define XZC_UTILS_H_

//�ṩһЩ ���ߺ��� ��������� ����ַ���֮��

#include <fstream>
#include <string>
#include <sstream>

#include "Ptr.h"

namespace xzc{
	using namespace std;
//������
class Utils {
public:
	template<typename T>
	static void swap(T& a, T& b){
		T t(a);
		a=b;
		b=t;
	}
	//�ļ��Ƿ����
	static bool isFileExists(string path) {
		ifstream fin(path.c_str(), ios::in);
		bool ret = fin.is_open();
		fin.close();
		return ret;
	}
	//����һ�����ļ� ����Ѿ�����Ҳ�ᱻ�سɿյ�
	//���ز����Ƿ�ɹ�
	static bool createEmptyFile(string path) {
		ofstream fout(path.c_str(), ios::out);
		bool ret = fout.good();
		fout.close();
		return ret;
	}
	static void ensureFileExists(string path){
		if(!isFileExists(path))
			createEmptyFile(path);
	}
	static int getFileSize(string path){
		ifstream fin(path.c_str(),ios::in|ios::binary);
		fin.seekg(0,ios::end);
		int ret=fin.tellg();
		fin.close();
		return ret;
	}
	template<typename T>
	static string toString(const T& t){
		ostringstream oss;
		oss<<t;
		return oss.str();
	}

	//����Ƿ��ָ�� ��� �� ʹ��
	template<typename T>
	static void checkNullPointException(ptr<T> p,const string& fileName, int lineNo){
		if(p.isNull()){
			ostringstream oss;
			oss<<fileName<<" "<<lineNo<<": NullPointerException.";
			throw Exception(oss.str());
		}
	}
	//��Ϻ�ʹ�� �����ӳ�һ���쳣 ������׳��ص���ļ������к�
	static void throwException(string msg,const string& fileName, int lineNo){
		ostringstream oss;
		oss<<fileName<<" "<<lineNo<<": "<<msg<<endl;
		throw Exception(oss.str());
	}
	//���������
	static int randomInt(){
#if RAND_MAX==0x7fff
	return rand()*rand();
#else
		return rand();
#endif
	}
	static int randomInt(int min, int max){
		if(min>max){
			int t=min;
			min=max;
			max=t;
		}
		return min+( (unsigned int)randomInt() )%(max-min);
	}
	//��������ַ���
	static string randomString(){
		return randomString(1,randomInt(1,10));
	}
	static string randomString(int min,int max){
		int len=randomInt(min,max);
		string ret;
		ret.reserve(len);
		for(int i=0;i<len;++i)
			ret+=(char)randomInt('a','z'+1);
		return ret;
	}
};

//null pointer
#define assertNotNull(a) Utils::checkNullPointException(a,__FILE__,__LINE__)

#define xthrow(msg) Utils::throwException(msg,__FILE__,__LINE__)

#define _T(a) Utils::toString(a)



}
#endif /* UTILS_H_ */
