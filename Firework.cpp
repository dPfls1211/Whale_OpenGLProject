#include "Firework.h"

double randDouble(double low, double high)
{
	double d = (double)rand() / RAND_MAX;
	return (low + d * (high - low));
}

Vec3<double> randVec3(Vec3<double> low, Vec3<double> high)
{
	return Vec3<double>(randDouble(low.x(), high.x()), randDouble(low.y(), high.y()), randDouble(low.z(), high.z()));
}

int randInt(int max)
{
	return rand() % max;
}

Firework::Firework()
{
}

Firework::~Firework()
{
}

bool Firework::Update(double dt)
{
	Integrate(dt);
	m_Age -= dt;
	return (m_Age < 0.0) || (m_Position.y() < 0.0);
}

FireworkRule::FireworkRule(void)
{
	m_NumPayLoad = 0;
}

FireworkRule::~FireworkRule(void)
{
	m_PayLoads.clear();
}

void FireworkRule::Init(int numPayLoad)
{
	m_NumPayLoad = numPayLoad;
	m_PayLoads.clear();
	m_PayLoads.resize(numPayLoad);
}

void FireworkRule::SetParam(int type, double minAge, double maxAge, Vec3<double> minVel, Vec3<double> maxVel, double damping)
{
	m_Type = type;
	m_MinAge = minAge;
	m_MaxAge = maxAge;
	m_MinVelocity = minVel;
	m_MaxVelocity = maxVel;
	m_Damping = damping;
}

void FireworkRule::Create(Firework *fw, Firework *parent)
{
	fw->m_Type = m_Type;
	fw->m_Age = randDouble(m_MinAge, m_MaxAge);	

	Vec3<double> vel;

	if (parent) {
		// The position and velocity are based on the parent
		fw->SetPos(parent->GetPos());
		vel += parent->GetVel();
	} else {
		int x = randInt(3) - 1;
		Vec3<double> startPos;
		startPos(0) = 5.0f * double(x);
		fw->SetPos(startPos);
	}

	vel += randVec3(m_MinVelocity, m_MaxVelocity);
	fw->SetVel(vel);

	fw->SetMass(1.0);
	fw->SetDamping(m_Damping);
	fw->SetAcc(Vec3<double>(0.0, -9.81, 0.0)); // gravity
	fw->ClearForce();
}

FireworkEngine::FireworkEngine(void)
{
	m_Rules.clear();
	m_Fireworks.clear();
	m_Rules.resize(m_MaxNumFireworks);
	m_Fireworks.resize(m_MaxNumFireworks);

	Init();
}

FireworkEngine::~FireworkEngine(void)
{
}

void FireworkEngine::Init(void)
{
	for (auto &fw : m_Fireworks) {
		fw.m_Type = 0;
	}

	// Init : firework rules 0~8
	m_Rules[0].Init(2);
	m_Rules[0].SetParam(1, 0.5, 1.4, Vec3<double>(-5.0, 25.0, -5.0), Vec3<double>(5.0, 28.0, 5.0), 0.1);
	m_Rules[0].m_PayLoads[0].Set(3, 5);
	m_Rules[0].m_PayLoads[1].Set(5, 5);

	m_Rules[1].Init(1);
	m_Rules[1].SetParam(2, 0.5, 1.0, Vec3<double>(-5.0, 10.0, -5.0), Vec3<double>(5.0, 20.0, 5.0), 0.8);
	m_Rules[1].m_PayLoads[0].Set(4, 2);

	m_Rules[2].Init(0);
	m_Rules[2].SetParam(3, 0.5, 1.5, Vec3<double>(-5.0, -5.0, -5.0), Vec3<double>(5.0, 5.0, 5.0), 0.1);

	m_Rules[3].Init(0);
	m_Rules[3].SetParam(4, 0.25, 0.5, Vec3<double>(-20.0, 5.0, -5.0), Vec3<double>(20.0, 5.0, 5.0), 0.2);

	m_Rules[4].Init(1);
	m_Rules[4].SetParam(5, 0.5, 1.0, Vec3<double>(-20.0, 2.0, -5.0), Vec3<double>(20.0, 18.0, 5.0), 0.01);
	m_Rules[4].m_PayLoads[0].Set(3, 5);

	m_Rules[5].Init(0);
	m_Rules[5].SetParam(6, 3.0, 5.0, Vec3<double>(-5.0, -5.0, -5.0), Vec3<double>(5.0, 10.0, 5.0), 0.95);

	m_Rules[6].Init(1);
	m_Rules[6].SetParam(7, 4.0, 5.0, Vec3<double>(-5.0, 50.0, -5.0), Vec3<double>(5.0, 60.0, 5.0), 0.01);
	m_Rules[6].m_PayLoads[0].Set(8, 10);

	m_Rules[7].Init(0);
	m_Rules[7].SetParam(8, 0.25, 0.5, Vec3<double>(-1.0, -1.0, -1.0), Vec3<double>(1.0, 1.0, 1.0), 0.01);

	m_Rules[8].Init(0);
	m_Rules[8].SetParam(9, 3.0, 5.0, Vec3<double>(-15.0, 10.0, -5.0), Vec3<double>(15.0, 15.0, 5.0), 0.95);
}

void FireworkEngine::Create(int type, Firework *parent)
{
	static int id = 0;
	auto &rule = m_Rules[type - 1];
	rule.Create(&m_Fireworks[id], parent);
	id = (id + 1) % m_MaxNumFireworks;
}

void FireworkEngine::Create(int type, int num, Firework *parent)
{
	for (int i = 0; i < num; i++) {
		Create(type, parent);
	}
}

void FireworkEngine::Update(void)
{
	double dt = 0.01;

	for (auto &fw : m_Fireworks) {
		if (fw.m_Type > 0) {
			if (fw.Update(dt)) {
				auto &rule = m_Rules[fw.m_Type - 1];
				fw.m_Type = 0;	
				for (int i = 0; i < rule.m_NumPayLoad; i++) {
					auto &payLoad = rule.m_PayLoads[i];
					Create(payLoad.m_Type, payLoad.m_NumParticle, &fw);
				}
			}
		}
	}
}

void FireworkEngine::Draw(void)
{
	glPushMatrix();
	glPointSize(4.0f);
	glBegin(GL_POINTS);
	for (auto &fw : m_Fireworks) {
		if (fw.m_Type > 0) {
			switch (fw.m_Type) 
			{
			case 1: glColor3f(1.0f, 0.0f, 0.0f); break;
			case 2: glColor3f(1.0f, 0.5f, 0.0f); break;
			case 3: glColor3f(1.0f, 1.0f, 0.0f); break;
			case 4: glColor3f(0.0f, 1.0f, 0.0f); break;
			case 5: glColor3f(0.0f, 1.0f, 1.0f); break;
			case 6: glColor3f(0.4f, 0.4f, 1.0f); break;
			case 7: glColor3f(1.0f, 0.0f, 1.0f); break;
			case 8: glColor3f(1.0f, 1.0f, 1.0f); break;
			case 9: glColor3f(1.0f, 0.5f, 0.5f); break;
			};

			auto pos = fw.GetPos();
			glVertex3f(pos.x(), pos.y(), pos.z());
		}
	}
	glEnd();
	glPointSize(1.0f);
	glPopMatrix();
}