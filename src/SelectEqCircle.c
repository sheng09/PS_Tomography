#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <string.h>

#define  PI 3.1415926535
#define  LAT2T(x) (0.5*PI-(x)/180.0*PI)
#define  LON2F(x) ((x)/180.0*PI)
#define  T2LAT(x) ((0.5*PI-x)*180.0/PI)
#define  F2LON(x) ((x)*180.0/PI)
#define  DIS(f1,t1,f2,t2) (180.0/PI*acos(sin((t1))*sin((t2))*cos((f1)-(f2))+cos((t1))*cos((t2))))

static char HMSG[]=
{"\n\
SelectEqCircle  - Select earthquakes given a central point and a radius interval.\n\
\n\
Usage: SelectEqCircle  -I <infile name>\n\
                 -C <longitude/latitude>\n\
                 -R <radius floor/radius ceil>  [0 < radius < 180]\n\
                 -N <number of headlines>\n\
Note:\n\
The format of input data is:\n\
longitude  latitude  otherinfos\n\
\n\
Description:\n\
          The longitude and latitude is in DEGREE format.\n\
              Longitude: [0,360], Latitude: [-90,90].\n\
          The radius is in DEGREE format.\n\
              Radius: (0,180)\n\
Example:\n\
\n\
          SelectEqCircle -I in.dat -C 110.0/35.4 -R 30.0/120.0 -N 20 \n\
\n\
Author: Wangsheng, IGGCAS, PRC.\n\
Email:  wangsheng.cas@gmail.com\
\n\
"};
int main(int argc, char  *argv[])
{
	char  *str_file;
	char  *str_center;
	char  *str_radius;
	char  *str_n;
	int   flag_file    = 0,
	      flag_center  = 0,
		  flag_radius  = 0,
		  flag_n       = 0;
    int   oc;
    char  ec;
    int   _Index;
    static struct  option _Option[]=
    {
    	{"infile",  required_argument,  NULL,   'I'},
    	{"center",  required_argument,  NULL,   'C'},
    	{"radius",  required_argument,  NULL,	'R'},
    	{"number",  optional_argument,  NULL,   'N'},
    	{0,0,0,0}
    };
    while( (oc= getopt_long(argc, argv, ":I:C:R:N:H", _Option, &_Index)) != -1 )
    {
    	switch(oc)
    	{
    		case 'I':
    			str_file     = optarg;
    			flag_file    = 1;
    			break;
    		case 'C':
    			str_center   = optarg;
    			flag_center  = 1;
    			break;
    		case 'R':
    			str_radius   = optarg;
    			flag_radius  = 1;
    			break;
    		case 'N':
    			str_n        = optarg;
    			flag_n       = 1;
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
    if(flag_file != 1 || flag_radius != 1 || flag_center != 1 || flag_n != 1)
    {
    	fprintf(stderr, " More arguments are required!\n");
    	//fprintf(stderr, " %s\n", HMSG );
    	exit(0);
    }
    ////////////////////////////////////////////
    FILE   *fp;
    double lon0,lat0;
    double f0,  t0;
    double r0,r1;
    int    n;
    if((fp = fopen(str_file,"r"))== NULL)
    {
    	fprintf(stderr, "Err: can't open file '%s' for read!\n", str_file);
    	exit(0);
    }
    sscanf(str_center,"%lf/%lf",&lon0,&lat0);
    if(lon0 < 0 || lon0 > 360 || lat0 < -90 || lat0 > 90)
    {
    	fprintf(stderr, "Err: invalid center coordinates! '%s'\n",str_center );
    	exit(0);
    }
    f0 = LON2F(lon0);
    t0 = LAT2T(lat0);
    sscanf(str_radius,"%lf/%lf",&r0,&r1);
    if(r0 < 0.0 || r1 < 0.0 || r0 >= r1)
    {
    	fprintf(stderr, "Err: invalid radius interval! '%s'\n",str_radius );
    	exit(0);
    }
    sscanf(str_n,"%d",&n);
    if(n < 0)
    {
    	fprintf(stderr, "Err: invalid number of headlines! '%s'\n",str_n );
    	exit(0);
    }
    ////////////////////////////////////////////////////////
    char Line[4096];
    int  i = 0;
    double lon,lat;
    double dis;
    double f,t;
    while(1)
    {
    	++i;
    	memset(Line,0,4096);
    	if( fgets(Line,4096,fp) == NULL) break;
    	if(i == n) break;
    }
    while(1)
    {
    	memset(Line,0,4096);
    	if( fgets(Line,4096,fp) == NULL) break;
    	sscanf(Line,"%lf,%lf",&lon,&lat);
        f = LON2F(lon);
        t = LAT2T(lat);
    	dis = DIS(f0,t0,f,t);
    	//printf("%d %lf %lf\n",i,lat,lon );
    	if(dis < r1 && dis > r0)
    	{
    		Line[67] = 0;
    		printf("%s\n", Line);
    	}
    }
    fclose(fp);
	return 0;
}