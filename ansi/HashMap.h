#ifndef XZC_HASHMAP_H
#define XZC_HASHMAP_H

//����HASH��Map
//���õķ����ǿ� T��Ͱ
//Ȼ��ÿ��Ͱ�ŵ���һ������
#include <vector>
#include "IMap.h"
#include "Algorithm.h"
#include "Utils.h"


namespace xzc{
	using namespace std;

	template<typename KeyType, typename ValueType>
	class HashMap: public IMap<KeyType,ValueType>{
		//Ĭ�ϵ�Ͱ����
		static const int DEFAULT_HASHMAP_TONG_SIZE=1048576;
		//hash��һ����Ŀ
		struct Entry{
			KeyType key;
			ValueType value;
			int hashCode;
			Entry* next;
			Entry(const KeyType&k, const ValueType&v, int hashCode):key(k),value(v),hashCode(hashCode),next(NULL){}
			Entry(){}
		};	
	public:
		HashMap(int tongSize){
			_init(tongSize);
		}
		HashMap(){
			_init(DEFAULT_HASHMAP_TONG_SIZE);
		}
		void setTongSize(int tongSize){
			int t=1;
			while(t<tongSize)
				t<<=1;
			clear();
			_init(t);
		}
		//������ڷ���true ���� ��ֵд�� pvalue
		bool get(const KeyType& key, ValueType* pvalue){
			Entry* e=_findEntry(key,0,0);
			if(e){
				if(pvalue)
					*pvalue=e->value;
				return true;
			}
			return false;
		}
		//������ھͷ�����ֵ ������Ĭ�ϵĹ��캯��������ֵ
		ValueType get(const KeyType& key){
			Entry* e=_findEntry(key,0,0);
			return e?e->value:ValueType();
		}
		//���Ԫ��
		//���keyԭ���Ѿ����ھ��滻������value
		//����ͼ����µ�Entry
		void put(const KeyType& key, const ValueType& value){
			int h;
			int tong;
			Entry* e=_findEntry(key,&h,&tong);
			if(e==NULL){
				e=new Entry(key,value,h);
				e->next=m_vector[tong];
				m_vector[tong]=e;
				++m_size;
			}else{
				e->value=value;
			}
		}

		int size(){
			return m_size;
		}

		//�ҵ���Ӧ��Entry Ȼ���������ɾ����
		bool remove(const KeyType& key){
			int h=_getHashCode(key);
			int tong=_getTong(h);
			Entry* e=m_vector[tong];
			Entry* p=NULL;
			while(e){
				if(h==e->hashCode&&e->key==key){
					if(p==NULL){
						m_vector[tong]=e->next;
					}else{
						p->next=e->next;
					}
					--m_size;
					delete e;
					return true;
				}
				p=e;
				e=e->next;
			}
			return false;
		}

		//������нڵ�
		void clear(){
			m_size=0;
			for(int i=0;i<m_tongSize;++i){
				Entry* e=m_vector[i];
				while(e){
					Entry* ee=e;
					e=e->next;
					delete ee;
				}
				m_vector[i]=NULL;
			}
		}

		//�Ƿ����
		bool contains(const KeyType& key){
			return _findEntry(key,NULL,NULL)!=NULL;
			/*int h=_getHashCode(key);
			int tong=h%m_tongSize;
			return _findInTong(key,h,tong)!=NULL;*/
		}
		
		//��key��value��䵽����vector��
		virtual void getKeyAndValueSet(vector<KeyType>& keys, vector<ValueType>& values){
			keys.clear();
			values.clear();
			keys.reserve(size());
			values.reserve(size());
			for(int t=0;t<m_tongSize;++t){
				Entry* e=m_vector[t];
				while(e){
					keys.push_back(e->key);
					values.push_back(e->value);
					e=e->next;
				}
			}
		}


		virtual void getKeySet(vector<KeyType>& keys) {
			keys.clear();
			keys.reserve(size());
			for(int t=0;t<m_tongSize;++t){
				Entry* e=m_vector[t];
				while(e){
					keys.push_back(e->key);
					e=e->next;
				}
			}
		}	
		void swap(HashMap<KeyType,ValueType>& o){
			Utils::swap(m_tongSize,o.m_tongSize);
			m_vector.swap(o.m_vector);
			Utils::swap(m_size,o.m_size);
		}
	private:
		int _getTong(int h){
			h=h>0?h:-h;
			return h&(m_tongSize-1);
		}
		Entry* _findEntry(const KeyType& key,int* ph,int* ptong){
			int h=_getHashCode(key);
			if(ph)
				*ph=h;
			int tong=_getTong(h);
			if(ptong)
				*ptong=tong;
			Entry* e=_findInTong(key,h,tong);
			return e;
		}

		int _getHashCode(const KeyType& k){
			int h=m_getHashCode(k);
			h ^= (h >> 20) ^ (h >> 12);
			h=h ^ (h >> 7) ^ (h >> 4);
			return h;
		}
		Entry* _findInTong(const KeyType&k, int h, int tong){
			Entry* e=m_vector[tong];
			while(e){
				if(h==e->hashCode&&e->key==k)
					break;
				e=e->next;
			}
			return e;
		}

		void _init(int tongSize){
			m_size=0;
			m_tongSize=tongSize;
			m_vector.clear();
			m_vector.reserve(m_tongSize);
			for(int i=0;i<tongSize;++i)
				m_vector.push_back(NULL);
	//		m_vector.resize(m_tongSize,NULL);
		}

		int m_tongSize;
		vector<Entry*> m_vector;
		int m_size;
		GetHashCode<KeyType> m_getHashCode;
	};

}
#endif