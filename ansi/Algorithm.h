#ifndef XZC_ALGORITHM_H
#define XZC_ALGORITHM_H
//һЩ���õ��㷨 �� ������


#include <iostream>
#include <string>

namespace xzc{
	using namespace std;
	//���ڲ���hashcode �������ͬ
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



	//�ػ���string�Ĵ���
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

	//���´�����ʱû�õ� ���ͷ�ļ�����ǰд��
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


	//TODO �ȼ����ǰ� < ����
	//����ֵ����Ǹ��� ���� -5 ���ʾ v ���Ԫ�ز����� ������ڵĻ� ��ô����λ��Ӧ���� �±�Ϊ4(��(5-1))�ĵط�
	//�������ֵ��-1���ʾ ������ڵĻ� ����λ��Ӧ���� 1-1=0�ĵط�
	//����ǷǸ��� ��ô���Ǹ�Ԫ�ص�λ����
	//ע�������Ԫ���ظ����ֵĻ� ��ô���ص�λ���������е������һ��
	//����ָ���Ƚ���
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