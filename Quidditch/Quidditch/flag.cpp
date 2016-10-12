#include "flag.h"
#include "gl/glut.h"
#include "gl/glu.h"
#include <vector>
#include <cmath>
#include "settings.h"
#include "texture.h"

GLfloat Flag::knots_x[KNOT_N_X] = { 0.0, 0.0, 0.0, 0.0, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875, 1.0, 1.0, 1.0, 1.0 };
GLfloat Flag::knots_y[KNOT_N_Y] = { 0.0, 0.0, 0.0, 0.0, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875, 1.0, 1.0, 1.0, 1.0 };

GLfloat Flag::model[P_X][P_Y][3] = {
	{
		{ 0.0, 0.0, 0.0 },
		{ 25.0, 0.0, 0.0 },
		{ 50.0, 0.0, 0.0 },
		{ 75.0, 0.0, 0.0 },
		{ 100.0, 0.0, 0.0 },
	},
	{
		{ 0.0, 20.0, 0.0 },
		{ 25.0, 20.0, 0.0 },
		{ 50.0, 20.0, 0.0 },
		{ 75.0, 20.0, 0.0 },
		{ 100.0, 20.0, 0.0 }
	},
	{
		{ 0.0, 40.0, 0.0 },
		{ 25.0, 40.0, 0.0 },
		{ 50.0, 40.0, 0.0 },
		{ 75.0, 40.0, 0.0 },
		{ 100.0, 40.0, 0.0 }
	},
	{
		{ 0.0, 60.0, 0.0 },
		{ 25.0, 60.0, 0.0 },
		{ 50.0, 60.0, 0.0 },
		{ 75.0, 60.0, 0.0 },
		{ 100.0, 60.0, 0.0 }
	},
	{
		{ 0.0, 80.0, 0.0 },
		{ 25.0, 80.0, 0.0 },
		{ 50.0, 80.0, 0.0 },
		{ 75.0, 80.0, 0.0 },
		{ 100.0, 80.0, 0.0 }
	},
};

bool Flag::first = true;
GLfloat Flag::texture[CTP_X][CTP_Y][2];

GLuint ft;

Flag::Flag(int xx, int yy, int zz, float scale1):x(xx), y(yy), z(zz), scale(scale1), particals(P_X * P_Y){
	quadratic = gluNewQuadric();
	type = FLAG;
	surf = gluNewNurbsRenderer();
	if (first) {
		first = false;
		for (int i = 0; i < 11; i++) {
			for (int j = 0; j < 11; j++) {
				texture[i][j][0] = (j - 3) * 0.25;
				texture[i][j][1] = (i - 3) * 0.25;
				if (texture[i][j][0] < 0)
					texture[i][j][0] = 0;
				if (texture[i][j][0] > 1)
					texture[i][j][0] = 1;
				if (texture[i][j][1] < 0)
					texture[i][j][1] = 0;
				if (texture[i][j][1] > 1)
					texture[i][j][1] = 1;
			}
		}
		ft = ATLLoadTexture("image/flag.jpg");
	}

	t = clock();
	for (int i = 0; i < P_X; i++) {
		for (int j = 0; j < P_Y; j++) {
			ctrlpoints[i][j][0] = model[i][j][0] * scale + xx;
			ctrlpoints[i][j][1] = model[i][j][1] * scale + yy;
			ctrlpoints[i][j][2] = model[i][j][2] * scale + zz;
			int now = i * P_Y + j;
			particals[now].position.x = ctrlpoints[i][j][0];
			particals[now].position.y = ctrlpoints[i][j][1];
			particals[now].position.z = ctrlpoints[i][j][2];
			particals[now].velocity.x = 0;
			particals[now].velocity.y = 0;
			particals[now].velocity.z = 0;
			particals[now].force.x = 0;
			particals[now].force.y = 0;
			particals[now].force.z = 0;
			particals[now].mass = 1;
			//结构弹簧, 向左下方向
			if (j > 0) {
				Spring s;
				s.p1 = now - 1;
				s.p2 = now;
				s.rl = 25 * scale;
				s.ks = CONSTRUCT_K;
				springs.push_back(s);
			}
			if (i > 0) {
				Spring s;
				s.p1 = now - P_Y;
				s.p2 = now;
				s.rl = 20 * scale;
				s.ks = CONSTRUCT_K;
				springs.push_back(s);
			}
			//剪切弹簧
			if (j > 0 && i > 0) {
				Spring s;
				s.p1 = now - P_Y - 1;
				s.p2 = now;
				s.rl = sqrt(25 * 25 + 20 * 20) * scale;
				s.ks = CUT_K;
				springs.push_back(s);
			}
			if (j < P_Y - 1 && i > 0) {
				Spring s;
				s.p1 = now - P_Y + 1;
				s.p2 = now;
				s.rl = sqrt(25 * 25 + 20 * 20) * scale;
				s.ks = CUT_K;
				springs.push_back(s);
			}
		}
	}
}



void Flag::draw() {
	glPushMatrix();
	glTranslatef(x, 0, z);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(quadratic, 2, 2, y + 80 * scale, 32, 32); 
	glPopMatrix();
	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 80.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); 
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	for (int i = 0; i < P_X; i++) {
		for (int j = 0; j < P_Y; j++) {
			int now = i * P_Y + j;
			ctrlpoints[i + DEGREE - 1][j + DEGREE - 1][0] = particals[now].position.x;
			ctrlpoints[i + DEGREE - 1][j + DEGREE - 1][1] = particals[now].position.y;
			ctrlpoints[i + DEGREE - 1][j + DEGREE - 1][2] = particals[now].position.z;
		}
	}
	for (int i = DEGREE - 1; i < DEGREE + P_X - 1; i++) {
		for (int j = 0; j < DEGREE - 1; j++) {
			ctrlpoints[i][j][0] = ctrlpoints[i][DEGREE - 1][0];
			ctrlpoints[i][j][1] = ctrlpoints[i][DEGREE - 1][1];
			ctrlpoints[i][j][2] = ctrlpoints[i][DEGREE - 1][2];
			ctrlpoints[i][DEGREE + P_X - 1 + j][0] = ctrlpoints[i][DEGREE + P_X - 2][0];
			ctrlpoints[i][DEGREE + P_X - 1 + j][1] = ctrlpoints[i][DEGREE + P_X - 2][1];
			ctrlpoints[i][DEGREE + P_X - 1 + j][2] = ctrlpoints[i][DEGREE + P_X - 2][2];
		}
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 11; j++) {
			ctrlpoints[i][j][0] = ctrlpoints[3][j][0];
			ctrlpoints[i][j][1] = ctrlpoints[3][j][1];
			ctrlpoints[i][j][2] = ctrlpoints[3][j][2];
			ctrlpoints[8 + i][j][0] = ctrlpoints[7][j][0];
			ctrlpoints[8 + i][j][1] = ctrlpoints[7][j][1];
			ctrlpoints[8 + i][j][2] = ctrlpoints[7][j][2];
		}
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ft);
	//for (int i = 0; i < CTP_X; i++) {
	//	for (int j = 0; j < CTP_Y; j++) {
	//		cout << "(" << texture[i][j][0] << "," << texture[i][j][1] << ")";
	//	}
	//	cout << endl;
	//}
	//for (int i = 0; i < CTP_X; i++) {
	//	for (int j = 0; j < CTP_Y; j++) {
	//		cout << "(" << ctrlpoints[i][j][0] << "," << ctrlpoints[i][j][1] << "," << ctrlpoints[i][j][2] << ")";
	//	}
	//	cout << endl;
	//}
	gluBeginSurface(surf);
	gluNurbsSurface(surf, KNOT_N_X, knots_x, KNOT_N_Y, knots_y, CTP_Y * 2, 2, &texture[0][0][0], DEGREE, DEGREE, GL_MAP2_TEXTURE_COORD_2);
	gluNurbsSurface(surf, KNOT_N_X, knots_x, KNOT_N_Y, knots_y, CTP_Y * 3, 3, &ctrlpoints[0][0][0], DEGREE, DEGREE, GL_MAP2_VERTEX_3);
	gluEndSurface(surf);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void Flag::move() {
	//先按照v移动
	for (int i = 0; i < particals.size(); i++) {
		if (i % P_Y == 0) continue;	//最左侧的点不动
		particals[i].position = particals[i].position + particals[i].velocity * (1.0 / FPS);
	}
	//再更新v
	//内力
	for (int i = 0; i < particals.size(); i++) {
		particals[i].force = Vec(0, 0, 0);
	}
	for (int i = 0; i < springs.size(); i++) {
		int p1 = springs[i].p1;
		int p2 = springs[i].p2;
		double dis = (particals[p2].position - particals[p1].position).length();
		particals[p1].force = particals[p1].force + 
			(particals[p2].position - particals[p1].position).normalize() * ((dis - springs[i].rl) * springs[i].ks);
		particals[p2].force = particals[p2].force +
			(particals[p1].position - particals[p2].position).normalize() * ((dis - springs[i].rl) * springs[i].ks);
	}
	//重力&风力
	for (int i = 0; i < particals.size(); i++) {
		particals[i].force = particals[i].force + Vec(0, -3, 0) * particals[i].mass;
		if (((clock() - t) / int(CLOCKS_PER_SEC * 0.3)) % 2 == 0)
			particals[i].force = particals[i].force + Vec(0, 0, 300);
		else
			particals[i].force = particals[i].force + Vec(300, 0, 0);
	}
	//阻力-v^2
	for (int i = 0; i < particals.size(); i++) {
		particals[i].force = particals[i].force + (-0.001) * (particals[i].velocity * particals[i].velocity) * particals[i].velocity.normalize();
	}
	for (int i = 0; i < particals.size(); i++) {
		particals[i].velocity = particals[i].velocity + particals[i].force * (1.0 / FPS / particals[i].mass);
	}
}

/*
//质点
struct Partical {
	// int num;
	Vec position;
	Vec velocity;
	Vec force;
	float mass;
};
// 弹簧
struct Spring {
	// int num;
	int p1, p2;
	float rl, ks, kd;
};

#define P_X 30
#define P_Y 30

vector<Partical> particals(P_X * P_Y);
vector<Spring> springs;

#define CONSTRUCT_K 3000
#define CUT_K 100
#define BENDING_K 100


Flag::Flag(int xx, int yy, int zz, float scale1) :x(xx), y(yy), z(zz), scale(scale1) {
	t = clock();
	for (int i = 0; i < P_X; i++) {
		for (int j = 0; j < P_Y; j++) {
			int now = i * P_Y + j;
			particals[now].position.x = j * 5 * scale + x;
			particals[now].position.y = i * 5 * scale + y;
			particals[now].position.z = 0 + z;
			particals[now].velocity.x = 0;
			particals[now].velocity.y = 0;
			particals[now].velocity.z = 0;
			particals[now].force.x = 0;
			particals[now].force.y = 0;
			particals[now].force.z = 0;
			particals[now].mass = 10;
			//结构弹簧, 向左下方向
			if (j > 0) {
				Spring s;
				s.p1 = now - 1;
				s.p2 = now;
				s.rl = 5 * scale;
				s.ks = CONSTRUCT_K;
				springs.push_back(s);
			}
			if (i > 0) {
				Spring s;
				s.p1 = now - P_Y;
				s.p2 = now;
				s.rl = 5 * scale;
				s.ks = CONSTRUCT_K;
				springs.push_back(s);
			}
			//弯曲弹簧
			if (j > 1) {
				Spring s;
				s.p1 = now - 2;
				s.p2 = now;
				s.rl = 5 * scale * 2;
				s.ks = BENDING_K;
				springs.push_back(s);
			}
			if (i > 1) {
				Spring s;
				s.p1 = now - 2 * P_Y;
				s.p2 = now;
				s.rl = 5 * scale * 2;
				s.ks = BENDING_K;
				springs.push_back(s);
			}
			//剪切弹簧
			if (j > 0 && i > 0) {
				Spring s;
				s.p1 = now - P_Y - 1;
				s.p2 = now;
				s.rl = sqrt(5 * 5 + 5 * 5) * scale;
				s.ks = CUT_K;
				springs.push_back(s);
			}
			if (j < P_Y - 1 && i > 0) {
				Spring s;
				s.p1 = now - P_Y + 1;
				s.p2 = now;
				s.rl = sqrt(5 * 5 + 5 * 5) * scale;
				s.ks = CUT_K;
				springs.push_back(s);
			}
		}
	}
}

Vec norm(Vec v1, Vec v2, Vec v3) {
	Vec x1 = v2 - v1;
	Vec x2 = v3 - v1;
	return x1.dc(x2).normalize();
}

void Flag::draw() {
	GLfloat mat_ambient[] = { 0.3, 0.3, 0, 1.0 };
	GLfloat mat_diffuse[] = { 0.7, 0.7, 0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 80.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); glPushMatrix();

	Vec n;

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < P_X; i++) {
		for (int j = 0; j < P_Y; j++) {
			int now = i * P_Y + j;
			if (i < P_X - 1 && j < P_Y - 1) {
				n = norm(particals[now].position, particals[now + P_Y].position, particals[now + 1].position);
				glVertex3f(particals[now].position.x, particals[now].position.y, particals[now].position.z);
				glNormal3f(n.x, n.y, n.z);
				glVertex3f(particals[now + 1].position.x, particals[now + 1].position.y, particals[now + 1].position.z);
				glNormal3f(n.x, n.y, n.z);
				glVertex3f(particals[now + P_Y].position.x, particals[now + P_Y].position.y, particals[now + P_Y].position.z);
				glNormal3f(n.x, n.y, n.z);
				n = norm(particals[now + P_Y + 1].position, particals[now + 1].position, particals[now + P_Y].position);
				glVertex3f(particals[now + 1].position.x, particals[now + 1].position.y, particals[now + 1].position.z);
				glNormal3f(n.x, n.y, n.z);
				glVertex3f(particals[now + P_Y].position.x, particals[now + P_Y].position.y, particals[now + P_Y].position.z);
				glNormal3f(n.x, n.y, n.z);
				glVertex3f(particals[now + P_Y + 1].position.x, particals[now + P_Y + 1].position.y, particals[now + P_Y + 1].position.z);
				glNormal3f(n.x, n.y, n.z);
			}
		}
	}
	glEnd();
}

void Flag::move() {
	//先按照v移动
	for (int i = 0; i < particals.size(); i++) {
		if (i % P_Y == 0) continue;	//最左侧的点不动
		particals[i].position = particals[i].position + particals[i].velocity * (1.0 / FPS);
	}
	//再更新v
	//内力
	for (int i = 0; i < particals.size(); i++) {
		particals[i].force = Vec(0, 0, 0);
	}
	for (int i = 0; i < springs.size(); i++) {
		int p1 = springs[i].p1;
		int p2 = springs[i].p2;
		double dis = (particals[p2].position - particals[p1].position).length();
		particals[p1].force = particals[p1].force +
			(particals[p2].position - particals[p1].position).normalize() * ((dis - springs[i].rl) * springs[i].ks);
		particals[p2].force = particals[p2].force +
			(particals[p1].position - particals[p2].position).normalize() * ((dis - springs[i].rl) * springs[i].ks);
	}
	//重力&风力
	for (int i = 0; i < particals.size(); i++) {
		//particals[i].force = particals[i].force + Vec(0, -1, 0) * particals[i].mass;
		particals[i].force = particals[i].force + Vec(0, 0, 500);
	}
	//阻力-v^2
	for (int i = 0; i < particals.size(); i++) {
		particals[i].force = particals[i].force + (-0.001) * (particals[i].velocity * particals[i].velocity) * particals[i].velocity.normalize();
	}
	for (int i = 0; i < particals.size(); i++) {
		particals[i].velocity = particals[i].velocity + particals[i].force * (1.0 / FPS / particals[i].mass);
	}
}
*/