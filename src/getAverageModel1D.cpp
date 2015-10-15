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
    getAverageModel1d  -I <inputfile> \n\
                       -N <iteration number>\n\
"};

int main(int argc, char  *argv[])
{

	int 	oc;
	char 	ec;
	static  struct  option _Option[]=
	{
		{"input",		required_argument, 	NULL,	'I'},
		{"iteration",	optional_argument,	NULL,	'N'},
		{0,				0,					0,		0}
	};
	int 	_Index;
	char 	*str_infile,
			*str_iteration;
	int 	flag_infile        =0,
			flag_iteration     =0; 
	while( (oc = getopt_long(argc,argv,":I:N:H", _Option, &_Index)) != -1 )
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
	if(flag_infile != 1)
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

	m3d.ModelAverage(&zs,&vs,itn);

	for (int i = 0; i < zs.size(); ++i)
	{
		fprintf(stderr, "%-6d %10.4lf %10.4lf\n",i,zs[i], vs[i] );
	}

	fclose(fpin);
	return 0;

	return 0;
}