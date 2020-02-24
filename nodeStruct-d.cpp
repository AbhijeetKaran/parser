#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<vector>
#include<map>
#include <cstdlib>
#include<math.h>
using namespace std;

vector<vector <int>> line_list;
//node structures
class TreeNode
{
	public:
	int start;
	int end;
	int height;
	char mark;
	vector<int> l_ref;
	TreeNode *left;
	TreeNode *right;
};

class ListNode : public TreeNode
{
	public:
	string id;
	int chr_count;
	ListNode *next;
	TreeNode *ds;
};

namespace BASE
{
	int hashc(string s1)
	{
		int hashsum = 0;
		int is = 0;
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
	TreeNode *createTreeNode(int s_value, int e_value, int ln)
	{
		TreeNode *s = new TreeNode();
		s->height = 0;
		s->start = s_value;
		s->end = e_value;
		s->mark = 'N';
		s->l_ref.insert(s->l_ref.end(),ln);
		s->left = NULL;
		s->right = NULL;
		return s;
	}

	ListNode *createNode(string key, int s_value, int e_value, int ln)
	{	
		ListNode *p = new ListNode();
		TreeNode *t;
		t = createTreeNode(s_value,e_value,ln);
		p->id = key;
		p->ds = t;
		p->chr_count = 1;
		p->next = NULL;
		return p;
	}	

	ListNode *insert_head_listnode(ListNode *h, string key, int s_value, int e_value, int ln)
	{
		ListNode *p =  createNode(key,s_value,e_value,ln);
		p->next = h;
		return p;
	}	
	
	void insert_tree(TreeNode *h, int s_value, int e_value, int ln)
	{
		TreeNode *q;
		q = h;
		if((s_value < q->start) && (q->left != NULL))
		{
			q->height = q->height + 1;
			insert_tree(q->left,s_value,e_value,ln);
		}
		else if((s_value < q->start) && (q->left == NULL))
		{
			TreeNode *n = createTreeNode(s_value,e_value,ln);
			q->left = n;
			q->height = 1;
			return;
		}
		else if((s_value > q->start) && (q->right != NULL))
		{
			q->height = q->height - 1;
			insert_tree(q->right,s_value,e_value,ln);		
		}
		else if((s_value > q->start) && (q->right == NULL))
		{
			TreeNode *n = createTreeNode(s_value,e_value,ln);
			q->right = n;
			q->height = -1;
			return;
		}
		else if(s_value == q->start)
		{
			q->mark = 'D';
			q->l_ref.insert(q->l_ref.end(),ln);
		}
	}

	ListNode *insert_list(ListNode *h, string key, int s_value, int e_value, int ln)
	{
		ListNode *q ,*g;
		bool found = false;
		//comparing with the head node
		if(hashc(h->id) == hashc(key))
		{
			found = true;
			h->chr_count++;
			TreeNode *t = h->ds;
			insert_tree(t,s_value,e_value,ln);
		}		
		else if(hashc(key) < hashc(h->id))
		{
			h = insert_head_listnode(h,key,s_value,e_value,ln);
			found = true;
		}
		else //comparing with the rest of node
		{
			q = h;
			g = q->next;
			while(q->next != NULL)
			{
				if(hashc(q->id) == hashc(key))
				{
		 			found = true;
					q->chr_count = q->chr_count + 1;
					insert_tree(q->ds,s_value,e_value,ln);
				}
				else if((hashc(key) > hashc(q->id)) && (hashc(key) < hashc(g->id)) && (g != NULL))
				{
					found = true;
					ListNode *p = createNode(key,s_value,e_value,ln);
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
					insert_tree(q->ds,s_value,e_value,ln);
			}
			if(!found)
			{
				ListNode *p = createNode(key,s_value,e_value,ln);
				q->next = p;
				q = q->next;
			}	
		}
	return h;
	}	

	void preorder(TreeNode *q,string id)
	{
		if(q != NULL)
		{
			preorder(q->left,id);
			line_list.insert(line_list.end(),q->l_ref);
			preorder(q->right,id);
		}
	}	

	void sort(ListNode *h)
	{
		ListNode *q = h;
		while(q != NULL)
		{
			if(q->ds == NULL)
			{
				continue;
			}
			else
			{
				preorder(q->ds,q->id);	
			}
			q = q->next;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace MERGE_BASE
{
	void insert_tree(TreeNode *h, int s_value, int e_value, int ln)
	{
		TreeNode *q;
		q = h;
		// for both
		if((s_value < q->start) && ((e_value >= q->start) && (e_value <= q->end)))
		{
			q->start = s_value;
			q->mark = 'M';
			return;
		}
		else if((e_value > q->end) && ((s_value >= q->start) && (s_value <= q->end)))
		{
			q->end = e_value;
			q->mark = 'M';
			return;
		}
		else if ((s_value < q->start) && (e_value > q->end))
		{
			q->start = s_value;
			q->end = e_value;
			q->mark = 'M';
			return;
		}
		// all left side
		else if((s_value < q->start) && (e_value < q->start) && (q->left != NULL))
		{
			q->height = q->height + 1;
			insert_tree(q->left,s_value,e_value,ln);
		}
		else if((s_value < q->start) && (e_value < q->start) && (q->left == NULL))
		{
			TreeNode *n = BASE::createTreeNode(s_value,e_value,ln);
			q->left = n;
			q->height = 1;
			return;
		}
		// all right side
		else if((s_value > q->start) && (e_value > q->end) && (q->right != NULL))
		{
			q->height = q->height - 1;
			insert_tree(q->right,s_value,e_value,ln);		
		}
		else if((s_value > q->start) && (e_value > q->end) && (q->right == NULL))
		{
			TreeNode *n = BASE::createTreeNode(s_value,e_value,ln);
			q->right = n;
			q->height = -1;
			return;
		}
	}
	
	ListNode *insert_list(ListNode *h, string key, int s_value, int e_value, int ln)
	{
		ListNode *q ,*g;
		bool found = false;
		if(BASE::hashc(h->id) == BASE::hashc(key))
		{
			found = true;
			TreeNode *t = h->ds;
			insert_tree(t,s_value,e_value,ln);
		}
		else if(BASE::hashc(key) < BASE::hashc(h->id))
		{
			found = true;
			h = BASE::insert_head_listnode(h,key,s_value,e_value,ln);
		}
		else
		{
			q = h;
			g = q->next;
			while(q->next != NULL)
			{
				if(BASE::hashc(q->id) == BASE::hashc(key))
				{
		 			found = true;
					q->chr_count = q->chr_count + 1;
					insert_tree(q->ds,s_value,e_value,ln);
				}
				else if((BASE::hashc(key) > BASE::hashc(q->id)) && (BASE::hashc(key) < BASE::hashc(g->id)) && (g != NULL))
				{
					found = true;
					ListNode *p = BASE::createNode(key,s_value,e_value,ln);
					p->next = q->next;
					q->next = p;
					q = q->next;
				} 
				q = q->next;
				g = g->next;
			}
			if(BASE::hashc(q->id) == BASE::hashc(key))
			{
					found = true;
					q->chr_count = q->chr_count + 1;
					insert_tree(q->ds,s_value,e_value,ln);
			}
			if(!found)
			{
				ListNode *p = BASE::createNode(key,s_value,e_value,ln);
				q->next = p;
				q = q->next;
			}	
		}
	return h;
	}
	
	void preorder_merge(TreeNode *q,string id)
	{
		if(q != NULL)
		{
			preorder_merge(q->left,id);
			cout<<id<<"\t"<<q->start<<"\t"<<q->end<<endl;
			preorder_merge(q->right,id);
		}
	}	

	void merge(ListNode *h)
	{
		ListNode *q = h;
		while(q != NULL)
		{
			if(q->ds == NULL)
			{
				continue;
			}
			else
			{
				preorder_merge(q->ds,q->id);	
			}
			q = q->next;
		}
	}
}

