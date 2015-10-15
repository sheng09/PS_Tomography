/* Eqsta.h declared data classes, structures and subroutines to read and write Earthquake-Station data.
   The default input format is displayed within ./example/ ***.dat. It is free to append other subroutines
   for read and write.
   C/C++ is used, while "vector" is the base container.
   Horizontal tabs sizes is '4', it is simple to replace 'tabs' with 4 or 8 'space', thus make it easy to
   read.

   Designed by Wangsheng, Institude of Geology and Geophysics,Chinese Academy of Science.
   Email: wangsheng.cas@gmail.com
   Time: 11/2014
   Lasted revised time:
*/

#ifndef EQSTA
#define EQSTA

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <cctype>
#include <vector>
#include <string>
#include <algorithm>


using std::vector;
using std::string;

#define FLAG unsigned

#define R0 6371.277
#define PI 3.1415926535
#define _USEFUL    1
#define _NONUSEFUL 0

#define _ALL ~0  // 1...11111b
#define _PGY 1u  // 0...00001b
#define _PGN 0u
#define _SGY 2u  // 0...00010b
#define _SGN 0u
#define _PNY 4u  // 0...00100b
#define _PNN 0u
#define _SNY 8u  // 0...01000b
#define _SNN 0u

#define _INEQ    -9
#define _TELEEQ

#define _CMPSTAY  1  // Station info(especially coordinates) is completed
#define _CMPSTAN  0  // .............  is not completed

#define _DELTAY 1    // Earthquake-Station distance(great circle) is calculated
#define _DELTAN 0    // ............................is not calculated

#define _TRAVTY 1    // Observated traveltime is calculated
#define _TRAVTN 0    // ......................is not calculated

#define _FWDTY  1    // Forward traveltime calculation is done
#define _FWDTN  0    //                           .....is not...

#define _CORDTRFY 1  // Coordinats is transformed from Longitude-Latitude to x-y-z
#define _CORDTRFN 0  //      ......is  not .......


#define SQ(x) ((x)*(x))

typedef     char    _C4[4];
typedef     char    _C8[8];
typedef    	char	_Name[8];    //
typedef		char	_Grid[8];    //
typedef 	char 	_Mtype[4];   // Magnitude type
typedef 	char    _Line[1024]; //

class _Coord
{
	public:
	double x;       //km
	double y;
	double z;
	double theta;   //co-latitude in rad
	double fai;
	double r;		//km
	double lon;
	double lat;
	double h;		//km, positive oversurface, negative undersurface, zero on the surface
	int	CoordTransf_Car();	// Coordinates transformation from Cartersian coordinates
	int	CoordTransf_Sph();	//
	int	CoordTransf_Geo();	//
	int	CoordTransf_Geo2();	//
	_Coord();
	~_Coord();
};
class Time
{
	public:
	int		y;		//year
	int		mo;		//month
	int		d;		//day
	int		h;		//hour
	int		m;		//minute
	double  s;		//second
	double  t;      //seconds relative to  '1970-1-1 00:00:00'

	double absoluteTime(); // Calculate 'this->t'
	Time();
	Time(int _y, int _mo, int _d, int _h, int _m, float _s);
	~Time();
};

class StationTime
{
	public:
	double rsdT;   // Residual traveltime
	double trlT;   // Observed traveltime
	Time   arvT;   // arrivetime
	string phase;  // seismic phase name corresponding to traveltime and arrtivetime
	//FLAG   phaseFlag;
	double TravelTime();
	StationTime();
	~StationTime();
};
class Station
{
	public:
	FLAG 		IsUseful;	// whether this station is useful or not? Default value is _USEFUL.
	FLAG        IsCmpSta;   // whether coordinates is complete.
	FLAG        IsCordTrf;  // whether coordinates is transformed.

	int			num1;		// old station number
	int			num2;		// new station number
	_Name 		add;		// Addition info
	_Name 		name; 		// station name
	_Grid 		grid; 		// station grid name
	_Coord      coord;      // station coordinates
	double 		delta;      // great circle distance between this station and its dependent earthquke

	StationTime staT;       // Station time info

	int	CoordTransf_Car();	// Coordinates transformation from Cartersian coordinates
	int	CoordTransf_Sph();	//
	int	CoordTransf_Geo();	//
	int	CoordTransf_Geo2();	//

	// Pick station given region, return value appoint whether this station is useful or not.
	int PickSta(double latmin, double latmax, double lonmin, double lonmax);
	// Pick station given type of wave, return value appoint whether this station is useful or not.
	int PickStaWaveType(FLAG wavetype);
	// Output station information in format: <num1 num2 lat lon height>
	int OutSta(FILE **fp);
	// Output station information for gmt plot, format <lon lat height>
	int OutSta4GMT(FILE **fp);

	Station();
	~Station();
};

class Earthquake
{
	public:
	FLAG 		IsUseful;				// Whether this earthquake is useful or not, default value is _USEFUL.
	FLAG        IsCordTrf;  			// whether coordinates is transformed.

	_Coord      coord;                  // Eartquake coordinates
	Time	 	originT;				// Origin time

	int	   		num;					// Earthquake number
	_Name  		name;					// related name
	_Grid  		grid;					// related grid
	_Mtype 		mtype;	            	// magnitude type
	double 		mag;		            // magnitude
	int    		type;					//

	int	CoordTransf_Car();	// Coordinates transformation from Cartersian coordinates
	int	CoordTransf_Sph();	//
	int	CoordTransf_Geo();	//
	int	CoordTransf_Geo2();	//
	// Pick earthquke given region field, return value appoint whether this earthquake is useful or not.
	int PickEq(double latmin, double latmax, double lonmin, double lonmax);
	// Output earthquake information for gmt plot, format<lon lat height mag>
	// (Sometimes, mag is not given from asc9 data, thus, default value '0' is output)
	int OutEq4GMT(FILE **fp);
	Earthquake();
	~Earthquake();
};



class EqSta
{
	public:
	/**
	 * FLAG, for default ASC9 file reading,
	 * 		IsTravt  = _TRAVTN       After  TravelTime(),       = _TRAVTY
	 * 		IsResdt  = _RESTN        If dt!= 0.0,               = _RESTY
	 * 		IsFwdt   = _FWDTN        If dt!= 0.0,               = _FWDTN
	 * 		IsCmpSta = _CMPSTAN   	 After CompleteSta(),       = _CMPSTAY
	 * 		IsDelta  = _DELTAN       After  CalculateDelta(), 	= _DELTAY
	 * 		IsUseful = _USEFUL       After Pick*(),             = (Depends on the Pick* result)
	 */
	FLAG IsTravt; 		// Actual  travel time is calculated or not?
	FLAG IsFwdt;		// Forward travel time is calculated or not?
	FLAG IsCmpSta;		// Station's coordinates is given(complete) or not?
	FLAG IsDelta;		// Delta is calculated or not?
	FLAG IsUseful;		// This record is useful or not?
	FLAG IsCordTrf;  	// Whether coordinates is transformed.

	/**
	 * Data 1.earthquake  2.stations[]
	 */
	Earthquake eq;			// Earthquake data
	int	ns;					// Number of total stations (sta.size() )
	int nsUseful;			// Number of stations that are useful
	vector<Station> sta;	// Station records data

	EqSta();
	~EqSta();

	// Pick records given region field and min number of records of per earthquake.
	int PickEqSta(double latmin, double latmax, double lonmin, double lonmax, int _min);

	// Pick records given residual traveltime upper limit and ...
	int PickEqStaDt(double _dt, int _min);

	// Pick records given distance range [_minDis, _maxDis) and ...
	int PickEqStaDis(double _minDis, int _maxDis, int _min);

	// Pick records given type of wave and ...
	int PickEqStaWaveType(int wavetype, int _min);

	// Pick records given min number of records of per earthquake.
		int PickEqStaNum(int _min);

	// Count num of useful stations
	int GetNumSta();
	// Count num of useful stations for different kind of waves, ak is returned
	int GetNumStaWaveType(FLAG wavetype);

	// Caculate actual travel time
	// Related info will be output is calculated travel time greater than _max
	int TravelTime( double _max);

	//  Complete station info(coordinates) according to its 'num2', given external station recorsds
	int CompleteSta(vector<Station> *_sta);

	// 'eq' and 'sta' 's  coordinates tranformation
	int CoordTransf_Geo2();

	// Caculate delta. CompleteSta() function should run fisrtly
	int CalculateDelta();


	/*************************** Output Subroutines ******************************/
	// Output ray path data for gmt plot. (Formate  eq.lon eq.lat sta.lon sta.lat)
	int OutRaypath4GMT(FILE **fp);

	// Output Delta-Traveltime data for gmt plot, _f is used to distinguish
	// _Wavetype = (_PGY | _SGY | _PNY | _SNY)
	int OutDelta_Time4GMT(FILE **fp, FLAG _Wavetype);

	// Output Delta-Residule traveltime for gmt plot, _f is used to distinguish
	// _Wavetype = (_PGY | _SGY | _PNY | _SNY)
	int OutDelta_ResidualTime4GMT(FILE **fp, FLAG _Wavetype);

	// Output Delta-Forward calculated traveltime for gmt plot, _f is used to distinguish
	// _Wavetype = (_PGY | _SGY | _PNY | _SNY)
	int OutDelta_FWDTime4GMT(FILE **fp, FLAG _Wavetype);


	// Output earthquake-station data in ASC9 format to file, given type of wave
	//int OutputASC9(FILE **fp, FLAG wavetype);
	// Output useful earthquake-station data in ASC9 format to file
	int OutputASC9(FILE **fp);

};

double Distance(_Coord p1, _Coord p2 );
/**
 * Calculate time intervel(seconds) given two Time point
 * @param  t1 [time point 1]
 * @param  t2 [time point 2]
 * @return    [time intervel(seconds) between t1 and t2]
 */
double Dt(Time t1,Time t2);  // t1<t2
//double Dt(Time t1, Time t2); // t1< t2

/**
 * Sort station[] according to its 'num2'
 * @param  sta [vector pointer of station records]
 * @return     [1]
 */
int SortStation(vector<Station> *sta);

/**
 * Read station records from <**fp>
 * @param  sta [vector pointer to store station recorsds]
 * @param  Ns  [number of stations]
 * @param  fp  [file pointer of stations input file]
 * @return     [Ns]
 */
int ReadSta(vector<Station> *sta, int *Ns, FILE **fp);


/**
 * Read station record from <**fp> (format: num1 num2 lat lon height(m) Grid Name)
 * @param  sta [vector pointer to store station recorsds]
 * @param  Ns  [number of stations]
 * @param  fp  [file pointer of stations input file]
 * @return     [description]
 */
int ReadSta4NAME(vector<Station> *sta, int *Ns, FILE **fp);

int ReadStaFormat1(vector<Station> *sta, int *Ns, FILE **fp);

int ReadStaShandong(vector<Station> *sta, int *Ns, FILE **fp);

int ReadStaHeibei(vector<Station> *sta, int *Ns, FILE **fp);



/**
 * Read earthquake-stations data from ASC9 format file
 * @param  record [vector pointer to store earthquake-station data]
 * @param  fp     [file pointer of ASC9 format input file]
 * @return        [description]
 */
int ReadASC9(vector<EqSta> *record, FILE **fp);
/**
 * Read earthquake-stations data from Liaoling records, and complete its station num given '*sta'
 * @param  record [vector pointer to store earthquake-station data]
 * @param  fp     [file pointer of ASC9 format input file]
 * @return        [description]
 */
//int ReadFormatLiaoling(vector<EqSta> *record, vector<Station> *sta, FILE **fp);
int ReadFormatLiaoling(vector<EqSta> *record, vector<Station> *sta, FILE **fp, FLAG _waveflag);

int ReadFormat1(vector<EqSta> *record, FILE **fp);

int ReadFormatShandong(vector<EqSta> *record, FILE **fp);

int ReadFormatHeibei2000(vector<EqSta> *record, FILE **fp);


#endif