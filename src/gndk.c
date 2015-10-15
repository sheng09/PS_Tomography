#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
/*
First line: Hypocenter line
[1-4]   Hypocenter reference catalog (e.g., PDE for USGS location, ISC for
        ISC catalog, SWE for surface-wave location, [Ekstrom, BSSA, 2006])
[6-15]  Date of reference event
[17-26] Time of reference event
[28-33] Latitude
[35-41] Longitude
[43-47] Depth
[49-55] Reported magnitudes, usually mb and MS
[57-80] Geographical location (24 characters)

Second line: CMT info (1)
[1-16]  CMT event name. This string is a unique CMT-event identifier. Older
        events have 8-character names, current ones have 14-character names.
        See note (1) below for the naming conventions used.
[18-61] Data used in the CMT inversion. Three data types may be used:
        Long-period body waves (B), Intermediate-period surface waves (S),
        and long-period mantle waves (M). For each data type, three values
        are given: the number of stations used, the number of components
        used, and the shortest period used.
[63-68] Type of source inverted for: "CMT: 0" - general moment tensor;
        "CMT: 1" - moment tensor with constraint of zero trace (standard);
        "CMT: 2" - double-couple source.
[70-80] Type and duration of moment-rate function assumed in the inversion.
        "TRIHD" indicates a triangular moment-rate function, "BOXHD" indicates
        a boxcar moment-rate function. The value given is half the duration
        of the moment-rate function. This value is assumed in the inversion,
        following a standard scaling relationship (see note (2) below),
        and is not derived from the analysis.

Third line: CMT info (2)
[1-58]  Centroid parameters determined in the inversion. Centroid time, given
        with respect to the reference time, centroid latitude, centroid
        longitude, and centroid depth. The value of each variable is followed
        by its estimated standard error. See note (3) below for cases in
        which the hypocentral coordinates are held fixed.
[60-63] Type of depth. "FREE" indicates that the depth was a result of the
        inversion; "FIX " that the depth was fixed and not inverted for;
        "BDY " that the depth was fixed based on modeling of broad-band
        P waveforms.
[65-80] Timestamp. This 16-character string identifies the type of analysis that
        led to the given CMT results and, for recent events, the date and
        time of the analysis. This is useful to distinguish Quick CMTs ("Q-"),
        calculated within hours of an event, from Standard CMTs ("S-"), which
        are calculated later. The format for this string should not be
        considered fixed.

Fourth line: CMT info (3)
[1-2]   The exponent for all following moment values. For example, if the
        exponent is given as 24, the moment values that follow, expressed in
        dyne-cm, should be multiplied by 10**24.
[3-80]  The six moment-tensor elements: Mrr, Mtt, Mpp, Mrt, Mrp, Mtp, where r
        is up, t is south, and p is east. See Aki and Richards for conversions
        to other coordinate systems. The value of each moment-tensor
	  element is followed by its estimated standard error. See note (4)
	  below for cases in which some elements are constrained in the inversion.

Fifth line: CMT info (4)
[1-3]   Version code. This three-character string is used to track the version
        of the program that generates the "ndk" file.
[4-48]  Moment tensor expressed in its principal-axis system: eigenvalue,
        plunge, and azimuth of the three eigenvectors. The eigenvalue should be
        multiplied by 10**(exponent) as given on line four.
[50-56] Scalar moment, to be multiplied by 10**(exponent) as given on line four.
[58-80] Strike, dip, and rake for first nodal plane of the best-double-couple
        mechanism, repeated for the second nodal plane. The angles are defined
        as in Aki and Richards.
        */
typedef char Line[84];
typedef struct ndk
{
	Line line[5];
}ndk;

static char _HMSG[]=
{"\
gndk: Stream editor for filtering and reforming ndk ASCII file.\n\
\n\
Usage:\n\
    gndk -I <infilename> -S <syntax> [-M <segment>] \n\
\n\
Description:\n\
    gndk is a file stream editor. A stream editor is used to\n\
    perform basic columns arrangment for Focal Mechanism.\n\
    info from ndk file.\n\
    The layout is specialized by -S setting.\n\
\n\
    -I    input filename in ndk format\n\
    -S    syntax to imply the layout of columns, each type of data \n\
          is implied by a unique characters.\n\
          Eg. \"Lat Lon Dep Mec N\".\n\
\n\
        Ref : line1[1-4]   Hypocenter reference catalog \n\
                           (e.g., PDE for USGS location, ISC for ISC \n\
                           catalog, SWE for surface-wave location,\n\
                           [Ekstrom, BSSA, 2006])\n\
        D   : line1[6-15]  Date of reference event\n\
        T   : line1[17-26] Time of reference event\n\
        Lat : line1[28-33] Latitude\n\
        Lon : line1[35-41] Longitude\n\
        Dep : line1[43-47] Depth\n\
        M   : line1[49-55] Reported magnitudes, usually mb and MS\n\
        Geo : line1[57-80] Geographical location (24 characters)\n\
        N   : line2[1-16]  CMT event name. This string is a unique CMT-event \n\
                           identifier. Older events have 8-character names, \n\
                           current ones have 14-character names. \n\
        Data: line2[18-61] Data used in the CMT inversion. Three data types\n\
                           may be used: Long-period body waves (B), Intermediate\n\
                           -period surface waves (S),and long-period mantle \n\
                           waves (M). For each data type, three values are given:\n\
                           the number of stations used, the number of components\n\
                           used, and the shortest period used.\n\
        Ts  : line2[63-68] Type of source inverted for: \"CMT: 0\" - general \n\
                           moment tensor; \n\
                           \"CMT: 1\" - moment tensor with constraint of zero\n\
                            trace (standard);\n\
                           \"CMT: 2\" - double-couple source.\n\
        Td  : line2[70-80] Type and duration of moment-rate function assumed \n\
                           in the inversion. \"TRIHD\" indicates a triangular\n\
                           moment-rate function, \"BOXHD\" indicates a boxcar\n\
                           moment-rate function. The value given is half the\n\
                           duration of the moment-rate function. This value is\n\
                           assumed in the inversion, following a standard \n\
                           scaling relationship (see note (2) below), and is\n\
                           not derived from the analysis.\n\
        Cp  : line3[1-58]  Centroid parameters determined in the inversion. \n\
                           Centroid time, given with respect to the reference\n\
                           time, centroid latitude, centroid longitude, and \n\
                           centroid depth. The value of each variable is followed\n\
                           by its estimated standard error. See note (3) below\n\
                           for cases in which the hypocentral coordinates are \n\
                           held fixed.\n\
        Tp  : line3[60-63] Type of depth. \"FREE\" indicates that the depth was\n\
                           a result of the inversion; \"FIX \" that the depth\n\
                           was fixed and not inverted for; \"BDY \" that the\n\
                           depth was fixed based on modeling of broad-band\n\
                           P waveforms.\n\
        St  : line3[65-80] Timestamp. This 16-character string identifies the\n\
                           type of analysis that led to the given CMT results\n\
                           and, for recent events, the date and time of the\n\
                           analysis. This is useful to distinguish Quick\n\
                           CMTs (\"Q-\"), calculated within hours of an event,\n\
                           from Standard CMTs (\"S-\"), which are calculated\n\
                           later. The format for this string should not be\n\
                           considered fixed.\n\
        Exp : line4[1-2]   The exponent for all following moment values.\n\
                           For example, if the exponent is given as 24, the\n\
                           moment values that follow, expressed in dyne-cm,\n\
                           should be multiplied by 10**24.\n\
        Mec : line4[3-80]  The six moment-tensor elements: Mrr, Mtt, Mpp,\n\
                           Mrt, Mrp, Mtp, where r is up, t is south, and p\n\
                           is east. See Aki and Richards for conversions to\n\
                           other coordinate systems. The value of each moment\n\
                           -tensor element is followed by its estimated standard\n\
                           error. \n\
        V   : line5[1-3]   Version code. This three-character string is used\n\
                           to track the version of the program that generates\n\
                           the \"ndk\" file.\n\
        Eig : line5[4-48]  Moment tensor expressed in its principal-axis system:\n\
                           eigenvalue, plunge, and azimuth of the three eigen\n\
                           -vectors. The eigenvalue should be multiplied by\n\
                           10**(exponent) as given on line four.\n\
        Mecs: line5[50-56] Scalar moment, to be multiplied by 10**(exponent) as\n\
                           given on line four.\n\
        Np  : line5[58-80] Strike, dip, and rake for first nodal plane of the best\n\
                           -double-couple mechanism, repeated for the second nodal\n\
                           plane. The angles are defined as in Aki and Richards.\n\
        Line1:             Output 1st line record of ndk\n\
        Line2:             Output 2nd line record of ndk\n\
        Line3:             Output 3rd line record of ndk\n\
        Line4:             Output 4th line record of ndk\n\
        Line5:             Output 5th line record of ndk\n\
\n\
    -M    Field separator. Default value is backspace.\n\
    -H    Display this help message.\n\
Please visit \"http://www.ldeo.columbia.edu/~gcmt/projects/CMT/catalog/allorder.ndk_explained\"\n\
for more details about ndk format.\n\
\n\
Author: Wangsheng, IGGCAS, PRC.\n\
Email:  wangsheng.cas@gmail.com\n\
"};
int syncmp(char* syntax, char* trg, int ntrg);
int indk(FILE *fp, ndk** data);
int ondk(FILE *fp, ndk* data, long nrcrd);
int strndk(ndk* data, long nrcrd, char* syntax, char seg);
int syncmp(char* syntax, char* trg, int ntrg);

int main(int argc, char  *argv[])
{
	int 	oc;
	//char 	ec;
	static  struct  option _Option[]=
	{
		{"input",	required_argument, 	NULL,	'I'},
		{"syntax",  optional_argument,  NULL,	'S'},
		{"help",	optional_argument, 	NULL,	'H'},
		{"segment", optional_argument,  NULL,   'M'},
		{0,			0,					0,		0}
	};
	int 	_Index;
	char 	*str_infile = NULL,
			*str_syntax = NULL,
			chr_seg =' ';
	int 	flag_infile        =0,
			flag_syntax        =0;
			//flag_seg           =0;
	while( (oc = getopt_long(argc,argv,":I:S:M:H", _Option, &_Index)) != -1 )
	{
		switch(oc)
		{
			case 'I':
				str_infile      = optarg;
				flag_infile     = 1;
				break;
			case 'S':
				str_syntax      = optarg;
				flag_syntax     = 1;
				break;
			case 'M':
				sscanf(optarg,"%c",&chr_seg);
				//flag_seg        = 1;
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
	if(flag_infile != 1 || flag_syntax != 1)
	{
		fprintf(stderr, "More argumets are required!\n\"%s -H\" for help message\n",argv[0]);
		//fprintf(stderr, "%s\n",_HMSG );
		exit(0);
	}
	///////////////////////////////////////////////////////////////////////////////////////
	FILE *fp;
	ndk  *data;
	long n;
	if((fp = fopen(str_infile,"r")) == NULL)
	{
		fprintf(stderr, "Err: can't open file for read! \"%s\"\n",str_infile );
		exit(0);
	}
	n = indk(fp,&data);

	if(flag_syntax == 1)
		strndk(data, n, str_syntax, chr_seg);
	fclose(fp);
	free(data);
	return 0;
}


// return value is the number of data
int indk(FILE *fp, ndk** data)
{
	char linetmp[512];
	long i;
	long nline = 0;
	long nrcrd;
	ndk* pndk;
	while(1)
	{
		++nline; // Get the lines of ndk file
		if(fgets(linetmp,512,fp) == NULL) break;
	}
	nrcrd = nline / 5;
	//fprintf(stderr, "%ld %ld\n",nrcrd, nline );
	fseek(fp,0,SEEK_SET);

	if( ((*data) = (ndk*) malloc(sizeof(ndk)*(nrcrd+1) ) ) == NULL) // (nrcrd+1) is for insurance
	{
		fprintf(stderr, "Err: can't allocate memory for data!\n");
		exit(0);
	}
	pndk = (*data);
	for(i = 0; i < nrcrd; ++i, ++pndk)
	{
		if(NULL == fgets( pndk->line[0],128,fp) )
		{
			fprintf(stderr, "Err: can't read file indk()\n");
			exit(1);
		}
		if(NULL == fgets( pndk->line[1],128,fp) )
		{
			fprintf(stderr, "Err: can't read file indk()\n");
			exit(1);
		}
		if(NULL == fgets( pndk->line[2],128,fp) )
		{
			fprintf(stderr, "Err: can't read file indk()\n");
			exit(1);
		}
		if(NULL == fgets( pndk->line[3],128,fp) )
		{
			fprintf(stderr, "Err: can't read file indk()\n");
			exit(1);
		}
		if(NULL == fgets( pndk->line[4],128,fp) )
		{
			fprintf(stderr, "Err: can't read file indk()\n");
			exit(1);
		}

		// In fact, column 81 is '\n'
		pndk->line[0][80] = '\0';
		pndk->line[1][80] = '\0';
		pndk->line[2][80] = '\0';
		pndk->line[3][80] = '\0';
		pndk->line[4][80] = '\0';
	}
	return nrcrd;
}


int ondk(FILE *fp, ndk* data, long nrcrd)
{
	long i;
	for(i = 0; i < nrcrd; ++i, ++data )
	{
		printf("%s\n", data->line[0]);
		printf("%s\n", data->line[1]);
		printf("%s\n", data->line[2]);
		printf("%s\n", data->line[3]);
		printf("%s\n", data->line[4]);
	}
	return 0;
}

/*
Syntax is bracketed by '', which determine the layout
of columns, each column is implied by unique characters,
and separated by 'seg'.

Ref  : line1[1-4]   Hypocenter reference catalog
D    : line1[6-15]  Date of reference event
T    : line1[17-26] Time of reference event
Lat  : line1[28-33] Latitude
Lon  : line1[35-41] Longitude
Dep  : line1[43-47] Depth
M    : line1[49-55] Reported magnitudes, usually mb and MS
Geo  : line1[57-80] Geographical location (24 characters)
N    : line2[1-16]  CMT event name.
Data : line2[18-61] Data used in the CMT inversion.
Ts   : line2[63-68] Type of source
Td   : line2[70-80] Type and duration of moment-rate function assumed in the inversion.
Cp   : line3[1-58]  Centroid parameters determined in the inversion
Tp   : line3[60-63] Type of depth
St   : line3[65-80] Timestamp
Exp  : line4[1-2]   The exponent
Mec  : line4[3-80]  The six moment-tensor elements.Mrr, Mtt, Mpp, Mrt, Mrp, Mtp, where r is up, t is south, and p is east
V    : line5[1-3]   Version code
Eig  : line5[4-48]  Moment tensor expressed in its principal-axis system: eigenvalue
Mecs : line5[50-56] Scalar moment
Np   : line5[58-80] Strike, dip, and rake for two nodal plane  */
int strndk(ndk* data, long nrcrd, char* syntax, char seg)
{
	int i,j;
	char* pstr    = syntax;

	int   npara  = 0;

	int   nline[21]  = {0};
	int   pincrm[21] = {-1};
	int   len[21]    = {0};

	int*  pnline     = nline;
	int*  ppincrm    = pincrm;
	int*  plen       = len;

	char  oline[512];
	char* poline     = oline;

	while( *pstr != 0 )
	{
		//printf("BJ %c\n", *pstr);
		if( *pstr>= 'A' && *pstr <= 'Z' ){
			//                                               Number of lines   Pointer increment  Parameter length
		  if(1 == syncmp(pstr,"Ref" ,3))/*  0 Ref  : line1[1 -4 ]  */  {*pnline = 0; *ppincrm = 1 -1 ; *plen = 4 -1 +1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"D"   ,1))/*  1 D    : line1[6 -15]  */  {*pnline = 0; *ppincrm = 6 -1 ; *plen = 15-6 +1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"T"   ,1))/*  2 T    : line1[17-26]  */  {*pnline = 0; *ppincrm = 17-1 ; *plen = 26-17+1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Lat" ,3))/*  3 Lat  : line1[28-33]  */  {*pnline = 0; *ppincrm = 28-1 ; *plen = 33-28+1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Lon" ,3))/*  4 Lon  : line1[35-41]  */  {*pnline = 0; *ppincrm = 35-1 ; *plen = 41-35+1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Dep" ,3))/*  5 Dep  : line1[43-47]  */  {*pnline = 0; *ppincrm = 43-1 ; *plen = 47-43+1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"M"   ,1))/*  6 M    : line1[49-55]  */  {*pnline = 0; *ppincrm = 49-1 ; *plen = 55-49+1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Geo" ,3))/*  7 Geo  : line1[57-80]  */  {*pnline = 0; *ppincrm = 57-1 ; *plen = 80-57+1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"N"   ,1))/*  8 N    : line2[1 -16]  */  {*pnline = 1; *ppincrm = 1 -1 ; *plen = 16-1 +1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Data",4))/*  9 Data : line2[18-61]  */  {*pnline = 1; *ppincrm = 18-1 ; *plen = 61-18+1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Ts"  ,2))/* 10 Ts   : line2[63-68]  */  {*pnline = 1; *ppincrm = 63-1 ; *plen = 68-63+1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Td"  ,2))/* 11 Td   : line2[70-80]  */  {*pnline = 1; *ppincrm = 70-1 ; *plen = 80-70+1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Cp"  ,2))/* 12 Cp   : line3[1 -58]  */  {*pnline = 2; *ppincrm = 1 -1 ; *plen = 58-1 +1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Tp"  ,2))/* 13 Tp   : line3[60-63]  */  {*pnline = 2; *ppincrm = 60-1 ; *plen = 63-60+1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"St"  ,2))/* 14 St   : line3[65-80]  */  {*pnline = 2; *ppincrm = 65-1 ; *plen = 80-65+1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Exp" ,3))/* 15 Exp  : line4[1 -2 ]  */  {*pnline = 3; *ppincrm = 1 -1 ; *plen = 2 -1 +1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Mec" ,3))/* 16 Mec  : line4[3 -80]  */  {*pnline = 3; *ppincrm = 3 -1 ; *plen = 80-3 +1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"V"   ,1))/* 17 V    : line5[1 -3 ]  */  {*pnline = 4; *ppincrm = 1 -1 ; *plen = 3 -1 +1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Eig" ,3))/* 18 Eig  : line5[4 -48]  */  {*pnline = 4; *ppincrm = 4 -1 ; *plen = 48-4 +1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Mecs",4))/* 19 Mecs : line5[50-56]  */  {*pnline = 4; *ppincrm = 50-1 ; *plen = 56-50+1;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Np"  ,2))/* 20 Np   : line5[58-80]  */  {*pnline = 4; *ppincrm = 58-1 ; *plen = 80-58+1;  ++pnline;++ppincrm;++plen;++npara;}
		  ///
		  if(1 == syncmp(pstr,"Line1"  ,5))/* Line*/  {*pnline = 0; *ppincrm = 0 ; *plen = 80;  ++pnline;++ppincrm;++plen;++npara;}
	      if(1 == syncmp(pstr,"Line2"  ,5))/* Line*/  {*pnline = 1; *ppincrm = 0 ; *plen = 80;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Line3"  ,5))/* Line*/  {*pnline = 2; *ppincrm = 0 ; *plen = 80;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Line4"  ,5))/* Line*/  {*pnline = 3; *ppincrm = 0 ; *plen = 80;  ++pnline;++ppincrm;++plen;++npara;}
		  if(1 == syncmp(pstr,"Line5"  ,5))/* Line*/  {*pnline = 4; *ppincrm = 0 ; *plen = 80;  ++pnline;++ppincrm;++plen;++npara;}
		  ///

		}		//
		++pstr;
	}
	if(npara <=0 )
	{
		fprintf(stderr, "Err: Wrong syntax! \"%s\"\n",syntax );
		exit(0);
	}
	//printf("SSSSSSSSSSS  %s %d\n", syntax, npara);
	//for(j = 0; j < npara; ++j)
	//{
	//	printf("%d %d %d %d\n",j,nline[j],pincrm[j],len[j] );
	//}
	//return 0;

	for(i = 0; i < nrcrd; ++i,++data)
	{
		memset(oline,0, 512);
		poline = oline;
		for(j = 0; j < npara; ++j)
		{
			strncpy(poline, (data->line[ nline[j]]) + pincrm[j], len[j]);
			poline += len[j];

			//for(k = 0; k < len[j]; ++k,++poline)
			//	*poline = *( (data->line[ nline[j]]) + pincrm[j] +k );
			*(poline++)     = seg;
		}
		*(--poline) = 0;
		printf("%-10d %s\n",i+1,oline);
	}
	return 0;
}

//Compare whether two string is the same from [0 to ntrg]
int syncmp(char* syntax, char* trg, int ntrg)
{
	int i;
	for(i = 0; i < ntrg; ++i, ++syntax, ++trg)
		if(*syntax != *trg)
			return -1;
	if(*syntax < 'z' && *syntax > 'a')
		return -1;
	return 1;
}