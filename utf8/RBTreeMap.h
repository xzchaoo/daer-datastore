#ifndef XZC_RBTREEMAP_H
#define XZC_RBTREEMAP_H

//基于红黑树实现的map 实现了IMap 接口
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
		//y是x的父节点
		RBNode* y=NULLNODE;
		//x是要插入的位置
		RBNode* x=m_root;
		while(x!=NULLNODE){
			if(key==x->key){
				x->value=value;
				return;
			}
			y=x;
			x=(key<x->key)?x->left:x->right;
		}
		//创建节点 z是要插入的节点
		//创建之后有 z.left=z.right=z.parent=nullnode z.size=1 z.color=0(红色)
		//设置z的父节点
		//z->parent=y;
		RBNode* z=new RBNode(key,value,y,NULLNODE);
		++m_size;
		//y==nullnode 表明整个树是空的
		if(y==NULLNODE)
			this->m_root=z;
		else if(key<y->key)//否则就是向左向右插入
			y->left=z;
		else
			y->right=z;
		//插入完之后 修复红黑树性质
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
		int color;//红0 黑1
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
				//能够运行到这里必有 x.color=w.color=black
				if(w->left->color==1&&w->right->color==1){
					w->color=0;
					x=x->parent;
				}else{
					//能够运行到这里必有 x.color=w.color=black w的子儿子不全为黑
					if(w->right->color==1){
						w->left->color=1;
						w->color=0;
						rr(w);
						w=x->parent->right;
					}
					//能运行到这里必有 x.color=w.color=black w.right.color=red
					w->color=x->parent->color;
					x->parent->color=w->right->color=1;
					lr(x->parent);
					//w->left->color=w->right->color=1;
					//这里x=root主要是要让循环退出
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
	//获得以x为根的最小值的节点
	RBNode* getMin(RBNode* x){
		while(x->left!=NULLNODE)
			x=x->left;
		return x;
	}
	//z是要删除的节点
	//y是x的父节点
	//x是发生移动或删除的节点
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
	//v代替u的位置
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

	//修复红黑树 z是刚刚插入的新节点有 z.left=z.right=nullnode z.parent=某某 z.size=1 z.color=0(红色)
	//能进这个函数的节点 有可能是 根节点(当第一个插入的时候) 或 第二层及其以下的节点
	void insert_fixup(RBNode* z){
		//如果是根节点则while条件不满足 直接走人... 所以这里不用判断z是否为root
		//一旦进入这个函数则会一直有
		//	1.z.coor=red
		while(z->parent->color==0){
			//这里是来判断z节点附近的树的形状
			//下面这些步骤最好画一下草图
			if(z->parent==z->parent->parent->left){
				RBNode* y=z->parent->parent->right;
				//注意会有一些隐含条件
				//比如y->color==0 则说明 z的爷爷为黑色 y有两个NULLNODE
				if(y->color==0){ // case 1 and case 2
					//父亲和叔叔变黑色
					z->parent->color=1;
					y->color=1;
					//爷爷变红色
					z->parent->parent->color=0;
					//迭代修复爷爷
					z=z->parent->parent;
				}else{
					if(z==z->parent->right){ //case 3
						//能走到这里则说明 z的叔叔是nullnode
						z=z->parent;
						lr(z);
					}//case 4
					z->parent->color=1;
					z->parent->parent->color=0;
					rr(z->parent->parent);
				}
			}else{//下面这些代码 直接复制上面的然后将 left 与 right 互换
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