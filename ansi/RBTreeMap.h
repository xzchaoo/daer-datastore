#ifndef XZC_RBTREEMAP_H
#define XZC_RBTREEMAP_H

//���ں����ʵ�ֵ�map ʵ����IMap �ӿ�
#include <iostream>
#include "IMap.h"
namespace xzc{
	using namespace std;


template<typename KeyType,typename ValueType>
class RBTreeMap:public IMap<KeyType,ValueType>{
public:
	RBTreeMap(){
		createNullNode();
		m_size=0;
	}
	virtual ValueType get( const KeyType& key ) {
		RBNode* n=findNode(key);
		if(n!=NULLNODE)
			return n->value;
		return ValueType();
	}

	virtual void put( const KeyType& key, const ValueType& value ) {
		//y��x�ĸ��ڵ�
		RBNode* y=NULLNODE;
		//x��Ҫ�����λ��
		RBNode* x=m_root;
		while(x!=NULLNODE){
			if(key==x->key){
				x->value=value;
				return;
			}
			y=x;
			x=(key<x->key)?x->left:x->right;
		}
		//�����ڵ� z��Ҫ����Ľڵ�
		//����֮���� z.left=z.right=z.parent=nullnode z.size=1 z.color=0(��ɫ)
		//����z�ĸ��ڵ�
		//z->parent=y;
		RBNode* z=new RBNode(key,value,y,NULLNODE);
		++m_size;
		//y==nullnode �����������ǿյ�
		if(y==NULLNODE)
			this->m_root=z;
		else if(key<y->key)//��������������Ҳ���
			y->left=z;
		else
			y->right=z;
		//������֮�� �޸����������
		insert_fixup(z);
	}

	virtual bool remove( const KeyType& key ) {
		RBNode* n=findNode(key);
		if(n!=NULLNODE){
			--m_size;
			_remove(n);
			return true;
		}
		return false;
	}

	virtual bool contains( const KeyType& key ) {
		return findNode(key)!=NULLNODE;
	}

	virtual int size() {
		return m_size;
	}

	virtual void clear(){
		_clear(m_root);
		m_root=NULLNODE;
	}

	~RBTreeMap(){
		clear();
		delete NULLNODE;
		m_root=NULLNODE=NULL;
	}
	virtual void getKeySet( vector<KeyType>& keys ){
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
	struct RBNode{
		KeyType key;
		ValueType value;
		RBNode* left;
		RBNode* right;
		RBNode* parent;
		int color;//��0 ��1
		RBNode(const KeyType& _key, const ValueType& _value,RBNode* parent,RBNode* nullnode):key(_key),value(_value),parent(parent),color(0),left(nullnode),right(nullnode){}
		RBNode(){}
	};
	void _getKeySet(vector<KeyType>& vec,RBNode* n){
		if(n==NULLNODE)
			return;
		_getKeySet(vec,n->left);
		vec.push_back(n->key);
		_getKeySet(vec,n->right);
	}
	void _getKeyAndValueSet(vector<KeyType>& keys,vector<ValueType>& values, RBNode* n){
		if(n==NULLNODE)
			return;
		_getKeyAndValueSet(keys,values,n->left);
		keys.push_back(n->key);
		values.push_back(n->value);
		_getKeyAndValueSet(keys,values,n->right);
	}

	RBNode* m_root;
	RBNode* NULLNODE;
	int m_size;

	void _clear(RBNode* n){
		if(n==NULLNODE)
			return;
		_clear(n->left);
		_clear(n->right);
		delete n;
	}

	void delete_fixup(RBNode* x){
		while(x!=m_root&&x->color==1){
			if(x==x->parent->left){
				RBNode* w=x->parent->right;
				if(w->color==0){
					w->color=1;
					x->parent->color=0;
					lr(x->parent);
					w=x->parent->right;
				}
				//�ܹ����е�������� x.color=w.color=black
				if(w->left->color==1&&w->right->color==1){
					w->color=0;
					x=x->parent;
				}else{
					//�ܹ����е�������� x.color=w.color=black w���Ӷ��Ӳ�ȫΪ��
					if(w->right->color==1){
						w->left->color=1;
						w->color=0;
						rr(w);
						w=x->parent->right;
					}
					//�����е�������� x.color=w.color=black w.right.color=red
					w->color=x->parent->color;
					x->parent->color=w->right->color=1;
					lr(x->parent);
					//w->left->color=w->right->color=1;
					//����x=root��Ҫ��Ҫ��ѭ���˳�
					x=m_root;
				}
			}else{
				RBNode* w=x->parent->left;
				if(w->color==0){
					w->color=1;
					x->parent->color=0;
					rr(x->parent);
					w=x->parent->left;
				}
				if(w->left->color==1&&w->right->color==1){
					w->color=0;
					x=x->parent;
				}else{
					if(w->left->color==1){
						w->right->color=1;
						w->color=0;
						lr(w);
						w=x->parent->left;
					}
					w->color=x->parent->color;
					x->parent->color=w->left->color=1;
					rr(x->parent);
					x=m_root;
				}
			}
		}
		x->color=1;
	}
	//�����xΪ������Сֵ�Ľڵ�
	RBNode* getMin(RBNode* x){
		while(x->left!=NULLNODE)
			x=x->left;
		return x;
	}
	//z��Ҫɾ���Ľڵ�
	//y��x�ĸ��ڵ�
	//x�Ƿ����ƶ���ɾ���Ľڵ�
	void _remove(RBNode* z){
		RBNode* y=z;
		RBNode* x=NULLNODE;
		int old_color=y->color;
		if(z->left==NULLNODE){
			x=z->right;
			daiti(z,z->right);
		}else if(z->right==NULLNODE){
			x=z->left;
			daiti(z,z->left);
		}else{
			y=getMin(z->right);
			old_color=y->color;
			x=y->right;
			if(y->parent==z)
				x->parent=y;
			else{
				daiti(y,y->right);
				y->right=z->right;
				y->right->parent=y;
			}
			daiti(z,y);

			y->left=z->left;
			y->left->parent=y;

			y->color=z->color;
		}
		if(old_color==1)
			delete_fixup(x);

		delete z;
	}
	//v����u��λ��
	void daiti(RBNode* u, RBNode* v){
		if(u->parent==NULLNODE)
			m_root=v;
		else if(u==u->parent->left)
			u->parent->left=v;
		else
			u->parent->right=v;
		v->parent=u->parent;
	}

	RBNode* findNode(const KeyType& key){
		RBNode* t=m_root;
		while(t!=NULLNODE)
			if(key==t->key)
				break;
			else
				t=key<t->key?t->left:t->right;
		return t;
	}

	//�޸������ z�Ǹող�����½ڵ��� z.left=z.right=nullnode z.parent=ĳĳ z.size=1 z.color=0(��ɫ)
	//�ܽ���������Ľڵ� �п����� ���ڵ�(����һ�������ʱ��) �� �ڶ��㼰�����µĽڵ�
	void insert_fixup(RBNode* z){
		//����Ǹ��ڵ���while���������� ֱ������... �������ﲻ���ж�z�Ƿ�Ϊroot
		//һ����������������һֱ��
		//	1.z.coor=red
		while(z->parent->color==0){
			//���������ж�z�ڵ㸽����������״
			//������Щ������û�һ�²�ͼ
			if(z->parent==z->parent->parent->left){
				RBNode* y=z->parent->parent->right;
				//ע�����һЩ��������
				//����y->color==0 ��˵�� z��үүΪ��ɫ y������NULLNODE
				if(y->color==0){ // case 1 and case 2
					//���׺�������ɫ
					z->parent->color=1;
					y->color=1;
					//үү���ɫ
					z->parent->parent->color=0;
					//�����޸�үү
					z=z->parent->parent;
				}else{
					if(z==z->parent->right){ //case 3
						//���ߵ�������˵�� z��������nullnode
						z=z->parent;
						lr(z);
					}//case 4
					z->parent->color=1;
					z->parent->parent->color=0;
					rr(z->parent->parent);
				}
			}else{//������Щ���� ֱ�Ӹ��������Ȼ�� left �� right ����
				RBNode* y=z->parent->parent->left;
				if(y->color==0){
					z->parent->color=1;
					y->color=1;
					z->parent->parent->color=0;
					z=z->parent->parent;
				}else{
					if(z==z->parent->left){
						z=z->parent;
						rr(z);
					}
					z->parent->color=1;
					z->parent->parent->color=0;
					lr(z->parent->parent);
				}
			}
		}
		this->m_root->color=1;
	}
	void lr(RBNode* x){
		RBNode* y=x->right;
		x->right=y->left;
		if(y->left!=NULLNODE)
			y->left->parent=x;

		y->parent=x->parent;
		if(x->parent==NULLNODE)
			this->m_root=y;
		else if(x==x->parent->left)
			x->parent->left=y;
		else
			x->parent->right=y;

		y->left=x;
		x->parent=y;

	}

	void rr(RBNode* x){
		RBNode* y=x->left;
		x->left=y->right;
		if(y->right!=NULLNODE)
			y->right->parent=x;

		y->parent=x->parent;
		if(x->parent==NULLNODE)
			this->m_root=y;
		else if(x==x->parent->left)
			x->parent->left=y;
		else
			x->parent->right=y;

		y->right=x;
		x->parent=y;

	}

	void createNullNode(){
		m_root=NULLNODE=new RBNode(KeyType(),ValueType(),0,0);
		NULLNODE->left=NULLNODE->right=NULLNODE;
		NULLNODE->parent=NULLNODE;
		NULLNODE->color=1;
	}

};


}
#endif