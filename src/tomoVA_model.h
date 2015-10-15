
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef TOMOVA_M_IO
#define TOMOVA_M_IO

typedef struct
{
    int    NP;
    int    NR;
    int    NH;
    float *PN;
    float *RN;
    float *HN;
    float *VELP, *VELS;
}Mod3D;
/*----------------- Discontinous Surface----------------------*/
typedef struct{
    int    Nlat, Nlon;
    float *DisLat, *DisLon;
    float *DisDep;
}DisSur;

int ReadRTM(char *RTMfile, Mod3D *RTMod,int *NumofLayer, int itnneed);
int   M_LocateXF(float xx[], int *nf, float *xf,
                 int *j);
int   M_SendDis(float *lat, float *lon, int *N, DisSur *DisMod,
                float *dep);
int   M_SendLayer( float *lat, float *lon, float *dep, DisSur *DisMod, int *Nlayer,
                   int *LayerN);
float M_GetVelRTM(float *lat, float *lon, float *dep,
                  int *KindWave,
                  Mod3D *M3D, int *Nlayer, DisSur *DisMod,
                  float *vel);
int Read_DisSur(char *Disfile, int *Nlayer, DisSur *DisMod);

#endif