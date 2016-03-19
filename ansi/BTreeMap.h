#ifndef XZC_BTREEMAP_H
#define XZC_BTREEMAP_H

//����B��ʵ�ֵ�map
#include "IMap.h"
namespace xzc{
	using namespace std;
	template<typename KeyType,typename ValueType>
	class BTreeMap:public IMap<KeyType,ValueType>{
		static const int MAXN=100;
		static const int T=MAXN/2;
		typedef struct BTNodeTag{
			int n;
			KeyType key[MAXN-1];
			ValueType value[MAXN-1];
			BTNodeTag* child[MAXN];
			bool leaf;
		}BTNode;
	public:
		BTreeMap(){
			m_root=_createNode();
			m_size=0;
		}
		void put(const KeyType& key,const ValueType& value){
			if(m_root->n==2*T-1){
				BTNode* x=m_root;
				m_root=_createNode();
				m_root->leaf=false;
				m_root->n=0;
				m_root->child[0]=x;
				_splitChild(m_root,0);
			}
			_insertNotFull(m_root,key,value);
			++m_size;
		}
		ValueType get(const KeyType& key){
			int index;
			BTNode* x=_findNode(key,&index);
			if(x!=NULL)
				return x->value[index];
			return ValueType();
		}
		bool remove(const KeyType& k){
			if(contains(k)){
				--m_size;
				_remove(m_root,k,false,0,0);
				return true;
			}
			return false;
		}
		bool contains(const KeyType& k){
			return _contains(m_root,k);
		}
		int size(){
			return m_size;
		}
		void clear(){
			_destroy(m_root);
			m_root=_createNode();
			m_size=0;
		}
		virtual void getKeySet( vector<KeyType>& keys ) {
			keys.clear();
			keys.reserve(size());
			_getKeySet(keys,m_root);
		}
		virtual void getKeyAndValueSet( vector<KeyType>& keys, vector<ValueType>& values ) {
			keys.clear();
			values.clear();
			keys.reserve(size());
			values.reserve(size());
			_getKeyAndValueSet(keys,values,m_root);
		}

private:
		void _destroy(BTNode* x){
			if(!x->leaf)
				for(int i=0;i<=x->n;++i){
					_destroy(x->child[i]);
				}
				delete x;
		}
		virtual void _getKeyAndValueSet( vector<KeyType>& keys, vector<ValueType>& values ,BTNode* x) {
			if(!x)
				return;
			int ilen=x->n;
			if(x->leaf){
				for(int i=0;i<ilen;++i){
					keys.push_back(x->key[i]);
					values.push_back(x->value[i]);
				}
			}else{
				for(int i=0;i<ilen;++i){
					_getKeyAndValueSet(keys,values,x->child[i]);
					keys.push_back(x->key[i]);
					values.push_back(x->value[i]);
				}
				_getKeyAndValueSet(keys,values,x->child[ilen]);
			}
		}
		void _getKeySet(vector<KeyType>& keys,BTNode* x){
			if(!x)
				return;
			int ilen=x->n;
			if(x->leaf){
				for(int i=0;i<ilen;++i){
					keys.push_back(x->key[i]);
				}
			}else{
				for(int i=0;i<ilen;++i){
					_getKeySet(keys,x->child[i]);
					keys.push_back(x->key[i]);
				}
				_getKeySet(keys,x->child[ilen]);
			}
		}
		void _remove(BTNode* x,const KeyType& key,bool flag,KeyType* pkey, ValueType* pvalue){
			//�ҳ�k���ڵ�����
			int i=0;
			int ilen=x->n;
			for(;i<ilen;++i)
				if(key<=x->key[i])
					break;
			//ͣ������λ���� k<=x->key[i] ����x->key[x->n]��ֵ�����޴�

			if(x->leaf){
				//ɾ����һ���ڵ� ǰ�����ģʽ
				if(i==0&&flag){
					*pkey=x->key[0];
					*pvalue=x->value[0];
					removeKeyAt(x,0);
					decn(x);
					return;
				}
				if(i==ilen&&flag){
					*pkey=x->key[ilen-1];
					*pvalue=x->value[ilen-1];
					decn(x);
					return;
				}
				if(flag){
					while(i<ilen&&key==x->key[i])
						++i;
					if(i==ilen){
						*pkey=x->key[ilen-1];
						*pvalue=x->value[ilen-1];
						decn(x);
						return ;
					}
					exit(0);
				}
				//û�ҵ�
				if(i==ilen||x->key[i]!=key){
					cout<<"case 2"<<endl;
					exit(0);
				}
				//k��λ����iֱ��ɾ��k
				removeKeyAt(x,i);
				decn(x);

				//TODO 1
				return ;//key;
			}else{
				if(i!=ilen&&x->key[i]==key){
					if(x->child[i]->n>=T){
						if(flag){
							*pkey=x->key[i];
							*pvalue=x->value[i];
						}
						_remove(x->child[i],key,true,&x->key[i],&x->value[i]);
						return;
					}else if(x->child[i+1]->n>=T){
						if(flag){
							*pkey=x->key[i];
							*pvalue=x->value[i];
						}
						_remove(x->child[i+1],key,true,&x->key[i],&x->value[i]);
						return;
					}else{
						_merge(x,i);
						_remove(x->child[i],key,flag,pkey,pvalue);//TODO
						return;
					}
				}else{
					//��֤�����Ľڵ������㹻
					if(x->child[i]->n<T){
						if(i>0&&x->child[i-1]->n>=T){
							BTNode* y=x->child[i-1];
							BTNode* z=x->child[i];

							//z�ڳ���һ��λ��
							int jlen=z->n;
							for(int j=jlen;j>=1;--j){
								z->key[j]=z->key[j-1];
								z->value[j]=z->value[j-1];
							}
							for(int j=jlen+1;j>=1;--j)
								z->child[j]=z->child[j-1];
							++z->n;

							//��x->key[i-1]������
							z->key[0]=x->key[i-1];
							z->value[0]=x->value[i-1];
							//��y->key[last()]����ȥ���� x->key[i-1]
							jlen=y->n;
							x->key[i-1]=y->key[jlen-1];
							x->value[i-1]=y->value[jlen-1];
							//�޸����ӹ�ϵ
							z->child[0]=y->child[jlen];
							decn(y);
							_remove(z,key,flag,pkey,pvalue);
							return;
							//��߳���
						}else if(i<ilen&&x->child[i+1]->n>=T){
							BTNode* y=x->child[i+1];
							BTNode* z=x->child[i];

							//��x->key[i]������
							int zlen=z->n;
							z->key[zlen]=x->key[i];
							z->value[zlen]=x->value[i];

							z->child[zlen+1]=y->child[0];
							++z->n;

							//��y->key[0]����ȥ
							x->key[i]=y->key[0];
							x->value[i]=y->value[0];

							//ɾ��y�ĵ�һ��key��child
							removeKeyAt(y,0);
							removeChildAt(y,0);
							decn(y);
							_remove(z,key,flag,pkey,pvalue);
							return;
							//�ұ߳���
						}else{
							//��������
							if(i==ilen){
								_merge(x,--i);
							}else{
								_merge(x,i);
							}
							_remove(x->child[i],key,flag,pkey,pvalue);
							return;
						}
					}
					_remove(x->child[i],key,flag,pkey,pvalue);
					return;
				}
			}
		}


		bool _contains(BTNode* x,KeyType k){
			return _findNode(k,NULL)!=NULL;
			int i;
			while(true){
				int xn=x->n;
				for(i=0;i<xn;++i)
					if(k<=x->key[i])
						break;
				if(i<xn&&k==x->key[i])
					return true;
				if(x->leaf)
					break;
				x=x->child[i];
			}
			return false;
		}




		//�ϲ� x->child[i] k x->child[i+1] T+1+T =2T-1 ��Ϊ x->child[i]
		void _merge(BTNode* x, int i){

			//���кϲ�...��ͼ������
			BTNode* y=x->child[i];
			BTNode* z=x->child[i+1];
			//kΪy�ĵ�T��key
			y->key[T-1]=x->key[i];
			y->value[T-1]=x->value[i];

			//����ʣ�µ�T-1key
			for(int j=0;j<T-1;++j){
				y->key[j+T]=z->key[j];
				y->value[j+T]=z->value[j];
			}
			//����T��child
			for(int j=0;j<T;++j)
				y->child[T+j]=z->child[j];

			//����y��n
			y->n=2*T-1;

			//��x��ɾ��k
			removeKeyAt(x,i);
			removeChildAt(x,i+1);

			decn(x);
		}

		void decn(BTNode* x){
			if(--x->n==0){
				if(!x->leaf)
					m_root=x->child[0];
			}
		}
		void removeKeyAt(BTNode* x,int i){
			int ilen=x->n-1;
			for(;i<ilen;++i){
				x->key[i]=x->key[i+1];
				x->value[i]=x->value[i+1];
			}
		}


		void removeChildAt(BTNode* x, int i){
			int ilen=x->n;
			for(;i<ilen;++i){
				x->child[i]=x->child[i+1];
			}
		}

		void _insertNotFull(BTNode* x, const KeyType& key,const ValueType& value){
			if(x->leaf) {
				int i=_singleBinarySearch(x,key);
				for(int j=x->n;j>i;--j){
					x->key[j]=x->key[j-1];
					x->value[j]=x->value[j-1];
				}
				x->key[i]=key;
				x->value[i]=value;
				++x->n;
			}else{
				int i=_singleBinarySearch(x,key);
				if(x->child[i]->n==2*T-1){
					_splitChild(x,i);
					if(key>x->key[i])
						++i;
				}
				_insertNotFull(x->child[i],key,value);
			}
		}

		//n�Ƿ��� n->child[i]����
		void _splitChild(BTNode* x,int i){
			BTNode* z=_createNode();
			BTNode* y=x->child[i];

			z->leaf=y->leaf;

			z->n=T-1;
			y->n=T-1;

			//����key
			for(int j=0;j<T-1;++j){
				z->key[j]=y->key[j+T];
				z->value[j]=y->value[j+T];
			}
			for(int j=0;j<T;++j)
				z->child[j]=y->child[j+T];

			//����z��x��iλ��
			//��[i,n)��Ԫ�ش�1λ
			int xn=x->n;
			for(int j=xn;j>i;--j){
				x->key[j]=x->key[j-1];
				x->value[j]=x->value[j-1];
			}
			x->key[i]=y->key[T-1];
			x->value[i]=y->value[T-1];

			//��[i,n]��child��1λ
			for(int j=xn+1;j>i+1;--j)
				x->child[j]=x->child[j-1];
			x->child[i+1]=z;

			++x->n;
		}

		BTNode* _createNode(){
			BTNode* n=new BTNode();
			n->leaf=true;
			n->n=0;
			return n;
		}
		BTNode* _findNode(const KeyType& key, int* pindex){
			return _binarySearch(key,pindex);

			int i;
			BTNode* x=m_root;
			while(true){
				int xn=x->n;
				for(i=0;i<xn;++i)
					if(key<=x->key[i])
						break;
				if(i<xn&&key==x->key[i]){
					if(pindex)
						*pindex=i;
					return x;
				}
				if(x->leaf)
					break;
				x=x->child[i];
			}
			return NULL;
		}
		int _singleBinarySearch(BTNode* x, const KeyType& key){
			int left=0;
			int right=x->n-1;
			//�ҵ���һ������key<=x.key[i]��i
			int ans=x->n;
			int mid;
			while(left<=right){
				mid=(left+right)>>1;
				if(key<=x->key[mid]){
					ans=mid;
					right=mid-1;
				}else
					left=mid+1;
			}
			return ans;
		}
		BTNode* _binarySearch(const KeyType& key,int* pindex){
			BTNode* x=m_root;
			while(true){
				int index=_singleBinarySearch(x,key);
				if(index<x->n&&x->key[index]==key){
					if(pindex)
						*pindex=index;
					return x;
				}
				if(x->leaf)
					break;
				x=x->child[index];
			}
			return NULL;
		}
		int m_size;
		BTNode* m_root;
	};

}
#endif