#ifndef __BALLISTIC_H__
#define __BALLISTIC_H__

#pragma once

#include "Vec3.h"
#include "Particle.h"
#include <vector>
#include "GL\glut.h"

using namespace std;

#define MAX_NUM_AMMUNITION 70

enum ShotType // Ÿ��
{
	UNUSED = 0,
	PISTOL,
	ARTILLERY,
	FIREBALL,
	LASER
};

class Ammunition // �Ѿ��� ����
{
public:
	Particle	m_Particle; // �ϳ��� ���� ������
	ShotType	m_Type;		// �ϳ��� �Ѿ��� ����
public:
	double		m_Age;

public:
	Ammunition(void) {};
	~Ammunition(void) {};
public:
	void Darw(void);
};

class Ballistic // ����
{
public:
	vector<Ammunition>	m_Ammunitions; // �Ѿ� ��Ʈ
	ShotType			m_CurrShotType; // ����Ÿ��

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