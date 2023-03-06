#pragma once
#include"Vec3.h"

class Texture
{
public:
	Vec3<double>	_textureCoord;
public:
	Texture();
	Texture(double u, double v, double w) {
		_textureCoord.Set(u, v, w);
	}
	~Texture();
public:
	inline double x(void) { return _textureCoord[0]; }
	inline double y(void) { return _textureCoord[1]; }
	inline double z(void) { return _textureCoord[2]; }
};
