#include "Ballistic.h"

void Ammunition::Darw(void)
{
	Vec3<double> pos = m_Particle.GetPos();
	glColor3f(0, 0, 0);
	glPushMatrix();
	glTranslatef(pos.x(), pos.y(), pos.z());
	glColor4f(0.0f, 0.0f, 0.75f,0.3f);
	glutSolidSphere(0.05f, 5, 4);
	glPopMatrix();


	glColor4f(0.0f, 0.0f, 0.75f, 0.3f);
	glPushMatrix();
	glTranslatef(pos.x(), 0, pos.z());
	glScalef(1.0f, 0.1f, 1.0f);
	//glutSolidSphere(0.6f, 5, 4);
	glPopMatrix();
}
Ballistic::Ballistic(int size) {
	m_CurrShotType = ShotType::LASER;
	m_Ammunitions.clear();
	// 16개의 총알을 삽입
	m_Ammunitions.resize(size);

	for (auto &p : m_Ammunitions) {
		p.m_Type = ShotType::UNUSED;
		// 준비된 상태 (=총알 미사용)
	}
}
Ballistic::Ballistic()
{
	m_CurrShotType = ShotType::LASER;
	m_Ammunitions.clear();
	// 16개의 총알을 삽입
	m_Ammunitions.resize(MAX_NUM_AMMUNITION); 
	
	for (auto &p : m_Ammunitions) {
		p.m_Type = ShotType::UNUSED;
		// 준비된 상태 (=총알 미사용)
	}
}

Ballistic::~Ballistic()
{
}

void Ballistic::Update(void)
{
	double dt = 0.01;

	for (auto &p : m_Ammunitions) {
		if (p.m_Type != ShotType::UNUSED) {
			p.m_Particle.Integrate(dt);
			if (p.m_Particle.GetPos().y() < -1.0 || p.m_Particle.GetPos().z() > 200.0) {
				p.m_Type = ShotType::UNUSED;
			}
			if (p.m_Particle.GetPos().y() > 5.0f) {
				for (int i = 0; i < 10; i++) {
					//printf("ch");
					Ballistic childBallistic;
					float theta = i * rand() % 360 / 180.0f * 3.141592;
					double px = 5.0f *cos(theta);
					double pz = 5.0f * sin(theta);
					childBallistic.m_CurrShotType = ShotType::ARTILLERY;
					childBallistic.Fire();
					childBallistic.Firedd(px, pz,7.0);
					for (auto &sp : childBallistic.m_Ammunitions) {
						if (sp.m_Type != ShotType::UNUSED) {
							sp.Darw();

						}
					}
				}
			}
				
		}
	}
}
void Ballistic::Firedd(double x, double z, double y) {
	Ballistic *shot;
	for (auto &p : m_Ammunitions) { // 16번
		if (p.m_Type == ShotType::UNUSED) {
			switch (m_CurrShotType) {
			case PISTOL:
				p.m_Particle.SetMass(2.0f);
				p.m_Particle.SetVel(0.0f, 0.0f, 35.0f);
				p.m_Particle.SetAcc(0.0f, -1.0f, 0.0f);
				p.m_Particle.SetDamping(0.99f);
				break;
			case ARTILLERY:
				p.m_Particle.SetMass(200.0f);
				p.m_Particle.SetVel(x, 5.0f, z);  //x<5 z<5
				p.m_Particle.SetAcc(0.0f, -10.0f, 0.0f);
				p.m_Particle.SetDamping(0.99f);
				break;
			case FIREBALL:
				p.m_Particle.SetMass(1.0f);
				p.m_Particle.SetVel(0.0f, 0.0f, 10.0f);
				p.m_Particle.SetAcc(0.0f, 0.6f, 0.0f); // Floats up
				p.m_Particle.SetDamping(0.9f);
				break;
			case LASER:
				p.m_Particle.SetMass(200.0f);
				p.m_Particle.SetVel(0.0f, 15.0f, 0.0f);
				p.m_Particle.SetAcc(0.0f, -25.0f, 0.0f); // No gravity
				p.m_Particle.SetDamping(0.99f);
				break;
			}
			// 총알에 대한 재질의 정의
			// Set the data common to all particle types
			// 총알의 위치를 초기화
			p.m_Particle.SetPos(-1.5, y, 0.0);
			p.m_Particle.ClearForce();
			p.m_Type = m_CurrShotType;
			break;
		}
	}
}

void Ballistic::Fire(void)
{
	Ballistic *shot;
	for (auto &p : m_Ammunitions) { // 100번
		if (p.m_Type == ShotType::UNUSED) {
		switch (m_CurrShotType) {
		case PISTOL:
			p.m_Particle.SetMass(2.0f); 
			p.m_Particle.SetVel(0.0f, 0.0f, 35.0f); 
			p.m_Particle.SetAcc(0.0f, -1.0f, 0.0f);
			p.m_Particle.SetDamping(0.99f);
			break;
		case ARTILLERY:
			p.m_Particle.SetMass(200.0f); 
			p.m_Particle.SetVel(0.0f, 5.0f, 5.0f); 
			p.m_Particle.SetAcc(0.0f, -20.0f, 0.0f);
			p.m_Particle.SetDamping(0.99f);				
			break;
		case FIREBALL:
			p.m_Particle.SetMass(1.0f);
			p.m_Particle.SetVel(0.0f, 0.0f, 10.0f);
			p.m_Particle.SetAcc(0.0f, 0.6f, 0.0f); // Floats up
			p.m_Particle.SetDamping(0.9f);				
			break;
		case LASER :
			p.m_Particle.SetMass(200.0f); 
			p.m_Particle.SetVel(0.0f, 15.0f, 0.0f); 
			p.m_Particle.SetAcc(0.0f, -20.0f, 0.0f); // No gravity
			p.m_Particle.SetDamping(0.99f);				
			break;
		}
		// 총알에 대한 재질의 정의
		// Set the data common to all particle types
		// 총알의 위치를 초기화
		p.m_Particle.SetPos(-1.5, 1.0, 0.0);
		p.m_Particle.ClearForce();
		p.m_Type = m_CurrShotType;
		break;
		//Fire();
	}		
	}
}