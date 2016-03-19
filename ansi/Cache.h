#ifndef XZC_CACHE_H
#define XZC_CACHE_H

//ʵ����һ������LRU�Ļ���
//�õ������ݽṹ��
//map heap

#include <vector>
#include <fstream>
#include "HashMap.h"
#include "BTreeMap.h"
#include "Utils.h"

namespace xzc{
	using namespace std;

//�򵥵Ļ���
class Cache{
	//һ��Page
	struct Page{
		int formatOffset;//��������
		char* data;//�������
		int flag;//���Ϊ1��ʾ�Ǵ�Ӳ������� TODO
		int lastTime;//ʱ���
	};
	static const int DEFAULT_MAX_CACHE=1024;
	static const int DEFAULT_PAGE_SIZE=4096;
public:
	Cache(){
		setMaxCacheAndPageSize(DEFAULT_MAX_CACHE,DEFAULT_PAGE_SIZE);
	}
	void setMaxCacheAndPageSize(int maxCache,int pageSize){
		m_maxCache=maxCache;
		m_pageSize=pageSize;
		m_time=0;
		m_freePage=NULL;
		m_heap.reserve(maxCache);
		m_pos.reserve(maxCache);
		m_vector.reserve(maxCache);
		m_vectorOffsetMap.setTongSize(maxCache*4/3);
	}
	//���ļ�
	void open(string filename){
		Utils::ensureFileExists(filename);
		m_fin.open(filename.c_str(),ios::in|ios::binary);
		m_fout.open(filename.c_str(),ios::in|ios::out|ios::binary);
		_evictAll(false);
	}
	void close(){
		if(m_fout.is_open()){
			_evictAll(true);
			m_fout.flush();
			m_fout.close();
			m_fin.close();
		}
	}
	void read(int offset,int length,char* data){
		//ҳ����
		int formatOffset=offset/m_pageSize*m_pageSize;
		//�����ҳ�����
		for(int i=0;length>0;++i){
			Page* p=_getPage(formatOffset);
			//�����if else ���Կ��Ǻϲ�һ��
			if(i==0){
				int firstReadLength=min(m_pageSize-(offset-formatOffset),length);
				_copy(p->data+offset-formatOffset,firstReadLength,data);
				length-=firstReadLength;
				data+=firstReadLength;
			}else{
				int readLength=min(m_pageSize,length);
				_copy(p->data,readLength,data);
				length-=readLength;
				data+=readLength;
			}
			formatOffset+=m_pageSize;
		}
	}
	//��read�Ĵ���ǳ�����
	void write(int offset,int length,char* data){
		//����
		int formatOffset=offset/m_pageSize*m_pageSize;
		for(int i=0;length>0;++i){
			Page* p=_getPage(formatOffset);
			//���ϱ�� ˵�����޸Ĺ�
			p->flag=2;
			if(i==0){
				int firstWriteLength=min(m_pageSize-(offset-formatOffset),length);
				_copy(data,firstWriteLength,p->data+offset-formatOffset);
				data+=firstWriteLength;
				length-=firstWriteLength;
			}else{
				int writeLength=min(m_pageSize,length);
				_copy(data,writeLength,p->data);
				data+=writeLength;
				length-=writeLength;
			}
			formatOffset+=m_pageSize;
		}
	}
	~Cache(){
		close();
	}
	//��� ��������Page ����д��Ӳ��
	void clear(){
		_evictAll(false);
	}
	void tempClose(){
		m_fin.close();
		m_fout.close();
	}
	void tempOpen(string filename){
		m_fin.open(filename.c_str(),ios::in|ios::binary);
		m_fout.open(filename.c_str(),ios::in|ios::out|ios::binary);
	}
	void swap(Cache& o){
		m_heap.swap(o.m_heap);
		m_pos.swap(o.m_heap);
		m_vectorOffsetMap.swap(o.m_vectorOffsetMap);
		m_vector.swap(o.m_vector);
		Utils::swap(m_time,o.m_time);
		Utils::swap(m_maxCache,o.m_maxCache);
		Utils::swap(m_pageSize,o.m_pageSize);
		Utils::swap(m_freePage,o.m_freePage);
	}
private:
	//��֤�õ�һ�����õ�Page
	//���Ǵӻ�������
	//�Ҳ����Ļ���ȥӲ�̶�
	Page* _getPage(int formatOffset){
		Page* p=_findPageInCache(formatOffset);
		if(p==NULL){
	//		cout<<"��Ӳ�̶�ȡҳ ["<<formatOffset<<","<<formatOffset+m_pageSize<<")"<<endl;
			p=_readPageFromDisk(formatOffset);
		}else{
	//		cout<<"�ڻ������ҵ�ҳ ["<<formatOffset<<","<<formatOffset+m_pageSize<<")"<<endl;
		}
		return p;
	}
	//��Ӳ��ָ��λ�ö�һ��ҳ
	Page* _readPageFromDisk(int formatOffset){
		Page* p;
		if(m_freePage!=NULL){
			p=m_freePage;
			m_freePage=NULL;
		}else{
			p=new Page;
			p->data=new char[m_pageSize];
		}
		p->flag=1;//���ϱ��
		p->formatOffset=formatOffset;
		//��ˢ�ٶ�
		m_fout.flush();
		m_fin.clear(ios::goodbit);
		m_fin.seekg(formatOffset,ios::beg);
		m_fin.read(p->data,m_pageSize);
		_insertPageIntoCache(formatOffset,p);
		return p;
	}
	//дһ��ҳ
	void _writePageToDisk(Page* p){
		m_fout.seekp(p->formatOffset,ios::beg);
		m_fout.write(p->data,m_pageSize);
	}
	//��������Page ������evictToDisk�ѱ��޸Ĺ���д�뵽Ӳ����
	void _evictAll(bool evictToDisk){
		int ilen=m_vector.size();
		for(int i=0;i<ilen;++i){
			Page* p=m_vector[i];
			if(evictToDisk&&p->flag==2){
				_writePageToDisk(p);
			}
			delete[] p->data;
			delete p;
		}
		if(m_freePage){
			delete[] m_freePage->data;
			delete m_freePage;
			m_freePage=NULL;
		}
		m_vector.clear();
		m_heap.clear();
		m_pos.clear();
		m_vectorOffsetMap.clear();
		m_time=0;
	}
	//�ڻ�������Page
	Page* _findPageInCache(int formatOffset){
		int index;
		if(m_vectorOffsetMap.get(formatOffset,&index)){
			Page* p=m_vector[index];
			p->lastTime=_nextTime();
			_down(m_pos[index]);
			return p;
		}
		return NULL;
	}
	void _evictAndInsert(Page* p){
		//int index=m_heap[0];
		int index=(rand()*rand())&(m_maxCache-1);
		m_freePage=m_vector[index];
		m_vectorOffsetMap.remove(m_freePage->formatOffset);
		if(m_freePage->flag==2){
			_writePageToDisk(m_freePage);
		}
		m_vector[index]=p;
		m_vectorOffsetMap.put(p->formatOffset,index);
	}
	void _down(int x){return;
		int total=m_heap.size();
		if(total<=1)
			return;
		int oldKey=m_heap[x];
		int t=(x<<1)+1;
		while(t<total){
			if(t+1<total&&m_vector[m_heap[t+1]]->lastTime<m_vector[m_heap[t]]->lastTime)
				++t;
			if(m_vector[m_heap[t]]->lastTime<m_vector[oldKey]->lastTime){
				m_heap[x]=m_heap[t];
				m_pos[m_heap[t]]=x;
				x=t;
				t=(t<<1)+1;
			}else break;
		}
		m_heap[x]=oldKey;
		m_pos[oldKey]=x;

	}
	//����һ��ҳ������
	void _insertPageIntoCache(int formatOffset, Page* p){
		p->lastTime=p->lastTime=_nextTime();
		if(m_vector.size()==m_maxCache){
			_evictAndInsert(p);
		}else{
			m_pos.push_back(m_heap.size());
			m_heap.push_back(m_vector.size());
			m_vectorOffsetMap.put(p->formatOffset,m_vector.size());
			m_vector.push_back(p);
		}
	//	cout<<"�ڻ������ҳ ["<<formatOffset<<","<<formatOffset+m_pageSize<<")"<<endl;
	}

	int _nextTime(){
		return ++m_time;
	}
	void _copy(char* from, int length,char* to){
		for(int i=0;i<length;++i)
			*to++=*from++;
	}



	vector<int> m_heap;
	vector<int> m_pos;

	HashMap<int,int> m_vectorOffsetMap;
	//BTreeMap<int,int> m_vectorOffsetMap;

	vector<Page*> m_vector;
	int m_time;
	ifstream m_fin;
	ofstream m_fout;
	int m_maxCache;
	int m_pageSize;
	Page* m_freePage;
};

}
#endif
