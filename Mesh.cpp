#include "Mesh.h"
#include <math.h>
#include "GL/glut.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Mesh::Mesh()
{
	_x = 0;
	_y = 0;
	_z = 0;
	vn[3] = { 0 };
}
Mesh::Mesh(char *obj, char *texture) {
	loadObj(obj);
	loadTexture(texture, _textureIndex);
}
Mesh::Mesh(char *obj, char *texture, float size, float _x, float _y, float _z, int ok) {
	loadObj(obj);
	loadTexture(texture, _textureIndex);
	this->_size = size;
	this->_x = _x;
	this->_y = _y;
	this->_z = _z;
}

Mesh::~Mesh()
{
	_x = 0;
	_y = 0;
	_z = 0;
}
void Mesh::loadTexture(char *file, GLuint &index) {
	glGenTextures(1, &index);
	//텍스쳐 여러개면 외부로 빼서 : glGenTextures(3, ...);

	FILE*fp;
	fopen_s(&fp, file, "rb");
	if (!fp) {
		printf("ERROR:NO %s. \n fail %d\n", file, index);
	}
	int width, height, channel;
	unsigned char *image = stbi_load_from_file(fp, &width, &height, &channel, 4);

	fclose(fp);

	glBindTexture(GL_TEXTURE_2D, index);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	//세부속성
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   //GL_LINEAR //GL_NEARES
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	

}
void Mesh::loadObj(char *filename)
{
	FILE *fp;
	char header[1000] = { 0 };
	double pos[3];
	int v_index[3], em[3]; 
	int index = 0;

	_minBoundary.Set(100000.0);
	_maxBoundary.Set(-100000.0);

	fopen_s(&fp, filename, "r");
	while (fscanf(fp, "%s %lf %lf %lf", header, &pos[0], &pos[1], &pos[2]) != EOF) {
		if (header[0] == 'v' && header[1] == NULL) {
			if (_minBoundary[0] > pos[0])	_minBoundary[0] = pos[0];
			if (_minBoundary[1] > pos[1])	_minBoundary[1] = pos[1];
			if (_minBoundary[2] > pos[2])	_minBoundary[2] = pos[2];
			if (_maxBoundary[0] < pos[0])	_maxBoundary[0] = pos[0];
			if (_maxBoundary[1] < pos[1])	_maxBoundary[1] = pos[1];
			if (_maxBoundary[2] < pos[2])	_maxBoundary[2] = pos[2];
			_vertices.push_back(new Vertex(index++, Vec3<double>(pos[0], pos[1], pos[2])));
		}
	}
	printf("num. vertices : %d\n", _vertices.size());
	printf("min boundary : %f, %f, %f\n", _minBoundary.x(), _minBoundary.y(), _minBoundary.z());
	printf("max boundary : %f, %f, %f\n", _maxBoundary.x(), _maxBoundary.y(), _maxBoundary.z());
		
	/*index = 0;
	fseek(fp, 0, SEEK_SET);
	while (fscanf(fp, "%s %lf %lf", header, &pos[0], &pos[1]) != EOF) {
		if (!strcmp(header, "vt")) {
			_textureCoords.push_back(new Texture(pos[0], 1.0 - pos[1], 0.0));
		}
	}
	printf("num. textureCoords : %d\n", _textureCoords.size());*/
	index = 0;
	fseek(fp, 0, SEEK_SET);
	while (fscanf(fp, "%s %d/%d/%d %d/%d/%d %d/%d/%d", header, &v_index[0], &vn[0], &em[0], &v_index[1], &vn[1], &em[1], &v_index[2], &vn[2], &em[2]) != EOF) {
		//fscanf(fp, "%s %d %d %d", header, &v_index[0], &v_index[1], &v_index[2]) != EOF) {
		if (header[0] == 'f' && header[1] == NULL) {
			auto v0 = _vertices[v_index[0] - 1];
			auto v1 = _vertices[v_index[1] - 1];
			auto v2 = _vertices[v_index[2] - 1];
			_faces.push_back(new Face(index++, v0, v1, v2));
		}
	}
	printf("num. faces : %d\n", _faces.size());
	moveToCenter(10.0);
	buildAdjacency();
	computeNormal();
	computeCotWeight();
	fclose(fp);
}

void Mesh::buildAdjacency(void)
{
	for (auto v : _vertices) {
		v->_nbFaces.clear();
		v->_nbVertices.clear();
	}

	// v-f
	for (auto f : _faces) {
		for (int j = 0; j < 3; j++) {
			f->_vertices[j]->_nbFaces.push_back(f);
		}
	}
	
	// v-v
	for (auto v : _vertices) {
		for (auto nf : v->_nbFaces) {
			auto pivot = nf->getIndex(v); // 0 : 1,2, 1 : 2,0, 2: 0,1
			int other_id0 = (pivot + 1) % 3;
			int other_id1 = (pivot + 2) % 3;
			if (!v->hasNbVertex(nf->_vertices[other_id0])) {
				v->_nbVertices.push_back(nf->_vertices[other_id0]);
			}
			if (!v->hasNbVertex(nf->_vertices[other_id1])) {
				v->_nbVertices.push_back(nf->_vertices[other_id1]);
			}
		}
	}
	computeEdge();
	//e-f
	for (auto e : _edges) {
		auto v = e->v(0);
		for (auto nf : v->_nbFaces) {
			if (nf->hasVertex(e->v(0)) && nf->hasVertex(e->v(1))) {
				if (!nf->hasEdge(e)) {
					nf->_edges.push_back(e);
				}
				if (!e->hasFace(nf)) {
					e->_nbFaces.push_back(nf);
				}
			}

		}

	}
	printf("build adjacency list\n");
}
void Mesh::computeEdge(void) {
	int index = 0;
	for (auto v : _vertices) {
		for (auto nv : v->_nbVertices) {
			if (!nv->_flag) {
				_edges.push_back(new Edge(index++, v, nv));
			}
		}
		v->_flag = true;
	}
	init();

	printf("num. edges : %d\n", _edges.size());
}
void Mesh::init(void) {
	for (auto v : _vertices) {
		v->_flag = false;
	}
}
void Mesh::computeNormal(void)
{
	// f-normal
	for (auto f : _faces) {
		auto a = f->_vertices[0]->_pos;
		auto b = f->_vertices[1]->_pos;
		auto c = f->_vertices[2]->_pos;
		auto normal = (a - b).Cross(a - c);
		normal.Normalize();
		f->_normal = normal;
	}

	// v-normal
	for (auto v : _vertices) {
		v->_normal.Clear();
		for (auto nf : v->_nbFaces) {
			v->_normal += nf->_normal;
		}
		v->_normal /= v->_nbFaces.size();
	}
	printf("compute normal\n");
}

void Mesh::computeCotWeight(void) {
	for (auto e : _edges) {
		if (e->_nbFaces.size() == 2) {
			double weight = 0.0;

			auto other_v = e->_nbFaces[0]->getOtherVertex(e);
			auto dir0 = e->v(0)->_pos - other_v->_pos;
			auto dir1 = e->v(1)->_pos - other_v->_pos;
			dir0.Normalize();
			dir1.Normalize();
			double angle = acos(dir0.Dot(dir1));
			weight += 1.0 / tan(angle);

			other_v = e->_nbFaces[1]->getOtherVertex(e);
			dir0 = e->v(0)->_pos - other_v->_pos;
			dir1 = e->v(1)->_pos - other_v->_pos;
			dir0.Normalize();
			dir1.Normalize();
			angle = acos(dir0.Dot(dir1));
			weight += 1.0 / tan(angle);

			e->_cotweight = weight;
		}
	}
}

void Mesh::moveToCenter(double scale)
{
	double max_length = fmax(fmax(_maxBoundary[0] - _minBoundary[0], _maxBoundary[1] - _minBoundary[1]), _maxBoundary[2] - _minBoundary[2]);
	auto center = (_maxBoundary + _minBoundary) / 2.0;
	Vec3<double> new_center(0.0, 0.0, 0.0);

	for (auto v : _vertices) {
		auto pos = v->_pos;
		auto grad = pos - center;
		grad /= max_length;
		grad *= scale;
		pos = new_center;
		pos += grad;
		v->_pos = pos;
	}
	printf("move to center\n");
}
//
//void Mesh::drawSurface()
//{
//	printf("surface");
//	glPushMatrix();
//	glEnable(GL_TEXTURE_2D);  //볼륨한다면 3D
//	glBindTexture(GL_TEXTURE_2D, _textureIndex);
//	glEnable(GL_LIGHTING);
//	//if (smoothing) {
//	//	//glBegin(GL_POLYGON);
//	//	glEnable(GL_SMOOTH);
//	//}
//	//else {
//	//	if (smooth)
//			glEnable(GL_FLAT);
//	//	else
//	//glEnable(GL_SMOOTH);
//	//}
//
//	for (auto f : _faces) {
//		glBegin(GL_POLYGON);	//GL_TRIANGLES   //GL_POLYGON
//		glNormal3f(f->_normal.x(), f->_normal.y(), f->_normal.z());
//
//		//for (auto v : f->_vertices) {
//		for (int i = 0; i < 3; i++) {
//			auto t = _textureCoords[f->_texelPos[i]];
//			
//			auto v = f->_vertices[i];
//			glTexCoord2f((GLfloat)t->x(), (GLfloat)t->y());
//			glVertex3f(v->x(), v->y(), v->z());
//			//glVertex3f((_size*v->x())+_x, (_size*v->y())+_y, (_size*v->z())+_z);
//		}
//
//		glEnd();
//	}
//
//	printf("faceConnect");
//	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
//	glPointSize(1.0f);
//	glDisable(GL_LIGHTING);
//	glDisable(GL_FLAT);
//	//glDisable(GL_SMOOTH);
//	glDisable(GL_TEXTURE_2D);
//	glPopMatrix();
//}

void Mesh::drawPoint(void)
{
	glDisable(GL_LIGHTING);
	glPointSize(2.0f);
	glBegin(GL_POINTS);
	for (auto v : _vertices) {
		glVertex3f(v->x(), v->y(), v->z());
	}
	glEnd();
	glPointSize(1.0f);
	glEnable(GL_LIGHTING);
}

void Mesh::drawGouraudShading(void)
{
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	for (auto f : _faces) {
		glBegin(GL_POLYGON);
		for (int j = 0; j < 3; j++) {
			glNormal3f(f->_vertices[j]->_normal.x(), f->_vertices[j]->_normal.y(), f->_vertices[j]->_normal.z());
			glVertex3f(f->_vertices[j]->x(), f->_vertices[j]->y(), f->_vertices[j]->z());
		}
		glEnd();
	}
	glShadeModel(GL_FLAT);
	glEnable(GL_LIGHTING);
}

void Mesh::drawSolid(void)
{
	glEnable(GL_LIGHTING);
	for (auto f : _faces) {
		glBegin(GL_POLYGON);
		glNormal3f(f->_normal.x(), f->_normal.y(), f->_normal.z());
		for (int j = 0; j < 3; j++) {
			glVertex3f(f->_vertices[j]->x(), f->_vertices[j]->y(), f->_vertices[j]->z());
		}
		glEnd();
	}
	glEnable(GL_LIGHTING);
}

void Mesh::drawWire(void)
{
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (auto f : _faces) {
		glBegin(GL_LINES);
		for (int j = 0; j < 3; j++) {
			int s = j % 3; // 0,1,2
			int e = (j + 1) % 3; // 0,1,2
			glVertex3f(f->_vertices[s]->x(), f->_vertices[s]->y(), f->_vertices[s]->z());
			glVertex3f(f->_vertices[e]->x(), f->_vertices[e]->y(), f->_vertices[e]->z());
		}
		glEnd();
	}
	glEnable(GL_LIGHTING);
}

void Mesh::smooth(int iter) 
{
	/*vector<Vec3<double>> newPos;
	for (int i = 0; i < iter; i++) {
		for (auto v : _vertices) {
			auto pos = v->_pos;
			for (auto nv : v->_nbVertices) {
				pos += nv->_pos;
			}
			pos /= (double)v->_nbVertices.size()+1;
			newPos.push_back(pos);
		}
		for (int j = 0; j < _vertices.size(); j++) {
			_vertices[j]->_pos = newPos[j];
		}
		newPos.clear();
	}
	computeNormal();
	printf("smoothing : %d\n", iter);*/
	vector<Vec3<double>> _newVertieces;
	int numvVertices = _vertices.size();
	for (int j = 0; j < iter; j++) {
		for (int i = 0; i < numvVertices; i++) {
			auto v = _vertices[i];
			auto pos = v->_pos;
			for (auto nv : v->_nbVertices) {
				pos += nv->_pos;
			}
			pos /= (double)(v->_nbVertices.size() + 1);
			_newVertieces.push_back(pos);
		}
		for (int i = 0; i < numvVertices; i++) {
			_vertices[i]->_pos = _newVertieces[i];
		}
		_newVertieces.clear();
	}
	
	//buildAdjacency();
	computeNormal();
}
void Mesh::laplacianUniform(int iter) 
{
	double lambda = 0.25;
	int numVertices = _vertices.size();

	for (int t = 0; t < iter; t++) {
		for (int i = 0; i < numVertices; i++) {
			auto v = _vertices[i];
			auto pos = v->_pos;
			Vec3<double> sumNbPos;
			for (auto nv : v->_nbVertices) {
				sumNbPos += nv->_pos;
			}
			int numNbVertices = v->_nbVertices.size();
			auto newPos = (sumNbPos / numNbVertices - pos)*lambda + pos;
			_vertices[i]->_pos = newPos;
		}
	}
	computeNormal();
	printf("Laplacian smoothing \n");
}
void Mesh::taubinSmooth(int iter)
{
	double lambda = 0.5;
	double mu = -0.67;
	int nVertices = _vertices.size();
	for (int t = 0; t < iter; t++) {
		for (int i = 0; i < nVertices; i++) {
			auto v = _vertices[i];
			auto pos = v->_pos;
			Vec3<double> sumNbPos;
			for (auto nv : v->_nbVertices) {
				sumNbPos += nv->_pos;
			}
			int numNbVertices = v->_nbVertices.size();
			auto newPos = (sumNbPos / numNbVertices - pos)*lambda + pos;
			newPos = (sumNbPos / numNbVertices - newPos)*mu + newPos;
			_vertices[i]->_pos = newPos;
		}
	}
	computeNormal();
	printf("taubinSmoothing \n" );
}
void Mesh::laplacianCotangent(int iter) 
{
	double lambda = 0.25;
	int numVertices = _vertices.size();

	for (int t = 0; t < iter; t++) {
		for (int i = 0; i < numVertices; i++) {
			auto v = _vertices[i];
			auto pos = v->_pos;
			Vec3<double> sumNbPos;
			double sumCotWeight = 0.0;
			//printf("cotangentSmoothing12 \n", iter);
			for (auto ne : v->_nbEdges) {
				//printf("cotangentSmoothing1 \n", iter);
				auto pos0 = ne->v(0)->_pos;
				auto pos1 = ne->v(1)->_pos;
				double cotweight = ne->_cotweight;
				if (v == ne->v(0)) {
					sumNbPos += (pos1 - pos0)*cotweight;
				}
				else {
					sumNbPos += (pos0 - pos1)*cotweight;

				}
				sumCotWeight += cotweight;
			}
			auto newPos = (sumNbPos / sumCotWeight)*lambda + pos;
			_vertices[i]->_pos = newPos;
		}
	}
	computeNormal();
	printf("cotangentSmoothing \n");
}
void Mesh::smoothing() {

}