#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>

static char HMSG[]=
{"\n\
getEq4Profile  - Pick earthquake given Profile and distance.\n\
\n\
Usage:    getEq4Depth  -I <inputfile>\n\
                       -L <lon1/lat1/lon2/lat2>\n\
                       -D <distance>\n\
Description:\n\
          The format of <inputfile> is \"longitude latitude depth magnitude\"\n\
          The distance is the max distance(km) perpendicular to the profile line\n\
Example:\n\
          getEq4Depth -I eq.dat -L 107.0/36.0/113.0/41.5 -D10.0\n\
\n\
Author: Wangsheng, IGGCAS, PRC.\n\
Email:  wangsheng.cas@gmail.com\
\n\
"};
int main(int argc, char *argv[])
{
	char  *str_infile = NULL;
	char  *str_pro    = NULL;
    char  *str_dis    = NULL;
	int   	flag_infile = 0,
			flag_pro    = 0,
            flag_dis    = 0;
    int  oc;
    char ec;
    int  _Index;
    static struct  option _Option[]=
    {
    	{"input",  required_argument,  NULL,   	'I'},
    	{"line",   required_argument,  NULL,	'L'},
        {"dis",    required_argument,  NULL,    'D'},
    	{0,0,0,0}
    };
    while( (oc= getopt_long(argc, argv, ":I:L:D:H", _Option, &_Index)) != -1 )
    {
    	switch(oc)
    	{
    		case 'I':
    			str_infile  = optarg;
    			flag_infile = 1;
    			break;
    		case 'L':
    			str_pro   = optarg;
    			flag_pro  = 1;
    			break;
            case 'D':
                str_dis   = optarg;
                flag_dis  = 1;
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
    if(flag_dis != 1 || flag_pro != 1 || flag_infile != 1)
    {
    	fprintf(stderr, " More arguments are required!\n");
    	fprintf(stderr, " %s\n", HMSG );
    	exit(0);
    }
    ////////////////////////////////////////////
    FILE 	*fp;
    char    inLine[1024];
    float  lon1,
            lat1,
            lon2,
            lat2;
    float  k,b,scale;
    float  dis;
    float  lon,
    		lat,
    		dep,
            mag;
    if( (fp =fopen(str_infile,"r")) == NULL)
    {
    	fprintf(stderr, " Err: Can't open file for read: %s\n", str_infile);
    	exit(0);
    }
    sscanf(str_pro,"%f/%f/%f/%f",&lon1,&lat1,&lon2,&lat2);
    k = (lat2-lat1)/(lon2-lon1);
    b = lat1 - k*lon1;
    scale = sqrt(k*k+1.0);
    sscanf(str_dis,"%f",&dis);
    dis = dis * 0.0091;
    while(1)
    {
    	memset(inLine,0,sizeof(char)*1024);
    	if( fgets(inLine,1024, fp) == NULL) break;
    	sscanf(inLine,"%f %f %f %f",&lon, &lat, &dep, &mag);
        if( fabs((k*lon-lat+b)/scale) <= dis )
    	{
    		printf("%-20.8f %-20.8f %-20.8f %-20.8f\n",lon,lat,dep,mag );
    	}
    }
    fclose(fp);
	return 0;
}