#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "tomoVA_model.h"


static char HMSG[]=
{
"\n\
Name\n\
    Section  OutPut horizontal section solutions given region and depth.   \n\
Usage\n\
    Section -D <dis filename> -M <rtm filename> -R <lonmin/lonmax/latmin/latmax>         \n\
            -P <depth>\n\
            [-T <limit>] [-I <itnneed>]\n\
            [-N <nLon/nLat>] [-K <kind>] [-H]\n\
\n\
\n\
Description\n\
    Given data from <rtmfile> <disfile>, Section will generate a horizontal rectangle section  \n\
    which is specialized by <lonmin/lonmax/latmin/latmax> and <depth>.                       \n\
    <itnneed> appoint which iteration time the result inversed within <rtmfile> will be used \n\
    to generate the profile. Its default value is 0, corresponding to initial RTM model.\n\
    <nLon/nLat> appoint the horizontal density of the grid. Default value of <nLon/nLat> is <200/200>.\n\
    <kind> appoint whether absolute velocity, which has the value of <0> or relative velocity \n\
    percentage, which has the value <1>. Default value of <kind> is 1.\n\
\n\
Required Argument\n\
    -D <dis filename>                                           --disfile\n\
    -M <rtm filename>                                           --rtmfile\n\
    -R <lonmin/lonmax/latmin/latmax>                            --region   \n\
    -P <depth>                                                  --dep    \n\
\n\
Optional Argument\n\
    [-T <limit>]   Not useful.                                  --limit   \n\
    [-I <itnneed>] Default value is 0.                          --itnneed \n\
    [-N <nLon/nLat>]   Default value is <200,200>.              --nLL     \n\
    [-K <kind>]    0: Absolute velocity to generate.            --kind    \n\
                   1: Relative velocity percentage to generate.\n\
                   Default value is 0\n\
    [-H]           Help message.                                --help\n\
\n\
Example:\n\
    1.\n\
    Profile -D m.dis -M RTM.out -R 110/115/10/15 -P 30 -T 0.5 -I 1 -N 200/400 -K 0 \n\
    2.\n\
    Profile --disfile=m.dis --rtmfile=RTM.out --region=110/115/10/15 --dep=30 --limit=0.5\n\
            --itnneed=1 --nLL=200/400 --kind=1  \n\
\n\
Designed by WangSheng   2014/09/15  IGGCAS, Beijing.\n\n\
**** Help information  ******************************************************\n\
"
};

int main(int argc, char *argv[])
{
    //////////////////////////////////////////////////////////////
    char    *str_disfile = NULL,
            *str_modfile = NULL,
            *str_region  = NULL,
            *str_dep     = NULL,
            *str_limit   = NULL,
            *str_itnneed = NULL,
            *str_nLonLat = NULL,
            *str_kind    = NULL;
    int     flag_disfile = 0 ,
            flag_modfile = 0 ,
            flag_region  = 0 ,
            flag_dep     = 0 ,
            flag_limit   = 0 ,
            flag_itnneed = 0 ,
            flag_nLonLat = 0 ,
            flag_kind    = 0 ;

    int  oc;
    char ec;
    int  optionindex = -9999;
    static struct option ProfOptions[]=
    {
        {"disfile", required_argument, NULL, 'D'},  // disfile
        {"rtmfile", required_argument, NULL, 'M'},  // modfile
        {"region",  required_argument, NULL, 'R'},  // region
        {"dep",     required_argument, NULL, 'P'},  // which depth
        {"limit",   required_argument, NULL, 'T'},  // Solution within (-$,$)
        {"itnneed", required_argument, NULL, 'I'},  // ??
        {"nLL",     optional_argument, NULL, 'N'},  //
        {"kind",    optional_argument, NULL, 'K'},  // 1:Relative ak OR 0:Absolute ak
        {"help",    optional_argument, NULL, 'H'},
        {0,         0,                 0,     0}
    };
    while( (oc = getopt_long(argc, argv, ":D:M:R:P:T:I:N:K:H", ProfOptions, &optionindex)) != -1)
    {
        switch(oc)
        {
            case 'D':
                str_disfile  = optarg;
                flag_disfile = 1;
                break;
            case 'M':
                str_modfile  = optarg;
                flag_modfile = 1;
                break;
            case 'R':
                str_region     = optarg;
                flag_region    = 1;
                break;
            case 'P':
                str_dep      = optarg;
                flag_dep     = 1;
                break;
            case 'T':
                str_limit    = optarg;
                flag_limit   = 1;
                break;
            case 'I':
                str_itnneed  = optarg;
                flag_itnneed = 1;
                break;
            case 'N':
                str_nLonLat      = optarg;
                flag_nLonLat     = 1;
                break;
            case 'K':
                str_kind     = optarg;
                flag_kind    = 1;
                break;
            case 'H':
                printf("%s\n",HMSG);
                exit(0);
            case ':':
                break;
                printf("无效的选项字符");
                exit(0);
            case '?':
                ec = (char)optopt;
                printf("\'-%c\' OR \'--%s\' 缺少选项参数！\n",ec,ProfOptions[optionindex].name);
                exit(0);
                break;
        }

    }
    //////////////////////////////////
    if ( flag_disfile != 1 ||
         flag_modfile != 1 ||
         flag_region    != 1 ||
         flag_dep     != 1 ||
         flag_limit   != 1 ||
         flag_itnneed != 1 ||
         flag_nLonLat     != 1 ||
         flag_kind    != 1)
    {
        printf("%s\n",HMSG);
        exit(0);
    }
    float   lonmin,
            lonmax,
            latmin,
            latmax;
    float   dep;
    float   limit;
    int     itnneed;
    int     nLon,
            nLat;
    int     kind;
    Mod3D   RTMod[7];
    Mod3D   RTMod_Ori[7];
    DisSur  DisMod[6];
    int     NumofLayer;
    float   dlat,
            dlon;
    int     i,
            j;
    float   lon,
            lat;
    int     KindWave = 1;
    float   vel,
            vel_Ori;
    //proflzw dis=m.dis mod=RTM.out sl=110,10,115,15 dr=0,200 limit=0.5 nhv=200,100 itn=1 kind=0 >tmp
    sscanf(str_region,   "%f/%f/%f/%f", &lonmin, &lonmax, &latmin, &latmax);
    sscanf(str_dep,    "%f",&dep);
    sscanf(str_limit,  "%f", &limit);
    sscanf(str_itnneed,"%d", &itnneed);
    sscanf(str_nLonLat,    "%d/%d", &nLon, &nLat);
    sscanf(str_kind,   "%d", &kind);

    ReadRTM(str_modfile, RTMod_Ori ,&NumofLayer, 0);

    ReadRTM(str_modfile, RTMod ,&NumofLayer, itnneed);

    Read_DisSur(str_disfile, &NumofLayer, DisMod);

    dlat = (latmax - latmin)/nLat;
    dlon = (lonmax - lonmin)/nLon;
    for(i = 0; i <= nLon; ++i)
    {
        lon = dlon*i+lonmin;
        for(j = 0; j <= nLat; ++j)
        {
            lat = dlat*j+latmin;
            M_GetVelRTM(&lat, &lon, &dep,
                        &KindWave,
                        RTMod, &NumofLayer, DisMod,
                        &vel);
            if( kind == 0)
                printf("%10.4f %10.4f %9.3f \n",lon,lat,vel);
            else if( kind == 1)
            {
                M_GetVelRTM(&lat, &lon, &dep,
                  &KindWave,
                  RTMod_Ori, &NumofLayer, DisMod,
                  &vel_Ori);
                if((vel >= vel_Ori))
                {
                    printf("%10.4f %10.4f %15.10f %10.5lf\n",lon,lat,  fabs((vel-vel_Ori)/vel_Ori), 1.0);
                }
                else
                {
                    printf("%10.4f %10.4f %15.10f %10.5lf\n",lon,lat,  fabs((vel-vel_Ori)/vel_Ori),-1.0);
                }
                //printf("%10.4f %10.4f %15.10f %10.5lf\n",lon,lat,(vel-vel_Ori)/vel_Ori,  fabs((vel-vel_Ori)/vel_Ori));
            }
        }
    }

    for (i = 0; i < NumofLayer; ++i)
    {
        free(RTMod[i].PN);
        free(RTMod[i].RN);
        free(RTMod[i].HN);
        free(RTMod[i].VELP);
        free(RTMod_Ori[i].PN);
        free(RTMod_Ori[i].RN);
        free(RTMod_Ori[i].HN);
        free(RTMod_Ori[i].VELP);
    }
    return 0;
}