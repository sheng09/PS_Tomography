#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <math.h>
#include "Eqsta.h"
#include <iostream>

_Coord::_Coord()
{
	x     = -9999.9999;
	y     = -9999.9999;
	z     = -9999.9999;
	theta = -9999.9999;
	fai   = -9999.9999;
	r     = -9999.9999;
	lon   = -9999.9999;
	lat   = -9999.9999;
	h     = -9999.9999;
}
_Coord::~_Coord()
{}
// Coordinates transformation from Cartersian coordinates
int	_Coord::CoordTransf_Car()
{
	theta = atan2( sqrt( SQ(x)+SQ(y) ), z );
	fai   = atan2( y, x);
	r     = sqrt( SQ(x)+SQ(y)+SQ(z));

	if(fai<PI)
	{
		lon = fai *180.0/PI;
	}
	else
	{
		lon = fai *180.0/PI -360.0;
	}
	lat = 90.0 - theta*180.0/PI;
	h = r -R0;
	return 0;
}
int	_Coord::CoordTransf_Sph()
{
	x = r * sin(theta) *cos(fai);
	y = r * sin(theta) *sin(fai);
	z = r * cos(theta);

	if(fai<PI)
	{
		lon = fai *180.0/PI;
	}
	else
	{
		lon = fai *180.0/PI -360.0;
	}
	lat = 90.0-theta*180.0/PI;
	h   = r- R0;
	return 0;
}
int	_Coord::CoordTransf_Geo()
{
	theta = (90.0-lat)/180.0*PI;
	if(lon > 0.0)
	{
		fai = lon/180.0*PI;
	}
	else
	{
		fai = (360.0+lon)/180.0*PI;
	}
	h = r-R0;
	x = r * sin(theta) *cos(fai);
	y = r * sin(theta) *sin(fai);
	z = r * cos(theta);
	return 0;
}
int	_Coord::CoordTransf_Geo2()
{
	theta = (90.0-lat)/180.0*PI;
	if(lon > 0.0)
	{
		fai = lon/180.0*PI;
	}
	else
	{
		fai = (360.0+lon)/180.0*PI;
	}
	r = h+R0;
	x = r * sin(theta) *cos(fai);
	y = r * sin(theta) *sin(fai);
	z = r * cos(theta);
	return 0;
}

double Time::absoluteTime()
{
	Time zero(1970,1,1,0,0,0.0);
	t = Dt(*this,zero);
	return t;
}
Time::Time()
{
	y 	= 0;		//year
	mo	= 0;		//month
	d 	= 0;		//day
	h 	= 0;		//hour
	m 	= 0;		//minute
	s 	= 0.0;		//second
}
Time::Time(int _y, int _mo, int _d, int _h, int _m, float _s)
{
	y  = _y;
	mo = _mo;
	d  = _d;
	h  = _h;
	m  = _m;
	s  = _s;
}
Time::~Time()
{}

StationTime::StationTime()
{
	rsdT = 0.0;
	trlT = 0.0;
}
StationTime::~StationTime()
{}

Station::Station()
{
	IsUseful  = _USEFUL;
	IsCmpSta  = _CMPSTAN;
	IsCordTrf = _CORDTRFN;
	num1      = -9999;
	num2      = -9999;
	memset(add ,0,sizeof(add));
	memset(name,0,sizeof(name));
	memset(grid,0,sizeof(grid));
	delta = 0.0;
}
Station::~Station()
{}

// Coordinates transformation from Cartersian coordinates (xyz)
int Station::CoordTransf_Car()
{
	coord.CoordTransf_Car();
	return 0;
}
// Coordinates transformation from Spherical coordinates (r,theta,fai)
int Station::CoordTransf_Sph()
{
	coord.CoordTransf_Sph();
	return 0;
}
// Coordinates transformation from Geographical coordinates (r,longitude,latitude)
int Station::CoordTransf_Geo()
{
	coord.CoordTransf_Geo();
	return 0;
}
// Coordinates transformation from Geographical2 coordinates (h,longitude,latitude) (h = r- R0)
int Station::CoordTransf_Geo2()
{
	coord.CoordTransf_Geo2();
	return 0;
}
// Pick station given region, return value appoint whether this station is usdeful or no.
int Station::PickSta(double latmin, double latmax, double lonmin, double lonmax)
{
	// Judge whether this station is pre-nonuseful
	if(IsUseful == _NONUSEFUL) return _NONUSEFUL;
	// Judege whether stations coordinates are given
	if(IsCmpSta == _CMPSTAN)
	{
		fprintf(stderr, "Err: in Station::PickSta(), Station's coordinates have not been given!\n");
		fprintf(stderr, "Plese run EqSta::CompleteSta() firstly!\n" );
		exit(0);
	}
	if(coord.lon >= lonmin && coord.lon <= lonmax && coord.lat >=latmin && coord.lat <=latmax )
	{
		IsUseful = _USEFUL;
	}
	else
	{
		IsUseful = _NONUSEFUL;
	}
	return IsUseful;
}
// Pick station given type of wave, return value appoint whether this station is useful or not.
int Station::PickStaWaveType(FLAG wavetype)
{
	// Judge whether this station is pre-nonuseful
	if(IsUseful == _NONUSEFUL) return _NONUSEFUL;
	IsUseful = _NONUSEFUL;
	if(staT.phase == "PG" && (wavetype & _PGY) == _PGY)
		IsUseful = _USEFUL;
	else if(staT.phase == "PN" && (wavetype & _PNY) == _PNY)
		IsUseful = _USEFUL;
	else if(staT.phase == "SG" && (wavetype & _SGY) == _SGY)
		IsUseful = _USEFUL;
	else if(staT.phase == "SN" && (wavetype & _SNY) == _SNY)
		IsUseful = _USEFUL;
	else
		IsUseful = _NONUSEFUL;
	return IsUseful;
}
// Output station information in format: <num1 num2 lat lon height>
int Station::OutSta(FILE **fp)
{
	_Line inLine;
	// Judge whether this station is useful
	if(IsUseful == _NONUSEFUL) return 0;
	// Judge whether this station's coordinates are given
	if(IsCmpSta == _CMPSTAN)
	{
		fprintf(stderr, "Err: in Station::OutSta(), Stations' coordinates have not been given!\n" );
		exit(0);
	}
	memset(inLine,0,sizeof(_Line));
	sprintf(inLine, "%-15d %-15d %-20.8lf %-20.8lf %-20.8lf",num1, num2, coord.lat, coord.lon, coord.h);
	fputs(inLine,*fp);
	fprintf(*fp, "\n");
	return 0;
}
// Output station information for gmt plot, format <lon lat height>
int Station::OutSta4GMT(FILE **fp)
{
	_Line inLine;
	// Judge whether this station is useful
	if(IsUseful == _NONUSEFUL) return 0;
	// Judge whether this station's coordinates are given
	if(IsCmpSta == _CMPSTAN)
	{
		fprintf(stderr, "Err: in Station::OutSta(), Stations' coordinates have not been given!\n" );
		exit(0);
	}
	memset(inLine,0,sizeof(_Line));
	sprintf(inLine, "%-20.8lf %-20.8lf %-20.8lf", coord.lon, coord.lat, coord.h);
	fputs(inLine,*fp);
	fprintf(*fp, "\n");
	return 0;
}



Earthquake::Earthquake()
{
	IsUseful = _USEFUL;
	IsCordTrf= _CORDTRFN;
	num      = -9999;
	mag      = 0.0;
	memset(name,0,sizeof(name));
	memset(grid,0,sizeof(grid));
	memset(mtype,0,sizeof(mtype));
}
Earthquake::~Earthquake()
{}
// Coordinates transformation from Cartersian coordinates (xyz)
int Earthquake::CoordTransf_Car()
{
	coord.CoordTransf_Car();
	return 0;
}
// Coordinates transformation from Spherical coordinates (r,theta,fai)
int Earthquake::CoordTransf_Sph()
{
	coord.CoordTransf_Sph();
	return 0;
}
// Coordinates transformation from Geographical coordinates (r,longitude,latitude)
int Earthquake::CoordTransf_Geo()
{
	coord.CoordTransf_Geo();
	return 0;
}
// Coordinates transformation from Geographical2 coordinates (h,longitude,latitude) (h = r- R0)
int Earthquake::CoordTransf_Geo2()
{
	// Judge whether coordinates is transformed
	if(IsCordTrf == _CORDTRFY ) return 0;
	coord.CoordTransf_Geo2();
	IsCordTrf= _CORDTRFY;
	return 0;
}
// Pick earthquke given region field, return value appoint whether this earthquake is useful or not.
int Earthquake::PickEq(double latmin, double latmax, double lonmin, double lonmax)
{
	// Judge whether this earthquake is pre-nonuseful
	if(IsUseful == _NONUSEFUL) return _NONUSEFUL;
	if(coord.lon >= lonmin && coord.lon <= lonmax && coord.lat >=latmin && coord.lat <= latmax )
	{
		IsUseful = _USEFUL;
	}
	else
	{
		IsUseful = _NONUSEFUL;
	}
	return IsUseful;
}
// Output earthquake information for gmt plot, format<lon lat height mag>
// (Sometimes, mag is not given from asc9 data, thus, default value '0.0' is output)
int Earthquake::OutEq4GMT(FILE **fp)
{
	_Line inLine;
	memset(inLine,0,sizeof(_Line));
	if(IsUseful == _USEFUL)
	{
		sprintf(inLine,"%-20.8lf %-20.8lf %-20.8lf %-20.8lf",coord.lon,coord.lat,-1.0*coord.h,mag);
		fputs(inLine,*fp);
		fprintf(*fp, "\n");
	}
	return 0;
}


EqSta::EqSta()
{
	IsUseful = _USEFUL;
	IsTravt  = _TRAVTN;
	IsFwdt   = _FWDTN;
	IsCmpSta = _CMPSTAN;
	IsDelta  = _DELTAN;
	IsUseful = _USEFUL;
	IsCordTrf= _CORDTRFN;
	ns       = 0;
	nsUseful = 0;
	if(!sta.empty()) sta.clear();
}
EqSta::~EqSta()
{
	if(!sta.empty()) sta.clear();
	//
}
// Pick records given region field and min number of records of per earthquake.
int EqSta::PickEqSta(double latmin, double latmax, double lonmin, double lonmax, int _min)
{
	// Judge whether this record is pre-nonuseful
	if(IsUseful == _NONUSEFUL) return _NONUSEFUL;
	if(ns < _min)
	{
		IsUseful = _NONUSEFUL;
		return _NONUSEFUL;
	}
	// Pick earthquakes
	IsUseful = eq.PickEq( latmin,  latmax,  lonmin,  lonmax);
	nsUseful = 0;
	if(IsUseful == _NONUSEFUL) return _NONUSEFUL;
	// Judge whether 'sta[]' 's coordinates are given
	if(IsCmpSta == _CMPSTAN)
	{
		fprintf(stderr, " Err in EqSta::PickEqSta(): sta[] 's coordinates have not been given!\n" );
		fprintf(stderr, " Please run EqSta::CompleteSta() firstly!\n");
		exit(0);
	}
	// Pick stations
	for(int i=0; i<ns ;++i)
	{
		if(sta[i].IsUseful == _USEFUL)
			sta[i].PickSta( latmin,  latmax,  lonmin,  lonmax);
	}
	// Judge whether the number of useful stations is greater than '_min'
	GetNumSta();
	if(nsUseful < _min)	IsUseful = _NONUSEFUL;
	//printf("%d\n", IsUseful );
	return IsUseful;
}
// Pick records given residual traveltime upper limit
int EqSta::PickEqStaDt(double _dt, int _min)
{
	// Judge whether this record is pre-nonuseful
	if(IsUseful == _NONUSEFUL) return _NONUSEFUL;
	if(ns < _min)
	{
		IsUseful = _NONUSEFUL;
		return _NONUSEFUL;
	}for (int i = 0; i < sta.size(); ++i)
	{
		if(sta[i].IsUseful == _USEFUL)
			if(fabs(sta[i].staT.rsdT) > _dt)	sta[i].IsUseful = _NONUSEFUL;
	}
	GetNumSta();
	if(nsUseful < _min)	IsUseful = _NONUSEFUL;
	return IsUseful;
}
// Pick records given distance range [_minDis, _maxDis)
int EqSta::PickEqStaDis(double _minDis, int _maxDis, int _min)
{
	// Judge whether this record is pre-nonuseful
	if(IsUseful == _NONUSEFUL) return _NONUSEFUL;
	if(ns < _min)
	{
		IsUseful = _NONUSEFUL;
		return _NONUSEFUL;
	}
	// Calculate distance while required
	if( IsDelta == _DELTAN) CalculateDelta();
	// Pick given distance range
	for (int i = 0; i < ns; ++i)
	{
		if(sta[i].IsUseful == _USEFUL)
			if(sta[i].delta >= _maxDis || sta[i].delta < _minDis) sta[i].IsUseful = _NONUSEFUL;
	}
	GetNumSta();
	if(nsUseful < _min)	IsUseful = _NONUSEFUL;
	return IsUseful;
}
// Pick records given type of wave
int EqSta::PickEqStaWaveType(int wavetype, int _min)
{
	// Judge whether this record is pre-nonuseful
	if(IsUseful == _NONUSEFUL) return _NONUSEFUL;
	if(ns < _min)
	{
		IsUseful = _NONUSEFUL;
		return _NONUSEFUL;
	}
	// Pick given distance range
	for (int i = 0; i < ns; ++i)
	{
		if(sta[i].IsUseful == _USEFUL) sta[i].PickStaWaveType(wavetype);
		//if(sta[i].IsUseful == _USEFUL) printf("hello\n");
	}
	nsUseful = GetNumStaWaveType(wavetype);
	if(nsUseful < _min)	IsUseful = _NONUSEFUL;
	return IsUseful;
}
int EqSta::PickEqStaNum(int _min)
{
	// Judge whether this record is pre-nonuseful
	// Judge whether this record is pre-nonuseful
	if(IsUseful == _NONUSEFUL) return _NONUSEFUL;
	if(ns < _min)
	{
		IsUseful = _NONUSEFUL;
		return _NONUSEFUL;
	}
	if(nsUseful < _min)	IsUseful = _NONUSEFUL;
	return IsUseful;
}
// Count num of useful stations
int EqSta::GetNumSta()
{
	if (IsUseful == _NONUSEFUL)	return 0;
	nsUseful = 0;
	for (int i = 0; i < ns; ++i)
	{
		if(sta[i].IsUseful == _USEFUL)	++nsUseful;
	}
	return nsUseful;
}
// Count num of useful stations for different kind of waves, ak is returned
int EqSta::GetNumStaWaveType(FLAG wavetype)
{
	int itmp;
	if (IsUseful == _NONUSEFUL)	return 0;
	itmp = 0;
	for (int i = 0; i < ns; ++i)
	{
		if(sta[i].IsUseful == _USEFUL)
		{
			if     (sta[i].staT.phase == "PG" && (wavetype & _PGY) ==_PGY)
				++itmp;
			else if(sta[i].staT.phase == "SG" && (wavetype & _SGY) ==_SGY)
				++itmp;
			else if(sta[i].staT.phase == "PN" && (wavetype & _PNY) ==_PNY)
				++itmp;
			else if(sta[i].staT.phase == "SN" && (wavetype & _SGY) ==_SNY)
				++itmp;
		}
	}
	return itmp;
}

// Caculate actual travel time for all the seismic phase
// Related info will be output is calculated travel time greater than _max
int EqSta::TravelTime( double _max)
{
	for(int i = 0; i< ns; ++i)
	{
		sta[i].staT.trlT = Dt(eq.originT,sta[i].staT.arvT);
		if(sta[i].staT.trlT >= _max )
		{
			fprintf(stderr, " Warning: travel time may be invalid !\n");
			fprintf(stderr,"%d %d %d %d %d %lf\n",
				sta[i].staT.arvT.y, sta[i].staT.arvT.mo,
				sta[i].staT.arvT.d, sta[i].staT.arvT.h,
				sta[i].staT.arvT.m, sta[i].staT.arvT.s);
			fprintf(stderr,"%d %d %d %d %d %lf\n\n",
				eq.originT.y, eq.originT.mo, eq.originT.d, eq.originT.h, eq.originT.m, eq.originT.s);
		}
	}
	IsTravt = _TRAVTY;
	return 0;
}
//  Complete station info(coordinates) according to its 'num2', given external station recorsds
int EqSta::CompleteSta(vector<Station> *_sta)
{
	// Judge whether stations info is complete
	if(IsCmpSta == _CMPSTAY) return 0;
	int  flag = 0;
	for(int j= 0; j < sta.size(); ++j)
	{
		sta[j].IsUseful = _NONUSEFUL;
		for (int i= 0; i < (*_sta).size(); ++i)
		{
			if( (*_sta)[i].num2 == sta[j].num2 )
			{
				sta[j].coord     =(*_sta)[i].coord;
				sta[j].IsUseful  = _USEFUL;        // If this station has record in the sta.dat, it is useful, otherwise is not!
				flag = 1;
				break;
			}
		}
		if(sta[j].IsUseful == _NONUSEFUL);
			//fprintf(stderr, " Warning: none record for\" %d\" station!\n", sta[j].num2);
		sta[j].IsCmpSta = _CMPSTAY; // Coordinate is completed for single station 'sta[j]'
	}
	IsCmpSta = _CMPSTAY; // Coordinates is completed for all the stations within this 'EqSta'
	return 0;
}
// 'eq' and 'sta' 's  coordinates tranformation from Geographical2 coordinates (h,longitude,latitude) (h = r- R0)
int EqSta::CoordTransf_Geo2()
{
	// Judege whether 'eq' and 'sta'  's coordinates are transformed
	if(IsCordTrf == _CORDTRFY) return 0;
	// Judge whether Stations coordinates are given or not.
	if(IsCmpSta == _CMPSTAN)
	{
		fprintf(stderr, "Err In EqSta::CoordTransf_Geo2: Stations coordinates have not been given!\n");
		fprintf(stderr, "Please use EqSta::CompleteSta() firstly!\n");
		exit(0);
	}
	// Coordinates transformation
	eq.CoordTransf_Geo2();
	for (int i = 0; i < sta.size(); ++i)
	{
		sta[i].CoordTransf_Geo2();
	}
	IsCordTrf = _CORDTRFY;
	return 0;
}
// Caculate delta. CompleteSta() function should run fisrtly
int EqSta::CalculateDelta()
{
	// Judge whether 'delta' is caculated
	if(IsDelta == _DELTAY ) return 0;
	// Coordinates transformation
	CoordTransf_Geo2();
	for(int i = 0; i< ns; i++)
	{
		sta[i].delta = Distance(sta[i].coord,eq.coord);
	}
	IsDelta = _DELTAY;
	return 0;
}
// Output ray path data for gmt plot. (Formate  eq.lon eq.lat sta.lon sta.lat)
int EqSta::OutRaypath4GMT(FILE **fp)
{
	_Line outLine;
	int i;
	// Judge whether 'sta[]'  's coordinates are given
	if(IsCmpSta == _CMPSTAN)
	{
		fprintf(stderr, " Err in EqSta::OutRaypath4GMT(): Coordinates of 'sta[]' have been given!\n");
		fprintf(stderr, " Please run EqSta::CompleteSta() firstly!\n");
		exit(0);
	}
	for(i = 0; i< ns; ++i)
	{
		memset(outLine,0,sizeof(outLine));
		//col1: lon col2:lat
		sprintf(outLine,"%-20.8lf %-20.8lf",eq.coord.lon,eq.coord.lat);
		fputs(outLine,*fp);
		fprintf(*fp, "\n");

		memset(outLine,0,sizeof(outLine));
		sprintf(outLine,"%-20.8lf %-20.8lf",sta[i].coord.lon,sta[i].coord.lat);
		fputs(outLine,*fp);
		fprintf(*fp, "\n");
		fprintf(*fp, ">\n");
	}
	return 0;
}

// Output Delta-Traveltime data for gmt plot, _f is used to distinguish
// _Wavetype = (_PGY | _SGY | _PNY | _SNY)
int EqSta::OutDelta_Time4GMT(FILE **fp, FLAG _Wavetype)
{
	_Line outLine;
	double t;
	// Judge whether Traveltime is caculated
	if(IsTravt == _TRAVTN)
	{
		fprintf(stderr, " Err in EqSta::OutDelta_Time4GMT: Travel time has not been calculated!\n");
		fprintf(stderr, " Please run EqSta::TravelTime() firstly!\n");
		exit(0);
	}
	// Caculate delta
	CalculateDelta();
	for(int i = 0; i< ns; ++i)
	{
		//col1: delta col2: TravelTime
		if((sta[i].staT.phase == "PG") && (_Wavetype & _PGY) == _PGY)
		{
			memset(outLine,0,sizeof(_Line));
			sprintf(outLine,"%-20.8lf %-20.8lf\n",sta[i].delta, sta[i].staT.trlT);
			fputs(outLine,*fp);
		}
		else if((sta[i].staT.phase == "PN") && (_Wavetype & _PNY) == _PNY)
		{
			memset(outLine,0,sizeof(_Line));
			sprintf(outLine,"%-20.8lf %-20.8lf\n",sta[i].delta, sta[i].staT.trlT);
			fputs(outLine,*fp);
		}
		else if((sta[i].staT.phase == "SG") && (_Wavetype & _SGY) == _SGY)
		{
			memset(outLine,0,sizeof(_Line));
			sprintf(outLine,"%-20.8lf %-20.8lf\n",sta[i].delta, sta[i].staT.trlT);
			fputs(outLine,*fp);
		}
		else if((sta[i].staT.phase == "SN") && (_Wavetype & _SNY) == _SNY)
		{
			memset(outLine,0,sizeof(_Line));
			sprintf(outLine,"%-20.8lf %-20.8lf\n",sta[i].delta, sta[i].staT.trlT);
			fputs(outLine,*fp);
		}
	}
	return 0;
}
// Output Delta-Residule traveltime for gmt plot, _f is used to distinguish
// _Wavetype = (_PGY | _SGY | _PNY | _SNY)
int EqSta::OutDelta_ResidualTime4GMT(FILE **fp, FLAG _Wavetype)
{
	_Line outLine;
	double t;
	// Judge whether Traveltime is caculated
	if(IsTravt == _TRAVTN)
	{
		fprintf(stderr, " Err in EqSta::OutDelta_Time4GMT: Travel time has not been calculated!\n");
		fprintf(stderr, " Please run EqSta::TravelTime() firstly!\n");
		exit(0);
	}
	// Caculate delta
	CalculateDelta();
	for(int i = 0; i< ns; ++i)
	{
		//col1: delta col2: TravelTime

			if((sta[i].staT.phase == "PG") && (_Wavetype & _PGY) == _PGY)
			{
				memset(outLine,0,sizeof(_Line));
				sprintf(outLine,"%-20.8lf %-20.8lf\n",sta[i].delta, sta[i].staT.rsdT);
				fputs(outLine,*fp);
			}
			else if((sta[i].staT.phase == "PN") && (_Wavetype & _PNY) == _PNY)
			{
				memset(outLine,0,sizeof(_Line));
				sprintf(outLine,"%-20.8lf %-20.8lf\n",sta[i].delta, sta[i].staT.rsdT);
				fputs(outLine,*fp);
			}
			else if((sta[i].staT.phase == "SG") && (_Wavetype & _SGY) == _SGY)
			{
				memset(outLine,0,sizeof(_Line));
				sprintf(outLine,"%-20.8lf %-20.8lf\n",sta[i].delta, sta[i].staT.rsdT);
				fputs(outLine,*fp);
			}
			else if((sta[i].staT.phase == "SN") && (_Wavetype & _SNY) == _SNY)
			{
				memset(outLine,0,sizeof(_Line));
				sprintf(outLine,"%-20.8lf %-20.8lf\n",sta[i].delta, sta[i].staT.rsdT);
				fputs(outLine,*fp);
			}

	}
	return 0;
}
// Output Delta-Forward calculated traveltime for gmt plot_f is used to distinguish
// _Wavetype = (_PGY | _SGY | _PNY | _SNY)
int EqSta::OutDelta_FWDTime4GMT(FILE **fp, FLAG _Wavetype)
{
	_Line outLine;
	double t;
	// Judge whether Traveltime is caculated
	if(IsTravt == _TRAVTN)
	{
		fprintf(stderr, " Err in EqSta::OutDelta_Time4GMT: Travel time has not been calculated!\n");
		fprintf(stderr, " Please run EqSta::TravelTime() firstly!\n");
		exit(0);
	}
	// Caculate delta
	CalculateDelta();
	for(int i = 0; i< ns; ++i)
	{
		//col1: delta col2: TravelTime

			if((sta[i].staT.phase == "PG") && (_Wavetype & _PGY) == _PGY)
			{
				memset(outLine,0,sizeof(_Line));
				sprintf(outLine,"%-20.8lf %-20.8lf\n",sta[i].delta,
									sta[i].staT.rsdT +sta[i].staT.trlT);
				fputs(outLine,*fp);
			}
			else if((sta[i].staT.phase == "PN") && (_Wavetype & _PNY) == _PNY)
			{
				memset(outLine,0,sizeof(_Line));
				sprintf(outLine,"%-20.8lf %-20.8lf\n",sta[i].delta,
									sta[i].staT.rsdT +sta[i].staT.trlT);
				fputs(outLine,*fp);
			}
			else if((sta[i].staT.phase == "SG") && (_Wavetype & _SGY) == _SGY)
			{
				memset(outLine,0,sizeof(_Line));
				sprintf(outLine,"%-20.8lf %-20.8lf\n",sta[i].delta,
									sta[i].staT.rsdT +sta[i].staT.trlT);
				fputs(outLine,*fp);
			}
			else if((sta[i].staT.phase == "SN") && (_Wavetype & _SNY) == _SNY)
			{
				memset(outLine,0,sizeof(_Line));
				sprintf(outLine,"%-20.8lf %-20.8lf\n",sta[i].delta,
									sta[i].staT.rsdT +sta[i].staT.trlT);
				fputs(outLine,*fp);
			}

	}
	return 0;
}
// Output useful earthquake-station data in ASC9 format to file
int EqSta::OutputASC9(FILE **fp)
{
	int i;
	double t;
	double dt;
	_Line outHead1;
	_Line outHead2;
	_Line outHead3;
	_Line outData;
	if(*fp == NULL)
	{
		fprintf(stderr, "Err: In EqSta::OutputASC9 function, file array is NULL!\n");
		exit(0);
	}
	if(IsUseful == _USEFUL)
	{

		//print 3 head line
		sprintf(outHead1,"Y%-10d    %04d/%02d/%02d/%02d-%02d-%04.1lf/%-2s=%03.1lf/                               %s",
			eq.num,
			eq.originT.y,
			eq.originT.mo,
			eq.originT.d,
			eq.originT.h,
			eq.originT.m,
			eq.originT.s,
			eq.mtype,
			eq.mag,
			eq.name);
		sprintf(outHead2,"%5d %2d %3d %3d %4d %4d",0,0,0,2,-9,nsUseful);
		sprintf(outHead3,"   %-9.4lf %-7.4lf %-10.4lf %-9.4lf %-9.4lf %-9.4lf %-9.4lf %-9.4lf",
			-1.0*eq.coord.h,
			eq.coord.lat,
			eq.coord.lon,
			eq.originT.s,
			0.0,
			0.0,
			0.001,
			0.01);
		fputs(outHead1,*fp);
		fprintf(*fp, "\n");
		fputs(outHead2,*fp);
		fprintf(*fp, "\n");
		fputs(outHead3,*fp);
		fprintf(*fp, "\n");
		for (int i = 0; i < ns; ++i)
		{
			if(sta[i].IsUseful == _USEFUL)
			{

				sprintf(outData,"%15d %-3d   %-8.2lf %-10.2lf %-18.8lf %-12.8lf %-5.2lf   %-4s %-4d   %-8s\n",
							sta[i].num2, -1, sta[i].staT.arvT.s, sta[i].delta, 0.0, 0.0, sta[i].staT.rsdT, sta[i].staT.phase.data(), -99 , sta[i].add);
				fputs(outData,*fp);
			}
		}
	}
	return 0;
}


/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
double Distance(_Coord p1, _Coord p2 )
{
	//double delta =R0* acos(sin(p1.theta)*cos(p1.fai)*sin(p2.theta)*cos(p2.fai)+
	//				       sin(p1.theta)*sin(p1.fai)*sin(p2.theta)*sin(p2.fai)+
	//				       cos(p1.theta)*            cos(p2.theta) );
	double delta = R0 * acos((p1.x*p2.x+ p1.y*p2.y+ p1.z*p2.z)
							/sqrt(SQ(p1.x)+ SQ(p1.y)+ SQ(p1.z))
							/sqrt(SQ(p2.x)+ SQ(p2.y)+ SQ(p2.z)) );
	return delta;
}
double Dt(Time t1,Time t2)  // t1<t2
{
    long     d1 ,d2;
    double   s1 ,s2;
    double   rs1,rs2;
	int month[2][13]={{0,31,28,31,30,31,30,31,31,30,31,30,31},
		   		      {0,31,29,31,30,31,30,31,31,30,31,30,31}};
    if(t1.y % 4 != 0 || (t1.y % 100 == 0 && t1.y % 400 != 0) )
    {
    	d1 = 0;
    	for (int i = 1; i < t1.mo; ++i)
    		d1 += month[0][i];
    	d1 += t1.d;
    	s1  = 24.0*3600.0*d1 + 3600.0*t1.h +60.0*t1.m +t1.s;
    	rs1 = 24.0*3600.0*365.0 - s1;
    }
    else
    {
    	d1 = 0;
    	for (int i = 1; i < t1.mo; ++i)
    		d1 += month[1][i];
    	d1 += t1.d;
    	s1 = 24.0*3600.0*d1 + 3600.0*t1.h +60.0*t1.m;
    	s1 += t1.s;
    	rs1= 24.0*3600.0*366.0 - s1;
    }

    if(t2.y % 4 != 0 || (t2.y %100 ==0 && t2.y % 400 != 0))
    {
    	d2 = 0;
    	for (int i = 0; i < t2.mo; ++i)
    		d2 += month[0][i];
    	d2 += t2.d;
    	s2 = 24.0*3600.0*d2 + 3600.0*t2.h + 60.0* t2.m +t2.s;
    	rs2= 24.0*3600.0*365.0 - s2;
    }
	else
	{
		d2 = 0;
    	for (int i = 0; i < t2.mo; ++i)
    		d2 += month[1][i];
    	d2 += t2.d;
    	s2 = 24.0*3600.0*d2 + 3600.0*t2.h + 60.0* t2.m +t2.s;
    	rs1= 24.0*3600.0*366.0 - s1;
	}
    if(t1.y == t2.y)
    	return fabs(s1-s2);
    else if(t1.y > t2.y)
    {
    	double _t = 0.0;
    	for (int i = t2.y+1; i < t1.y-1; ++i)
    	{
    		if( i % 4 != 0  || (i % 100 == 0 && i % 400 != 0))
    			_t += 365.0 * 24.0 * 3600.0;
    		else
    			_t += 366.0 * 24.0 * 3600.0;
    	}
    	_t += s1;
    	_t += rs2;
    	return _t;
    }
    else
    {
    	double _t = 0.0;
    	for (int i = t1.y+1; i < t2.y-1 ; ++i)
    	{
    		for (int i = t2.y+1; i < t1.y-1; ++i)
    		{
    			if( i % 4 != 0  || (i % 100 == 0 && i % 400 != 0))
    				_t += 365.0 * 24.0 * 3600.0;
    			else
    				_t += 366.0 * 24.0 * 3600.0;
    		}
    	}
    	_t += s2;
    	_t += rs1;
    	return _t;
    }
}

//Other fucntion
/*double Dt(Time t1, Time t2) // t1< t2
{

	int i,j,k;
	double dt = 0;
	int dyd;
	int dd1;
	int dd2;

	Time t3;
	if(t1.y > t2.y)
	{
		t3=t1;
		t1=t2;
		t2=t3;
	}
	else if(t1.mo > t2.mo)
	{
		t3=t1;
		t1=t2;
		t2=t3;
	}
	else if(t1.d > t2.d)
	{
		t3=t1;
		t1=t2;
		t2=t3;
	}
	else if(t1.h > t2.h)
	{
		t3=t1;
		t1=t2;
		t2=t3;
	}
	else if(t1.m > t2.m)
	{
		t3=t1;
		t1=t2;
		t2=t3;
	}
	else if(t1.s > t2.s)
	{
		t3=t1;
		t1=t2;
		t2=t3;
	}

	dyd = 0;

	if(t1.y == t2.y)
	{
		if(t1.y % 4 != 0 ||(t1.y % 400 != 0 && t1.y % 100 ==0) )
		{
			dyd = -365;
		}
		else
		{
			dyd = -366;
		}
	}
	else
	{
		for (i=t1.y+1; i <t2.y; ++i )
		{
			if(i % 4 != 0 ||(i % 400 != 0 && i % 100 ==0) )
			{
				dyd += 365;
			}
			else
			{
				dyd += 366;
			}
		}
	}

	switch(t1.mo)
	{
		case 12:
			dd1 = 31-t1.d;
			break;
		case 11:
			dd1 = 30-t1.d+31;
			break;
		case 10:
			dd1 = 31-t1.d+30+31;
			break;
		case 9:
			dd1 = 30-t1.d+31+30+31;
			break;
		case 8:
			dd1 = 31-t1.d+30+31+30+31;
			break;
		case 7:
			dd1 = 31-t1.d+31+30+31+30+31;
			break;
		case 6:
			dd1 = 30-t1.d+31+31+30+31+30+31;
			break;
		case 5:
			dd1 = 31-t1.d+30+31+31+30+31+30+31;
			break;
		case 4:
			dd1 = 30-t1.d+31+30+31+31+30+31+30+31;
			break;
		case 3:
			dd1 = 31-t1.d+30+31+30+31+31+30+31+30+31;
			break;
		case 2:
			if(t1.y %4 != 0 || (t1.y%100 ==0 && t1.y % 400 != 0))
			{
				dd1 = 28-t1.d+31+30+31+30+31+31+30+31+30+31;
			}
			else
			{
				dd1 = 29-t1.d+31+30+31+30+31+31+30+31+30+31;
			}
			break;
		case 1:
			if(t1.y %4 != 0 || (t1.y%100 ==0 && t1.y % 400 != 0))
			{
				dd1 = 31-t1.d+28+31+30+31+30+31+31+30+31+30+31;
			}
			else
			{
				dd1 = 31-t1.d+29+31+30+31+30+31+31+30+31+30+31;
			}
			break;
	}

	switch(t2.mo)
	{
		case 1:
			dd2 = t2.d-1;
			break;
		case 2:
			dd2 = t2.d-1+31;
			break;
		case 3:
			if(t1.y %4 != 0 || (t1.y%100 ==0 && t1.y % 400 != 0))
			{
				dd2 = t2.d-1+31+28;
			}
			else
			{
				dd2 = t2.d-1+31+29;
			}
			break;
		case 4:
			if(t1.y %4 != 0 || (t1.y%100 ==0 && t1.y % 400 != 0))
			{
				dd2 = t2.d-1+31+28+31;
			}
			else
			{
				dd2 = t2.d-1+31+29+31;
			}
			break;
		case 5:
			if(t1.y %4 != 0 || (t1.y%100 ==0 && t1.y % 400 != 0))
			{
				dd2 = t2.d-1+31+28+31+30;
			}
			else
			{
				dd2 = t2.d-1+31+29+31+30;
			}
			break;
		case 6:
			if(t1.y %4 != 0 || (t1.y%100 ==0 && t1.y % 400 != 0))
			{
				dd2 = t2.d-1+31+28+31+30+31;
			}
			else
			{
				dd2 = t2.d-1+31+29+31+30+31;
			}
			break;
		case 7:
			if(t1.y %4 != 0 || (t1.y%100 ==0 && t1.y % 400 != 0))
			{
				dd2 = t2.d-1+31+28+31+30+31+30;
			}
			else
			{
				dd2 = t2.d-1+31+29+31+30+31+30;
			}
			break;
		case 8:
			if(t1.y %4 != 0 || (t1.y%100 ==0 && t1.y % 400 != 0))
			{
				dd2 = t2.d-1+31+28+31+30+31+30+31;
			}
			else
			{
				dd2 = t2.d-1+31+29+31+30+31+30+31;
			}
			break;
		case 9:
			if(t1.y %4 != 0 || (t1.y%100 ==0 && t1.y % 400 != 0))
			{
				dd2 = t2.d-1+31+28+31+30+31+30+31+31;
			}
			else
			{
				dd2 = t2.d-1+31+29+31+30+31+30+31+31;
			}
			break;
		case 10:
			if(t1.y %4 != 0 || (t1.y%100 ==0 && t1.y % 400 != 0))
			{
				dd2 = t2.d-1+31+28+31+30+31+30+31+31+30;
			}
			else
			{
				dd2 = t2.d-1+31+29+31+30+31+30+31+31+30;
			}
			break;
		case 11:
			if(t1.y %4 != 0 || (t1.y%100 ==0 && t1.y % 400 != 0))
			{
				dd2 = t2.d-1+31+28+31+30+31+30+31+31+30+31;
			}
			else
			{
				dd2 = t2.d-1+31+29+31+30+31+30+31+31+30+31;
			}
			break;
		case 12:
			if(t1.y %4 != 0 || (t1.y%100 ==0 && t1.y % 400 != 0))
			{
				dd2 = t2.d-1+31+28+31+30+31+30+31+31+30+31+30;
			}
			else
			{
				dd2 = t2.d-1+31+29+31+30+31+30+31+31+30+31+30;
			}
			break;
	}
	dt = 24.0*3600.0-(t1.h*3600.0+t1.m*60.0+t1.s);
	dt += t2.h*3600.0+t2.m*60.0+t2.s;
	dt += (dyd+dd1+dd2)*24.0*3600.0;
	return dt;
}*/

// Read stations' data (Format: num1 num2 lat lon height(m))
int ReadSta(vector<Station> *sta, int *Ns, FILE **fp)
{
	_Line inLine;
	Station _sta;
	_sta.IsCmpSta = _CMPSTAY;
	*Ns           = 0;
	if(!(*sta).empty()) (*sta).clear();
	while(1)
	{
		memset(inLine,0,sizeof(_Line));
		if(fgets(inLine,sizeof(_Line),*fp)== NULL) break;
		sscanf(inLine,"%d %d %lf %lf %lf",
				&(_sta.num1), &(_sta.num2), &(_sta.coord.lat), &(_sta.coord.lon), &_sta.coord.h );
		_sta.coord.h=_sta.coord.h * 0.001; // height for station within file is 'm', while in this program, it is 'km'.
		(*sta).push_back(_sta);
		++(*Ns);
	}
	return *Ns;
}

// Read stations' data (Format: num1 num2 lat lon height(m) grid name)
int ReadSta4NAME(vector<Station> *sta, int *Ns, FILE **fp)
{
	_Line inLine;
	Station _sta;
	_sta.IsCmpSta = _CMPSTAY;
	*Ns           = 0;
	if(!(*sta).empty()) (*sta).clear();
	while(1)
	{
		memset(inLine,0,sizeof(_Line));
		if(fgets(inLine,sizeof(_Line),*fp)== NULL) break;
		sscanf(inLine,"%d %d %lf %lf %lf %s %s ",
				&(_sta.num1), &(_sta.num2), &(_sta.coord.lat), &(_sta.coord.lon), &(_sta.coord.h),
				(_sta.grid), (_sta.name) );
		_sta.coord.h=_sta.coord.h * 0.001;
		(*sta).push_back(_sta);
		++(*Ns);
	}
	return *Ns;
}

int SortStation(vector<Station> *sta)
{
	Station _s;
	int Ns = (*sta).size();
	for (int i = 0; i < Ns-1; ++i)
	{
		for(int j = i+1; j< Ns; ++j)
		{
			if((*sta)[i].num2 > (*sta)[j].num2)
			{
				_s = (*sta)[i];
				(*sta)[i] = (*sta)[j];
				(*sta)[j] = _s;
			}
		}
	}
}

int ReadASC9(vector<EqSta> *record, FILE **fp)
{
	int i,j,k;
	_Line inLine;
	EqSta _eqsta;
	Station _sta;
	int tmp;
	double dtmp;
	//_C8    ctmp;
	double _s;
	double _dt;
	_C4   wavetype;
	while(1)
	{
		memset(inLine,0,sizeof(_Line));
		if((fgets(inLine, 1024, *fp)) == NULL) break;
		// Head line 1//
		// Y1998457       1998/12/31/02-28-31.1/Ml=0.0/                               xuyi
		sscanf(inLine,"Y%d %d/%d/%d/%d-%d-%lf/%[^=]=%lf/ %s",
				&(_eqsta.eq.num       ),
				&(_eqsta.eq.originT.y ),
				&(_eqsta.eq.originT.mo),
				&(_eqsta.eq.originT.d ),
				&(_eqsta.eq.originT.h ),
				&(_eqsta.eq.originT.m ),
				&(_eqsta.eq.originT.s ),
				 (_eqsta.eq.mtype     ),
				&(_eqsta.eq.mag       ),
				 (_eqsta.eq.name      ) );
		// Head line 2//
		// 0  0   0   2   -9   14
		memset(inLine,0,sizeof(inLine));
		if(fgets(inLine,1024,*fp) == NULL)
		{
			fprintf(stderr, " Err: in ReadASC9(), none 2nd head info!\n input ASC9 file is broken!\n" );
			exit(0);
		}
		sscanf(inLine,"%d %d %d %d %d %d",&tmp, &tmp, &tmp, &tmp, &tmp, &(_eqsta.ns) );
		_eqsta.nsUseful=_eqsta.ns;
		// Head line 3//
		// 14.1400   42.0610   75.0590   31.0870    0.0000    0.0000    0.0010    0.0100
		memset(inLine,0,sizeof(inLine));
		if(fgets(inLine,1024,*fp) == NULL)
		{
			fprintf(stderr, " Err: in ReadASC9(), none 3rd head info!\n input ASC9 file is broken!\n");
			exit(0);
		}
		sscanf(inLine,"%lf %lf %lf %lf %lf %lf %lf %lf",
				&(_eqsta.eq.coord.h  ),
				&(_eqsta.eq.coord.lat),
				&(_eqsta.eq.coord.lon),
				&(_eqsta.eq.originT.s),
				&dtmp,  &dtmp, &dtmp, &dtmp);
		_eqsta.eq.coord.h= -1.0* _eqsta.eq.coord.h; // In ASC9 file, under surface is positive for earthquake

		// Record lines
		//    386  -1    35.20    15.89   0.00000000   1.45194459      0.00     pg   -99     xuyi
		for(i = 0; i < _eqsta.ns; ++i)
		{
			// Data line//
			memset(wavetype,0,sizeof(_C4));
			memset(inLine,0,sizeof(inLine));
			fgets(inLine,1024,*fp);
			sscanf(inLine,"%d %d %lf %lf %lf %lf %lf %s %d %s",
					&(_sta.num2),
					&tmp		,
					&_s 		,
					&(_sta.delta),
					&dtmp		,
					&dtmp		,
					&_dt 		,
					 wavetype 	,
					&tmp 		,
				 	 _sta.add      );

			for(int k = 0; wavetype[k] != '\0' && k < 4; wavetype[k] = std::toupper(wavetype[k]),  ++k )
				;
			_sta.staT.phase = wavetype;

			Time   _time = _eqsta.eq.originT;
			_time.s = _s;

			_sta.staT.rsdT  = _dt;
			_sta.staT.trlT  = 0.0;
			_sta.staT.arvT  = _time;

			/*/ 2nd method to store time info
			_sta.pgT = _eqsta.eq.originT;
			_sta.sgT = _eqsta.eq.originT;
			_sta.pnT = _eqsta.eq.originT;
			_sta.snT = _eqsta.eq.originT;
			if(strcmp(wavetype, "PG") == 0 ||strcmp(wavetype, "Pg") == 0 ||strcmp(wavetype, "pG") == 0 ||strcmp(wavetype, "pg") == 0)
			{
				_sta.pgT.s=_s;
				_sta.dtpg =_dt;
			}
			else if(strcmp(wavetype, "SG") == 0 ||strcmp(wavetype, "Sg") == 0 ||strcmp(wavetype, "sG") == 0 ||strcmp(wavetype, "sg") == 0)
			{
				_sta.sgT.s=_s;
				_sta.dtsg =_dt;
			}
			else if(strcmp(wavetype, "PN") == 0 ||strcmp(wavetype, "Pn") == 0 ||strcmp(wavetype, "pN") == 0 ||strcmp(wavetype, "pn") == 0)
			{
				_sta.pnT.s=_s;
				_sta.dtpn =_dt;
			}
			else if(strcmp(wavetype, "SN") == 0 ||strcmp(wavetype, "Sn") == 0 ||strcmp(wavetype, "sN") == 0 ||strcmp(wavetype, "sn") == 0)
			{
				_sta.snT.s=_s;
				_sta.dtsn =_dt;
			}
			else
			{
				fprintf(stderr, "Err: None wavetype !\n");
			}
			*/
			/*  */
			_eqsta.sta.push_back(_sta);
		}
		(*record).push_back(_eqsta);
		_eqsta.sta.clear();
	}
	return 0;
}

//  This subroutine need more works
int ReadFormatLiaoling(vector<EqSta> *record, vector<Station> *sta, FILE **fp, FLAG _waveflag)
{
	_Line 	inLine;
	FLAG  	whichLine;
	FLAG    whichwave;
	FLAG    endoffile = 0;
	FLAG    firststa = 0;
	FLAG    wavetype_sele = 0;
	EqSta 	_eqsta;
	Station _sta;
	Time    _t;
	double  _dt;
	_Name   _nameold;
	_Grid   _gridold;

	char 	tmp[32];
	char    waveTYPE[16];
	double 	dtmp;
	int 	neq = 0;
	memset(_nameold,0,sizeof(_Name));
	memset(_gridold,0,sizeof(_Grid));
	// Read the first line, and judge whether it is an empty file.
	memset(inLine,0,sizeof(_Line));
	if(fgets(inLine, sizeof(_Line), *fp) ==NULL )
	{
		fprintf(stderr, " Err: In function ReadFormatLiaoling(), Input file is empty!\n");
		exit(0);
	}
	while(1)
	{
		// Judgement, whether this line is head or data
		if( inLine[8] == '0' ||	inLine[8] == '1' ||	inLine[8] == '2' ||	inLine[8] == '3' ||
			inLine[8] == '4' ||	inLine[8] == '5' ||
			inLine[8] == '6' ||	inLine[8] == '7' ||	inLine[8] == '8' ||	inLine[8] == '9')
		{
			whichLine = 1; // this is a head line
			firststa  = 1;
		}
		else
		{
			whichLine = 0; // this is a data line
		}

		if(whichLine == 1)
		{
			// scanf head line 1
			sscanf(inLine,"%s %s %d-%d-%d %d:%d:%lf    %lf     %lf     %lf %s    %lf  %lf   %lf %lf %lf %lf %lf %s",
							tmp, (_eqsta.eq.grid),
							&(_eqsta.eq.originT.y), &(_eqsta.eq.originT.mo), &(_eqsta.eq.originT.d),
							&(_eqsta.eq.originT.h), &(_eqsta.eq.originT.m),  &(_eqsta.eq.originT.s),
							&(_eqsta.eq.coord.lat),	&(_eqsta.eq.coord.lon),		 &(_eqsta.eq.coord.h),
							_eqsta.eq.mtype, &(_eqsta.eq.mag),
							&dtmp, &dtmp, &dtmp, &dtmp, &dtmp, &dtmp,
							_eqsta.eq.name );
			_eqsta.eq.coord.h = -1.0 *_eqsta.eq.coord.h;
			++neq;
			_eqsta.eq.num = _eqsta.eq.originT.y * 1000000 + _eqsta.eq.originT.mo * 10000 + neq;
			// scanf line 2 and 3, which is nonuseful
			fgets(inLine, sizeof(inLine), *fp);
			fgets(inLine, sizeof(inLine), *fp);
		}
		else if(whichLine == 0)
		{
			// scanf data line
			memset(waveTYPE,0,sizeof(_Name));
			memset(_sta.name,0,sizeof(_sta.name));
			memset(_sta.grid,0,sizeof(_sta.grid));

			sscanf(inLine,"%s %s %s %s",tmp,tmp, _sta.grid, _sta.name);
			memset(tmp,0,sizeof(tmp));
			strncpy(tmp,&(inLine[28]),4);
			sscanf(tmp,"%s",waveTYPE);
			for(int i = 0; i < sizeof(waveTYPE) && waveTYPE[i] != '\0'; ++i )
				waveTYPE[i]=std::toupper(waveTYPE[i]);
			memset(tmp,0,sizeof(tmp));
			strncpy(tmp,&(inLine[37]),23);
			sscanf(tmp,"%d-%d-%d %d:%d:%lf",&(_t.y), &(_t.mo), &(_t.d), &(_t.h), &(_t.m), &(_t.s));

			_dt  =  Dt(_eqsta.eq.originT, _t);
			_t   =  _eqsta.eq.originT;
			_t.s += _dt;
			// Judge which type of wave
			if( ((_waveflag & _PGY) ==_PGY && (strcmp(waveTYPE,"PG") == 0) ) ||
				((_waveflag & _PNY) ==_PNY && (strcmp(waveTYPE,"PN") == 0) ) ||
				((_waveflag & _SGY) ==_SGY && (strcmp(waveTYPE,"SG") == 0) ) ||
				((_waveflag & _SNY) ==_SNY && (strcmp(waveTYPE,"SN") == 0) )    )
			{
				_sta.staT.arvT  = _t;
				_sta.staT.phase = waveTYPE;
				_sta.staT.rsdT  = 0.0;
				//printf("%s %s  %f %f\n", _sta.name, waveTYPE,_dt, _t.s);
				wavetype_sele = 1;
			}
			else
			{
				wavetype_sele = 0;
			}
			// Initialization of _nameold and _gridold
			//if(firststa == 1 )
			//{
			//	strncpy(_nameold,_sta.name,sizeof(_nameold));
			//	strncpy(_gridold,_sta.grid,sizeof(_gridold));
			//	//firststa = 0;
			//}
			// Judge whether this is a new station, if it is, then pushback last sta into _eqsta.sta .
			//    However, for the last station within one record, it will not be acquired by this mean.
			if( wavetype_sele == 1 && ( (strcmp( _sta.name, _nameold) !=0 || strcmp(_sta.grid, _gridold) !=0 ) || firststa == 1 ) )
			{
				// Complete stations' num1 and num2 by comparing GIRD and NAME
				for (int i = 0; i < (*sta).size(); ++i)
				{
					if(strcmp((*sta)[i].grid, _sta.grid)== 0 && strcmp((*sta)[i].name, _sta.name)== 0    )
					{

						_sta.num1 = (*sta)[i].num1;
						_sta.num2 = (*sta)[i].num2;
						//_sta.coord= (*sta)[i].coord;
						strncpy(_sta.add, _sta.name,sizeof(_sta.name));
						_eqsta.sta.push_back(_sta);
						//printf("%d %d %s %s  %f %f\n", _sta.num1, _sta.num2, _sta.name, waveTYPE,_dt, _t.s);
						break;
					}
				}
				memset(_nameold,0,sizeof(_nameold));
				memset(_gridold,0,sizeof(_gridold));
				strncpy(_nameold,_sta.name,sizeof(_nameold));
				strncpy(_gridold,_sta.grid,sizeof(_gridold));

				if(firststa == 1)
					firststa = 0;
			}
		}
		// Read one line.
		memset(inLine,0,sizeof(_Line));
		// Judge whether this is the EOF
		if( fgets(inLine, sizeof(inLine), *fp) == NULL )
		{
			endoffile = 1;
		}
		// Judge whether next line is a new record or the end of file
		if( inLine[8] == '0' ||	inLine[8] == '1' ||	inLine[8] == '2' ||	inLine[8] == '3' ||
			inLine[8] == '4' ||	inLine[8] == '5' ||	inLine[8] == '6' ||	inLine[8] == '7' ||
			inLine[8] == '8' ||	inLine[8] == '9' || endoffile == 1)
		{
			// for last station record of per earthquake
			// Complete stations' num1 and num2 by comparing GIRD and NAME
			/*for (int i = 0; i < (*sta).size(); ++i)
			{
				if(strcmp((*sta)[i].grid, _sta.grid)== 0 && strcmp((*sta)[i].name, _sta.name)== 0    )
				{
					_sta.num1 = (*sta)[i].num1;
					_sta.num2 = (*sta)[i].num2;
					//_sta.coord= (*sta)[i].coord;
					_eqsta.sta.push_back(_sta);
					break;
				}
			}*/
			if(_eqsta.sta.size() > 0)
			{
				_eqsta.ns       = _eqsta.sta.size();
				_eqsta.nsUseful = _eqsta.ns;
				(*record).push_back(_eqsta);
				_eqsta.ns = 0;
				_eqsta.nsUseful = 0;
			}
			if(!_eqsta.sta.empty()) _eqsta.sta.clear();
		}
		// If EOF happens, then quit.
		if(1 == endoffile)
		{
			break;
		}
	}


	return 0;
}
