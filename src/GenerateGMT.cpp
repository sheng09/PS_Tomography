#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <getopt.h>
#include <vector>
#include "Eqsta.h"
using std::vector;

static char _HMSG[]=
{"\
GenerateGMT  - Generate data for gmt plot, including earthquake distribution, station distribution, \n\
               great circle raypath, delta-traveltime, delta-residual traveltime, delta-forward traveltime.\n\
\n\
Usage:  GenerateGMT  -I <asc9 inputfile> \n\
                     -T <station inputfile> \n\
                    [-E <Earthquake gmt outputfile>]\n\
                    [-S <station    gmt outputfile>]\n\
                    [-R <raypath    gmt outputfile>]\n\
                    [-D <delta-time outputfile>]\n\
                    [-U <delta-Residual time gmt outputfile> ]\n\
                    [-F <delta-forward travel time gmt outputfile >]\n\
                    [-W <wavetype for traveltime output>]\n\
\n\
Description:\n\
        The format of <asc9 inputfile> is displayed in ./***.dat.\n\
        The format of <station inputfile> is (num1[d] num2[d] lat[f/lf] long[f/lf] height[f/lf]),\n\
        'num1' is the old version station number,while 'num2' is the new version. \n\
        In fact, only num2 is useful, thus, 'num1' can be any int nuPickStaWaveTypember. \n\
        Geographical2 coordinate (lat-lon-h) is used for stations, in which 'h' equals to 'r-R0'.\n\
        Unit of input height is 'm'.Besides, using picked station is recommended\n\n\
        -E set output earthquake distribution file name. Columns have the format of(lon lat height mag)\n\
        -S set output station distribution file name. Columns have the formate of (lon lat height)\n\
        -R set output raypath file name. File format is :\n\
        -D set output delta-traveltime file name. File has the (x t) format\n\
        -U set output delta-residual traveltime file name. File has the (x t) format\n\
        -F set output delta-forward traveltime file name. File has the (x t) format\n\
        -W <PN/PG/SN/SG/ALL>set seismic phase for x-t outpout, default setting is 'ALL'\n\
Example:\n\
        GenerateGMT -I in.dat -T sta.dat -E eq.gmt.dat -S sta.gmt.dat -R raypath.gmt.dat -D x-t.dat\n\
                    -U x-dt.dat -F x-ft.dat -W PN/PG\n\
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
		{"stain",	required_argument,	NULL,	'T'},
		{"gmteq",	optional_argument,	NULL,	'E'},
		{"gmtsta",	optional_argument,	NULL,	'S'},
		{"gmtray",	optional_argument,	NULL,	'R'},
		{"gmtt",	optional_argument,	NULL,	'D'},
		{"gmtrest",	optional_argument,	NULL,	'U'},
		{"gmtfwdt",	optional_argument,	NULL, 	'F'},
		{"wavetype",optional_argument,  NULL,	'W'},
		{"help",	optional_argument, 	NULL,	'H'},
		{0,			0,					0,		0}
	};
	int 	_Index;
	char 	*str_infile,
			*str_stainfile,

			*str_eqoutfile,
			*str_staoutfile,
			*str_rayoutfile,
			*str_deloutfile,
			*str_resoutfile,
			*str_fwdtoutfile,
			*str_wavetype;
	int 	flag_infile        =0, 
			flag_stainfile     =0, 

			flag_eqoutfile     =0,
			flag_staoutfile    =0,
			flag_rayoutfile    =0,
			flag_deloutfile    =0,
			flag_resoutfile    =0,
			flag_fwdtoutfile   =0,
			flag_wavetype      =0 ;
	while( (oc = getopt_long(argc,argv,":I:T:E:S:R:D:U:F:W:H", _Option, &_Index)) != -1 )
	{
		switch(oc)
		{
			case 'I':
				str_infile      = optarg;
				flag_infile     = 1;
				break;
			case 'T':
				str_stainfile   = optarg;
				flag_stainfile  = 1;
				break;
			case 'E':
				str_eqoutfile   = optarg;
				flag_eqoutfile  = 1;
				break;
			case 'S':
				str_staoutfile  = optarg;
				flag_staoutfile = 1;
				break;
			case 'R':
				str_rayoutfile  = optarg;
				flag_rayoutfile = 1;
				break;
			case 'D':
				str_deloutfile  = optarg;
				flag_deloutfile = 1;
				break;
			case 'U':
				str_resoutfile  = optarg;
				flag_resoutfile = 1;
				break;
			case 'F':
				str_fwdtoutfile = optarg;
				flag_fwdtoutfile= 1;
				break;
			case 'W':
				str_wavetype    = optarg;
				flag_wavetype   = 1;
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
	if(flag_infile != 1 || flag_stainfile != 1)
	{
		fprintf(stderr, "More argumets required\n");
		fprintf(stderr, "%s\n",_HMSG );
		exit(0);
	}
	if(  flag_eqoutfile != 1  && flag_staoutfile != 1 &&  flag_rayoutfile != 1 && 
		 flag_deloutfile != 1 && flag_resoutfile != 1 && flag_fwdtoutfile != 1 )
	{
		fprintf(stderr, " None standard has been stated to generate data for gmt plot!\n" );
		fprintf(stderr, " Input data filename:    %-s\n", str_infile);
		fprintf(stderr, " Input station filename: %-s\n", str_stainfile);
		fprintf(stderr, " Have done nothing!\n\n");
		exit(0);
	}
	///////////////////////////////////////
	std::vector<Station> sta;
	std::vector<EqSta>   dat;
	FILE *fpsta;
	FILE *fpdat;
	
	FILE 	*fpgmtsta,
			*fpgmteq,
			*fpgmtray,
			*fpgmtdelta,
			*fpgmtrest,
			*fpgmtfwdt;
	FLAG    wavetype = _ALL;
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

	int Ns;

	// Read ASC9 format data
	ReadASC9(&dat, &fpdat);
	// Read stations data
	ReadSta(&sta, &Ns ,&fpsta);

	fprintf(stderr, "\n");
	//1. Output station data for gmt, format: <lon lat height>
	if(flag_staoutfile == 1)
	{
		if( (fpgmtsta= fopen(str_staoutfile,"w")) ==NULL )
		{
			fprintf(stderr, "Err: Can't open file for read: %s\n",str_staoutfile );
			exit(0);
		}
		for (int i = 0; i < sta.size(); ++i)
		{
			sta[i].OutSta4GMT(&fpgmtsta);
		}
		fprintf(stderr, " Sta output file: %s\n",str_staoutfile );
	}

	//2. Output earthquake data for gmt, format<lon lat height mag> (Sometimes, mag is not given from asc9 data, thus, default value '0' is output)
	if (flag_eqoutfile == 1)
	{
		if( (fpgmteq= fopen(str_eqoutfile,"w")) ==NULL )
		{
			fprintf(stderr, "Err: Can't open file for write: %s\n",str_eqoutfile );
			exit(0);
		}
		for (int i = 0; i < dat.size(); ++i)
		{
			dat[i].eq.OutEq4GMT(&fpgmteq);
		}
		fprintf(stderr, " Eq output file: %s\n",str_eqoutfile );
	}

	//3. Output ray path data for gmt. 
	if(flag_rayoutfile == 1)
	{
		if( (fpgmtray= fopen(str_rayoutfile,"w")) ==NULL )
		{
			fprintf(stderr, "Err: Can't open file for write: %s\n",str_rayoutfile );
			exit(0);
		}
		for (int i = 0; i < dat.size(); ++i)
		{
			dat[i].CompleteSta(&sta);
			dat[i].OutRaypath4GMT(&fpgmtray);
		}
		fprintf(stderr, " Raypath output file: %s\n",str_rayoutfile );
	}

	//4.0 
	if(flag_wavetype == 1)
	{
		// default value of wavetype is '_ALL'
		wavetype = 0u;
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
		fprintf(stderr, " Seismic phase for x-t output: %s\n",str_wavetype );
	}
	//4. Output delta-traveltime for gmt
	if(flag_deloutfile == 1)
	{
		if( (fpgmtdelta= fopen(str_deloutfile,"w")) ==NULL )
		{
			fprintf(stderr, "Err: Can't open file for write: %s\n",str_deloutfile );
			exit(0);
		}

		for (int i = 0; i < dat.size(); ++i)
		{
			//dat[i].TravelTime(_PGY,_SGN, _PNY, _SNN, 2000.0);
			dat[i].TravelTime(999999.0);
			dat[i].CompleteSta(&sta);
			dat[i].CoordTransf_Geo2();
			dat[i].CalculateDelta();

			dat[i].OutDelta_Time4GMT(&fpgmtdelta,  wavetype);
		}
		fprintf(stderr, " Delta-traveltime output file: %s\n",str_deloutfile );
	}

	//5. Output delta-residual traveltime for gmt
	if(flag_resoutfile == 1)
	{
		if( (fpgmtrest= fopen(str_resoutfile,"w")) ==NULL )
		{
			fprintf(stderr, "Err: Can't open file for write: %s\n",str_resoutfile );
			exit(0);
		}
		for (int i = 0; i < dat.size(); ++i)
		{
			dat[i].CompleteSta(&sta);
			dat[i].CoordTransf_Geo2();
			dat[i].CalculateDelta();

			dat[i].OutDelta_ResidualTime4GMT(&fpgmtrest,  wavetype);
		}
		fprintf(stderr, " Delta-residual traveltime output file: %s\n",str_resoutfile );
	}

	//5. Ouput delta-forward travel time for gmt
	if(flag_fwdtoutfile == 1)
	{
		if( (fpgmtfwdt= fopen(str_fwdtoutfile,"w")) == NULL )
		{
			fprintf(stderr, "Err: Can't open file for write: %s\n",str_fwdtoutfile );
			exit(0);
		}
		for (int i = 0; i < dat.size(); ++i)
		{
			dat[i].CompleteSta(&sta);
			dat[i].CoordTransf_Geo2();
			dat[i].CalculateDelta();

			dat[i].OutDelta_FWDTime4GMT(&fpgmtfwdt,  wavetype);
		}
		fprintf(stderr, " Delta-forward traveltime output file: %s\n",str_fwdtoutfile );
	}



	if(flag_staoutfile  == 1)	fclose(fpgmtsta);
	if(flag_eqoutfile   == 1)	fclose(fpgmteq);
	if(flag_rayoutfile  == 1)	fclose(fpgmtray);
	if(flag_deloutfile  == 1)	fclose(fpgmtdelta);
	if(flag_resoutfile  == 1)	fclose(fpgmtrest);
	if(flag_fwdtoutfile == 1)	fclose(fpgmtfwdt);
	

	sta.clear();
	dat.clear();
	return 0;
}

