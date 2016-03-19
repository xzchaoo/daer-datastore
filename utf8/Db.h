#ifndef XZC_DB_H_
#define XZC_DB_H_

//定义了数据库的基本操作

#include <string>
#include <vector>

#include "Ptr.h"
#include "HashMap.h"
#include "STLMap.h"
#include "Cache.h"
#include "Types.h"
#include "Utils.h"
#include "Debug.h"
#include "Assert.h"
#include "IO.h"
#include <algorithm>

namespace xzc{
	using namespace std;

class Database {
	struct IndexEntry{
		int offset;
		int length;
	};
public:
	Database(string basename){
		m_maxCache=1024;
		m_pageSize=4096;
		_init(basename);
	}

	Database(string basename,int maxCache, int pageSize){
		m_maxCache=maxCache;
		m_pageSize=pageSize;
		_init(basename);
	}
	bool contains(const string& key){
		return m_indexMap.contains(key);
	}
	ptr<Variant> get(const string& key){
		ptr<IndexEntry> pe=m_indexMap.get(key);
		if(pe.isNull()){
			return 0;
		}
		ptr<byte> pb(new byte[pe->length],pe->length);
		m_cache.read(pe->offset,pe->length,(char*)&*pb);
		return new Variant(pb);
	}
	bool remove(const string& key){
		return m_indexMap.remove(key);
	}
	void set(const string& key, ptr<Variant> value){

		ptr<IndexEntry> pe = m_indexMap.get(key);

		bool flag=false;
		//即要设置的key已经存在了 这里先删除 再添加
		if (!pe.isNull()) {
			flag=true;
		}else{
			pe=new IndexEntry;
		}
		ptr<byte> pb=value->get();

		pe->offset=m_fileLength;
		pe->length=pb.getLength();

		m_cache.write(m_fileLength,pe->length,(char*)&*pb);
		m_fileLength+=pe->length;

		if(!flag){
			m_indexMap.put(key,pe);
		}
	}
	void getAllKeys(vector<string>& keys){
		return m_indexMap.getKeySet(keys);
	}
	void sortShrink(){
		vector<string> keyset;
		vector<ptr<IndexEntry> > valueset;
		m_indexMap.getKeyAndValueSet(keyset,valueset);
		vector<tempStruct> vts;
		vts.reserve(keyset.size());
		for(int i=0;i<keyset.size();++i){
			tempStruct ts(keyset,i);
			vts.push_back(ts);
		}
		sort(vts.begin(),vts.end());
		Cache tempCache;
		tempCache.setMaxCacheAndPageSize(m_maxCache,m_pageSize);
		tempCache.open("temp");
		int filePosition=0;

		const int BUF_SIZE=4096;
		char buf[BUF_SIZE];

		int ilen=keyset.size();
		for(int i=0;i<ilen;++i){
			string key=keyset[vts[i].m_index];
			ptr<IndexEntry> pe=valueset[vts[i].m_index];
			int offset=pe->offset;
			int length=pe->length;

			pe->offset=filePosition;
			while(length>0){
				int len=min(length,BUF_SIZE);
				m_cache.read(offset,len,buf);
				tempCache.write(filePosition,len,buf);
				length-=len;
				filePosition+=len;
			}
		}
		m_cache.tempClose();
		tempCache.tempClose();
		m_cache.swap(tempCache);

		::remove(m_dataFileName.c_str());
		::rename("temp",m_dataFileName.c_str());

		m_cache.tempOpen(m_dataFileName);

	}
	void shrink(){
		vector<string> keyset;
		vector<ptr<IndexEntry> > valueset;
		m_indexMap.getKeyAndValueSet(keyset,valueset);
		
		Cache tempCache;
		tempCache.setMaxCacheAndPageSize(m_maxCache,m_pageSize);
		tempCache.open("temp");
		int filePosition=0;

		const int BUF_SIZE=4096;
		char buf[BUF_SIZE];

		int ilen=keyset.size();
		for(int i=0;i<ilen;++i){
			string key=keyset[i];
			ptr<IndexEntry> pe=valueset[i];
			int offset=pe->offset;
			int length=pe->length;

			pe->offset=filePosition;
			while(length>0){
				int len=min(length,BUF_SIZE);
				m_cache.read(offset,len,buf);
				tempCache.write(filePosition,len,buf);
				length-=len;
				filePosition+=len;
			}
		}
		m_cache.tempClose();
		tempCache.tempClose();
		m_cache.swap(tempCache);

		::remove(m_dataFileName.c_str());
		::rename("temp",m_dataFileName.c_str());

		m_cache.tempOpen(m_dataFileName);
	}
	void clear(){
		m_indexMap.clear();
		m_cache.clear();
		m_cache.close();
		Utils::createEmptyFile(m_dataFileName);
		m_fileLength=0;
		m_cache.open(m_dataFileName);
	}
	~Database(){
		Writer w(m_indexFileName,false);
		w.writeT(m_indexMap.size());
		vector<string> keyset;
		vector<ptr<IndexEntry> > valueset;
		m_indexMap.getKeyAndValueSet(keyset,valueset);
		int ilen=keyset.size();
		for(int i=0;i<ilen;++i){
			string key=keyset[i];
			ptr<IndexEntry> value=valueset[i];
			w.writeT(key).writeT(value->length).writeT(value->offset);
		}
		m_indexMap.clear();
	}

private:
	struct tempStruct{
		tempStruct(const vector<string>& keyset, int index):m_keyset(keyset),m_index(index){}
		tempStruct& operator=(const tempStruct& o){
			m_index=o.m_index;
			return *this;
		}
		bool operator<(const tempStruct& o)const{
			return m_keyset[m_index]<m_keyset[o.m_index];
		}
		const vector<string>& m_keyset;
		int m_index;
	};


	void _initIndex(){
		if (!Utils::isFileExists(m_indexFileName.c_str())) {
			Utils::createEmptyFile(m_indexFileName.c_str());
		}else{
			Reader r(m_indexFileName);
			try {
				int ilen;
				r.nextT(ilen);
				for(int i=0;i<ilen;++i){
					ptr<IndexEntry> pe=new IndexEntry;
					string key;
					r.nextT(key).nextT(pe->length).nextT(pe->offset);
					m_indexMap.put(key,pe);
				}
			} catch (Exception& ex) {
				warn << ex.getMessage() << endl;
			}
		}
	}
	void _init(string basename){
		m_fileLength=0;
		m_indexFileName=basename+".key";
		m_dataFileName=basename+".data";
		m_cache.setMaxCacheAndPageSize(m_maxCache,m_pageSize);
		m_cache.open(m_dataFileName);
		_initIndex();
	}
	HashMap<string,ptr<IndexEntry> > m_indexMap;
	//RBTreeMap<string,ptr<t1IndexEntry> > m_indexMap;
	//BTreeMap<string,ptr<IndexEntry> > m_indexMap;
	//STLMap<string,ptr<IndexEntry> > m_indexMap;

	string m_dataFileName;
	string m_indexFileName;
	Cache m_cache;
	int m_fileLength;
	int m_maxCache;
	int m_pageSize;
};


}
#endif /* DB_H_ */
