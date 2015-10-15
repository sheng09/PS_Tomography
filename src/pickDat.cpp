#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <math.h>
#include "Eqsta.h"
#include <getopt.h>
using std::vector;

static char _HMSG[]=
{"\
pickDat  - Pick data given requirements.\n\
\n\
Usage:  pickDat  -I <asc9 inputfile>\n\
                 -O <asc9 outputfile>\n\
                 -T <station inputfile>\n\
                 [-R <lonmin/lonmax/latmin/latmax>]\n\
                 [-N <min number of station within one record>]\n\
                 [-D <minDistance/maxDistance>]\n\
                 [-W <wavetype>]\n\
Description:\n\
        The format of <asc9 inputfile> is displayed in ./***.dat.\n\
        The format of <station inputfile> is (num1[d] num2[d] lat[f/lf] long[f/lf] height[f/lf]),\n\
        'num1' is the old version station number,while 'num2' is the new version. \n\
        In fact, only num2 is useful, thus, 'num1' can be any int nuPickStaWaveTypember. \n\
        Geographical2 coordinate (lat-lon-h) is used for stations, in which 'h' equals to 'r-R0'.\n\
        Unit of input height is 'm'. Besides, using picked station is recommended\n\n\
        -R Region limitation is [lonmin, lonmax] and [latmin latmax]\n\
        -N Limitation of stations' number is [min, +inf), default value is 1.\n\
        -D Distance Limitation is [minDis, maxDis), default value is [-1, 99999999.0)\n\
        -W <wavetype> is a string, has the [PG/SG/PN/SN/ALL] \n\n\
Example:\n\
        pickDat -I in.dat -O out.dat -T sta.dat -R 117/125/35/41.5 -N 3 -D -1/999999 -W PG/PN\n\
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
		{"stain",	required_argument,	NULL,	'T'},
		{"region",	optional_argument,	NULL,	'R'},
		{"min",		optional_argument,	NULL,	'N'},
		{"distance",optional_argument,	NULL,	'D'},
		{"wavetype",optional_argument,	NULL,	'W'},
		{"help",	optional_argument, 	NULL,	'H'},
		{0,			0,					0,		0}
	};
	int 	_Index = 0;
	char 	*str_infile,
			*str_outfile,
			*str_stainfile,
			*str_region,
			*str_min,
			*str_distance,
			*str_wavetype;
	int 	flag_infile        =0, 
			flag_outfile       =0, 
			flag_stainfile     =0, 
			flag_region        =0,
			flag_min           =0,
			flag_distance      =0,
			flag_wavetype      =0; 
	while( (oc = getopt_long(argc,argv,":I:O:T:R:N:D:W:H", _Option, &_Index)) != -1 )
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
				str_stainfile   = optarg;
				flag_stainfile  = 1;
				break;
			case 'R':
				str_region		= optarg;
				flag_region     = 1;
				break;
			case 'N':
				str_min         = optarg;
				flag_min        = 1;
				break;
			case 'D':
				str_distance    = optarg;
				flag_distance   = 1;
				break;
			case 'W':
				str_wavetype    = optarg;
				flag_wavetype   = 1;
				break;
			case 'H':
				fprintf(stderr, "%s\n",_HMSG );
				exit(0);
			case ':':
				//ec = (char) (oc);
				fprintf(stderr, "Parameters are required for argument!\n" );
				//fprintf(stderr, "\'-%c\' OR \'--%s\'required parameter\n",_Option[_Index].val, _Option[_Index].name);
				exit(0);
			case '?':
				fprintf(stderr, "Wrong argument!\n");
				exit(0);
		}
	}
	if(flag_infile != 1 || flag_outfile != 1 || flag_stainfile != 1)
	{
		fprintf(stderr, "More argumets are required\n");
		fprintf(stderr, "%s\n",_HMSG );
		exit(0);
	}
	if(flag_min != 1 && flag_region != 1 && flag_distance != 1 && flag_wavetype != 1 )
	{
		fprintf(stderr, " None standard has been stated to pick data! \n" );
		fprintf(stderr, " Input data filename:    %-s\n", str_infile);
		fprintf(stderr, " Input station filename: %-s\n", str_stainfile);
		fprintf(stderr, " Have done nothing!\n\n");
		exit(0);
	}
	///////////////////////////////////////
	double	latmin,
			latmax,
			lonmin,
			lonmax;
	int 	minN = 1;
	double  minDis = -1.0,
			maxDis = 99999999.0;
	FLAG    wavetype = 0u;
	std::vector<Station> sta;
	std::vector<EqSta>   dat;
	FILE *fpsta;
	FILE *fpdat;
	FILE *fpOdat;
	int   nSta = 0;
	int   ndat = 0;
	int   nDat = 0;
	int   nDatAll = 0;
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
	if( (fpsta= fopen(str_stainfile,"r")) ==NULL )
	{
		fprintf(stderr, "Err: Can't open file for read: %s\n",str_stainfile );
		exit(0);
	}
	if( (fpOdat= fopen(str_outfile,"w")) == NULL )
	{
		fprintf(stderr, "Err: Can't open file for write: %s\n",str_outfile );
		exit(0);
	}
	if(flag_wavetype == 1)
	{
		int i = 0;
		while(str_wavetype[i++] != '\0')
		{
			if(str_wavetype[i-1] == 'P' && str_wavetype[i] == 'G')
				wavetype = (wavetype | _PGY);
			else if(str_wavetype[i-1] == 'P' && str_wavetype[i] == 'N')
				wavetype = (wavetype | _PNY);
			else if(str_wavetype[i-1] == 'S' && str_wavetype[i] == 'G')
				wavetype = (wavetype | _SGY);
			else if(str_wavetype[i-1] == 'S' && str_wavetype[i] == 'N')
				wavetype = (wavetype | _SNY);
			else if(str_wavetype[i+1] != '\0')
				if(str_wavetype[i-1] == 'A' && str_wavetype[i] == 'L' && str_wavetype[i+1] == 'L')
					wavetype = _ALL;
		}
		if(wavetype == 0u)
		{
			fprintf(stderr, " Err: invalid wavetype limitation %s\n",str_wavetype );
			exit(0);
		}
	}

	int Ns;

	
	ReadASC9(&dat, &fpdat);
	ReadSta(&sta, &Ns ,&fpsta);

	// Complete station info
	for (int i = 0; i < dat.size(); ++i)
	{
		dat[i].CompleteSta(&sta);
	}

	// Pick data according to seismic phase
	//std::cout << minN<<'\n';
	if(flag_wavetype == 1)
	{
		//std::cout << wavetype <<'\n';
		fprintf(stderr, "\n Selected seismic phase: %-s\n",str_wavetype );
		for (int i = 0; i < dat.size(); ++i)
		{
			dat[i].PickEqStaWaveType(wavetype, minN);
		}
	}

	// Pick data according to distance range
	if(flag_distance == 1)
	{
		fprintf(stderr, " Selected Distance:      %-s\n", str_distance);
		sscanf(str_distance,"%lf/%lf",&minDis,&maxDis);
		if(minDis >= maxDis || maxDis <= 0.0)
		{
			fprintf(stderr, "Err: Illegal distance range : \" %s \"!\n", str_distance);
			exit(0);
		}
		for (int i = 0; i < dat.size(); ++i)
		{
			//dat[i].CalculateDelta();
			dat[i].PickEqStaDis(minDis, maxDis, minN);
		}
	}
	
	// Pick data according to 1.region  2.min number of stations
	if( flag_region == 1 )
	{
		fprintf(stderr, " Selected region:        %-s\n", str_region);
		sscanf(str_region,"%lf/%lf/%lf/%lf",  &lonmin, &lonmax, &latmin, &latmax);
		if(latmin > latmax || lonmin > lonmax)	
		{
			fprintf(stderr, "Err: Illegal region range : \" %s \"!\n", str_region);
			exit(0);
		}
		for (int i = 0; i < dat.size(); ++i)
		{
			dat[i].PickEqSta( latmin,  latmax,  lonmin,  lonmax, minN);
			//printf("%d\n",);
		}
	}

	for (int i = 0; i < dat.size(); ++i)
	{
		nDatAll += dat[i].ns;
		if(dat[i].IsUseful == _USEFUL)
		{
			++nEq;
			nDat += dat[i].nsUseful;
			if(nStaMax < dat[i].nsUseful) nStaMax = dat[i].nsUseful;	
		}
	}

	for (int i = 0; i < dat.size(); ++i)
	{
		//printf("%d\n",dat[i].nsUseful );
		//dat[i].OutputASC9(&fpOdat, _PNY);
		dat[i].OutputASC9(&fpOdat);
	}

	fclose(fpsta);
	fclose(fpdat);
	fclose(fpOdat);
	fprintf(stderr, " Input data filename:    %-s\n", str_infile);
	fprintf(stderr, " Input station filename: %-s\n", str_stainfile);
	fprintf(stderr, " Output data filename:   %-s\n", str_outfile);

	
	fprintf(stderr, " Number of total    earthquake:       %-10d\n",  (int)dat.size() );
	fprintf(stderr, " Number of selected earthquake:       %-10d\n",  nEq );
	fprintf(stderr, " Number of sta:                       %-10d\n",Ns);
	fprintf(stderr, " Max number of record per earthquake: %-10d\n",  nStaMax);
	fprintf(stderr, " Number of total    dat:              %-10d\n",  nDatAll);
	fprintf(stderr, " Number of selected dat:              %-10d\n",  nDat);
	fprintf(stderr, "--------------------------------------------------------------\n");
	sta.clear();
	dat.clear();
	return 0;
}


