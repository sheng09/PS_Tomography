#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "tomoVA_model.h"



/*
int   *NumofLayer;  //=== Layer Number of Raytracing Model.
int   itnneed;      //=== iteration number
char  *RTMfile;    //=== which iteration Needed
*/
int ReadRTM(char *RTMfile, Mod3D *RTMod,int *NumofLayer, int itnneed)
{
    int NP;
    int NR;
    int NH;
    int layerOrder;
    int itn;
    int i = 0;
    int j;
    FILE *fp;
    fp = fopen(RTMfile,"r");
    if (fp == NULL)
    {
        fprintf(stderr, "Err: Can't open Ray Tracing Model file for read:%s\n",RTMfile);
        exit(0);
    }
    itn    = -1;
    while(!feof(fp) && itn < itnneed )
    {
        //Record Head:
        //   21    33     9     2         0     0
        //   NP    NR     NH  NumofLayer  ?    IterationNumber
        //                                   0 means the initial data
        if(EOF == fscanf(fp,"%d %d %d %d %d %d", &NP, &NR, &NH, NumofLayer, &layerOrder, &itn) )
        {
        	fprintf(stderr, "Err: can't read file %s\n", RTMfile);
        	exit(1);
        }
        //fprintf(stderr,"%d %d %d %d %d %d", NP, NR, NH, *NumofLayer, layerOrder, itn);
        for(i = 0; i < *NumofLayer; ++i)
        {
            if(i != 0)
            {
                if(EOF == fscanf(fp,"%d %d %d %d %d %d", &NP, &NR, &NH, NumofLayer, &layerOrder, &itn) )
                {
                	fprintf(stderr, "Err: can't read file %s\n", RTMfile);
                	exit(1);
                }
            }
            RTMod[i].NP = NP;
            RTMod[i].NR = NR;
            RTMod[i].NH = NH;
            RTMod[i].PN =(float *) calloc(NP,sizeof(float));
            RTMod[i].RN =(float *) calloc(NR,sizeof(float));
            RTMod[i].HN =(float *) calloc(NH,sizeof(float));
            RTMod[i].VELP = (float *)calloc(NP*NR*NH,sizeof(float));
            for(j = 0; j < NP; ++j)
            	if(EOF == fscanf(fp,"%f", &(RTMod[i].PN[j])) )
            	{
            		fprintf(stderr, "Err: can't read file %s\n",RTMfile );
            		exit(1);
            	}
            for(j = 0; j < NR; ++j)
            	if(EOF == fscanf(fp,"%f", &(RTMod[i].RN[j])) )
            	{
            		fprintf(stderr, "Err: can't read file %s\n",RTMfile );
            		exit(1);
            	}
            for(j = 0; j < NH; ++j)
            	if(EOF == fscanf(fp,"%f", &(RTMod[i].HN[j])) )
            	{
            		fprintf(stderr, "Err: can't read file %s\n",RTMfile );
            		exit(1);
            	}
            for(j = 0; j < NP*NR*NH; ++j)
            	if(EOF == fscanf(fp, "%f", &(RTMod[i].VELP[j])) )
            	{
            		fprintf(stderr, "Err: can't read file %s\n",RTMfile );
            		exit(1);
            	}
        }
    }
    return 0;
}

int   M_LocateXF(float xx[], int *nf, float *xf,
                 int *j)
{
    //Given xx[0...n-1], n , x ; determine j which xx[j+1]>= x >=xx[j]
    //           xx[0][1][2][3][4][5][6][7][8][9]
    //Eg:  xx[10] = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
    //     nf     = 10
    //     xf = 5.5;  xf = 5.0;  xf = 0.0;  xf =-5.0;  xf =100.0
    //return:
    //     j  = 5  ;  j  = 4  ;  j  = 0  ;  j  = 0  ;  j  = 8

    int     ju;
    int     jm;
    int     jl;
    int     ascnd;
    int     n = *nf;
    float   x = *xf;

    jl    = 0;
    ju    = n-1;
    ascnd = (xx[n-1] > xx[0]);
    while (ju-jl > 1)
    {
        jm= (int)( (ju+jl )/2 );
        if ( (x > xx[jm]) == ascnd) jl=jm;
        else                        ju=jm;
    }
    if( x == xx[0] )     *j = 0;
    else if(x==xx[n-1] ) *j = n-2;
    else                 *j = jl;

    return *j;
}
int   M_SendDis(float *lat, float *lon, int *N, DisSur *DisMod,
                float *dep)
{
    /*=====Calculate Discontunity Depths Values, Given lat/lon/and which Layer ========*/
    int     Ilat;
    int     Ilon;
    int     Ndis;
    int     NN;
    float   u;
    float   v;

    Ndis = *N;
    NN   = Ndis - 1;

    M_LocateXF( DisMod[NN].DisLat, & (DisMod[NN].Nlat), lat, &Ilat);
    M_LocateXF( DisMod[NN].DisLon, & (DisMod[NN].Nlon), lon, &Ilon);
            //====Modified on 05/12/14
    u = ( (*lat) - DisMod[NN].DisLat[Ilat] )/( DisMod[NN].DisLat[Ilat+1]-DisMod[NN].DisLat[Ilat] );
    v = ( (*lon) - DisMod[NN].DisLon[Ilon] )/( DisMod[NN].DisLon[Ilon+1]-DisMod[NN].DisLon[Ilon] );

    *dep = DisMod[NN].DisDep[Ilat*DisMod[NN].Nlon+Ilon]       *(1-u)*(1-v)
         + DisMod[NN].DisDep[(Ilat+1)*DisMod[NN].Nlon+Ilon]   *u    *(1-v)
         + DisMod[NN].DisDep[Ilat*DisMod[NN].Nlon+Ilon+1]     *(1-u)*v
         + DisMod[NN].DisDep[(Ilat+1)*DisMod[NN].Nlon+Ilon+1] *u    *v;

    return 1;
}
int   M_SendLayer( float *lat, float *lon, float *dep, DisSur *DisMod, int *Nlayer,
                   int *LayerN)
{
    // Given: lat , lon ,dep, Nlayer, DisMod(.Dislat, .Dislon, Nlat, Nlon)
    //  calculat each depth within each dis interface, then get tmpdep array.
    //  by comparasion with dep and tmpdep[], then determine with layer this point belong to.
    //
    //    ---Surface----0 km------------
    //    |
    //    |     1st Layer             *  <-----dep =30, return Layer = 1
    //    |
    //    |--Moho-------52km------------ 1st Dis Surface
    //    |
    //    |     2nd layer                Nlayer = 2;
    //    |                              NDis Surface = Nlayer-1;
    //    |
    //    V
    //                    Diogram
    //
    // (WangSheng  2014/8/29)
    int    i;
    float  *tmpdep = (float *) calloc(*Nlayer,sizeof(float));
    for( i = 1; i < *Nlayer; ++i)
    {
        M_SendDis( lat, lon, &i, DisMod, &tmpdep[i]);
    }

    for(i = 1; i < *Nlayer; ++i)
    {
        if( *dep < tmpdep[i])
        {
            *LayerN = i;
            break;
        }
    }
    if( *dep >= tmpdep[*Nlayer-1]) *LayerN = *Nlayer;
    free(tmpdep);
    return *LayerN;
}
float M_GetVelRTM(float *lat, float *lon, float *dep,
                  int *KindWave,
                  Mod3D *M3D, int *Nlayer, DisSur *DisMod,
                  float *vel)
{
    int    i;
    int    j;
    int    k;

    int    I;
    int    J;
    int    K;
    int    N;
    int    NPNR;
    float  u;
    float  v;
    float  w;

    int    LayerN = 0;
    int    layer;
    //Get which layer this point is located in.
    M_SendLayer(lat, lon, dep,
                DisMod, Nlayer,
                &LayerN);
    layer = LayerN-1;

    M_LocateXF(M3D[layer].PN, &(M3D[layer].NP), lat, &I);
    M_LocateXF(M3D[layer].RN, &(M3D[layer].NR), lon, &J);
    M_LocateXF(M3D[layer].HN, &(M3D[layer].NH), dep, &K);

    (*vel) = 0.0;
    NPNR   = M3D[layer].NP * M3D[layer].NR;
    for( i = I; i <= I+1; ++i)
    {
        for( j = J; j <= J+1; ++j)
        {
            for( k = K; k <= K+1; ++k)
            {
                N = k*NPNR + i*M3D[layer].NR + j;
                u = 1.0 - fabs( (*lat-M3D[layer].PN[i]) / (M3D[layer].PN[I+1]-M3D[layer].PN[I]) );
                v = 1.0 - fabs( (*lon-M3D[layer].RN[j]) / (M3D[layer].RN[J+1]-M3D[layer].RN[J]) );
                w = 1.0 - fabs( (*dep-M3D[layer].HN[k]) / (M3D[layer].HN[K+1]-M3D[layer].HN[K]) );
                if( *KindWave  == 1)   (*vel) += u*v*w*M3D[layer].VELP[N]; //P wave
                else if( *KindWave==2) (*vel) += u*v*w*M3D[layer].VELS[N]; //S wave
                else
                {
                    fprintf(stderr," KindWave Must 1 or 2, Can't =%d. In Subroutine M_GetVelRTM\n", *KindWave);
                    exit(0);
                }
            }
        }
    }
    return *vel;
}
int Read_DisSur(char *Disfile, int *Nlayer, DisSur *DisMod)
{
    int     i;
    int     j;
    int     m;
    int     n;
    FILE  *fpdis;

    if ( !( fpdis=fopen(Disfile, "r") ) )
    {
        fprintf(stderr, "Cannot Open %s\n", Disfile);
        exit(0);
    }

    for( i = 0; i < (*Nlayer-1); ++i )
    {

        if( EOF ==fscanf(fpdis,"%d %d", &(DisMod[i].Nlat), &(DisMod[i].Nlon)) )
        {
        	fprintf(stderr, "Err: can't read file %s\n", Disfile);
        	exit(1);
        }

        DisMod[i].DisLat = (float *)calloc(DisMod[i].Nlat, sizeof(float) );
        DisMod[i].DisLon = (float *)calloc(DisMod[i].Nlon, sizeof(float) );
        DisMod[i].DisDep = (float *)calloc(DisMod[i].Nlat*DisMod[i].Nlon, sizeof(float) );

        for( j = 0; j < DisMod[i].Nlat; ++j)
        	if( EOF == fscanf(fpdis, "%f", &(DisMod[i].DisLat[j]) )  )
        	{
        		fprintf(stderr, "Err: can't read file %s\n", Disfile);
        		exit(1);
        	}
        for( j = 0; j < DisMod[i].Nlon; ++j)
        	if( EOF == fscanf(fpdis, "%f", &(DisMod[i].DisLon[j]) )  )
        	{
        		fprintf(stderr, "Err: can't read file %s\n", Disfile);
				exit(1);
        	}
        for( n = 0; n < DisMod[i].Nlat; ++n)
        {
            for( m = 0; m < DisMod[i].Nlon; ++m)
            {
                if( EOF == fscanf(fpdis,"%f", &(DisMod[i].DisDep[ n*DisMod[i].Nlon+m ]) ) )
                {
                	fprintf(stderr, "Err: can't read file %s\n", Disfile);
        			exit(1);
                }
                   //DisMod[i].DisDep[ n*DisMod[i].Nlon+m ] += 1.0;
            }
        }
    }
    fclose(fpdis);
    return 1;
     //test Moho Depth right or not
      /*
     for(i=0; i<DisMod[0].Nlat;i++)
     {
         for(j=0;j<DisMod[0].Nlon; j++)
         {
              fprintf(stderr,"%6.2f", DisMod[0].DisDep[ i*DisMod[0].Nlon+j ] );
         }
        fprintf(stderr,"\n");
     }
      */
}

