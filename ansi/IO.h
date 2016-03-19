#ifndef XZC_IO_H
#define XZC_IO_H

//提供了简单的读和写封装 用于帮助序列化

#include <fstream>
#include <string>

namespace xzc{
	using namespace std;
	//简单封装了一个读取
	class Reader{
	public:
		Reader(string filename):m_fin(filename.c_str(),ios::in|ios::binary){
		}
		template<typename T>
		Reader& nextT(T& t){
			m_fin.read((char*)&t,sizeof(T));
			assertEquals3_throw(m_fin.gcount(),sizeof(T),"流剩余不足1");
			return *this;
		}
		Reader& nextT(string& s){
			int len;
			m_fin.read((char*)&len,sizeof(int));
			assertEquals3_throw(m_fin.gcount(),sizeof(int),"流剩余不足2");
			ptr<char> pc(new char[len],len);
			m_fin.read(&*pc,len);
			assertEquals3_throw(m_fin.gcount(),len,"流量不足3");
			s=string(&*pc,&*pc+len);
			return *this;
		}
		~Reader(){
			if(m_fin.is_open())
				m_fin.close();
		}
	private:
		ifstream m_fin;
	};

	//简单封装了一个写
	class Writer{
	public:
		Writer(string filename,bool append){
			if(append)
				m_fout.open(filename.c_str(),ios::out|ios::binary|ios::in);
			else
				m_fout.open(filename.c_str(),ios::out|ios::binary);
		}
		template<typename T>
		Writer& writeT(const T& t){
			m_fout.write((const char*)&t,sizeof(T));
			return *this;
		}
		Writer& writeT(const string& s){
			int len=s.size();
			writeT(len);
			m_fout.write(s.c_str(),len);
			return *this;
		}
		~Writer(){
			if(m_fout.is_open()){
				m_fout.flush();
				m_fout.close();
			}
		}
	private:
		ofstream m_fout;
	};



}
#endif
