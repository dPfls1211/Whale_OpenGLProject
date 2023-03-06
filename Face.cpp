#include "Face.h"
#include "Edge.h"


Face::Face()
{
}


Face::~Face()
{
}

int	Face::getIndex(Vertex *v)
{
	for (int i = 0; i < 3; i++) {
		if (_vertices[i] == v) {
			return i;
		}
	}
	return -1;
}

int	Face::hasVertex(Vertex *nv) {
	for (auto v : _vertices) {
		if (v==nv) {
			return 1;
		}
	}
	return 0;
}
int	Face::hasEdge(Edge *ne) {
	for (auto e : _edges) {
		if (e == ne) {
			return 1;
		}
	}
	return 0;
}


Vertex *Face::getOtherVertex(Edge *e) {
	for (auto v : _vertices) {
		if (e->v(0) != v && e->v(1) != v) {
			return v;
		}
	}
	return nullptr;
}