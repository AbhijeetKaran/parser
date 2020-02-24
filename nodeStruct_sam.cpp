#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<vector>
#include<map>
#include <cstdlib>
#include<math.h>
using namespace std;

vector<vector <int>> sam_line_list;
//node structures
class samTreeNode
{
	public:
	int start;
	vector<int> length;
	vector<int> bitflag;
	vector<int> mapq;
	char mark;
	vector<int> l_ref;
	samTreeNode *left;
	samTreeNode *right;
};

class samListNode : public samTreeNode
{
	public:
	string id;
	int chr_count;
	samListNode *next;
	samTreeNode *ds;
};

namespace SAM_BASE
{
	void swap(samTreeNode *x,samTreeNode *y)
	{
		int s_e;
		vector<int> l_e;
		vector<int> bt_e;
		vector<int> mq_e;
		char m_e;
		vector<int> l_re;
		s_e = x->start;
		m_e = x->mark;
		l_e = x->length;
		bt_e = x->bitflag;
		mq_e = x->mapq;
		x->start = y->start;
		x->mark = y->mark;
		y->start = s_e;
		y->mark = m_e;
		x->l_ref.swap(y->l_ref);
		x->length.swap(y->length);
		x->bitflag.swap(y->bitflag);
		x->mapq.swap(y->mapq);
	}
	
	int height(samTreeNode *t)
	{
		int h = 0;
		if(t != NULL)
		{
			int l_height = height(t->left);
			int r_height = height(t->right);
			int max_height = max(l_height,r_height);
			h = max_height + 1;
		}
		return h;
	}
	
	int difference(samTreeNode *t)
	{
		int l_height = height(t->left);
		int r_height = height(t->right);
		int b_factor = l_height - r_height;
		return b_factor;
	}
	
	// rotations
	samTreeNode *left_rotation(samTreeNode *t)
	{
		samTreeNode *x,*y;
		x = t->left;
		if(x->right == NULL)
		{
			y = x->left;
			t->left = y;
			t->right = x;
			x->left = NULL;
			swap(x,t);
		}
		else
		{
			y = x->right;
			t->right =y;
			x->right = NULL;
			swap(t,y);
		}
		return t;
	}
	
	samTreeNode *right_rotation(samTreeNode *t)
	{
		samTreeNode *x,*y;
		x = t->right;
		if(x->left == NULL)
		{
			y = x->right;
			t->left = x;
			t->right = y;
			x->right = NULL;
			swap(x,t);
		}
		else
		{
			y = x->left;
			t->left = y;
			x->left = NULL;
			swap(t,y);
		}
		return t;
	}
	
	
	samTreeNode *balance(samTreeNode *t)
	{
		int bal_factor = difference(t);
		if(bal_factor > 1)
		{
			t = left_rotation(t);
		}
		else if(bal_factor < -1)
		{
			t = right_rotation(t);
		}
		return t;
	}
	

	int hashc(string s1)
	{
		int hashsum = 0;
		int is = 0;
		if(s1[0] == '0')
		{
			hashsum = 0;
			return hashsum;
		}
 		for(int i=0;i<s1.size();i++)
		{
			if((int(s1[i]) > 47) && (int(s1[i]) < 58))
			{	
				is=10*is+(int(s1[i])-48);
			}
			else
			{
				hashsum = hashsum + int(s1[i]);
			}
		}
		hashsum = hashsum + is;
		return hashsum;
	}
	//insert
	samTreeNode *createTreeNode(int s_value, int l_value, int bit_value, int mapq_value, int ln)
	{
		samTreeNode *s = new samTreeNode();
		s->start = s_value;
		s->mark = 'N';
		s->length.insert(s->length.end(),l_value);
		s->bitflag.insert(s->bitflag.end(),bit_value);
		s->mapq.insert(s->mapq.end(),mapq_value);
		s->l_ref.insert(s->l_ref.end(),ln);
		s->left = NULL;
		s->right = NULL;
		return s;
	}

	samListNode *createNode(string key, int s_value, int l_value, int bit_value, int mapq_value, int ln)
	{	
		samListNode *p = new samListNode();
		samTreeNode *t;
		t = createTreeNode(s_value,l_value,bit_value,mapq_value,ln);
		p->id = key;
		p->ds = t;
		p->chr_count = 1;
		p->next = NULL;
		return p;
	}	

	samListNode *insert_head_listnode(samListNode *h, string key, int s_value, int l_value, int bit_value, int mapq_value, int ln)
	{
		samListNode *p =  createNode(key, s_value, l_value, bit_value, mapq_value, ln);
		p->next = h;
		return p;
	}	
	
	void insert_tree(samTreeNode *h, int s_value, int l_value, int bit_value, int mapq_value, int ln)
	{
		samTreeNode *q;
		q = h;
		if((s_value < q->start) && (q->left != NULL))
		{
			insert_tree(q->left,s_value,l_value,bit_value,mapq_value,ln);
		}
		else if((s_value < q->start) && (q->left == NULL))
		{
			samTreeNode *n = createTreeNode(s_value,l_value,bit_value,mapq_value,ln);
			q->left = n;
			q = balance(q);
		}
		else if((s_value > q->start) && (q->right != NULL))
		{
			insert_tree(q->right,s_value, l_value, bit_value, mapq_value, ln);		
		}
		else if((s_value > q->start) && (q->right == NULL))
		{
			samTreeNode *n = createTreeNode(s_value, l_value, bit_value, mapq_value, ln);
			q->right = n;
			q = balance(q);
		}
		else if(s_value == q->start)
		{
			q->mark = 'D';
			q->l_ref.insert(q->l_ref.end(),ln);
			q->length.insert(q->length.end(),l_value);
			q->bitflag.insert(q->bitflag.end(),bit_value);
			q->mapq.insert(q->mapq.end(),mapq_value);
		}
	}

	samListNode *insert_list(samListNode *h, string key, int s_value, int l_value, int bit_value, int mapq_value, int ln)
	{
		samListNode *q ,*g;
		bool found = false;
		if(hashc(h->id) == hashc(key))
		{
			found = true;
			h->chr_count++;
			samTreeNode *t = h->ds;
			insert_tree(t,s_value,l_value,bit_value,mapq_value,ln);
		}		
		else if(hashc(key) < hashc(h->id))
		{
			h = insert_head_listnode(h,key,s_value,l_value,bit_value,mapq_value,ln);
			found = true;
		}
		else
		{
			q = h;
			g = q->next;
			while(q->next != NULL)
			{
				if(hashc(q->id) == hashc(key))
				{
					found = true;
					q->chr_count = q->chr_count + 1;
					insert_tree(q->ds,s_value,l_value,bit_value,mapq_value,ln);
				}
				else if((hashc(key) > hashc(q->id)) && (hashc(key) < hashc(g->id)) && (g != NULL))
				{
					found = true;
					samListNode *p = createNode(key, s_value, l_value, bit_value, mapq_value, ln);
					p->next = q->next;
					q->next = p;
					q = q->next;
				}
				q = q->next;
				g = g->next;
			}
			if(hashc(q->id) == hashc(key))
			{
					found = true;
					q->chr_count = q->chr_count + 1;
					insert_tree(q->ds,s_value,l_value,bit_value,mapq_value,ln);
			}	
			if(!found)
			{
				samListNode *p = createNode(key, s_value, l_value, bit_value, mapq_value, ln);
				q->next = p;
				q = q->next;
			}	
		}
	return h;
	}
	
	void preorder(samTreeNode *q,string id)
	{
		if(q != NULL)
		{
			preorder(q->left,id);			
			sam_line_list.insert(sam_line_list.end(),q->l_ref);
			preorder(q->right,id);
		}
	}	

	void sort(samListNode *h)
	{
		samListNode *q = h;
		while(q != NULL)
		{
			preorder(q->ds,q->id);	
			q = q->next;
		}
	}
	/*
	void bit_calc(samTreeNode *q)
	{
		static int mark = 0;
		if(q != NULL)
		{
			bit_calc(q->left);			
			if(q->mark == 'D')
				mark++;
			bit_calc(q->right);
		}
		cout<<q->start<<"\t"<<mark<<endl;
	}
	
	void stats(samListNode *h)
	{
		samListNode *q = h;
		while(q != NULL)
		{
			samTreeNode *qtn = q->ds;
			bit_calc(qtn);
			q = q->next;
		}
	}*/	
}

