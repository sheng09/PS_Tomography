#include "tx.h"
#include "VelModel.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
using namespace std;

Model1D::Model1D()
{
	ZeroDepth  = _ZERODEPTHY;
	DenseModel = _DENSEMODEL1DN;
	nz         = 0;
	nmoho      = 0;
	nzdense    = 0;
	densedz	   = 0;
	ndis       = 0;
	if(zdis.size() > 0) zdis.clear();
	if(zs.size() > 0) zs.clear();
	if(vs.size() > 0) vs.clear();

	if(zsdense.size() > 0) zsdense.clear();
	if(vsdense.size() > 0) vsdense.clear();
}
Model1D::~Model1D()
{
	if(zdis.size() > 0) zdis.clear();
	if(zs.size() > 0) zs.clear();
	if(vs.size() > 0) vs.clear();

	if(zsdense.size() > 0) zsdense.clear();
	if(vsdense.size() > 0) vsdense.clear();
}


/**
 * [Read 1D model data from file]
 * @param  fp [file pointer]
 * @return    [1]
 */
int Model1D::ReadMod1D(FILE **fp)
{
	if(*fp == NULL)
	{
		fprintf(stderr, "Err: in Model1D::ReadMod1D() function, the input 'file' is NULL\n" );
		exit(0);
	}
	_Line 	inLine;
	double 	_z;
	double 	_v;
	if (zs.size() > 0 ) zs.clear();
	if (vs.size() > 0 ) vs.clear();

	while(1)
	{
		memset(inLine,0,sizeof(inLine));
		if( fgets(inLine,sizeof(inLine),*fp) == NULL) break;
		sscanf(inLine,"%lf %lf",&_z, &_v);
		zs.push_back(_z);
		vs.push_back(_v);
	}
	nz = zs.size();
	return 1;
}
/**
 * [Read 1D model data from tomolzw format 1d model file]
 * @param  fp [file pointer]
 * @return    [1]
 */
int Model1D::ReadMod1Dtomolzw(FILE **fp)
{
	if(*fp == NULL)
	{
		fprintf(stderr, " Err: in Model1D::ReadMod1Dtomolzw() function,  the input 'file' is NULL!\n");
		exit(0);
	}
	int 	nytmp;
	int 	nxtmp;

	int     itmp;
	double 	dtmp;
	char    ctmp[16];

	fscanf(*fp,"%d %d %d %d",&nz, &nytmp, &nxtmp, &ndis);
	--ndis;

	if(zdis.size() > 0) zdis.clear();
	for (int i = 0; i < ndis; ++i)
	{
		fscanf(*fp,"%d",&itmp);
		zdis.push_back(itmp);
	}
	fscanf(*fp,"%d",&itmp);

	fscanf(*fp,"%lf",&dtmp);
	if(dtmp != 0.0)
	{
		for (int i = 0; i < zdis.size(); ++i)
		{
			++zdis[i];
		}
		ZeroDepth = _ZERODEPTHN;
		zs.push_back(0.0);
		vs.push_back(0.0);

		zs.push_back(dtmp);
		for (int i = 1; i < nz; ++i)
		{
			fscanf(*fp,"%lf",&dtmp);
			zs.push_back(dtmp);
		}
		for (int i = 0; i < nytmp+nxtmp; ++i)
		{
			fscanf(*fp,"%lf",&dtmp);
		}
		for (int i = 0; i < nz; ++i)
		{
			fscanf(*fp,"%d*%lf",&itmp,&dtmp);
			vs.push_back(dtmp);
		}
		vs[0] = LinearInterpolation(zs[1],vs[1],zs[2],vs[2],zs[0]);
		++nz;
	}
	else
	{
		ZeroDepth = _ZERODEPTHY;
		zs.push_back(dtmp);
		for (int i = 1; i < nz; ++i)
		{
			fscanf(*fp,"%lf",&dtmp);
			zs.push_back(dtmp);
		}
		for (int i = 0; i < nytmp+nxtmp; ++i)
		{
			fscanf(*fp,"%lf",&dtmp);
			//printf("%lf  ",dtmp );
		}
		for (int i = 0; i < nz; ++i)
		{
			fscanf(*fp,"%d*%lf",&itmp,&dtmp);
			//printf("%lf\n",dtmp );
			vs.push_back(dtmp);
		}
	}

	for (int i = 0; i < nz; ++i)
	{
		//fprintf(stderr, "%lf %lf\n",zs[i],vs[i] );
	}
	return 1;
}

/**
 * [Output 1D model data in format {z[i] v[i]} ]
 * @param  fp [file pointer]
 * @return    [1]
 */
int Model1D::OutMod1D(FILE **fp)
{
	if(*fp == NULL)
	{
		fprintf(stderr, " Err: in Model1D::OutMod1D() function, the input 'file' is NULL!\n");
		exit(0);
	}
	for (int i = 0; i < nz; ++i)
	{
		fprintf(*fp, "%-20.10lf %-20.10lf\n",zs[i],vs[i]);
	}
	return 1;
}

/**
 * [Using linear interpolation to generate a denser model in depth direction, 
 * 	  and save answer to 'vsdenser' and 'zsdenser' member variables]
 * @param  _dz [depth increment]
 * @return     [1]
 */
int Model1D::MakeModelDense(double _dz)
{
	// Denser model is already generated
	if(densedz == _dz && DenseModel ==  _DENSEMODEL1DY)
	{
		return 0;
	}

	if(zsdense.size() > 0) zsdense.clear();
	if(vsdense.size() > 0) vsdense.clear();

	double z;
	double v;
	for (int i = 0; i < nz-1; ++i)
	{
		z = zs[i];
		v = vs[i];
		zsdense.push_back(zs[i]);
		vsdense.push_back(vs[i]);
		while(1)
		{
			z += _dz;
			v  = LinearInterpolation(zs[i],vs[i],zs[i+1],vs[i+1],z);
			if( z < zs[i+1])
			{
				zsdense.push_back(z);
				vsdense.push_back(v);
			}
			else
			{
				break;
			}
		}
	}
	vsdense.push_back(vs[nz-1]);
	zsdense.push_back(zs[nz-1]);
	nzdense    = zsdense.size();
	densedz    = _dz;
	DenseModel =  _DENSEMODEL1DY;

	//for (int i = 0; i < zsdense.size(); ++i)
	//{
	//	fprintf(stderr, "%lf %lf\n", zsdense[i],vsdense[i]);
	//}

	return 1;
}
/**
 * [ Given depth of eq and ray angle, calculate traveltime, distance and ray path for direct wave]
 * @param  _angle [ray angle]
 * @param  _sh    [depth of eq]
 * @param  _t     [traveltime pointer]
 * @param  _x     [distance pointer]
 * @param  _xs    [ray path pointer]
 * @param  _zs    [ray path pointer]
 * @return        [1]
 */
int Model1D::CalculateDirectRaypath(
	double _angle,double _sh,double *_t,double *_x, vector<double> *_xs, vector<double> *_zs)
{
	// Judge whether the denser model is generated
	if(DenseModel == _DENSEMODEL1DN)
	{
		fprintf(stderr, "Err: 1d denser model has not been generated, in which case, calculated ray path is not smooth enough!\n");
		exit(0);
	}
	
	// Acquire the position of '_sh' within 1D depth velocity model
	int iz = locateXF( &zsdense, _sh);

	// Using linear interpolation to acquire correspoding velocity, as well as its ray parameters
	double v0 = LinearInterpolation(zsdense[iz],vsdense[iz],zsdense[iz+1],vsdense[iz+1],_sh);
	double p;
	if(_angle <= PI/2)
	{
		p  = sin(_angle)/ v0;	
	}
	else
	{
		p = sin(PI - _angle)/ v0;
	}
	
	// Judge whether the angle is rational
	if( _angle == 0)
	{
		fprintf(stderr, "Err: in Model1D::CalculateDirectRaypath() function, the angle is  0 ,which is inrational!\n");
		exit(0);
	}
	// Judge whether the ray parameter is rational
	if( p < 1.0/vsdense[vsdense.size()-1] && _angle < PI/2 ) 
	{
		fprintf(stderr, "Err: in Model1D::CalculateDirectRaypath() function, the ray parameter is so small that it will never bend to the surface\n");
		exit(0);
	}

	// This vector is to save the total raypath(virtual source and station are both on the surface)
	vector<double> rayx;
	vector<double> rayz;
	if ( rayx.size() > 0)	rayx.clear();
	if ( rayz.size() > 0)	rayz.clear();
	double x = 0.0;
	double t = 0.0;

	double  dx,		// distance increment for ray within per layer
			dt,		// traveltime increment ***
			dep;	// if ray turn in one layer, then dep is the deepest depth the ray can touch within this layer
	double  h,		// thickness for one layer
			utop,	// slowness for the top of of one layer
			ubot;	//               ***bottom***

	FLAG 	type;
	double 	dx_eq,
			dt_eq,
			dep_eq,
			dx_iz,
			dt_iz;

	// The ray is up propagate
	if(_angle >= PI/2 && _angle <= PI)
	{
		rayx.push_back(0);
		rayz.push_back(0);

		for (int i = 0; i < iz; ++i)
		{
			h    = zsdense[i+1]-zsdense[i];
			utop = 1.0/vsdense[i];
			ubot = 1.0/vsdense[i+1];
	
			type = fxt(p,h,utop,ubot,&dx,&dt,&dep);
			if( type == _PASS )
			{
				t = t + dt;
				x = x + dx;
	
				rayx.push_back(x);
				rayz.push_back(zsdense[i+1]);
			}
			else if(type == _CONSTVEL)
			{
				t = t + dt;
				x = x + dx;
	
				rayx.push_back(x);
				rayz.push_back(zsdense[i+1]);
			}
			else if(type == _TURN)
			{
				t = t + dt;
				x = x + dx;
	
				rayx.push_back(x);
				rayz.push_back(zsdense[i]+dep);
	
				break;
			}
			else if(type == _ZEROTHICKNESS)
			{}
			else if(type == _REFLECT)
			{}
		}

		utop = 1.0/vsdense[iz];
		ubot = 1.0/v0;
		h    = _sh-zsdense[iz];
		type = fxt( p, h, utop, ubot, &dx_eq, &dt_eq, &dep_eq);
		if(dx_eq != 0.0)
		{
			t = t + dt_eq;
			x = x + dx_eq;
			
			rayx.push_back(x);
			rayz.push_back(_sh);
		}

		if ( (*_xs).size() > 0)	(*_xs).clear();
		if ( (*_zs).size() > 0)	(*_zs).clear();

		for (int i = 0; i < rayx.size(); ++i)
		{
			(*_xs).push_back(x-rayx[rayx.size()-1-i]);
			(*_zs).push_back(rayz[rayx.size()-1-i]);
		}
		*_x = x;
		*_t = t;
		//for (int i = 0; i < (*_xs).size(); ++i)
		//{
		//	fprintf(stderr, "%d %lf %lf\n",i, (*_xs)[i], (*_zs)[i]);
		//}
		return 1;
	}

	// The ray is down propagate
	else if(_angle <PI/2 && _angle > 0)
	{
		// Calculate the raypath segments within the layer which the source locate
		utop = 1.0/vsdense[iz];
		ubot = 1.0/v0;
		h    = _sh-zsdense[iz];
		type = fxt( p, h, utop, ubot, &dx_eq, &dt_eq, &dep_eq);
	
	
		// Calculate the total raypath for this ray parameter
		dx_iz = 0.0;
		dt_iz = 0.0;	
		// The begin point on surface
		rayx.push_back(0);
		rayz.push_back(0);
	
		for (int i = 0; i < nzdense-1; ++i)
		{
			h    = zsdense[i+1]-zsdense[i];
			utop = 1.0/vsdense[i];
			ubot = 1.0/vsdense[i+1];
	
			type = fxt(p,h,utop,ubot,&dx,&dt,&dep);
			if(i < iz)
			{
				dx_iz +=dx;
				dt_iz +=dt;
			}
	
			if( type == _PASS )
			{
				t = t + dt;
				x = x + dx;
	
				rayx.push_back(x);
				rayz.push_back(zsdense[i+1]);
			}
			else if(type == _CONSTVEL)
			{
				t = t + dt;
				x = x + dx;
	
				rayx.push_back(x);
				rayz.push_back(zsdense[i+1]);
			}
			else if(type == _TURN)
			{
				t = t + dt;
				x = x + dx;
	
				rayx.push_back(x);
				rayz.push_back(zsdense[i]+dep);
	
				double xhalf = x;
				int    nhalf = rayx.size()-1;
	
				// For the other half ray path
				for (int j = 0; j < nhalf; ++j)
				{
					rayx.push_back( 2* xhalf - rayx[nhalf-j-1] );
					rayz.push_back(            rayz[nhalf-j-1] );
				}
				x = 2.0*x;
				t = 2.0*t;
				break;
			}
			else if(type == _ZEROTHICKNESS)
			{
			}
			else if(type == _REFLECT)
			{
				t = t + dt;
				x = x + dx;
	
				rayx.push_back(x);
				rayz.push_back(zsdense[i]+dep);
	
				double xhalf = x;
				int    nhalf = rayx.size()-1;
	
				// For the other half ray path
				for (int j = 0; j < nhalf; ++j)
				{
					rayx.push_back( 2* xhalf - rayx[nhalf-j-1] );
					rayz.push_back(            rayz[nhalf-j-1] );
				}
				x = 2.0*x;
				t = 2.0*t;
				break;
			}
		}
	
		//  Clear Vector
		if ( (*_xs).size() > 0)	(*_xs).clear();
		if ( (*_zs).size() > 0)	(*_zs).clear();
		// Save the actual raypath to '*_xs' and '*_zs'
		(*_xs).push_back(0);
		(*_zs).push_back(_sh);
	
		for (int i = iz+1; i < rayx.size(); ++i)
		{
			(*_xs).push_back(rayx[i]- dx_eq- dx_iz);
			(*_zs).push_back(rayz[i]);
		}
	
		*_x = x -dx_eq - dx_iz;
		*_t = t -dt_eq - dt_iz;
		////for test
		//fprintf(stderr, "total\n");
		//for (int i = 0; i < rayx.size(); ++i)
		//{
		//	fprintf(stderr, "%d %lf %lf\n",i, rayx[i], rayz[i]);
		//}
		//fprintf(stderr, "dx_eq dx_iz :%d %lf %lf \n",iz,dx_eq, dx_iz );
		//fprintf(stderr, "dt_eq dt_iz :%d %lf %lf \n",iz,dt_eq, dt_iz );
		//fprintf(stderr, "t-x %lf %lf\n",*_x, *_t );
		//fprintf(stderr, "useful\n");
		//for (int i = 0; i < (*_xs).size(); ++i)
		//{
		//	fprintf(stderr, "%d %lf %lf\n",i, (*_xs)[i], (*_zs)[i]);
		//}
		if ( rayx.size() > 0)	rayx.clear();
		if ( rayz.size() > 0)	rayz.clear();
	
		return 1;
	}	
}

/**
 * [Given depth of eq and order of discontinous interface, calculate traveltime, distance and ray path for head wave]
 * @param  _idis [order of discontinous interface {from 1 to ndis}]
 * @param  _dx   [distance for Head wave that propagate on discontinous interface]
 * @param  _sh   [depth of eq]
 * @param  _t    [traveltime pointer]
 * @param  _x    [distance pointer]
 * @param  _xs   [ray path pointer]
 * @param  _zs   [ray path pointer]
 * @return       [1]
 */
int Model1D::CalculateHeadRaypath(
	int _idis,double _dx,double _sh,double *_t,double *_x, vector<double> *_xs, vector<double> *_zs)
{
	--_idis;
	// Judge whether there is Head wave
	if(_idis > ndis)
	{
		fprintf(stderr, "Err: None Head wave exist for \"%dth\" interface!\n", _idis+1);
		fprintf(stderr, "     Max number of discontinous interface:\"%d\" \n",ndis );
		exit(0);
	}
	if(_sh >= zs[zdis[_idis]])
	{
		fprintf(stderr, "Err: Head wave does not exist for eq depth \"%lf\" !\n",_sh );
		fprintf(stderr, "     while depth of \"%dth\" discontinous interface is \"%lf\" !\n",_idis+1,zs[zdis[_idis]] );
		exit(0);
	}
	// Judge whether the denser model is generated
	if(DenseModel == _DENSEMODEL1DN)
	{
		fprintf(stderr, "Err: 1d denser model has not been generated, in which case, calculated ray path is not smooth enough!\n");
		exit(0);
	}
	// Acquire the position of '_sh' within 1D depth velocity model
	int iz = locateXF( &zsdense, _sh);
	// Using linear interpolation to acquire correspoding velocity, as well as its ray parameters
	double v0 = LinearInterpolation(zsdense[iz],vsdense[iz],zsdense[iz+1],vsdense[iz+1],_sh);

	double p  = 1.0/vs[zdis[_idis]];
	// This vector is to save the total raypath(virtual source and station are both on the surface)
	vector<double> rayx;
	vector<double> rayz;
	if ( rayx.size() > 0)	rayx.clear();
	if ( rayz.size() > 0)	rayz.clear();
	double x = 0.0;
	double t = 0.0;

	double  dx,		// distance increment for ray within per layer
			dt,		// traveltime increment ***
			dep;	// if ray turn in one layer, then dep is the deepest depth the ray can touch within this layer
	double  h,		// thickness for one layer
			utop,	// slowness for the top of of one layer
			ubot;	//               ***bottom***

	FLAG 	type;
	double 	dx_eq,
			dt_eq,
			dep_eq,
			dx_iz,
			dt_iz;
	// Calculate the raypath segments within the layer which the source locate
	utop = 1.0/vsdense[iz];
	ubot = 1.0/v0;
	h    = _sh-zsdense[iz];
	type = fxt( p, h, utop, ubot, &dx_eq, &dt_eq, &dep_eq);
	
	// Calculate the total raypath for this ray parameter
	dx_iz = 0.0;
	dt_iz = 0.0;	
	// The begin point on surface
	rayx.push_back(0);
	rayz.push_back(0);
	for (int i = 0; i < nzdense-1; ++i)
	{
		h    = zsdense[i+1]-zsdense[i];
		utop = 1.0/vsdense[i];
		ubot = 1.0/vsdense[i+1];
			type = fxt(p,h,utop,ubot,&dx,&dt,&dep);
		if(i < iz)
		{
			dx_iz +=dx;
			dt_iz +=dt;
		}
		if( type == _PASS )
		{
			t = t + dt;
			x = x + dx;
			rayx.push_back(x);
			rayz.push_back(zsdense[i+1]);
		}
		else if(type == _CONSTVEL)
		{
			t = t + dt;
			x = x + dx;
				rayx.push_back(x);
			rayz.push_back(zsdense[i+1]);
		}
		else if(type == _TURN)
		{
			//t = t + dt;
			//x = x + dx;
			//rayx.push_back(x);
			//rayz.push_back(zsdense[i]+dep);
			//double xhalf = x;
			//int    nhalf = rayx.size()-1;
			//
			//
			//// For the other half ray path
			//for (int j = 0; j < nhalf; ++j)
			//{
			//	rayx.push_back( 2* xhalf - rayx[nhalf-j-1] );
			//	rayz.push_back(            rayz[nhalf-j-1] );
			//}
			//x = 2.0*x;
			//t = 2.0*t;
			//break;
		}
		else if(type == _ZEROTHICKNESS)
		{
		}
		else if(type == _REFLECT)
		{
			t = t + dt;
			x = x + dx;
			rayx.push_back(x);
			rayz.push_back(zsdense[i]+dep);
			
			// For refrected wave propagate along moho
			x = x + _dx/2.0;
			t = t + _dx/2.0/vs[zdis[_idis]];

			
			rayx.push_back(x);
			rayz.push_back(zsdense[i]+dep);

			double xhalf = x;
			int    nhalf = rayx.size()-1;
			// For the other half ray path
			for (int j = 0; j < nhalf; ++j)
			{
				rayx.push_back( 2* xhalf - rayx[nhalf-j-1] );
				rayz.push_back(            rayz[nhalf-j-1] );
			}
			x = 2.0*x;
			t = 2.0*t;
			break;
		}
	}
		//  Clear Vector
	if ( (*_xs).size() > 0)	(*_xs).clear();
	if ( (*_zs).size() > 0)	(*_zs).clear();
	// Save the actual raypath to '*_xs' and '*_zs'
	(*_xs).push_back(0);
	(*_zs).push_back(_sh);
		for (int i = iz+1; i < rayx.size(); ++i)
	{
		(*_xs).push_back(rayx[i]- dx_eq- dx_iz);
		(*_zs).push_back(rayz[i]);
	}
		*_x = x -dx_eq - dx_iz;
	*_t = t -dt_eq - dt_iz;
	////for test
	//fprintf(stderr, "total\n");
	//for (int i = 0; i < rayx.size(); ++i)
	//{
	//	fprintf(stderr, "%d %lf %lf\n",i, rayx[i], rayz[i]);
	//}
	//fprintf(stderr, "dx_eq dx_iz :%d %lf %lf \n",iz,dx_eq, dx_iz );
	//fprintf(stderr, "dt_eq dt_iz :%d %lf %lf \n",iz,dt_eq, dt_iz );
	//fprintf(stderr, "t-x %lf %lf\n",*_x, *_t );
	//fprintf(stderr, "useful\n");
	//for (int i = 0; i < (*_xs).size(); ++i)
	//{
	//	fprintf(stderr, "%d %lf %lf\n",i, (*_xs)[i], (*_zs)[i]);
	//}
	if ( rayx.size() > 0)	rayx.clear();
	if ( rayz.size() > 0)	rayz.clear();
	return 1;
}


/**
 * [Calculate horizontal increment 'dx' and traveltime increment 'dt' for the seismic ray within one layer,
 *   different propogation type (reflect or turn in this layer...) will be distinguished ]
 * @param  p    [ray parameter]
 * @param  h    [thickness of the layer]
 * @param  utop [slowness for the top of of one layer ]
 * @param  ubot [slowness for the bottom of of one layer]
 * @param  dx   [hozitontal increment for the ray]
 * @param  dt   [traveltime increment]
 * @param  dep  [if ray turn in this layer, then 'dep' is the deepest depth the ray can touch within this layer]
 * @return      [type of propogation]
 */
int fxt(double p,double h, double utop, double ubot, double *dx, double *dt, double *dep)
{
	*dep = 0.0;
	if(p == 0)
	{
		*dx = 0.0;
		*dt = 2.0*h/(1.0/utop+1.0/ubot);
		return _VERTICAL;
	}
	// Reflect wave
	if(p >= utop)
	{
		*dx = 0.0;
		*dt = 0.0;
		return _REFLECT;
	}

	// Zero thickness
	if(h == 0)
	{
		*dx = 0;
		*dt = 0;
		return _ZEROTHICKNESS;
	}

	// Constant velocity
	double u1    = utop;
	double u2    = ubot;
	double v1    = 1.0/u1;
	double v2    = 1.0/u2;
	double b     = (v2-v1)/h;
	double eta1  = sqrt(SQ(u1)-SQ(p));
	
	if ( b == 0)
	{
		*dx   = h*p/eta1;
		*dt   = h*SQ(u1)/eta1;
    	return _CONSTVEL;
	}

	// Turn in layer
	double x1	= eta1/(u1*b*p);
	double tau1	= (log((u1+eta1)/p)-eta1/u1)/b;
	if (p>ubot)
	{
		*dx   =x1;
		double dtau =tau1;
		*dt   =dtau+p*(*dx);
		*dep    =(1/p-v1)/b;
		return _TURN;
	}

	// Pass
	double eta2 =sqrt(SQ(u2)-SQ(p));
	double x2   =eta2/(u2*b*p);
	double tau2 =(log((u2+eta2)/p)-eta2/u2)/b;
	*dx         =x1-x2;
	double dtau =tau1-tau2;
	*dt         =dtau+p*(*dx);
	return _PASS;
}
