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
pickDat4dt  - Pick data given residual traveltime requirements.\n\
\n\
Usage:  pickDat4dt  -I <asc9 inputfile>\n\
                    -O <asc9 outputfile>\n\
	                -T <upper limit of residual travel time>\n\
	                -N <min number of station within one record>\n\
Description:\n\
        The format of <asc9 inputfile> is displayed in ./***.dat.\n\
        -T Upper limitation of resdsidual travel time, [0, upper]\n\
        -N Limitation of stations' number is [min, +inf), default value is 1.\n\
Example:\n\
        pickDat4dt -I in.dat -O out.dat -T 5 -N 3 \n\
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
		{"output",	required_argument,	NULL,	'O'},
		{"time",	required_argument,	NULL,	'T'},
		{"min",		optional_argument,	NULL,	'N'},
		{"help",	optional_argument, 	NULL,	'H'},
		{0,			0,					0,		0}
	};
	int 	_Index;
	char 	*str_infile,
			*str_outfile,
			*str_uppdt,
			*str_region,
			*str_min;
	int 	flag_infile        =0, 
			flag_outfile       =0, 
			flag_uppdt     	   =0, 
			flag_min           =0; 
	while( (oc = getopt_long(argc,argv,":I:O:T:N:H", _Option, &_Index)) != -1 )
	{
		switch(oc)
		{
			case 'I':
				str_infile      = optarg;
				flag_infile     = 1;
				break;
			case 'O':
				str_outfile     = optarg;
				flag_outfile    = 1;
				break;
			case 'T':
				str_uppdt  		= optarg;
				flag_uppdt  	= 1;
				break;
			case 'N':
				str_min         = optarg;
				flag_min        = 1;
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
	if(flag_infile != 1 || flag_outfile != 1 || flag_uppdt != 1)
	{
		fprintf(stderr, "More argumets are required\n");
		fprintf(stderr, "%s\n",_HMSG );
		exit(0);
	}
	///////////////////////////////////////
	double	dt;
	int 	minN = 1;
	std::vector<EqSta>   dat;
	FILE *fpdat;
	FILE *fpOdat;
	int   nDat = 0;
	int   nEq  = 0;
	int   nStaMax = 0;
	if(flag_min == 1)
	{
		sscanf(str_min,"%d",&minN);
		if(minN < 1)
		{
			fprintf(stderr, "Err: Argument -N (--min) should be greater than 0 !\n");
			exit(0);
		}
	}
	if( (fpdat= fopen(str_infile,"r")) == NULL )
	{
		fprintf(stderr, "Err: Can't open file for read: %s\n",str_infile );
		exit(0);
	}
	if( (fpOdat= fopen(str_outfile,"w")) == NULL )
	{
		fprintf(stderr, "Err: Can't open file for write: %s\n",str_outfile );
		exit(0);
	}

	sscanf(str_uppdt,"%lf", &dt);
	ReadASC9(&dat, &fpdat);

	for (int i = 0; i < dat.size(); ++i)
	{
		dat[i].PickEqStaDt( dt, minN);
		nDat += dat[i].nsUseful;
		if(nStaMax < dat[i].nsUseful)
		{
			nStaMax = dat[i].nsUseful;
		}
	}
	for (int i = 0; i < dat.size(); ++i)
	{
		dat[i].OutputASC9(&fpOdat);
	}

	fclose(fpdat);
	fclose(fpOdat);
	fprintf(stderr, " Input data filename:    %-128s\n", str_infile);
	fprintf(stderr, " Output data filename:   %-128s\n", str_outfile);
	fprintf(stderr, " Number of earthquake:                %-10d\n",  (int)dat.size() );
	fprintf(stderr, " Max number of record per earthquake: %-10d\n",   nStaMax);
	fprintf(stderr, " Number of dat:                       %-10d\n",   nDat);
	dat.clear();
	return 0;
}


