#include "Particle.h"
#include <stdio.h>

Particle::Particle()
{
}


Particle::~Particle()
{
}

void Particle::Integrate(double dt)
{
	if (m_InverseMass <= 0.0f) {
		return;
	}

	// accumulating acc : a = f/m (Newton's 2)
	Vec3<double> acc = m_Acceleration; 
	acc += m_Force * m_InverseMass;

	m_Velocity += acc * dt; // intgrate by time
	m_Velocity *= pow(m_Damping, dt); // resistance force
	m_Position += m_Velocity * dt; // integrate by time
	
	ClearForce();
}