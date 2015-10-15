#include <stdio.h>
#include <stdlib.h>
#include "getopt.h"
#include <math.h>
static char HMSG[]=
{"\n\
       GenerateGMT4check  -I <inputfile> \n\
                          -D <depth>\n\
                          -N <number>\n\
                          -H \n\
\n\
Input file format:\n\
360.00 125.00  41.50   8.85 0.0000    8       0.0\n\
\n\
"};

int main(int argc, char  *argv[])
{
	char  	*str_infile = NULL,
			*str_depth  = NULL,
			*str_number = NULL;
	int     flag_infile  = 0,
			flag_depth   = 0,
			flag_number  = 0;
	int oc;
	char ec;
	int  Index;
	static struct option Options[]=
    {
        {"infile",  required_argument, 	NULL, 'I'},  // disfile
        {"depth",	required_argument,	NULL, 'D'},
        {"number",	required_argument,	NULL, 'N'},
        {"help",    optional_argument, 	NULL, 'H'},
        {0,         0,                 	0,     0}    
    };
	while( (oc = getopt_long(argc, argv, ":I:D:N:H", Options, &Index)) != -1)
	{
		switch(oc)
		{
			case 'I':
				str_infile  = optarg;
				flag_infile = 1;
				break;
			case 'D':
				str_depth  = optarg;
				flag_depth = 1;
				break;
			case 'N':
				str_number = optarg;
				flag_number= 1;
				break;
			case ':':
        	    printf("无效的选项字符");
        	    exit(0);
        	case '?':
        	    ec = (char)optopt;
        	    printf("\'-%c\' OR \'--%s\' 缺少选项参数！\n",ec,Options[Index].name);
        	    exit(0);
        	    break;
		}
	}
	if(flag_depth != 1 || flag_infile != 1 || flag_number != 1)
	{
		fprintf(stderr, " More arguments are required!\n" );
		fprintf(stderr, "%s\n", HMSG);
		exit(0);
	}
	////////////////////////////////////////////////////
	FILE *fpin;
	if( (fpin = fopen(str_infile,"r")) == NULL)
	{
		fprintf(stderr, " Err: Can't open file for read: %s\n",str_infile );
		exit(0);
	}
	double	depth;
	double  tmp,
			lon,
			lat,
			vel1,
			vel2;
	int     n,
			number;
	double  ray;

	sscanf(str_number,"%d",&number);
	sscanf(str_depth,"%lf",&depth);
	while(1)
	{
		       //360.00 125.00  41.50   8.85 0.0000    8       0.0
		if( fscanf(fpin,"%lf %lf %lf %lf %lf %d %lf", &tmp, &lon, &lat, &vel1, &vel2, &n, &ray) == EOF ) break;

		if(tmp == depth && n == number)
		{
			if(vel2 > 0)
			{
				printf("%8.2lf %8.2lf %8.2lf %8.2lf %8.2lf \n",lon,lat,1.0,fabs(vel2*10),ray );
			}
			else
			{
				printf("%8.2lf %8.2lf %8.2lf %8.2lf %8.2lf \n",lon,lat,-1.0,fabs(vel2*10),ray );	
			}
			
		}
	}

	return 0;
}