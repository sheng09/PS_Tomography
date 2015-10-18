#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <getopt.h>
#include "Eqsta.h"
#include "VelModel.h"


using namespace std;

static char _HMSG[]=
{"\n\
Description: Given 3d model and a point(lon,lat), return 1d model\n\
    getModel1D  -I <inputfile> \n\
                -N <iteration number>\n\
                -P <lon/lat>\n\
\n\
Example:\n\
    getModel1D -I RTM.out -N 0 -P 120/38\n\
"};

int main(int argc, char  *argv[])
{

	int 	oc;
	char 	ec;
	static  struct  option _Option[]=
	{
		{"input",		required_argument, 	NULL,	'I'},
		{"iteration",	optional_argument,	NULL,	'N'},
		{"point",       required_argument,  NULL,   'P'},
		{0,				0,					0,		0}
	};
	int 	_Index;
	char 	*str_infile,
			*str_iteration,
			*str_point;
	int 	flag_infile        =0,
			flag_iteration     =0,
			flag_point         =0;
	double  x,y;
	while( (oc = getopt_long(argc,argv,":I:N:P:H", _Option, &_Index)) != -1 )
	{
		switch(oc)
		{
			case 'I':
				str_infile      = optarg;
				flag_infile     = 1;
				break;
			case 'N':
				str_iteration   = optarg;
				flag_iteration  = 1;
				break;
			case 'P':
				str_point       = optarg;
				flag_point      = 1;
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
	if(flag_infile != 1 || flag_point != 1)
	{
		fprintf(stderr, "More argumets required\n");
		fprintf(stderr, "%s\n",_HMSG );
		exit(0);
	}
	///////////////////////////////////////

	FILE *fpin;
	Model3D m3d;
	if((fpin = fopen(str_infile,"r")) == NULL)
	{
		fprintf(stderr, "Err: in open file for read: %s\n",str_infile );
		exit(0);
	}

	m3d.ReadModel3D(&fpin);

	vector<double> zs;
	vector<double> vs;

	int itn;
	if(flag_iteration == 1)
	{
		sscanf(str_iteration,"%d",&itn);
	}
	else
	{
		itn = 0;
	}

	sscanf(str_point, "%lf/%lf", &x, &y);
	m3d.Model1D(&zs,&vs,itn, x, y );

	for (int i = 0; i < zs.size(); ++i)
	{
		fprintf(stderr, "%-6d %10.4lf %10.4lf\n",i,zs[i], vs[i] );
	}

	fclose(fpin);
	return 0;
}