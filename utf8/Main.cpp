#include <iostream>
#include <string>

#include "Test.h"
using namespace std;
using namespace xzc;

int main() {
	try{
		runTest();
	}catch(Exception& e){
		cout<<e.getMessage()<<endl;
	}
	return 0;
}