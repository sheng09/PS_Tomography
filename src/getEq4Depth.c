#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

static char HMSG[]=
{"\n\
getEq4Depth  - Pick earthquake given depth interval.\n\
\n\
Usage:    getEq4Depth  -I <inputfile>\n\
                       -P <depmin/depmax>  [depmin<= dep < depmax]\n\
Description:\n\
          The format of <inputfile> is \"longitude latitude depth magnitude\"\n\
          The depth interval is [depmin,depmax)\n\
Example:\n\
          getEq4Depth -I eq.dat -P 10.0/23.0\n\
\n\
Author: Wangsheng, IGGCAS, PRC.\n\
Email:  wangsheng.cas@gmail.com\
\n\
"};
int main(int argc, char *argv[])
{
	char  *str_infile = NULL;
	char  *str_depth  = NULL;
	int   	flag_infile = 0,
			flag_depth  = 0;
    int  oc;
    char ec;
    int  _Index;
    static struct  option _Option[]=
    {
    	{"input",  required_argument,  NULL,   	'I'},
    	{"depth",  required_argument,  NULL,	'P'},
    	{0,0,0,0}
    };
    while( (oc= getopt_long(argc, argv, ":I:P:H", _Option, &_Index)) != -1 )
    {
    	switch(oc)
    	{
    		case 'I':
    			str_infile  = optarg;
    			flag_infile = 1;
    			break;
    		case 'P':
    			str_depth   = optarg;
    			flag_depth  = 1;
    			break;
    		case 'H':
    			fprintf(stderr, " %s\n", HMSG);
    			exit(0);
    		case ':':
				ec = (char) (oc);
				fprintf(stderr, "\'-%c\' OR \'--  \'required parameter\n",ec);
				exit(0);
			case '?':
				fprintf(stderr, "Wrong argument!\n");
				exit(0);
    	}
    }
    if(flag_depth != 1 || flag_infile != 1)
    {
    	fprintf(stderr, " More arguments are required!\n");
    	fprintf(stderr, " %s\n", HMSG );
    	exit(0);
    }
    ////////////////////////////////////////////
    FILE 	*fp;
    char    inLine[1024];
    float  depmin,
    		depmax;
    float  lon,
    		lat,
    		dep,
            mag;
    if( (fp =fopen(str_infile,"r")) == NULL)
    {
    	fprintf(stderr, " Err: Can't open file for read: %s\n", str_infile);
    	exit(0);
    }
    sscanf(str_depth,"%f/%f",&depmin,&depmax);
    while(1)
    {
    	memset(inLine,0,sizeof(inLine));
    	if( fgets(inLine,sizeof(inLine), fp) == NULL) break;
    	sscanf(inLine,"%f %f %f %f",&lon, &lat, &dep, &mag);
    	if(dep < depmax && dep >= depmin)
    	{
    		printf("%-20.8f %-20.8f %-20.8f %-20.8f\n",lon,lat,dep,mag );
    	}
    }
    fclose(fp);
	return 0;
}