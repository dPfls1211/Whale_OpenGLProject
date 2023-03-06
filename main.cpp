#include <stdio.h>
#include <Windows.h> 
#include "GL\glut.h"
#include "Mesh.h"
#include "Ballistic.h"
#include<MMSystem.h>
#pragma comment(lib,"Winmm.lib")

float _zoom = 15.0f; // 화면 확대,축소
float _rot_x = 0.0f; // x축 회전
float _rot_y = 0.001f; // y축 회전
float _trans_x = 0.0f; // x축 이동
float _trans_y = 0.0f; // y축 이동
int _last_x = 0; // 이전 마우스 클릭 x위치
int _last_y = 0; // 이전 마우스 클릭 y위치
unsigned char _buttons[3] = { 0 }; // 마우스 상태(왼쪽,오른쪽,휠 버튼)

Mesh *_mesh;
int _renderMode = 0;

Ballistic m_Ballistic;

Ballistic childBallistic(400);
float _timeLeft = 2.0f;
float dt = 0.002f;


void Init(void)
{

	// 깊이값 사용 여부
	glEnable(GL_DEPTH_TEST);
	// ...
}

void light(void) {
	glEnable(GL_LIGHTING); // 조명 활성화
	glEnable(GL_LIGHT0); // 첫번째 조명

	GLfloat mat_ambient[4] = { 0.33, 0.22, 0.33, 1.0 };
	GLfloat mat_diffuse[4] = {0.0f, 0.5f, 0.75f, 0.5f };
	//	GLfloat mat_diffuse[4] = { 1.0, 1.0, 1.0, 0.0 };
	//GLfloat mat_specular[4] = { 0.99, 0.91, 0.81, 1.0 };
	GLfloat mat_shininess = 27.8f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

}
void DrawParticle(void)
{
	// Render each particle in turn
	for (auto &p : m_Ballistic.m_Ammunitions) {
		if (p.m_Type != ShotType::UNUSED) {
			p.Darw(); 
			
		}
	}
	
		for (auto &sp : childBallistic.m_Ammunitions) {
			if (sp.m_Type != ShotType::UNUSED) {
				sp.Darw();

			}
		}
	
}
void Draw(void)
{
	light();

	DrawParticle();
	/*_mesh->drawSurface();*/
	switch (_renderMode)
	{
	case 0:
		_mesh->drawSolid();
		break;
	case 1:
		_mesh->drawWire();
		break;
	case 2:
		_mesh->drawPoint();
		break;
	case 3:
		_mesh->drawGouraudShading();
		break;
	}
	m_Ballistic.Fire();

	//double dt = 0.01;
	//for (auto &p : childBallistic.m_Ammunitions) {
	//p.m_Particle.Integrate(dt);
		/*if (p.m_Particle.GetPos().y() > 5.0f) {
			printf("y");*/
	_timeLeft -= dt;
	if (_timeLeft < 1.9f) {
		for (int i = 0; i < 20; i++) {
			float theta = i * rand() % 360 / 180.0f * 3.141592;
			double px = 5.0f *cos(theta);
			double pz = 5.0f * sin(theta);
			childBallistic.Firedd(px, pz, 6.5f);
		}
		
	}
		//}
	//}
	//childBallistic.Firedd();
	glDisable(GL_LIGHTING);
}

void Display(void)
{
	glClearColor(0.1, 0.4, 0.70, 0.33);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0, 0, -_zoom);
	glTranslatef(_trans_x, _trans_y, 0);
	glRotatef(_rot_x, 1, 0, 0);
	glRotatef(_rot_y, 0, 1, 0);

	Draw(); // 그려지는 물체
	glutSwapBuffers(); // GLUT_DOUBLE 
}

void Reshape(int w, int h)
{
	if (w == 0) {
		h = 1;
	}
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluPerspective(45.0, (float)w / h, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void Update(void)
{
	m_Ballistic.Update();
	childBallistic.Update();
	::glutPostRedisplay();
}
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'Q':
	case 'q':
		exit(0);
	case 's':
	case 'S':
		_renderMode = 0;
		break;
	case 'w':
	case 'W':
		_renderMode = 1;
		break;
	case 'p':
	case 'P':
		_renderMode = 2;
		break;
	case 'g':
	case 'G':
		_renderMode = 3;
		//_mesh->laplacianUniform(10);
		break;
	case 'a':
	case 'A':
		_mesh->smooth(1);
		break;
	case 'o': //laplacianUniform
	case 'O':
		_mesh->laplacianUniform(10);
		break;
	case 't': //taubin
	case 'T':
		_mesh->taubinSmooth(10);
		break;
	case 'C':
	case 'c':
		_mesh->laplacianCotangent(1);
		break;
	case '2':
		m_Ballistic.m_CurrShotType = ShotType::ARTILLERY;
		break;
	default:
		break;
	}
	glutPostRedisplay(); // 화면갱신
}

void Mouse(int button, int state, int x, int y)
{
	// 이전 마우스 클릭 위치
	_last_x = x;
	_last_y = y;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		_buttons[0] = state == GLUT_DOWN ? 1 : 0;
		break;
	case GLUT_MIDDLE_BUTTON:
		_buttons[1] = state == GLUT_DOWN ? 1 : 0;
		break;
	case GLUT_RIGHT_BUTTON:
		_buttons[2] = state == GLUT_DOWN ? 1 : 0; 
		if (state == GLUT_DOWN) {
			//for(int i = 0 ;i<15; i++)
				m_Ballistic.Fire();
		}
		break;
	default:
		break;
	}
	
	glutPostRedisplay(); // 화면갱신
}

void Motion(int x, int y)
{
	int diff_x = x - _last_x;
	int diff_y = y - _last_y;
	_last_x = x;
	_last_y = y;

	if (_buttons[2]) {
		_zoom -= (float)0.02f * diff_x;
	}
	else if (_buttons[1]) {
		_trans_x += (float)0.02f * diff_x;
		_trans_y -= (float)0.02f * diff_y;
	}
	else if (_buttons[0]) {
		_rot_x += (float)0.2f * diff_y;
		_rot_y += (float)0.2f * diff_x;
	}
	glutPostRedisplay(); // 화면갱신
}

void main(int argc, char** argv)
{
	PlaySound(TEXT("wave2.wav"), 0, SND_FILENAME | SND_ASYNC |SND_LOOP);
	_mesh = new Mesh("OBJ\\whales7.obj", "obj\\whales7.png");
	//_mesh = new Mesh("OBJ\\bunny.obj");
	childBallistic.m_CurrShotType = ShotType::ARTILLERY;
	//_mesh->laplacianCotangent(150);
	//_mesh->taubinSmooth(10);
	//_mesh->laplacianCotangent(50);
	//_mesh->taubinSmooth(10);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Whale");
	glutDisplayFunc(Display); // 화면에 보여질 부분
	glutIdleFunc(Update);
	glutReshapeFunc(Reshape); // 화면창 초기화, 창의 크기 변경
	glutMouseFunc(Mouse); // 마우스 클릭 이벤트
	glutMotionFunc(Motion); // 마우스 이동 이벤트
	glutKeyboardFunc(Keyboard); // 키보드 클릭 이벤트
	Init(); // 각종 클래스, 변수 초기화
	glutMainLoop();
}