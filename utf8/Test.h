#ifndef XZC_TEST_H
#define XZC_TEST_H

//专门用于测试的头文件

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
		//检查是否内存泄露 对象个数应该全为0
		assertEquals(CountObject::getObjectCount(),0);
	}

	void testDb(){
		testCase0();
		testCase1();
		testCase2();
		testCase3();
		
		//100万数据量 数据量50~100MB 256MB缓存
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
	//测试基本数据类型 以int为例子
	
	//赋值测试
	ptr<int> p1=new int(1);
	assertEquals(*p1,1);
	*p1=2;
	assertEquals(*p1,2);
	
	//测试赋值构造函数
	ptr<int> p2(p1);
	assertEquals(p1,p2);
	assertEquals(*p1,*p2);
	*p1=7;
	assertEquals(p1,p2);
	assertEquals(*p1,*p2);

	//测试赋值函数
	ptr<int> p3;
	p3=p1;
	assertEquals(p1,p3);
	assertEquals(p2,p3);
	assertEquals(*p1,*p3);
	assertEquals(*p2,*p3);
	
	//测试相等 和 不等 由于上面的assertEquals实际上就已经测试 这两个函数了 所以就不测试了

	//测试 重载* -> []
	ptr<Parent> pa1=new Parent(1,"parent1");
	assertEquals(pa1->getId(),1);
	assertEquals(pa1->getName(),string("parent1"));
	
	ptr<Parent> pa2=new Parent(2,"parent2");
	assertEquals((*pa2).getId(),2);
	assertEquals((*pa2).getName(),string("parent2"));
	
	//是数组的话 第二个参数 务必为true 否则内存泄露
	ptr<int> a(new int[4],4);
	a[0]=0;
	a[1]=1;
	a[2]=4;
	a[3]=9;
	assertEquals(0,a[0]);
	assertEquals(1,a[1]);
	assertEquals(4,a[2]);
	assertEquals(9,a[3]);
	
	//测试父类与子类 智能指针的转换
	
	//构造的时候用子类的指针
	ptr<Parent> pp1=new Child(1,"xzc",20);
	//测试其多态
	assertEquals(string("I am child"),pp1->say());
	
	ptr<Child> pc1=new Child(2,"xzc2",20);

	//子类的指针赋给父类智能指针
	pp1=pc1;
	//父类智能指针与子类智能指针 ==
	assertEquals(pp1,pc1);

	//其实为了方便 是可以允许任何种类的智能指针互换的
	//不过有安全上的问题
	//因此加入一个函数 as 用于强制转换
	//这个强制转换本来想用dynamic_cast来实现 因为这样具有安全性(当实际类型不对的时候会抛出异常)
	//不过dynamic_cast只支持具有多态的类型
	
	//下面这条是将父类的智能指针 赋给 子类的智能指针 即向下转型
	//会产生编译错误
	//pc1=pp1;
	
	//可以使用as函数安全转
	pc1=pp1.as<Child>();
	assertEquals(pc1->getName(),pp1->getName());
	
	
	//由于dynamic_cast只能用于具有多态性的类
	//因此一些普通的类和基本数据类型是无法使用的
	//因此使用的是 (Type*) 的强转方式 具有危险性

	ptr<int> pi;
	ptr<double> pd=new double(3.14);
	pi=pd.forceAs<int>();
	assertEquals(3.14,*(double*)&*pi);
	
	//数组测试
	//建立了10个Parent的数组
	ptr<Parent> ppa(new Parent[10],10);
	//最后通过检查objectCount是否为0 就可以知道ptr有没有正确处理数组的析构
	info<<"pass ptr test..."<<endl;
}



void testCase0(){
	srand(time(0));
	//简单测试数据存储的正确性
	Utils::createEmptyFile("0.key");
	Utils::createEmptyFile("0.data");
	ptr<Database> db=new Database("0",4,4);
	for(int i=0;i<100;++i){
		db->set("int"+Utils::toString(i),new Variant((int)i));
	}
	db=NULL;
	db=new Database("0",4,4);
	//随机删除一些
	std::set<string> delKey;
	for(int i=0;i<100;++i){
		string key="int"+Utils::toString(Utils::randomInt()%100);
		db->remove(key);
		delKey.insert(key);
	}
	//关掉DB
	db=NULL;
	//再打开
	db=new Database("0");
	for(int i=0;i<100;++i){
		string key="int"+Utils::toString(i);
		if(delKey.count(key))
			continue;
		assertEquals(i,db->get(key)->toInt());
	}
	//断言通过被删除的key获得的数据都是null
	for(std::set<string>::const_iterator cit=delKey.begin();cit!=delKey.end();++cit){
		assertEquals(true,db->get(*cit).isNull());
	}

	//进行优化
	db->shrink();
	
	//关掉数据库
	db=NULL;

	//判断文件的大小是否跟预期的一致
	assertEquals(Utils::getFileSize("0.data"),4096);
	info<<"pass testCase0..."<<endl;
}


//插入 scale 条数据 每条数据是 len长的 string
//然后
//按照 [插入顺序] 访问 全部key 一次
//按照 [随机顺序] 删除 全部key 一次
//断言 所有key对应的value为null
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
	timeEnd("建立 keys values");

	timeBegin();
	for(int i=0;i<scale;++i)
		db->set(keys[i],values[i]);
	timeEnd("插入数据");
	
	//查询
	timeBegin();
	for(int i=scale-1;i>=0;--i){
		assertEquals(values[i]->toString(),db->get(keys[i])->toString());
	}
	timeEnd("查询");

	//乱序
	random_shuffle(keys.begin(),keys.end());

	timeBegin();
	for(int i=0;i<scale;++i){
		db->remove(keys[i]);
	}
	timeEnd("删除");
	
	for(int i=0;i<scale;++i){
		if(!db->get(keys[i]).isNull()){
			info<<"fail"<<endl;
			exit(0);
		}
	}

	db->shrink();
	db=NULL;

	//由于我们删除了所有数据
	//data文件的大小肯定是0
	assertEquals(Utils::getFileSize("1.data"),0);
	//key文件里只放了1个int 所以大小是4
	assertEquals(Utils::getFileSize("1.key"),4);
	info<<"pass testCase1"<<endl;
}


//插入 scale 条数据 每条数据是 len长的 string
//然后
//按照 [key增长顺序] 访问 全部key 一次
//按照 [随机顺序] 删除 全部key 一次
//断言 所有key对应的value为null
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
	timeEnd("建立 keys values");
	scale=datas.size();

	timeBegin();
	for(int i=0;i<scale;++i)
		db->set(datas[i].first,datas[i].second);
	timeEnd("插入数据");


	//按key排序后访问
	sort(datas.begin(),datas.end(),com1);

	//查询
	timeBegin();
	for(int i=0;i<scale;++i){
		assertEquals(datas[i].second->toString(),db->get(datas[i].first)->toString()); 
	}
	timeEnd("查询");

	//乱序 后 删除
	random_shuffle(datas.begin(),datas.end());

	timeBegin();
	for(int i=0;i<scale;++i){
		db->remove(datas[i].first);
	}
	timeEnd("删除");

	for(int i=0;i<scale;++i){
		if(!db->get(datas[i].first).isNull()){
			info<<"fail"<<endl;
			exit(0);
		}
	}

	db->shrink();
	db=NULL;

	//由于我们删除了所有数据
	//data文件的大小肯定是0
	assertEquals(Utils::getFileSize("2.data"),0);
	//key文件里只放了1个int 所以大小是4
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
	timeEnd("建立 keys values");
	scale=datas.size();

	timeBegin();
	for(int i=0;i<scale;++i)
		db->set(datas[i].first,datas[i].second);
	timeEnd("插入数据");


	//按key排序后访问
	sort(datas.begin(),datas.end(),com1);
	db->sortShrink();

	//查询
	timeBegin();
	for(int i=0;i<scale;++i){
		assertEquals(datas[i].second->toString(),db->get(datas[i].first)->toString()); 
	}
	timeEnd("查询");

	//乱序 后 删除
	random_shuffle(datas.begin(),datas.end());

	timeBegin();
	for(int i=0;i<scale;++i){
		db->remove(datas[i].first);
	}
	timeEnd("删除");

	for(int i=0;i<scale;++i){
		if(!db->get(datas[i].first).isNull()){
			info<<"fail"<<endl;
			exit(0);
		}
	}

	db->shrink();
	db=NULL;

	//由于我们删除了所有数据
	//data文件的大小肯定是0
	assertEquals(Utils::getFileSize("3.data"),0);
	//key文件里只放了1个int 所以大小是4
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
	timeEnd("插入数据");

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
