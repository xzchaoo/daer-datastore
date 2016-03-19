#ifndef XZC_TEST_H
#define XZC_TEST_H

//ר�����ڲ��Ե�ͷ�ļ�

#include <iostream>
#include <algorithm>
#include <ctime>
#include <set>

#include "Ptr.h"
#include "Assert.h"
#include "Debug.h"
#include "Utils.h"
#include "Db.h"

namespace xzc{
	using namespace std;

	void test_ptr();
	void testDb();
	void testCase0();
	void testCase1();
	void testCase2();
	void testCase3();
	void testCase4(int scale,int maxCache, int minLen, int maxLen);

	//runTest
	void runTest(){
		info<<"test all begin..."<<endl;
		test_ptr();
		testDb();
		info<<"test all finish..."<<endl;
		//����Ƿ��ڴ�й¶ �������Ӧ��ȫΪ0
		assertEquals(CountObject::getObjectCount(),0);
	}

	void testDb(){
		testCase0();
		testCase1();
		testCase2();
		testCase3();
		
		//100�������� ������50~100MB 256MB����
		testCase4(1000000,1024*64,50,100);

		//testCase4();
		//int scales[1]={1000000};
		/*
		int maxCaches[4];
		for(int i=0;i<4;++i)
			maxCaches[i]=(1<<i)*1024;
		int minLens[5]={5,50,100,200,300};
		int maxLens[5]={50,100,200,300,400};
		*/
		//testCase4(scales[1],maxCaches[3],minLens[0],maxLens[0]);

		/*for(int i=0;i<sizeof(scales)/4;++i)
			for(int j=0;j<sizeof(maxCaches)/4;++j)
				for(int k=0;k<sizeof(minLens)/4;++k){
					testCase4(scales[i],maxCaches[j],minLens[k],maxLens[k]);
				}*/
		
	}


class Parent: private CountObject{
public:
	Parent(){}
	Parent(int id,string name):m_id(id),m_name(name){}
	int getId()const{
		return m_id;
	}
	string getName()const{
		return m_name;
	}
	void setId(int id){
		m_id=id;
	}
	void setName(string name){
		m_name=name;
	}
	virtual string say(){
		return "I am parent";
	}
	virtual ~Parent(){}
private:
	int m_id;
	string m_name;
};

class Child:public Parent{
public:
	Child(int id,string name,int age):Parent(id,name),m_age(age){}
	int getAge()const{
		return m_age;
	}
	void setAge(int age){
		m_age=age;
	}
	virtual string say(){
		return "I am child";
	}
private:
	int m_age;
};

class OtherPerson:private CountObject{
public:
	OtherPerson(string name):m_name(name){;}
	string getName()const{
		return m_name;
	}
	void setName(string name){
		m_name=name;
	}
private:
	string m_name;
};

void test_ptr(){
	//���Ի����������� ��intΪ����
	
	//��ֵ����
	ptr<int> p1=new int(1);
	assertEquals(*p1,1);
	*p1=2;
	assertEquals(*p1,2);
	
	//���Ը�ֵ���캯��
	ptr<int> p2(p1);
	assertEquals(p1,p2);
	assertEquals(*p1,*p2);
	*p1=7;
	assertEquals(p1,p2);
	assertEquals(*p1,*p2);

	//���Ը�ֵ����
	ptr<int> p3;
	p3=p1;
	assertEquals(p1,p3);
	assertEquals(p2,p3);
	assertEquals(*p1,*p3);
	assertEquals(*p2,*p3);
	
	//������� �� ���� ���������assertEqualsʵ���Ͼ��Ѿ����� ������������ ���ԾͲ�������

	//���� ����* -> []
	ptr<Parent> pa1=new Parent(1,"parent1");
	assertEquals(pa1->getId(),1);
	assertEquals(pa1->getName(),string("parent1"));
	
	ptr<Parent> pa2=new Parent(2,"parent2");
	assertEquals((*pa2).getId(),2);
	assertEquals((*pa2).getName(),string("parent2"));
	
	//������Ļ� �ڶ������� ���Ϊtrue �����ڴ�й¶
	ptr<int> a(new int[4],4);
	a[0]=0;
	a[1]=1;
	a[2]=4;
	a[3]=9;
	assertEquals(0,a[0]);
	assertEquals(1,a[1]);
	assertEquals(4,a[2]);
	assertEquals(9,a[3]);
	
	//���Ը��������� ����ָ���ת��
	
	//�����ʱ���������ָ��
	ptr<Parent> pp1=new Child(1,"xzc",20);
	//�������̬
	assertEquals(string("I am child"),pp1->say());
	
	ptr<Child> pc1=new Child(2,"xzc2",20);

	//�����ָ�븳����������ָ��
	pp1=pc1;
	//��������ָ������������ָ�� ==
	assertEquals(pp1,pc1);

	//��ʵΪ�˷��� �ǿ��������κ����������ָ�뻥����
	//�����а�ȫ�ϵ�����
	//��˼���һ������ as ����ǿ��ת��
	//���ǿ��ת����������dynamic_cast��ʵ�� ��Ϊ�������а�ȫ��(��ʵ�����Ͳ��Ե�ʱ����׳��쳣)
	//����dynamic_castֻ֧�־��ж�̬������
	
	//���������ǽ����������ָ�� ���� ���������ָ�� ������ת��
	//������������
	//pc1=pp1;
	
	//����ʹ��as������ȫת
	pc1=pp1.as<Child>();
	assertEquals(pc1->getName(),pp1->getName());
	
	
	//����dynamic_castֻ�����ھ��ж�̬�Ե���
	//���һЩ��ͨ����ͻ��������������޷�ʹ�õ�
	//���ʹ�õ��� (Type*) ��ǿת��ʽ ����Σ����

	ptr<int> pi;
	ptr<double> pd=new double(3.14);
	pi=pd.forceAs<int>();
	assertEquals(3.14,*(double*)&*pi);
	
	//�������
	//������10��Parent������
	ptr<Parent> ppa(new Parent[10],10);
	//���ͨ�����objectCount�Ƿ�Ϊ0 �Ϳ���֪��ptr��û����ȷ�������������
	info<<"pass ptr test..."<<endl;
}



void testCase0(){
	srand(time(0));
	//�򵥲������ݴ洢����ȷ��
	Utils::createEmptyFile("0.key");
	Utils::createEmptyFile("0.data");
	ptr<Database> db=new Database("0",4,4);
	for(int i=0;i<100;++i){
		db->set("int"+Utils::toString(i),new Variant((int)i));
	}
	db=NULL;
	db=new Database("0",4,4);
	//���ɾ��һЩ
	std::set<string> delKey;
	for(int i=0;i<100;++i){
		string key="int"+Utils::toString(Utils::randomInt()%100);
		db->remove(key);
		delKey.insert(key);
	}
	//�ص�DB
	db=NULL;
	//�ٴ�
	db=new Database("0");
	for(int i=0;i<100;++i){
		string key="int"+Utils::toString(i);
		if(delKey.count(key))
			continue;
		assertEquals(i,db->get(key)->toInt());
	}
	//����ͨ����ɾ����key��õ����ݶ���null
	for(std::set<string>::const_iterator cit=delKey.begin();cit!=delKey.end();++cit){
		assertEquals(true,db->get(*cit).isNull());
	}

	//�����Ż�
	db->shrink();
	
	//�ص����ݿ�
	db=NULL;

	//�ж��ļ��Ĵ�С�Ƿ��Ԥ�ڵ�һ��
	assertEquals(Utils::getFileSize("0.data"),4096);
	info<<"pass testCase0..."<<endl;
}


//���� scale ������ ÿ�������� len���� string
//Ȼ��
//���� [����˳��] ���� ȫ��key һ��
//���� [���˳��] ɾ�� ȫ��key һ��
//���� ����key��Ӧ��valueΪnull
void testCase1(){
	Utils::createEmptyFile("1.key");
	Utils::createEmptyFile("1.data");
	ptr<Database> db = new Database(string("1"),10240,4096);

	srand(77);
	int scale=1000000;
	int len=4;
	
	vector<string> keys;
	vector<ptr<Variant> > values;
	keys.reserve(scale);
	values.reserve(scale);
	
	timeBegin();
	for(int i=0;i<scale;++i){
		keys.push_back(Utils::toString(i));
		values.push_back(new Variant(string(Utils::randomString(len,len+1))));
	}
	timeEnd("���� keys values");

	timeBegin();
	for(int i=0;i<scale;++i)
		db->set(keys[i],values[i]);
	timeEnd("��������");
	
	//��ѯ
	timeBegin();
	for(int i=scale-1;i>=0;--i){
		assertEquals(values[i]->toString(),db->get(keys[i])->toString());
	}
	timeEnd("��ѯ");

	//����
	random_shuffle(keys.begin(),keys.end());

	timeBegin();
	for(int i=0;i<scale;++i){
		db->remove(keys[i]);
	}
	timeEnd("ɾ��");
	
	for(int i=0;i<scale;++i){
		if(!db->get(keys[i]).isNull()){
			info<<"fail"<<endl;
			exit(0);
		}
	}

	db->shrink();
	db=NULL;

	//��������ɾ������������
	//data�ļ��Ĵ�С�϶���0
	assertEquals(Utils::getFileSize("1.data"),0);
	//key�ļ���ֻ����1��int ���Դ�С��4
	assertEquals(Utils::getFileSize("1.key"),4);
	info<<"pass testCase1"<<endl;
}


//���� scale ������ ÿ�������� len���� string
//Ȼ��
//���� [key����˳��] ���� ȫ��key һ��
//���� [���˳��] ɾ�� ȫ��key һ��
//���� ����key��Ӧ��valueΪnull
bool com1(const pair<string,ptr<Variant> >& a,const pair<string,ptr<Variant> >& b){
	return a.first<b.first;
}

void testCase2(){
	Utils::createEmptyFile("2.key");
	Utils::createEmptyFile("2.data");
	ptr<Database> db = new Database(string("2"),1024,4096);

	srand(77);
	int scale=1000000;
	int len=4;
	
	vector<pair<string,ptr<Variant> > > datas;
	datas.reserve(scale);
	set<string> keySet;
	timeBegin();
	for(int i=0;i<scale;++i){
		string key=Utils::toString(Utils::randomInt());
		if(keySet.count(key))
			continue;
		keySet.insert(key);
		pair<string,ptr<Variant> > p;
		p.first=key;
		p.second=new Variant(Utils::randomString(len,len+1));
		datas.push_back(p);
	}
	timeEnd("���� keys values");
	scale=datas.size();

	timeBegin();
	for(int i=0;i<scale;++i)
		db->set(datas[i].first,datas[i].second);
	timeEnd("��������");


	//��key��������
	sort(datas.begin(),datas.end(),com1);

	//��ѯ
	timeBegin();
	for(int i=0;i<scale;++i){
		assertEquals(datas[i].second->toString(),db->get(datas[i].first)->toString()); 
	}
	timeEnd("��ѯ");

	//���� �� ɾ��
	random_shuffle(datas.begin(),datas.end());

	timeBegin();
	for(int i=0;i<scale;++i){
		db->remove(datas[i].first);
	}
	timeEnd("ɾ��");

	for(int i=0;i<scale;++i){
		if(!db->get(datas[i].first).isNull()){
			info<<"fail"<<endl;
			exit(0);
		}
	}

	db->shrink();
	db=NULL;

	//��������ɾ������������
	//data�ļ��Ĵ�С�϶���0
	assertEquals(Utils::getFileSize("2.data"),0);
	//key�ļ���ֻ����1��int ���Դ�С��4
	assertEquals(Utils::getFileSize("2.key"),4);
	info<<"pass testCase2"<<endl;
}


void testCase3(){
	Utils::createEmptyFile("3.key");
	Utils::createEmptyFile("3.data");
	ptr<Database> db = new Database(string("3"),1024,4096);

	srand(77);
	int scale=1000000;
	int len=4;

	vector<pair<string,ptr<Variant> > > datas;
	datas.reserve(scale);
	set<string> keySet;
	timeBegin();
	for(int i=0;i<scale;++i){
		string key=Utils::toString(Utils::randomInt());
		if(keySet.count(key))
			continue;
		keySet.insert(key);
		pair<string,ptr<Variant> > p;
		p.first=key;
		p.second=new Variant(Utils::randomString(len,len+1));
		datas.push_back(p);
	}
	timeEnd("���� keys values");
	scale=datas.size();

	timeBegin();
	for(int i=0;i<scale;++i)
		db->set(datas[i].first,datas[i].second);
	timeEnd("��������");


	//��key��������
	sort(datas.begin(),datas.end(),com1);
	db->sortShrink();

	//��ѯ
	timeBegin();
	for(int i=0;i<scale;++i){
		assertEquals(datas[i].second->toString(),db->get(datas[i].first)->toString()); 
	}
	timeEnd("��ѯ");

	//���� �� ɾ��
	random_shuffle(datas.begin(),datas.end());

	timeBegin();
	for(int i=0;i<scale;++i){
		db->remove(datas[i].first);
	}
	timeEnd("ɾ��");

	for(int i=0;i<scale;++i){
		if(!db->get(datas[i].first).isNull()){
			info<<"fail"<<endl;
			exit(0);
		}
	}

	db->shrink();
	db=NULL;

	//��������ɾ������������
	//data�ļ��Ĵ�С�϶���0
	assertEquals(Utils::getFileSize("3.data"),0);
	//key�ļ���ֻ����1��int ���Դ�С��4
	assertEquals(Utils::getFileSize("3.key"),4);
	info<<"pass testCase3"<<endl;
}

void testCase4(int scale, int maxCache, int minLen, int maxLen){
	info<<"scale="<<scale<<" maxCache="<<maxCache<<" minLen="<<minLen<<" maxLen="<<maxLen<<endl;
	Utils::createEmptyFile("4.key");
	Utils::createEmptyFile("4.data");
	ptr<Database> db = new Database(string("4"),maxCache,4096);

	vector<string> keys;
	keys.reserve(scale);
	vector<string> keys2;
	keys2.reserve(scale);
	for(int i=0;i<scale;++i){
		keys.push_back("key"+Utils::toString(i));
		keys2.push_back("k2_"+Utils::toString(i));
	}
	ptr<Variant> pv=new Variant(Utils::randomString(minLen,minLen+1));
	ptr<Variant> pbig=new Variant(Utils::randomString(maxLen,maxLen+1));

	timeBegin();
	for(int i=0;i<scale;++i){
		db->set(keys[i],pv);
	}
	timeEnd("��������");

	int ilen=5*scale;

	timeBegin();

	for(int i=1;i<=ilen;++i){
		db->get(keys[Utils::randomInt()%scale]);

		if(i%37==0){
			db->remove(keys[Utils::randomInt()%scale]);
		}
		if(i%11==0){
			db->set(keys2[Utils::randomInt()%scale],pv);
		}
		if(i%17==0){
			if(i%34==0){
				db->set(keys[Utils::randomInt()%scale],pv);
			}else{
				db->set(keys[Utils::randomInt()%scale],pbig);
			}
		}
	}
	timeEnd();

	timeBegin();
	vector<string> keyset;
	db->getAllKeys(keyset);
	ilen=keyset.size();

	for(int i=0;i<ilen;++i){
		if(db->remove(keyset[i])){
			for(int j=0;j<10;++j){
				db->get(keyset[Utils::randomInt()%scale]);
			}
		}
	}
	timeEnd();

	//db->clear();
}




}
#endif
