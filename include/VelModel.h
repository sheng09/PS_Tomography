//This head is to define data structure, class and related functions
#ifndef VELMODEL
#define VELMODEL
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <math.h>
#include "Eqsta.h"
using namespace std;

// linear interpolation function
#define LI(x1,y1,x2,y2,x) ( ((x)-(x1)) / ((x2)-(x1)) * (y2) + ((x2)-(x)) / ((x2)-(x1))*(y1))
/**
	 * Coordinates convention
	 *                           UP
	 *                                      North
	 *                        z  |
	 *                           |        y
	 *                           |       /
	 *                           |     /
	 *                           |   /
	 *                           | /           x
	 *                           .-------------   East
	 *
	 */

class _CartesianCoord
{
public:
	double x;
	double y;
	double z;
	_CartesianCoord();
	~_CartesianCoord();
};

class Block
{
public:
	_CartesianCoord cord;

	double 		vp;
	double 		vs;

	Block();
	~Block();
};

class Model3DLayer
{
public:
	int     LayerOrder;
	int     TotalNumLayer;
	int     LoopOrder;

	int 	nx;
	int     ny;
	int     nz;
	vector<double> xs;
	vector<double> ys;
	vector<double> zs;
	vector<Block>  blks;

	Model3DLayer();
	~Model3DLayer();

	int ClearModel3DLayer();
	int ReadModel3DLayer(FILE **fp);
	int OutModel3DLayer(FILE **fp);
};

/**
 *   Model3D class is used to read, save and processing data from 'RTM.out' format file.
 *   In 'RTM.out' file, the data is saved orderly according to layers and iteration number.
 *  Thus, {Model3d } using a {vector <Model3Dlayer> layers} to save these orderly data one by one.
 *  For each {layers[i]}, it has its special {6 info numbers()}, {coordinates}, {velocity array}
 *
 *  Member variables:
 *  	TotalNumLayer: Number of layers within the model
 *  	layers       : vector orderly store every layer according iteration number and layer orders
 *
 *  Member subroutines:
 *  	ClearModel3D() : clear vector 'layers'
 *  	ReadModel3D()  : read data from 'RTM.out' and save it into 'layers'
 *  	OutModel3D()   : output 'layers' into 'RTM.out'
 *  	ModelAverage() : acquire the 1d average model according to 'layers',
 *  	                 given '_itn'(number of iteration number, 0 represents initial model).
 */
class Model3D
{
public:
	int 	TotalNumLayer; // it do not equal to layers.size(), but number of layers of the model.
	vector<Model3DLayer> layers;

	Model3D();
	~Model3D();

	int ClearModel3D();
	int ReadModel3D(FILE **fp);
	int OutModel3D(FILE **fp);

	int ModelAverage(vector<double> *_zs, vector<double> *_vs, int _itn);
	int Model1D(vector<double> *_zs, vector<double> *_vs, int _itn, double x, double y);
};

int locateXF(vector<double> *xs, double x);

#endif