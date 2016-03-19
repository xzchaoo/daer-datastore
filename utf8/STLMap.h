#ifndef XZC_STLMAP_H
#define XZC_STLMAP_H

//将stl的map进行包装

#include <map>
#include "IMap.h"

namespace xzc{
	using namespace std;

	template<typename KeyType,typename ValueType>
	class STLMap:public IMap<KeyType,ValueType>{
		typedef typename map<KeyType,ValueType>::iterator ITER;
	public:
		virtual ValueType get( const KeyType& key ) {
			ITER i=m_stlMap.find(key);
			if(i==m_stlMap.end())
				return ValueType();
			return i->second;
		}

		virtual void put( const KeyType& key, const ValueType& value ) {
			ITER i=m_stlMap.find(key);
			if(i==m_stlMap.end()){
				m_stlMap.insert(make_pair(key,value));
			}else{
				i->second=value;
			}
		}

		virtual bool remove( const KeyType& key ) {
			ITER i=m_stlMap.find(key);
			if(i==m_stlMap.end()){
				return false;
			}else{
				m_stlMap.erase(i);
				return true;
			}
		}

		virtual bool contains( const KeyType& key ){
			return m_stlMap.count(key)==1;
		}

		virtual int size() {
			return m_stlMap.size();
		}
		virtual void clear(){
			m_stlMap.clear();
		}

		virtual void getKeySet( vector<KeyType>& keys ) {
			keys.clear();
			keys.reserve(size());
			for(typename map<KeyType,ValueType>::const_iterator cit=m_stlMap.begin();cit!=m_stlMap.end();++cit)
				keys.push_back(cit->first);
		}

		virtual void getKeyAndValueSet( vector<KeyType>& keys, vector<ValueType>& values ) {
			keys.clear();
			values.clear();
			keys.reserve(size());
			values.reserve(size());
			for(typename map<KeyType,ValueType>::const_iterator cit=m_stlMap.begin();cit!=m_stlMap.end();++cit){
				keys.push_back(cit->first);
				values.push_back(cit->second);
			}
		}

	private:
		map<KeyType,ValueType> m_stlMap;
	};



}

#endif
