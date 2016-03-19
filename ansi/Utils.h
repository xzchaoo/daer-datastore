#ifndef XZC_UTILS_H_
#define XZC_UTILS_H_

//提供一些 工具函数 比如随机数 随机字符串之类

#include <fstream>
#include <string>
#include <sstream>

#include "Ptr.h"

namespace xzc{
	using namespace std;
//工具类
class Utils {
public:
	template<typename T>
	static void swap(T& a, T& b){
		T t(a);
		a=b;
		b=t;
	}
	//文件是否存在
	static bool isFileExists(string path) {
		ifstream fin(path.c_str(), ios::in);
		bool ret = fin.is_open();
		fin.close();
		return ret;
	}
	//创建一个空文件 如果已经存在也会被截成空的
	//返回操作是否成功
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

	//检查是否空指针 配合 宏 使用
	template<typename T>
	static void checkNullPointException(ptr<T> p,const string& fileName, int lineNo){
		if(p.isNull()){
			ostringstream oss;
			oss<<fileName<<" "<<lineNo<<": NullPointerException.";
			throw Exception(oss.str());
		}
	}
	//配合宏使用 用于扔出一个异常 会加上抛出地点的文件名和行号
	static void throwException(string msg,const string& fileName, int lineNo){
		ostringstream oss;
		oss<<fileName<<" "<<lineNo<<": "<<msg<<endl;
		throw Exception(oss.str());
	}
	//产生随机数
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
	//产生随机字符串
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
