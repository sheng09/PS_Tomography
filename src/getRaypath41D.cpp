#include "tx.h"
#include "VelModel.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <getopt.h>
using namespace std;

static char  _HMSGetRaypath41D[]=
{"\n\
getRaypath41D 	 -I <1d modle filename> \n\
				[-D <Direct wave raypath output filename>] \n\
				[-A <anglemin/anglemax/numberOfRays>]\n\
				[-E <Head wave raypath output filename>\n\
				[-X <direct x-t data output filename>] \n\
				[-T <head x-t data output filename>]\n\
				[-P <depth of the earthquake>] \n\
				[-S <resolution>]\n\
				[-V <gmt velocity model >]\n\
				[-M <gmt layer model>]\n\
				[-H]\n\
Eg:\n\
getRaypath41D    -I m1d.dat\n\
				 -D DirectRaypath.dat\n\
				 -A 40.0/70.0/1.0\n\
				 -E HeadRaypath.dat\n\
				 -X DirectXT.dat \n\
				 -P 10.0\n\
				 -S 0.1 \n\
				 -V m1d.vel.dat \n\
				 -M m1d.layer.dat\n\
"};

int main(int argc, char *argv[])
{
	int 	oc;
	char 	ec;
	static  struct  option _Option[]=
	{
		{"input",		required_argument, 	NULL,	'I'},
		{"direct",		required_argument,	NULL,	'D'},
		{"angle",		optional_argument,	NULL,	'A'},
		{"head",		optional_argument,	NULL,	'E'},
		{"directXT",	required_argument,	NULL,	'X'},
		{"headXT",		required_argument,	NULL,	'T'},
		{"depth",		optional_argument,	NULL,	'P'},
		{"resolution",	optional_argument,	NULL,	'S'},
		{"velocity",	optional_argument,	NULL,	'V'},
		{"model",		optional_argument,	NULL,	'M'},
		{"help",		optional_argument, 	NULL,	'H'},
		{0,			0,					0,		0}
	};
	int 	_Index;
	char 	*str_infile,
			*str_DirectRayOutfile,
			*str_angle,
			*str_HeadRayOutfile,
			*str_DirectXToutfile,
			*str_HeadXToutfile,
			*str_depth,
			*str_resolution,
			*str_Velm1doutfile,
			*str_Layerm1doutfile;

	int 	flag_infile           =0, 
			flag_DirectRayOutfile =0, 
			flag_HeadRayOutfile   =0,
			flag_depth            =0,
			flag_DirectXToutfile  =0, 
			flag_HeadXToutfile    =0,
			flag_angle            =0,
			flag_resolution       =0,
			flag_Velm1doutfile    =0,
			flag_Layerm1doutfile  =0; 

	while( (oc = getopt_long(argc,argv,":I:D:A:E:X:T:P:S:V:M:H", _Option, &_Index)) != -1 )
	{
		switch(oc)
		{
			case 'I':
				str_infile            = optarg;
				flag_infile           = 1;
				break;
			case 'D':
				str_DirectRayOutfile  = optarg;
				flag_DirectRayOutfile = 1;
				break;
			case 'A':
				str_angle             = optarg;
				flag_angle            = 1;
				break;
			case 'E':
				str_HeadRayOutfile    = optarg;
				flag_HeadRayOutfile   = 1;
				break;
			case 'X':
				str_DirectXToutfile   = optarg;
				flag_DirectXToutfile  = 1;
				break;
			case 'T':
				str_HeadXToutfile     = optarg;
				flag_HeadXToutfile    = 1;
				break;
			case 'P':
				str_depth             = optarg;
				flag_depth            = 1;
				break;      
			case 'S':      
				str_resolution        = optarg;
				flag_resolution       = 1;
				break;
			case 'V':
				str_Velm1doutfile     = optarg;
				flag_Velm1doutfile    = 1;
				break; 
			case 'M': 
				str_Layerm1doutfile   = optarg;
				flag_Layerm1doutfile  = 1;
				break;
			case 'H':
				fprintf(stderr, "%s\n",_HMSGetRaypath41D );
				exit(0);
			case '?':
				ec = (char) (oc);
				fprintf(stderr, "Wrong argument!\n");
				exit(0);
			case ':':
				ec = (char) (oc);
				fprintf(stderr, "\'-%c\' OR \'--%s\'required parameter\n",ec, _Option[_Index].name);
				exit(0);
		}
	}
	if(flag_infile != 1)
	{
		fprintf(stderr, "More argumets required\n");
		fprintf(stderr, "%s\n",_HMSGetRaypath41D );
		exit(0);
	}
	////////////////////////////////////////////////////
	Model1D m1d 				;

	FILE *fpm1d    				;
	FILE *fp_DirectRayOutfile	;
	FILE *fp_HeadRayOutfile		;
	FILE *fp_DirectXToutfile    ;
	FILE *fp_HeadXToutfile      ;
	FILE *fp_Velm1doutfile		;
	FILE *fp_Layerm1doutfile	;

	double  anglemax;
    double  anglemin;
    double  dangle;
    int     nseg;

    double  anglemaxtheo;
    double  anglemintheo;

    double  p;
    double  pmax;
    double  pmin;
    double  dp;

	double  x,
		    t;

    double  angle;
    double  sh;
    int     iz;
    double  viz;
    double  dz;
   	double  xmax = 0.0;
	vector<double> xs;
	vector<double> zs;
	int     nray = 0;
	int     i;

	//1 Read in model data
	if( (fpm1d=fopen(str_infile,"r")) == NULL )
	{
		fprintf(stderr, "Err: can not open file for read: %s\n",str_infile );
		exit(0);
	}
	m1d.ReadMod1Dtomolzw(&fpm1d);
 


    //2 Output velocity model for gmt plot
    if(flag_Velm1doutfile == 1)
	{
		if( (fp_Velm1doutfile = fopen(str_Velm1doutfile, "w")) == NULL )
		{
			fprintf(stderr, "Err: can not open file for write: %s\n",str_Velm1doutfile );
			exit(0);
		}
		m1d.OutMod1D(&fp_Velm1doutfile);;
	}
	//3 Output layer model for gmt plot
	if(flag_Layerm1doutfile  == 1)
	{
		if( (fp_Layerm1doutfile = fopen(str_Layerm1doutfile, "w")) == NULL )
		{
			fprintf(stderr, "Err: can not open file for write: %s\n",str_Layerm1doutfile );
			exit(0);
		}
		for (int i = 0; i < m1d.nz; ++i)
		{
			fprintf(fp_Layerm1doutfile, "%lf %lf\n",-1000000.0,m1d.zs[i] );
			fprintf(fp_Layerm1doutfile, "%lf %lf\n", 1000000.0,m1d.zs[i] );
			//fprintf(fp_Layerm1doutfile, "%lf %lf\n",-1.0*xmax,m1d.zs[i] );
			//fprintf(fp_Layerm1doutfile, "%lf %lf\n",     xmax,m1d.zs[i] );
			fprintf(fp_Layerm1doutfile, ">\n");
		}
	}

	//4 Resolution setting
	if(flag_resolution == 1)
	{
		sscanf(str_resolution,"%lf",&dz);
	}
	else
	{// Default setting of resolution
		dz = 1.0;
	}
   	m1d.MakeModelDense(dz);

	//5 Acquire Eq depth
	if(flag_depth == 1)
	{
		sscanf(str_depth,"%lf",&sh);
		// Judge whether input source depth is rational
		if(sh > m1d.zs[m1d.zs.size()-1] || sh < 0.0)
		{
			fprintf(stderr, "Err: earthquake source depth is illegal: \" %lf\"\n",sh);
			fprintf(stderr, "     earthquake source depth shoule between (0.0,%lf) \n",m1d.zs[m1d.zs.size()-1]);
			exit(0);
		}
	}
	else
	{	// Default eq depth is 0.0 km
		sh = 0.0;
	}
	
    //6 For direct wave 
	if(flag_DirectRayOutfile == 1 ||
	   flag_DirectXToutfile  == 1)
	{
		// Direct wave x-t data output file
		if(flag_DirectXToutfile == 1)
		{
			if( (fp_DirectXToutfile = fopen(str_DirectXToutfile, "w")) == NULL )
			{
				fprintf(stderr, "Err: can not open file for write: %s\n",str_DirectXToutfile );
				exit(0);
			}
		}
		if(flag_DirectRayOutfile == 1)
		{
			if( (fp_DirectRayOutfile=fopen(str_DirectRayOutfile,"w")) == NULL )
			{
				fprintf(stderr, "Err: can not open file for write: %s\n",str_DirectRayOutfile );
				exit(0);
			}
		}

		// Acquire angle interval and increment
		iz           = locateXF( &m1d.zs, sh);
		viz          = LinearInterpolation(m1d.zs[iz],m1d.vs[iz],m1d.zs[iz+1],m1d.vs[iz+1],sh);
		anglemintheo = asin(viz/m1d.vs[m1d.nz-1]);
		anglemaxtheo = PI/2;
		if(flag_angle == 1)
		{
			sscanf(str_angle,"%lf/%lf/%d",&anglemin, &anglemax, &nseg);
			//dangle   = dangle/180.0*PI;
			anglemin = anglemin /180.0*PI;
			anglemax = anglemax /180.0*PI;
			// Judge whether input angle is rational
			if(anglemin < anglemintheo)
			{
				fprintf(stderr, "Err: anglemin is illegal: \"%lf\"\n",anglemin*180.0/PI );
				fprintf(stderr, "     angle interval should between (%lf,180.0)\n",anglemintheo*180.0/PI );
				exit(0);
			}
			if(anglemin > anglemax || nseg <= 0)
			{
				fprintf(stderr, "Err: angle info is illegal!\n");
				exit(0);
			}
		}
		else
		{	
				anglemin = anglemintheo;
				anglemax = PI/2.0;
				nseg     = 10;
		}
   		
   		// Only one ray 
		if(nseg == 1)
		{
				angle = anglemin;
				m1d.CalculateDirectRaypath( angle, sh, &t, &x, &xs, &zs);
				if(flag_DirectRayOutfile == 1)
				{
					for (int j = 0; j < xs.size(); ++j)
					{
						fprintf(fp_DirectRayOutfile, "%15.8lf %15.8lf\n", xs[j],zs[j] );
					}
					fprintf(fp_DirectRayOutfile,">\n");
				}
				if(flag_DirectXToutfile == 1)
				{
					fprintf(fp_DirectXToutfile, "%15.8lf %15.8lf\n",x,t );
				}
				++nray;
		}
   		// For down-up propagate ray
   		if(anglemin < PI/2 && nseg > 1)
   		{
   			if(anglemax <= PI/2)
   			{
   				pmax = sin(anglemax)/viz;
   				pmin = sin(anglemin)/viz;
   				dp   = (pmax - pmin)/(double)(nseg-1);
   			}
   			else
   			{
   				pmax = 1.0/viz;
   				pmin = sin(anglemin)/viz;
   				dp   = (pmax - pmin)/(double)(nseg-1);
   			}
   			p    = pmin;
   			i    = 0;
    		while(1)
			{
				i++;
				if(xs.size() > 0) xs.clear();
				if(zs.size() > 0) zs.clear();
				angle =asin(viz*p);
				//printf("%lf %20.17lf\n", angle*180.0/PI,viz*p);
				if(p > pmax)
				{
					if(i == nseg)
					{
						angle = PI/2;
					}
					else
					{
						break;
					}
				} 
				
				m1d.CalculateDirectRaypath( angle, sh, &t, &x, &xs, &zs);
				if(flag_DirectRayOutfile == 1)
				{
					for (int j = 0; j < xs.size(); ++j)
					{
						fprintf(fp_DirectRayOutfile, "%15.8lf %15.8lf\n", xs[j],zs[j] );
					}
					fprintf(fp_DirectRayOutfile,">\n");
				}
				if(flag_DirectXToutfile == 1)
				{
					fprintf(fp_DirectXToutfile, "%15.8lf %15.8lf\n",x,t );
				}
				++nray;
				p += dp;
				//if(x > xmax)
				//{
				//	xmax = x;
				//}
			}
   		}
		// For up propagate ray
		if(sh > 0.0 && anglemax > PI/2 && nseg >1)
		{
			dangle = ((anglemax-PI/2)/(nseg-1));
			if(anglemin < PI/2)
			{
				angle  = PI/2;	
			}
			else
			{
				angle = anglemin;
			}
			i = 0;
			while(1)
			{
				i++;
				if(angle > anglemax)
				{
					if(i == nseg)
					{
						angle = anglemax;
					}
					else
					{
						break;		
					}
				} 
				if(xs.size() > 0) xs.clear();
				if(zs.size() > 0) zs.clear();
				m1d.CalculateDirectRaypath( angle, sh, &t, &x, &xs, &zs);
				if(flag_DirectRayOutfile == 1)
				{
					for (int j = 0; j < xs.size(); ++j)
					{
						fprintf(fp_DirectRayOutfile, "%15.8lf %15.8lf\n", xs[j],zs[j] );
					}
					fprintf(fp_DirectRayOutfile,">\n");
				}
				if(flag_DirectXToutfile == 1)
				{
					fprintf(fp_DirectXToutfile, "%15.8lf %15.8lf\n",x,t );
				}
				angle+=dangle;
				++nray;
			}
			//if(x > xmax)
			//{
			//	xmax = x;
			//}
		}
	}

	//7 For Head wave plot
	if(flag_HeadRayOutfile == 1 || flag_HeadXToutfile == 1)
	{
		if(sh >= m1d.zs[m1d.zdis[m1d.ndis-1]])
		{
			fprintf(stderr, "Err: Head wave does not exist for depth \"%lf\" !\n",sh );
			exit(0);
		}
		if(flag_HeadRayOutfile == 1)
		{
			if( (fp_HeadRayOutfile=fopen(str_HeadRayOutfile,"w")) == NULL )
			{
				fprintf(stderr, "Err: can not open file for write: %s\n",str_HeadRayOutfile );
				exit(0);
			}
		}
		if(flag_HeadXToutfile == 1)
		{
			if( (fp_HeadXToutfile=fopen(str_HeadXToutfile,"w")) == NULL )
			{
				fprintf(stderr, "Err: can not open file for write: %s\n",str_HeadXToutfile );
				exit(0);
			}
		}

		if(xs.size() > 0) xs.clear();
		if(zs.size() > 0) zs.clear();
		for(int i = 1; i <= m1d.ndis; ++i)
		{
			for(int j = 0; j < 10; ++j)
			{
				m1d.CalculateHeadRaypath(i, 100.0*j , sh, &t, &x, &xs, &zs);
				if(flag_HeadRayOutfile == 1)
				{
					for (int k = 0; k < xs.size(); ++k)
					{
						fprintf(fp_HeadRayOutfile, "%15.8lf %15.8lf\n", xs[k],zs[k] );
					}
					fprintf(fp_HeadRayOutfile, ">\n");
				}
				if(flag_HeadXToutfile == 1)
				{
					fprintf(fp_HeadXToutfile, "%15.8lf %15.8lf\n",x,t );
				}
			}
		}
			
	}

    //// Info output
    fprintf(stderr, "\n");
    fprintf(stderr, " Source depth:               %-10.5lf\n", sh);
    fprintf(stderr, " Theoretical angle interval (%-lf, %-lf) DEG\n",anglemintheo*180.0/PI , 180.0 );
    fprintf(stderr, " Actual      angle interval (%-lf, %-lf) DEG\n",anglemin*180.0/PI, anglemax*180.0/PI );
    fprintf(stderr, " Number of rays              %d\n", nray);
//
    //fprintf(stderr, " 1D model input file:  %-s\n",str_infile);
    //if(flag_DirectRayOutfile == 1) fprintf(stderr, " raypath output file:   %-s\n",str_DirectRayOutfile);
    //if(flag_DirectXToutfile  == 1) fprintf(stderr, " x-t data output file:  %-s\n",str_DirectXToutfile);
    //if(flag_Velm1doutfile   == 1) fprintf(stderr, " 1d velocity model for gmt plot:%-s\n",str_Velm1doutfile ); 
    //if(flag_Layerm1doutfile   == 1) fprintf(stderr, " 1d layer model for gmt plot:   %-s\n",str_Layerm1doutfile );
	//fprintf(stderr, "\n");
//
    //if(xs.size() > 0) xs.clear();
	//if(zs.size() > 0) zs.clear();
    fclose(fpm1d);
    if(flag_DirectRayOutfile == 1)    fclose(fp_DirectRayOutfile);
    if(flag_DirectXToutfile  == 1)    fclose(fp_DirectXToutfile);
    if(flag_Layerm1doutfile  == 1)    fclose(fp_Layerm1doutfile);
    if(flag_Velm1doutfile    == 1)    fclose(fp_Velm1doutfile);
    if(flag_HeadRayOutfile   == 1)    fclose(fp_HeadRayOutfile);
    if(flag_HeadXToutfile    == 1)    fclose(fp_HeadXToutfile);
	

	return 0;
}