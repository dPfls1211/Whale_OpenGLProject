#ifndef __FIREWORKS_H__
#define __FIREWORKS_H__

#pragma once
#include "Particle.h"
#include <vector>
#include "GL\glut.h"

using namespace std;

class Firework : public Particle
{
public:
	int		m_Type;
	double	m_Age;
public:
	Firework();
	~Firework();
	bool	Update(double dt);
};

class FireworkRule
{
	struct PayLoad
	{
		int	m_Type;
		int	m_NumParticle;
		void Set(int type, int num)
		{
			PayLoad::m_Type = type;
			PayLoad::m_NumParticle = num;
		}
	};
public:
	int				m_Type;
	double			m_MinAge;
	double			m_MaxAge;
	double			m_Damping;
	Vec3<double>	m_MinVelocity;
	Vec3<double>	m_MaxVelocity;
	int				m_NumPayLoad;
	vector<PayLoad>	m_PayLoads;
public:
	FireworkRule(void);
	~FireworkRule(void);
public:
	void	Init(int numPayLoad);
	void	SetParam(int type, double minAge, double maxAge, Vec3<double> minVel, Vec3<double> maxVel, double damping);
	void	Create(Firework *fw, Firework *parent = NULL);
};

class FireworkEngine
{
public:
	const static int		m_MaxNumFireworks = 1024;
	const static int		m_NumRules = 9;
	vector<Firework>		m_Fireworks;
	vector<FireworkRule>	m_Rules;
public:
	FireworkEngine(void);
	~FireworkEngine(void);
public:
	void	Init(void);
	void	Update(void);
	void	Draw(void);
	void	Create(int type, Firework *parent);
	void	Create(int type, int num, Firework *parent);
};

#endif