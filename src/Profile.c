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
    Profile   OutPut Vertical Profile Solutions Given Depth and two points.   \n\
Usage\n\
    Profile -D<dis filename> -M<rtm filename> -L<lon1/lat1/lon2/lat2>         \n\
            -P<mindep/maxdep>\n\
            -S<mohodata filename>\n\
            [-T<limit>] [-I<itnneed>]\n\
            [-N<nh/nv>] [-K<kind>] [-H]\n\
\n\
\n\
Description\n\
    Given data from <rtmfile> <disfile>, Profile will generate a vertical rectangle profile  \n\
    which is specialized by <lon1,lat1,lon2,lat2> and <mindep,maxdep>.                       \n\
    <itnneed> appoint which time the result inversed within <rtmfile> will be used to generate\n\
    the profile. Its default value is 0, corresponding to initial RTM model.                  \n\
    <nh> appoint the horizontal density of the grid, as <nv> does to the vertical direction.  \n\
    Default value of <nh,nv> is <200,200>.\n\
    <kind> appoint whether absolute velocity, which has the value of <0> or relative velocity \n\
    percentage, which has the value <1> is to generate. Default value of <kind> is 1.\n\
\n\
Required Argument\n\
    -D<dis filename>                                           --disfile\n\
    -M<rtm filename>                                           --rtmfile\n\
    -L<lon1/lat1/lon2/lat2>                                    --line   \n\
    -P<mindep/maxdep>                                          --dep    \n\
    -S<mohodata filename>                                      --moho   \n\
\n\
Optional Argument\n\
    [-T<limit>]   Not useful.                                  --limit   \n\
    [-I<itnneed>] Default value is 0.                          --itnneed \n\
    [-N<nh/nv>]   Default value is <200,200>.                  --nhv     \n\
    [-K<kind>]    0: Absolute velocity to generate.            --kind    \n\
                  1: Relative velocity percentage to generate.\n\
                  Default value is 0\n\
    [-H]          Help message.                                --help\n\
\n\
Example:\n\
    1.\n\
    Profile -D m.dis -M RTM.out -L 110/10/115/15 -P 0/200 -T 0.5 -I 1 -N 200/100 -K 0 \n\
    2.\n\
    Profile --disfile=m.dis --rtmfile=RTM.out --line=110,10,115,15 --dep=0,200 --limit=0.5\n\
            --itnneed=1 --nhv=200,100 --kind=1  \n\
\n\
Designed by Li Zhiwei   2005/12/04  IGGCAS, Beijing.\n\
Modified by WangSheng   2014/09/15  IGGCAS, Beijing.\n\n\
**** Help information  ******************************************************\n\
"
};
int main(int argc, char *argv[])
{
    //////////////////////////////////////////////////////////////
    char    *str_disfile = NULL,
            *str_modfile = NULL,
            *str_line    = NULL,
            *str_dep     = NULL,
            *str_limit   = NULL,
            *str_moho    = NULL,
            *str_itnneed = NULL,
            *str_nhv     = NULL,
            *str_kind    = NULL;
    int     flag_disfile = 0 ,
            flag_modfile = 0 ,
            flag_line    = 0 ,
            flag_dep     = 0 ,
            flag_limit   = 0 ,
            flag_moho    = 0 ,
            flag_itnneed = 0 ,
            flag_nhv     = 0 ,
            flag_kind    = 0 ;

    int  oc;
    char ec;
    int  optionindex = -9999;
    static struct option ProfOptions[]=
    {
        {"disfile", required_argument, NULL, 'D'},  // disfile
        {"rtmfile", required_argument, NULL, 'M'},  // modfile
        {"line",    required_argument, NULL, 'L'},  // line
        {"dep",     required_argument, NULL, 'P'},  // ??
        {"moho",    required_argument, NULL, 'S'},  // Moho depth
        {"limit",   required_argument, NULL, 'T'},  // Solution within (-$,$)
        {"itnneed", required_argument, NULL, 'I'},  // ??
        {"nhv",     optional_argument, NULL, 'N'},  // ??
        {"kind",    optional_argument, NULL, 'K'},  // 1:Relative ak OR 0:Absolute ak
        {"help",    optional_argument, NULL, 'H'},
        {0,         0,                 0,     0}
    };
    while( (oc = getopt_long(argc, argv, ":D:M:L:P:S:T:I:N:K:H", ProfOptions, &optionindex)) != -1)
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
            case 'L':
                str_line     = optarg;
                flag_line    = 1;
                break;
            case 'P':
                str_dep      = optarg;
                flag_dep     = 1;
                break;
            case 'S':
                str_moho     = optarg;
                flag_moho    = 1;
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
                str_nhv      = optarg;
                flag_nhv     = 1;
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
         flag_line    != 1 ||
         flag_dep     != 1 ||
         flag_limit   != 1 ||
         flag_moho    != 1 ||
         flag_itnneed != 1 ||
         flag_nhv     != 1 ||
         flag_kind    != 1)
    {
        printf("%s\n",HMSG);
        exit(0);
    }
    float   lon1,
            lat1,
            lon2,
            lat2;
    float   depmin,
            depmax;
    float   limit;
    int     itnneed;
    int     nh,
            nv;
    int     kind;
    Mod3D   RTMod[7];
    Mod3D   RTMod_Ori[7];
    DisSur  DisMod[6];
    int     NumofLayer;
    float   dh,
            dlat,
            dlon;
    int     i,
            j;
    float   dep,
            lon,
            lat;
    int     KindWave = 1;
    float   vel,
            vel_Ori;
    float   disdep;
    int     Ndis = 1;
    FILE    *fpmoho;
    if((fpmoho=fopen(str_moho,"w")) == NULL)
    {
        fprintf(stderr, " Err: Can't open file for write: %s\n", str_moho);
        exit(0);
    }
    //proflzw dis=m.dis mod=RTM.out sl=110,10,115,15 dr=0,200 limit=0.5 nhv=200,100 itn=1 kind=0 >tmp
    sscanf(str_line,   "%f/%f/%f/%f", &lon1, &lat1, &lon2, &lat2);
    sscanf(str_dep,    "%f/%f",&depmin, &depmax);
    sscanf(str_limit,  "%f", &limit);
    sscanf(str_itnneed,"%d", &itnneed);
    sscanf(str_nhv,    "%d/%d", &nh, &nv);
    sscanf(str_kind,   "%d", &kind);

    // Read Initial model velocity
    ReadRTM(str_modfile, RTMod_Ori ,&NumofLayer, 0);

    // Read itnneed model velocity
    ReadRTM(str_modfile, RTMod ,&NumofLayer, itnneed);

    // Read Moho depth data
    Read_DisSur(str_disfile, &NumofLayer, DisMod);

    dh   = (depmax - depmin) /nv;
    dlat = (lat2 - lat1)/nh;
    dlon = (lon2 - lon1)/nh;

    // Acquire moho depth data
    for(j = 0; j <= nh; ++j)
    {
        lat = lat1 + dlat*j;
        lon = lon1 + dlon*j;
        //M_GetVelRTM(&lat, &lon, &dep,
        //      &KindWave,
        //      RTMod, &NumofLayer, DisMod,
        //      &vel);
        M_SendDis(&lat, &lon, &Ndis, DisMod,
              &disdep);
        fprintf(fpmoho,"%10.4f %10.4f %10.4f\n",lon,lat,disdep);
    }
    // Acquire velocity data
    for (i = 0; i <=nv; ++i)
    {
        dep = depmin + dh * i;
        for(j = 0; j <= nh; ++j)
        {
            lat = lat1 + dlat*j;
            lon = lon1 + dlon*j;
            M_GetVelRTM(&lat, &lon, &dep,
                  &KindWave,
                  RTMod, &NumofLayer, DisMod,
                  &vel);
            //M_SendDis(&lat, &lon, &Ndis, DisMod,
            //      &disdep);
            if( kind == 0)
                printf("%10.4f %10.4f %10.4f %9.3f\n",lon,lat,dep,vel);

            if( kind == 1)
            {
                M_GetVelRTM(&lat, &lon, &dep,
                  &KindWave,
                  RTMod_Ori, &NumofLayer, DisMod,
                  &vel_Ori);
                printf("%10.4f %10.4f %10.4f %15.10f\n",lon,lat,dep,(vel-vel_Ori)/vel_Ori);
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