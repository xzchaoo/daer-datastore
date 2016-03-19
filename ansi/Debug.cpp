#include <ctime>
#include <stack>
#include <fstream>

#include "Debug.h"

using namespace std;


namespace xzc{

static stack<long> time_stack;
static long begin=-1;
void timeBegin(){
	if(begin!=-1)
		time_stack.push(begin);
	begin=clock();
}

long timeEnd(){
	return timeEnd("");
}

long timeEnd(string msg){
	long end=clock();
	long t=(end-begin);
#if CLOCKS_PER_SEC==1000000
	t/=1000;
#endif
	info<<msg<<" ºÄÊ±"<<t<<"ºÁÃë"<<endl;

	if(!time_stack.empty()){
		begin=time_stack.top();
		time_stack.pop();
	}
	return t;
}

int CountObject::objectCount=0;

Xout info("info",true,cout);
Xout warn("warn",false,cout);
Xout debug("debug",true,cout);


}
