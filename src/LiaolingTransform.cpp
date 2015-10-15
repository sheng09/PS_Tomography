#include "Eqsta.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <vector>
using namespace std;

static char HMSG[]=
{"\n\
	LiaolingTransform -I <input filename>  -P <pg output filename> -S <sg output filename> -T <station filename>\n\
"};

int main(int argc, char  *argv[])
{
	char 	*str_infile;
	char	*str_PGoutfile;
	char	*str_SGoutfile;
	char 	*str_stafile;
	int		flag_infile    = 0,
			flag_PGoutfile = 0,
			flag_SGoutfile = 0,
			flag_stafile   = 0;
	int 	oc;
	char 	ec;
	int 	Index;
	static struct  option LLoption[]=
	{
		{"input"	,	required_argument,	NULL,	'I'},
		{"pgout"	,	required_argument,	NULL,	'P'},
		{"sgout"	,	required_argument,	NULL,	'S'},
		{"station"	,	required_argument,	NULL,	'T'},
		{"help"		,	optional_argument, 	NULL,	'H'},
		{0			,	0				 ,	0   ,	 0}
	};
	while( (oc =getopt_long(argc,argv,":I:P:S:T:H",LLoption, &Index)) != -1)
	{
		switch(oc)
		{
			case 'I':
				str_infile	=	optarg;
				flag_infile =  	1;
				break;
			case 'P':
				str_PGoutfile = 	optarg;
				flag_PGoutfile= 	1;
				break;
			case 'S':
				str_SGoutfile = 	optarg;
				flag_SGoutfile= 	1;
				break;
			case 'T':
				str_stafile = 	optarg;
				flag_stafile= 	1;
				break;
			case 'H':
				fprintf(stderr, "%s\n",HMSG );
				break;
			case ':':
				fprintf(stderr, "Incorrect argument!\n");
				exit(0);
				break;
			case '?':
				ec = (char) oc;
				fprintf(stderr, " \'-%c\' OR  \'%s\' required parameters!\n",ec, LLoption[Index].name );
				break;
		}
	}
	if (flag_infile != 1 || flag_PGoutfile != 1 || flag_SGoutfile != 1 || flag_stafile != 1)
	{
		fprintf(stderr, "More arguments are reiqured\n" );
		fprintf(stderr, "%s\n", HMSG);
		exit(0);
	}
	//////////////////////////////////////////////////////////////////////////////
	FILE 	*fpin,
			*fpPGout,
			*fpSGout,
			*fpsta;

	vector<EqSta>   dat;
	vector<Station> sta;

	int 	Nsta;
	int     Ndat;
	if( (fpin = fopen(str_infile,"r")) == NULL)
	{
		fprintf(stderr, "Err: Can't open file for read:%s\n",str_infile );
		exit(0);
	}
	if( (fpPGout = fopen(str_PGoutfile,"w")) == NULL )
	{
		fprintf(stderr, "Err: Can't open file for write:%s\n",str_PGoutfile );
		exit(0);
	}
	if( (fpSGout = fopen(str_SGoutfile,"w")) == NULL )
	{
		fprintf(stderr, "Err: Can't open file for write:%s\n",str_SGoutfile );
		exit(0);
	}
	if( (fpsta = fopen(str_stafile,"r")) == NULL )
	{
		fprintf(stderr, "Err: Can't open file for read:%s\n",str_stafile );
		exit(0);
	}

	ReadSta4NAME(&sta, &Nsta, &fpsta);
	ReadFormatLiaoling(&dat, &sta, &fpin, _PGY);

	Ndat = 0;
	for (int i = 0; i < dat.size(); ++i)
	{
		Ndat += dat[i].sta.size();

		//dat[i].OutputASC9(&fpPGout, _PGY);
		dat[i].OutputASC9(&fpPGout);
		//dat[i].OutputASC9(&fpSGout, _SGY);
		//dat[i].OutputASC9(&fpSGout);
	}

	fseek(fpin, 0, SEEK_SET);;
	ReadFormatLiaoling(&dat, &sta, &fpin, _SGY);
	for (int i = 0; i < dat.size(); ++i)
	{
		Ndat += dat[i].sta.size();

		//dat[i].OutputASC9(&fpPGout, _PGY);
		//dat[i].OutputASC9(&fpPGout);
		//dat[i].OutputASC9(&fpSGout, _SGY);
		dat[i].OutputASC9(&fpSGout);
	}

	fprintf(stderr, " Input file:            %-20s\n",str_infile);
	fprintf(stderr, " PG output file:        %-20s\n",str_PGoutfile);
	fprintf(stderr, " SG output file:        %-20s\n",str_SGoutfile);
	fprintf(stderr, " Input station file:    %-20s\n",str_stafile);
	fprintf(stderr, " Number of earthquake   %-20d\n",dat.size() );
	fprintf(stderr, " Number of station      %-20d\n",sta.size() );
	fprintf(stderr, " Number of data         %-20d\n\n",Ndat );

	fclose(fpsta);
	fclose(fpin);
	fclose(fpPGout);
	fclose(fpSGout);
	sta.clear();
	dat.clear();
	return 0;
}