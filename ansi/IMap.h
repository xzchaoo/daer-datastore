#ifndef XZC_IMAP_H
#define XZC_IMAP_H

//�Զ����Map�Ľӿ�

#include <vector>

namespace xzc{
	using namespace std;

template<typename KeyType, typename ValueType>
class IMap{
public:
	virtual ValueType get(const KeyType& key)=0;
	virtual void put(const KeyType& key, const ValueType& value)=0;
	virtual bool remove(const KeyType& key)=0;
	virtual bool contains(const KeyType& key)=0;
	virtual void getKeySet(vector<KeyType>& keys)=0;
	virtual void getKeyAndValueSet(vector<KeyType>& keys, vector<ValueType>& values)=0;
	virtual int size()=0;
	virtual void clear()=0;
	virtual ~IMap(){}
};


}
#endif