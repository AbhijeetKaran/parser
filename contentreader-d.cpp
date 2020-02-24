#include<iostream>
#include<sstream>
#include<fstream>
#include<vector>
#include<iterator>
#include<string.h>
#include "nodeStruct_sam.cpp"
#include "nodeStruct_bed.cpp"

#define BED_PARAM {0,1,2}
#define SAM_PARAM {1,2,3,4,8}
#define VCF_PARAM {0,3,4}


using namespace std;
using namespace BASE;
using namespace MERGE_BASE;
using namespace SAM_BASE;

class ContentReader : public ListNode , public samListNode
{
	public:
	string file_name;
	vector<string> files_name;
	int cigar_lc(string cigar);
	ListNode *ContentReader_BEDbase(vector<int> cols);
	ListNode *ContentReader_BEDmerge(vector<int> cols);
	ListNode *ContentReader_SAM(vector<int> cols);
	ListNode *ContentReader_VCF(vector<int> cols);
	samListNode *ContentReader_SAMbase(vector<int> cols);
	
	void ContentFetcher_BEDbase(ListNode *h);
	void ContentFetcher_BEDmerge(ListNode *h);
	void ContentFetcher_BEDtoFASTA(ListNode *h);
	void ContentFetcher_BEDXtoBED(ListNode *h);
	void ContentFetcher_BEDstats(ListNode *h);
	void ContentFetcher_BEDIntersect(vector<int> cols);
	
	void ContentFetcher_SAMbase(samListNode *h);
	void ContentFetcher_SAMtoBED();
	void ContentFetcher_SAMtoFASTA();
	void ContentFetcher_SAMtoFASTQ();
	void ContentFetcher_SAMstat(samListNode *h);
	
	ContentReader(string fnames)
	{
		file_name = fnames;
	}
	
	ContentReader(vector<string> fnames)
	{
		files_name = fnames;
	}
	
};

int ContentReader::cigar_lc(string cigar)
{
	return 0;
}

// BED base FUNCTIONS
ListNode *ContentReader::ContentReader_BEDbase(vector<int> cols)
{
	ifstream mfh;
	ListNode *h;
	h = NULL;
	mfh.open(file_name,ios::in);
	string str;
	int line_number = 0;
	while(getline(mfh,str))
	{
		line_number = mfh.tellg();
		int op = str.length();
		line_number = line_number - (op+1);
		stringstream buffer(str);
		string element;
		int coln = 0;
		string key;
		int s_value;
		int e_value;
		while(getline(buffer,element,'\t'))
		{
				if (coln == cols[0])
				{
					key = element;
				}
				if(coln == cols[1])
				{
					s_value = stoi(element);
				}
				if(coln == cols[2])
				{
					e_value = stoi(element);
				}
				coln++;
		}
		if(h == NULL)
		{
			h = BASE::createNode(key,s_value,e_value,line_number);
		}
		else
		{
			h = BASE::insert_list(h,key,s_value,e_value,line_number);
		}
	}
	mfh.close();
	return h;
}

void ContentReader::ContentFetcher_BEDbase(ListNode *h)
{
	ifstream mfh;
	extern vector<vector <int>> line_list;
	BASE::sort(h);
	for(int ptr = 0; ptr < line_list.size(); ptr++)
	{
		for(int dptr = 0; dptr < line_list[ptr].size(); dptr++)
		{
			int line_number = line_list[ptr][dptr];
			mfh.open(file_name);
			mfh.seekg(line_number,ios::beg);
			string line;
			getline(mfh,line);
			cout<<line<<endl;
			mfh.close();
		}
	}
}

//BED merge functions
ListNode *ContentReader::ContentReader_BEDmerge(vector<int> cols)
{
	ifstream mfh;
	ListNode *h;
	h = NULL;
	mfh.open(file_name,ios::in);
	string str;
	int line_number = 0;
	while(getline(mfh,str))
	{
		stringstream buffer(str);
		string element;
		int coln = 0;
		string key;
		int s_value;
		int e_value;
		line_number = mfh.tellg();
		int op = str.length();
		line_number = line_number - (op+1);
		while(getline(buffer,element,'\t'))
		{
				if (coln == cols[0])
				{
					key = element;
				}
				if(coln == cols[1])
				{
					s_value = stoi(element);
				}
				if(coln == cols[2])
				{
					e_value = stoi(element);
				}
				coln++;
		}
		if(h == NULL)
		{
			h = BASE::createNode(key,s_value,e_value,line_number);
		}
		else
		{
			MERGE_BASE::insert_list(h,key,s_value,e_value,line_number);
		}
	}
	mfh.close();
	return h;
}

void ContentReader::ContentFetcher_BEDmerge(ListNode *h)
{
	MERGE_BASE::merge(h);
}

//BED extra functions 
void ContentReader::ContentFetcher_BEDXtoBED(ListNode *h)
{
	ifstream mfh;
	extern vector<vector <int>> line_list;
	BASE::sort(h);
	for(int ptr = 0; ptr < line_list.size(); ptr++)
	{
		for(int dptr = 2; dptr < line_list[ptr].size(); dptr++)	
		{
			int line_number = line_list[ptr][dptr];
			mfh.open(file_name);
			mfh.seekg(line_number,ios::beg);
			string line;
			getline(mfh,line);
			stringstream buffer(line);
			string needed_col;
			int col_n = 0;
			while(getline(buffer,needed_col,'\t'))
			{
				if(col_n < 3)
				{
					cout<<needed_col<<"\t";
				}
				col_n++;
			}
			cout<<endl;
			mfh.close();
		}
	}
}

void ContentReader::ContentFetcher_BEDstats(ListNode *h)
{
	ListNode *q;
	q = h;
	int nodes = 0;
	int chr_nodes = 0;
	while(q != NULL)
	{
		nodes++;
		chr_nodes = chr_nodes + q->chr_count;
		cout<<q->id<<"\t"<<q->chr_count<<endl;
		q = q->next;
	}
	cout<<"number of chr : "<<nodes<<endl;
	cout<<"Total number of reads : "<<chr_nodes<<endl;
}

//SAM FUNCTIONS
samListNode *ContentReader::ContentReader_SAMbase(vector<int> cols)
{
	ifstream mfh;
	samListNode *h;
	h = NULL;
	mfh.open(file_name,ios::in);
	string str;
	int line_number = 0;
	while(getline(mfh,str))
	{
		line_number = mfh.tellg();
		int op = str.length();
		line_number = line_number - (op+1);
		stringstream buffer(str);
		string element;
		int coln = 0;
		string key;
		int s_value;
		int l_value;
		int bit_value;
		int mapq_value;
		while(getline(buffer,element,'\t'))
		{
				if (coln == cols[0])
				{
					bit_value = stoi(element);
				}
				if(coln == cols[1])
				{
					key = element;
				}
				if(coln == cols[2])
				{
					s_value = stoi(element);
				}
				if(coln == cols[3])
				{
					mapq_value = stoi(element);
				}
				if(coln == cols[4])
				{
					l_value = stoi(element);
				}
				coln++;
		}
		if(h == NULL)
		{
			h = SAM_BASE::createNode(key,s_value,l_value,bit_value,mapq_value,line_number);
		}
		else
		{
			h = SAM_BASE::insert_list(h,key,s_value,l_value,bit_value,mapq_value,line_number);
		}
	}
	mfh.close();
	return h;
}

void ContentReader::ContentFetcher_SAMbase(samListNode *h)
{
	ifstream mfh;
	extern vector<vector <int>> sam_line_list;
	SAM_BASE::sort(h);	
	for(int ptr = 0; ptr < sam_line_list.size(); ptr++)
	{
		for(int dptr = 0; dptr < sam_line_list[ptr].size(); dptr++)	
		{
			int line_number = sam_line_list[ptr][dptr];
			mfh.open(file_name);
			mfh.seekg(line_number,ios::beg);
			string line;
			getline(mfh,line);
			cout<<line<<endl;
			mfh.close();
		}
	}
}

void ContentReader::ContentFetcher_SAMtoFASTQ()
{
	ifstream mfh;
	mfh.open(file_name);
	string line;
	while(getline(mfh,line))
	{
		stringstream buffer(line);
		string needed_col;
		int col_n = 0;
		while(getline(buffer,needed_col,'\t'))
		{
			if(col_n == 0)
			{
				cout<<needed_col<<endl;
			}
			else if(col_n == 9)
			{
				cout<<needed_col<<endl<<"+"<<endl;
			}
			else if(col_n == 10)
			{
				cout<<needed_col<<endl;
			}
			col_n++;
		}
	}
	mfh.close();
}

void ContentReader::ContentFetcher_SAMtoFASTA()
{
	ifstream mfh;
	mfh.open(file_name);
	string line;
	while(getline(mfh,line))
	{
		stringstream buffer(line);
		string needed_col;
		int col_n = 0;
		while(getline(buffer,needed_col,'\t'))
		{
			if(col_n == 0)
			{
				cout<<">"<<needed_col<<endl;
			}
			else if(col_n == 9)
			{
				cout<<needed_col<<endl;
			}
			col_n++;
		}
	}
	mfh.close();
}

void ContentReader::ContentFetcher_SAMtoBED()
{
	ifstream mfh;
	mfh.open(file_name);
	string line;
	while(getline(mfh,line))
	{
		stringstream buffer(line);
		string needed_col;
		int col_n = 0;
		string q_id;
		string chr_id; 
		int start;
		int t_end; 
		while(getline(buffer,needed_col,'\t'))
		{
			if(col_n == 0)
			{
				q_id = needed_col;
			}
			else if(col_n == 2)
			{
				chr_id = needed_col;
			}
			else if(col_n == 3)
			{
				start = stoi(needed_col);
			}
			else if(col_n == 7)
			{
				t_end = stoi(needed_col);
			}
			col_n++;
		}
		if(chr_id[0] != '*')
			cout<<chr_id<<"\t"<<start<<"\t"<<t_end<<"\t"<<q_id<<endl;
	}
	mfh.close();
}

void ContentReader::ContentFetcher_SAMstat(samListNode *h)
{
	samListNode *q = h;
	int nodes = 0;
	int chr_nodes = 0;
	cout<<"#@MAP"<<endl;
	while(q != NULL)
	{
		nodes++;
		chr_nodes = chr_nodes + q->chr_count;
		if(q->id[0] == '*')
		{
			cout<<"Unmapped Reads"<<"\t"<<q->chr_count<<endl;
		}
		else
			cout<<q->id<<"\t"<<q->chr_count<<endl;
		q = q->next;
	}
	cout<<"#Total number of aligned refrence chr : "<<nodes-1<<endl;
	cout<<"#Total number of reads : "<<chr_nodes<<endl;
}

void ContentReader::ContentFetcher_BEDIntersect(vector<int> cols)
{
		ifstream mfh1;
		ListNode *h;
		h = NULL;
		mfh1.open(files_name[0],ios::in);
		string str;
		int line_number = 0;
		while(getline(mfh1,str))
		{
			
			line_number = mfh1.tellg();
			int op = str.length();
			line_number = line_number - (op+1);
			stringstream buffer(str);
			string element;
			int coln = 0;
			string key;
			int s_value;
			int e_value;
			while(getline(buffer,element,'\t'))
		{
				if (coln == cols[0])
				{
					key = element;
				}
				if(coln == cols[1])
				{
					s_value = stoi(element);
				}
				if(coln == cols[2])
				{
					e_value = stoi(element);
				}
				coln++;
		}
		if(h == NULL)
		{
			h = BASE::createNode(key,s_value,e_value,line_number);
		}
		else
		{
			h = BASE::insert_list(h,key,s_value,e_value,line_number);
		}
	}
	mfh1.close();
	ListNode *d = h;
	for(vector<string>::iterator ptr = files_name.begin()+1;ptr < files_name.end();ptr++)
	{ 	
		cout<<"A = "<<files_name[0]<<"\t"<<"B = "<<*ptr<<endl;
		ifstream mfh2;
		mfh2.open(*ptr,ios::in);
		string str1;
		while(getline(mfh2,str1))
		{
			stringstream buffer(str1);
			string element1;
			int coln = 0;
			string key;
			int s_value;
			int e_value;
			while(getline(buffer,element1,'\t'))
			{
					if (coln == cols[0])
					{
						key = element1;
					}
					if(coln == cols[1])
					{
						s_value = stoi(element1);
					}
					if(coln == cols[2])
					{
						e_value = stoi(element1);
					}
					coln++;
			}
			BASE::searchlist(d,key,s_value,e_value);
			mfh2.close();
		}
		cout<<endl;
	}
}

int main()
{
	ContentReader fp("true.bed");
	//vector<string> n = {"top10.bed","to10.bed","top2.bed"};
	//ContentReader fp(n);
	ListNode *h = fp.ContentReader_BEDbase(BED_PARAM);
	//fp.ContentFetcher_BEDIntersect(BED_PARAM);
	//ListNode *h = fp.ContentReader_BEDmerge(BED_PARAM);
	fp.ContentFetcher_BEDbase(h);
	//fp.ContentFetcher_BEDmerge(h);
	//fp.ContentFetcher_BEDXtoBED(h);
	//fp.ContentFetcher_BEDstats(h);
	
	//samListNode *h = fp.ContentReader_SAMbase(SAM_PARAM);
	//fp.ContentFetcher_SAMbase(h);
	//fp.ContentFetcher_SAMstat(h);
	//fp.ContentFetcher_SAMtoFASTQ();
	//fp.ContentFetcher_SAMtoFASTA();
	//fp.ContentFetcher_SAMtoBED();
	return 0;
}

