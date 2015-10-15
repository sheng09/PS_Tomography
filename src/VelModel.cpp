#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <math.h>
#include "Eqsta.h"
#include "VelModel.h"
using namespace std;

_CartesianCoord::_CartesianCoord()
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
}
_CartesianCoord::~_CartesianCoord()
{}



Block::Block()
{
	vp = 0.0;
	vs = 0.0;
}
Block::~Block()
{}



Model3DLayer::Model3DLayer()
{
	LayerOrder    = 0;
	TotalNumLayer = 0;
	LoopOrder     = 0;
	nx            = 0;
	ny            = 0;
	nz            = 0;
	if (xs.size() > 0)	xs.clear();
	if (ys.size() > 0)	ys.clear();
	if (zs.size() > 0)	zs.clear();
	if (blks.size() > 0)	blks.clear();
}

Model3DLayer::~Model3DLayer()
{
	if (xs.size() > 0)	xs.clear();
	if (ys.size() > 0)	ys.clear();
	if (zs.size() > 0)	zs.clear();
	if (blks.size() > 0)	blks.clear();
}

int Model3DLayer::ClearModel3DLayer()
{
	LayerOrder    = 0;
	TotalNumLayer = 0;
	LoopOrder     = 0;
	nx            = 0;
	ny            = 0;
	nz            = 0;
	if (xs.size() > 0)	xs.clear();
	if (ys.size() > 0)	ys.clear();
	if (zs.size() > 0)	zs.clear();
	if (blks.size() > 0)	blks.clear();
}
int Model3DLayer::ReadModel3DLayer(FILE **fp)
{
	Block  _block;
	double dtmp;
	// Judge whether **fp is 'NULL'
	if(*fp == NULL)
	{
		fprintf(stderr, "Err: Model3DLayer::ReadModel3DLayer() function, input file pointer is \'NULL\'\n" );
		exit(0);
	}
	// Clear 'blks' vector
	if(blks.size() > 0 )
	{
		blks.clear();
	}
	// Read 2nd, 3rd, 4th  line 
	if (xs.size() > 0)	xs.clear();
	if (ys.size() > 0)	ys.clear();
	if (zs.size() > 0)	zs.clear();
	
	for (int i = 0; i < ny; ++i)
	{
		fscanf(*fp,"%lf",&dtmp);
		ys.push_back(dtmp);
	}
	for (int i = 0; i < nx; ++i)
	{
		fscanf(*fp,"%lf",&dtmp);
		xs.push_back(dtmp);
	}
	for (int i = 0; i < nz; ++i)
	{
		fscanf(*fp,"%lf",&dtmp);
		zs.push_back(dtmp);
	}
	
	// Read velocity
	if (blks.size() > 0)	blks.clear();
	for (int k = 0; k < nz; ++k)
	{
		for (int j = 0; j < ny; ++j)
		{
			for (int i = 0; i < nx; ++i)
			{
				fscanf(*fp,"%lf",&dtmp);
				_block.vp = dtmp;
				_block.cord.x=xs[i];
				_block.cord.y=ys[j];
				_block.cord.z=zs[k];
				blks.push_back(_block);
			}
		}
	}
	return (nx*ny*nz);
}
int Model3DLayer::OutModel3DLayer(FILE **fp)
{
	// Judge whether **fp is 'NULL'
	if( *fp == NULL)
	{
		fprintf(stderr, "Err: Model3DLayer::OutModel3DLayer() function, input file pointer is \'NULL\'\n");
		exit(0);
	}
	// Write 1st line (info line)
	//fputs(h1,*fp);
	fprintf(*fp,"%d %d %d %d %d %d\n",(ny), (nx), (nz),	(TotalNumLayer), (LayerOrder), (LoopOrder) );
	//fprintf(*fp, "\n");
	// Write 2nd line for y coordinates
	for (int i = 0; i < ny; ++i)
	{
		fprintf(*fp, "%-10.4lf ", ys[i]);
	}
	fprintf(*fp, "\n");
	// Write 3rd line for x coordinates
	for (int i = 0; i < nx; ++i)
	{
		fprintf(*fp, "%-10.4lf ", xs[i]);
	}
	fprintf(*fp, "\n");
	// Write 4th line for z coordinates
	for (int i = 0; i < nz; ++i)
	{
		fprintf(*fp, "%-10.4lf ", zs[i]);
	}
	fprintf(*fp, "\n");
	// Write velocity data
	for (int k = 0; k < nz; ++k)
	{
		for (int j = 0; j < ny; ++j)
		{
			for (int i = 0; i < nx; ++i)
			{
				fprintf(*fp, "%-7.3lf ",blks[k*ny*nx + j*nx +i].vp );
			}
			fprintf(*fp, "\n");
		}
	}
}



Model3D::Model3D()
{
	TotalNumLayer = 0;
	if(layers.size() > 0) layers.clear();
}
Model3D::~Model3D()
{

	if(layers.size() > 0) layers.clear();
}
int Model3D::ClearModel3D()
{
	TotalNumLayer = 0;
	if(layers.size() > 0) layers.clear();
	return 1;
}
int Model3D::ReadModel3D(FILE **fp)
{
	Model3DLayer _mlayer;
	// Judge whether **fp is 'NULL'
	if( *fp == NULL)
	{
		fprintf(stderr, "Err: Model3D::ReadModel3D() function, input file pointer is \'NULL\'\n");
		exit(0);
	}
	if(layers.size() > 0) layers.clear();
	while(1)
	{
		// Read head info
		if( fscanf(*fp,"%d %d %d %d %d %d",
				&(_mlayer.ny), &(_mlayer.nx), &(_mlayer.nz), 
				&(_mlayer.TotalNumLayer), &(_mlayer.LayerOrder), &(_mlayer.LoopOrder) ) 
			==EOF ) break;
		
		if(_mlayer.xs.size() > 0) 	_mlayer.xs.clear();
		if(_mlayer.ys.size() > 0) 	_mlayer.ys.clear();
		if(_mlayer.zs.size() > 0) 	_mlayer.zs.clear();
		if(_mlayer.blks.size() > 0) _mlayer.blks.clear();

		_mlayer.ReadModel3DLayer(fp);
		layers.push_back(_mlayer);
		_mlayer.ClearModel3DLayer();
	}
	//nlayers = layers.size();
	TotalNumLayer = layers[layers.size()-1].TotalNumLayer;
	return layers.size();
}
int Model3D::OutModel3D(FILE **fp)
{
	// Judge whether **fp is 'NULL'
	if( *fp == NULL)
	{
		fprintf(stderr, "Err: Model3D::OutModel3D() function, input file pointer is \'NULL\'\n");
		exit(0);
	}

	for (int i = 0; i < layers.size(); ++i)
	{
		layers[i].OutModel3DLayer(fp);
	}
	return 1;
}
int Model3D::ModelAverage(vector<double> *_zs, vector<double> *_vs, int _itn)
{
	double _v;
	double _z;
	int 	_itmp;
	if( _itn > layers[layers.size()-1].LoopOrder ) 
	{
		fprintf(stderr, " Err: in subroutine Model3D::ModelAverage() !\n");
		fprintf(stderr, " You are request for the %dth iteration ak, but the max iteration number is %d \n",
			_itn, layers[layers.size()-1].LoopOrder );
		exit(0);
	}
	if( (*_zs).size() > 0) (*_zs).clear();
	if( (*_vs).size() > 0) (*_vs).clear();

	int begin = 0;
	while(1)
	{
		if(layers[begin].LoopOrder == _itn ) break;
		++begin;
	}
	int end = begin+ layers[begin].TotalNumLayer;
	for (int i = begin; i < end; ++i)
	{
		_itmp = layers[i].nx * layers[i].ny;
		for(int j =0; j < layers[i].nz ;++j)
		{
			_z = layers[i].zs[j];
			_v = 0;
			for (int k = 0; k < _itmp; ++k)
			{
				_v += layers[i].blks[j*_itmp+k].vp;
			}
			_v = _v/_itmp;
			(*_zs).push_back(_z);
			(*_vs).push_back(_v);
		}
	}
	return 1;
}

/**
 * locate position given vector
 * @param  xs [data vector]
 * @param  x  [number to locate]
 * @return    [position]
 *
 * eg
 * 	 0  1  2  3  4  5
 * 	 9  12 45 50 60 70
 *  8  10   46   60        80
 *  0  0    2    4         4
 */
int locateXF(vector<double> *_xs, double _x)
{
	int _i;

	if( _x < (*_xs)[0]) return 0;
	if( _x >= (*_xs)[(*_xs).size()-1]) return ((*_xs).size()-1);
	for (int i = 1; i < (*_xs).size(); ++i)
	{
		if(_x < (*_xs)[i] && _x >=(*_xs)[i-1] )
		{
			_i = i - 1;
		}
	}

	return _i;
}
