#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <math.h>
#include "Eqsta.h"
#include <getopt.h>
using namespace std;

static char _HMSG[]=
{"\n\
	pickSta  -T <station inputfile> -P <station outputfile>  [-G <gmtplotfile>] -R <latmin/latmax/lonmin/lonmax>\n\
\n\
    Arguments:\n\
        required    -T\n\
                    -P\n\
                    -R\n\
        optional    -G\n\
"};

int main(int argc, char  *argv[])
{
	int 	oc;
	char 	ec;
	static  struct  option _Option[]=
	{
		{"stain",	required_argument,	NULL,	'T'},
		{"staout",	required_argument,	NULL,	'P'},
		{"region",	required_argument,	NULL,	'R'},
		{"stagmt",	optional_argument,	NULL,	'G'},
		{"help",	optional_argument, 	NULL,	'H'},
		{0,			0,					0,		0}
	};
	int 	_Index;
	char 	*str_stainfile,
			*str_staoutfile,
			*str_region,
			*str_gmt;
	int 	flag_stainfile     =0, 
			flag_staoutfile    =0, 
			flag_region        =0,
			flag_gmt           =0; 
	while( (oc = getopt_long(argc,argv,":T:P:R:G:H", _Option, &_Index)) != -1 )
	{
		switch(oc)
		{
			case 'T':
				str_stainfile   = optarg;
				flag_stainfile  = 1;
				break;
			case 'P':
				str_staoutfile	= optarg;
				flag_staoutfile = 1;
				break;
			case 'R':
				str_region		= optarg;
				flag_region     = 1;
				break;
			case 'G':
				str_gmt         = optarg;
				flag_gmt        = 1;
				break;
			case 'H':
				fprintf(stderr, "%s\n",_HMSG );
				exit(0);
			case '?':
				ec = (char) (oc);
				fprintf(stderr, "\'-%c\' OR \'--%s\'required parameter\n",ec, _Option[_Index].name);
				exit(0);
			case ':':
				fprintf(stderr, "Wrong argument!\n");
				exit(0);
		}
	}
	if(flag_stainfile != 1 || flag_staoutfile != 1 || flag_region != 1)
	{
		fprintf(stderr, "More argumets required\n");
		fprintf(stderr, "%s\n",_HMSG );
		exit(0);
	}
	///////////////////////////////////////
	double	latmin,
			latmax,
			lonmin,
			lonmax;
	vector<Station>   sta;
	FILE *fpsta;
	FILE *fpOsta;
	FILE *fpOgmt;
	if( (fpsta= fopen(str_stainfile,"r")) ==NULL )
	{
		fprintf(stderr, "Err: Can't open file for read: %s\n",str_stainfile );
		exit(0);
	}
	if( (fpOsta= fopen(str_staoutfile,"w")) ==NULL )
	{
		fprintf(stderr, "Err: Can't open file for write: %s\n",str_staoutfile );
		exit(0);
	}
	if(flag_gmt == 1)
	{
		if( (fpOgmt= fopen(str_gmt,"w")) ==NULL )
		{
			fprintf(stderr, "Err: Can't open file for write: %s\n",str_gmt );
			exit(0);
		}
	}

	int Ns;

	sscanf(str_region,"%lf/%lf/%lf/%lf", &latmin, &latmax, &lonmin, &lonmax);

	ReadSta(&sta, &Ns ,&fpsta);

	for (int i = 0; i < sta.size(); ++i)
	{
		sta[i].PickSta( latmin,  latmax,  lonmin,  lonmax);
	}
	for (int i = 0; i < sta.size(); ++i)
	{
		sta[i].OutSta(&fpOsta);
	}
	if(flag_gmt == 1)
	{
		for (int i = 0; i < sta.size(); ++i)
		{
			sta[i].OutSta4GMT(&fpOgmt);
		}
	}

	fclose(fpsta);
	fclose(fpOsta);
	if(flag_gmt == 1)	fclose(fpOgmt);
	sta.clear();
	return 0;
}

