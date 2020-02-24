#include<iostream>
#include<string>
#include <cstring>
#include "contentreader-d.cpp"
using namespace std;

#define PARAM_CHECK(param,paramlength,actuallength) (strncmp(argv[i],param,min(actuallength,paramlength))==0) && (actuallength == paramlength)
#define PARAM_F_CHECK(param,paramlength,actuallength) (strncmp(argv[i+1],param,min(actuallength,paramlength))==0)
#define BED_PARAM {0,1,2}
#define SAM_PARAM {1,2,3,4,8}
#define VCF_PARAM {0,3,4}

#define CONVERT_OPTION
#define COMPARE_OPTION
#define SORT_OPTION
#define STATS_OPTION
#define MERGE_OPTION



bool _option = false;
bool _function = false;
bool _input = false;
bool CONVERT = false;
bool COMPARE = false;
bool SORT = false;
bool STATS = false;
bool MERGE = false;
bool DISPLAY = false;


std::ostream& bold_on(std::ostream& os)
{
    return os << "\e[1m";
}

std::ostream& bold_off(std::ostream& os)
{
    return os << "\e[0m";
}

void function_help(string option)
{
	if(option.compare("convert") == 0)
		CONVERT = true;
	else if(option.compare("compare") == 0)
		COMPARE = true;
	else if(option.compare("sort") == 0)
		SORT = true;
	else if(option.compare("stats") == 0)
		STATS = true;
	else if(option.compare("merge") == 0)
		MERGE = true;
	else if(option.compare("display") == 0)
		DISPLAY = true;
	else
	{
		CONVERT = true;
		COMPARE = true;
		SORT = true;
		STATS = true;
		MERGE = true;
		DISPLAY = true;
	}
	if(CONVERT)
	{
		cout<<endl;
		cout<<bold_on<<"xtools -op/--option convert -f/--function <options given below>"<<bold_off<<endl;
		cout<<"		samtobam	: SAM to BAM"<<endl;
		cout<<"		samtobed	: SAM to BED"<<endl;
		cout<<"		samtofasta	: SAM to FASTA"<<endl;
		cout<<"		samtofastq	: SAM to FASTQ"<<endl;
	}
	if(COMPARE)
	{
		cout<<endl;
		cout<<bold_on<<"xtools -op/--option compare -f/--function <options given below>"<<bold_off<<endl;
		cout<<"		samtosam	: compare two sam files"<<endl;
		cout<<"		samtobed	: compare a sam file to bed file"<<endl;
		cout<<"		samtovcf	: compare a sam file to vcf file"<<endl;
		cout<<"		bedtosam	: compare a bed file to a sam file"<<endl;
		cout<<"		bedtobed	: compare two bed files"<<endl;
		cout<<"		bedtovcf	: compare a bed file to a vcf file"<<endl;
		cout<<"		vcftosam	: compare a vcf file to a sam file"<<endl;
		cout<<"		vcftobed	: compare a vcf file to a bed file"<<endl;
		cout<<"		vcftovcf	: compare a vcf file to a vcf file"<<endl;
	}
	if(SORT)
	{
		cout<<endl;
		cout<<bold_on<<"xtools -op/--option sort -f/--function <options given below>"<<bold_off<<endl;
		cout<<"		sam	: sort sam file/files"<<endl;
		cout<<"		bam	: sort bam file/files"<<endl;
		cout<<"		bed	: sort bed file/files"<<endl;
		cout<<"		vcf	: sort vcf file/files"<<endl;
	}
	if(STATS)
	{
		cout<<endl;
		cout<<bold_on<<"xtools -op/--option stats -f/--function <options given below>"<<bold_off<<endl;
		cout<<"		sam/bam	: SAM/BAM file"<<endl;
		cout<<"		bed	: BED file"<<endl;
		cout<<"		vcf	: VCF file"<<endl;
		cout<<"		fasta	: FASTA file"<<endl;
		cout<<"		fastq	: FASTQ file"<<endl;	
	}
	if(MERGE)
	{
		cout<<endl;
		cout<<bold_on<<"xtools -op/--option merge -f/--function <options given below>"<<bold_off<<endl;
		cout<<"		sam	: merge two or more SAM files"<<endl;
		cout<<"		bed	: merge two or more SAM files"<<endl;
		cout<<"		vcf	: merge two or more VCF files"<<endl;
		cout<<"		fasta	: merge two or more FASTA files"<<endl;
		cout<<"		fastq	: merge two or more FASTQ files"<<endl;	
	}
	if(DISPLAY)
	{
		cout<<endl;
		cout<<bold_on<<"xtools -op/--option display -f/--function <options given below>"<<bold_off<<endl;
		cout<<"		sam/bam	: display SAM/BAM file"<<endl;
		cout<<"		bed	: display BED file"<<endl;
		cout<<"		vcf	: display VCF file"<<endl;
		cout<<"		fasta	: display FASTA file"<<endl;
		cout<<"		fastq	: display FASTQ file"<<endl;
	}
}

void option_help()
{
	cout<<bold_on<<"xtools -op/--option <options given below>"<<bold_off<<endl;
	cout<<"		convert	: convert's one format to other"<<endl;
	cout<<"		compare	: compare two or more files"<<endl;
	cout<<"		sort	: sort one or more files"<<endl;
	cout<<"		stats	: gives general statistics of one or more files"<<endl;
	cout<<"		merge	: merge one or more files"<<endl;
	cout<<"		display	: display content of one or more files"<<endl;
}


void input_help()
{
	cout<<bold_on<<"xtools -op/--option display -f/--function [select one from function options] -i/--input_file <options given below>"<<bold_off<<endl;
	
}

void xtool_usage()
{
	cout<<endl<<"Usage :"<<endl;
	cout<<"xtools -op <operation> -f <function> -i <input file/files(seperated by comma) name> -o [output file/files(seperated by comma) name]"<<endl<<endl;
}

void xtools_help()
{
	cout<<bold_on<<"xtools version 1.0. use option -h or --help for manual."<<bold_off<<endl;
	cout<<"<<<<<<little more description>>>>>>"<<endl;
	xtool_usage();
	cout<<"Commands :"<<endl;
	option_help();
	cout<<endl;
	function_help("all");
	cout<<endl;
	input_help();
	cout<<endl<<endl;
	cout<<"Developed by ABHIJEET KARAN @ CCMB"<<endl;
	cout<<"licensed and author info"<<endl;
}

int main(int argc,char* argv[])
{
	if(argc-1 < 1)
	{
		xtools_help();
	}
	else
	{	
		for(int i = 1;i < argc;i++)
		{
			int paramlength = (int)strlen(argv[i]);
			if((PARAM_CHECK("-h",2,paramlength)) || (PARAM_CHECK("--help",6,paramlength)))
			{
				xtools_help();
			}
			if((PARAM_CHECK("-op",3,paramlength)) || (PARAM_CHECK("--option",6,paramlength)))
			{
				if((argv[i+1] == NULL) || (PARAM_F_CHECK("-",1,paramlength)))
				{
					cout<<"Error : Option parameter missing"<<endl;
					cout<<endl;
					xtool_usage();
					cout<<endl;
					exit(1);
				}
				else if((PARAM_F_CHECK("convert",7,paramlength)))
				{
					CONVERT=true;
				}
				else if((PARAM_F_CHECK("compare",7,paramlength)))
				{
					COMPARE=true;
				}
				else if((PARAM_F_CHECK("sort",4,paramlength)))
				{
					SORT=true;
				}
				else if((PARAM_F_CHECK("stats",5,paramlength)))
				{
					STATS=true;
				}
				else if((PARAM_F_CHECK("merge",5,paramlength)))
				{
					MERGE=true;
				}
				else if((PARAM_F_CHECK("display",7,paramlength)))
				{
					DISPLAY=true;
				}
				else
				{
					cout<<"Invalid option "<<argv[i+1]<<endl;
					
				}
			}
			if(CONVERT)
			{
				if((PARAM_CHECK("-f",2,paramlength)) || (PARAM_CHECK("--function",10,paramlength)))
				{	
					if((argv[i+1] == NULL) || (PARAM_F_CHECK("-",1,paramlength)))
					{
						xtools_help();
						function_help("convert");
					}
					else if(DISPLAY)
					{
						
					}
					
				}
			}
			if((PARAM_CHECK("-i",2,paramlength)) || (PARAM_CHECK("--input",7,paramlength)))
			{
				if((argv[i+1] == NULL) || (PARAM_F_CHECK("-",1,paramlength)))
				{
					cout<<"Input file missing !"<<endl;
					exit(1);
				}
				else if(CONVERT)
				{
					if(argv[i+1] == NULL)
						function_help("convert");
				}
				else if(COMPARE)
				{
				}
				else if(SORT)
				{
					if(argv[i+1] == NULL)
						function_help("sort");
					else
					{
						ContentReader fp(argv[i+1]);
						ListNode *h = fp.ContentReader_BEDbase(BED_PARAM);
						fp.ContentFetcher_BEDbase(h);
					}
				}
				else if(STATS)
				{
					if(argv[i+1] == NULL)
						function_help("stats");
					else
					{
						ContentReader fp(argv[i+1]);
						ListNode *h = fp.ContentReader_BEDbase(BED_PARAM);
						fp.ContentFetcher_BEDstats(h);
					}
				}
				else if(MERGE)
				{
					if(argv[i+1] == NULL)
						function_help("merge");
					else
					{
						ContentReader fp(argv[i+1]);
						ListNode *h = fp.ContentReader_BEDbase(BED_PARAM);
						fp.ContentFetcher_BEDmerge(h);
					}
				}
				else if(COMPARE)
				{
					if(argv[i+1] == NULL)
						function_help("Compare");
					else
					{
						ContentReader fp(argv[i+1]);
						ListNode *h = fp.ContentReader_BEDbase(BED_PARAM);
						//fp.ContentFetcher_BEDIntersect(h);
					}
				
				}
			}
		}
	}
}

