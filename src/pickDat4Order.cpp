#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <math.h>
#include "Eqsta.h"
#include <getopt.h>

using std::vector;

static char _HMSG[]=
{"\
pickDat4Order  - Pick data given numbers of order.\n\
\n\
Usage:  pickDat4dt  -I <asc9 inputfile>\n\
	                -N <crecord order>\n\
	                -E <Earthquake order>\n\
Description:\n\
        The format of <asc9 inputfile> is displayed in ./***.dat.\n\
        -N  contradicts  -E, only one option is selected!\n\
Example:\n\
        pickDat4dt -I -N 3 \n\
           Output the 3rd record!\n\
        pickDat4dt -I -E 1 \n\
           Output all the record for the 1st earthquake!\n\
\n\
Author: Wangsheng, IGGCAS, PRC.\n\
Email:  wangsheng.cas@gmail.com\
"};

int main(int argc, char  *argv[])
{
	int 	oc;
	char 	ec;
	static  struct  option _Option[]=
	{
		{"input",	required_argument, 	NULL,	'I'},
		{"order",	optional_argument,	NULL,	'N'},
		{"eorder",	optional_argument,	NULL,	'E'},
		{"help",	optional_argument, 	NULL,	'H'},
		{0,			0,					0,		0}
	};
	int 	_Index;
	char 	*str_infile,
			*str_order,
			*str_Eq;
	int 	flag_infile        =0, 
			flag_order         =0,
			flag_Eq            =0;
	while( (oc = getopt_long(argc,argv,":I:N:E:H", _Option, &_Index)) != -1 )
	{
		switch(oc)
		{
			case 'I':
				str_infile      = optarg;
				flag_infile     = 1;
				break;
			case 'E':
				str_Eq  		= optarg;
				flag_Eq  	    = 1;
				break;
			case 'N':
				str_order       = optarg;
				flag_order      = 1;
				break;
			case 'H':
				fprintf(stderr, "%s\n",_HMSG );
				exit(0);
			case ':':
				fprintf(stderr, "Parameters are required for argument!\n" );
				exit(0);
			case '?':
				fprintf(stderr, "Wrong argument!\n");
				exit(0);
		}
	}
	if(flag_infile != 1)
	{
		fprintf(stderr, "Input argument required_argument!\n");
		fprintf(stderr, "%s\n",_HMSG );
		exit(0);
	}
	if(flag_Eq == 1 && flag_order == 1)
	{
		fprintf(stderr, "\"-N\"  and  \"-E\" options contradict!\n");
		exit(0);
	}
	if(flag_Eq == 0 && flag_order == 0)
	{
		fprintf(stderr, "None criterion stated!\n");
		fprintf(stderr, "Nothing is done!\n");
		exit(0);
	}
	///////////////////////////////////////
	FILE *fpdat;
	if( (fpdat= fopen(str_infile,"r")) == NULL )
	{
		fprintf(stderr, "Err: Can't open file for read: %s\n",str_infile );
		exit(0);
	}
	std::vector<EqSta>   dat;
	ReadASC9(&dat, &fpdat);

	if(flag_order == 1)
	{
		int order;
		int j = 1;
		int k = 1;
		sscanf(str_order,"%d",&order);
		if(order < 1)		
		{	
			fprintf(stderr, "Err: \"%s\" is little than 1!\n",str_order );
			exit(0);
		}
		for (int i = 0; i < dat.size(); ++i)
		{
			k += dat[i].ns;
			if( order >= j && order < k)
			{
				for(int ii = 0; ii < dat[i].ns; ++ii)
				{
					dat[i].sta[ii].IsUseful = _NONUSEFUL;
				}
				dat[i].sta[order - j].IsUseful = _USEFUL;
				printf("Order of Eq: %d\n",i+1 );
				dat[i].OutputASC9(&stdout);
				break;
			}
			j += dat[i].ns;
		}
		if(order >= k)
		{
			fprintf(stderr, "Err: \"%s\" is greater than %d !\n",str_order, k-1 );
			exit(0);
		}
	}
	if(flag_Eq    == 1)
	{
		int Eqorder = 1;
		sscanf(str_Eq, "%d",&Eqorder);
		if(Eqorder < 1 || Eqorder > dat.size())
		{
			fprintf(stderr, "Err: \"%s\" is out the range [%d,%d]\n",str_Eq,1,dat.size() );
			exit(0);
		}
		dat[Eqorder-1].OutputASC9(&stdout);
	}

	
	fclose(fpdat);
	dat.clear();
	return 0;
}


