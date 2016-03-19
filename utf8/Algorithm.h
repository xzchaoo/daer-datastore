#ifndef XZC_ALGORITHM_H
#define XZC_ALGORITHM_H
//一些常用的算法 或 辅助类


#include <iostream>
#include <string>

namespace xzc{
	using namespace std;
	//用于产生hashcode 下面的相同
	template<typename T>
	struct GetHashCode{
		int operator()(const T& t)const{
			return *(int*)&t;
		}
	};
	

	template<>
	struct GetHashCode<int>{
		int operator()(const int& i)const{
			return i;
		}
	};
	
	template<>
	struct GetHashCode<double>{
		int operator()(const double& d)const{
			long long int bits = (long long int)(&d);
			return (int)(bits ^ (bits >> 32));
		}
	};



	//特化对string的处理
	template<>
	struct GetHashCode< string>{
		int operator()(const string& s)const{
			int h = 0;
			int ilen=s.size();
			for (int i = 0; i < ilen; i++) {
				h = 31 * h + s[i];
			}
			return h;
		}
	};

	//以下代码暂时没用到 这个头文件是以前写的
	template<typename T>
	struct Less{
		int operator()(const T& a,const T& b){
			return a<b?-1:b<a?1:0;
		}
	};
	template<typename T>
	struct LessEqual{
		int operator()(const T& a,const T& b){
			return a<=b?-1:b<=a?1:0;
		}
	};
	template<typename T>
	struct Greater{
		int operator()(const T& a,const T& b){
			return a>b?-1:b>a?1:0;
		}
	};
	template<typename T>
	struct GreaterEqual{
		int operator()(const T& a,const T& b){
			return a>=b?-1:b>=a?1:0;
		}
	};


	//TODO 先假设是按 < 排序
	//返回值如果是负数 比如 -5 则表示 v 这个元素不存在 如果存在的话 那么它的位置应该是 下标为4(即(5-1))的地方
	//如果返回值是-1则表示 如果存在的话 它的位置应该是 1-1=0的地方
	//如果是非负数 那么就是该元素的位置了
	//注意如果该元素重复出现的话 那么返回的位置是它们中的随机的一个
	//可以指定比较器
	template<typename T>
	int binarySearch(T* a,int len, const T& v){
		return binarySearch(a,len,v,Less<T>());
	}

	template<typename T, typename Comparator>
	int binarySearch(T* a, int len, const T& v,Comparator com){
		int left=0;
		int right=len-1;
		while(left<=right){
			int mid=(left+right)>>1;
			int cmp=com(v,a[mid]);
			if(cmp==0)
				return mid;
			if(cmp>0)
				left=mid+1;
			else
				right=mid-1;
		}
		return -(left+1);
	}
}
#endif