#ifndef __BALLISTIC_H__
#define __BALLISTIC_H__

#pragma once

#include "Vec3.h"
#include "Particle.h"
#include <vector>
#include "GL\glut.h"

using namespace std;

#define MAX_NUM_AMMUNITION 70

enum ShotType // 타입
{
	UNUSED = 0,
	PISTOL,
	ARTILLERY,
	FIREBALL,
	LASER
};

class Ammunition // 총알의 종류
{
public:
	Particle	m_Particle; // 하나의 입자 움직임
	ShotType	m_Type;		// 하나의 총알의 종류
public:
	double		m_Age;

public:
	Ammunition(void) {};
	~Ammunition(void) {};
public:
	void Darw(void);
};

class Ballistic // 권총
{
public:
	vector<Ammunition>	m_Ammunitions; // 총알 세트
	ShotType			m_CurrShotType; // 권총타입

	vector<Ballistic*> BalllistCh;
public:
	Ballistic();

	Ballistic(int size);
	~Ballistic();
public:
	void	Fire(void);
	void	Firedd(double x, double z, double y);
	void	Update(void); 
};

#endif