#pragma once
#ifndef __EDGE_H__
#define __EDGE_H__

#include "Vertex.h"

class Edge
{
public:
	int				_index;
	double			_cotweight;
	vector<Vertex*>	_vertices;
	vector<Face*>	_nbFaces;
public:
	Edge();
	Edge(int index, Vertex *v0, Vertex *v1) {
		_cotweight = 0.0;
		_index = index;
		_vertices.push_back(v0);
		_vertices.push_back(v1);
		v0->_nbEdges.push_back(this);
		v1->_nbEdges.push_back(this);
	}

public:
	inline Vertex *v(int id) { return _vertices[id]; }
	int hasFace(Face *nf);
};

#endif