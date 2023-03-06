#ifndef __VERTEX_H__
#define __VERTEX_H__

#pragma once
#include "Vec3.h"
#include <vector>

using namespace std;

class Face;
class Edge;
class Vertex
{
public:
	int				_index;
	bool			_flag;
	Vec3<double>	_pos; // X,Y,Z
	Vec3<double>	_normal;
	vector<Face*>	_nbFaces; // Neighbor face
	vector<Vertex*>	_nbVertices; // Neighbor vertex
	vector<Edge*>	_nbEdges;
public:
	Vertex();
	Vertex(int index, Vec3<double> pos)
	{
		_flag = false;
		_index = index;
		_pos = pos;
	}
	~Vertex();
public:
	inline double x(void) { return _pos.x(); }
	inline double y(void) { return _pos.y(); }
	inline double z(void) { return _pos.z(); }
public:
	bool	hasNbVertex(Vertex *v);
};

#endif