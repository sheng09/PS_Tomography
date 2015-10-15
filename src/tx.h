#ifndef TXHEAD
#define TXHEAD 

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "Eqsta.h"
// Propagation type
#define _REFLECT        0
#define _ZEROTHICKNESS  1
#define _TURN           2
#define _PASS           3
#define _CONSTVEL       4
#define _VERTICAL       5

// Whether denser model is generated
#define _DENSEMODEL1DY  1
#define _DENSEMODEL1DN  0

// Whether input model file contains '0' depth
#define _ZERODEPTHY     1
#define _ZERODEPTHN     0 


#define LinearInterpolation(x1,y1,x2,y2,x)  (((y2)-(y1))/((x2)-(x1))*((x)-(x1))+(y1))
#define SQ(x)           ((x)*(x))

using namespace std;


class Model1D
{
	public:
		FLAG DenseModel;
		FLAG ZeroDepth;
		int 			ndis;	// Number of discontinous interfaces
		vector<int> 	zdis;	// Positions of discontinous interfaces
		
		int  			nz;		// Number of interfaces
		int  			nmoho;	// Positions of moho interfaces within 'zs'

		vector<double> zs;		// Depth of interfaces
		vector<double> vs;		// Velocity corresponding to interfaces

		int    nzdense;
		double densedz;
		vector<double> zsdense;
		vector<double> vsdense;
		
		int ReadMod1D(FILE **fp);
		int ReadMod1Dtomolzw(FILE **fp);

		int OutMod1D(FILE **fp);

		int MakeModelDense(double _dz);

		int CalculateDirectRaypath(    double _angle,double _sh,double *_t,double *_x, vector<double> *_xs, vector<double> *_zs);
		int CalculateHeadRaypath(int _idis,double _dx,double _sh,double *_t,double *_x, vector<double> *_xs, vector<double> *_zs);
		
		
		//int OutDirectRaypath4GMT(FILE **fp);
		//int OutCriticalPnRaypath4GMT(FILE **fp);

		Model1D();
		~Model1D();

	/* data */
};
#endif

/**
 * [fxt description]
 * @param  p    [description]
 * @param  h    [description]
 * @param  utop [description]
 * @param  ubot [description]
 * @param  dx   [description]
 * @param  dt   [description]
 * @param  dep  [description]
 * @return      [description]
 */
int fxt(double p,double h, double utop, double ubot, double *dx, double *dt, double *dep);

