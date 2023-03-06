#ifndef __MESH_H__
#define __MESH_H__

#pragma once
#include "Face.h"
#include "GL\glut.h"
#include "Edge.h"
#include "Texture.h"

class Mesh
{
public:
	Vec3<double>	_minBoundary;
	Vec3<double>	_maxBoundary;
	vector<Vertex*>	_vertices;
	vector<Face*>	_faces;
	vector<Edge*>	_edges;
	vector<Texture*>	_textureCoords;
	GLuint				_textureIndex; //multi texture 시 배열 사용
public:
	int			vn[3];
	float		_size;
	float		_x, _y, _z;
public:
	Mesh();
	Mesh(char *filename)
	{
		loadObj(filename);
	}
	Mesh(char *filename, char *texture);
	Mesh(char *obj, char *texture, float size, float _x, float _y, float _z, int ok);
	~Mesh();
public:
	void	init(void);
	void	smooth(int iter = 1);
	void	loadObj(char *filename);
	void	loadTexture(char *file, GLuint &index);
	void	moveToCenter(double scale);
	void	computeEdge(void);
	void	computeNormal(void);
	void	computeCotWeight(void);
	void	buildAdjacency(void);
public:
	void	drawPoint(void);
	void	drawWire(void);
	void	drawSolid(void);
	void	drawGouraudShading(void);
	void	drawSurface(void);
public:
	void	laplacianUniform(int iter = 1);
	void	taubinSmooth(int iter = 1);
	void	laplacianCotangent(int iter = 1);
	void	smoothing(void);
};

#endif
