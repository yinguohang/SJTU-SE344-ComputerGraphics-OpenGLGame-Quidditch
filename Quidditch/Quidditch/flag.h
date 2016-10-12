#ifndef _FLAG_H_
#define _FLAG_H_

#include "drawable.h"
#include "gl/glut.h"
#include <time.h>
#include <vector>

// 质点弹簧模型中质点和弹簧的数量
#define P_X 5
#define P_Y 5

// NURBS的曲线度数
#define DEGREE 4

// knot数组长度 （5 + 3 + 3）+ 4
#define KNOT_N_X (P_X + 3 * DEGREE - 2)
#define KNOT_N_Y (P_Y + 3 * DEGREE - 2)

// 控制数组size P_X/Y + 2 * (DEGREE - 1)
#define CTP_X (P_X + 2 * DEGREE - 2)
#define CTP_Y (P_Y + 2 * DEGREE - 2)

#define CONSTRUCT_K 100
#define CUT_K 10

//质点
struct Partical {
	Vec position;
	Vec velocity;
	Vec force;
	float mass;
};

struct Spring {
	int p1, p2;
	float rl, ks, kd;
};

class Flag : public Drawable {
	clock_t t;
	int x, y, z;
	float scale;
	GLUnurbsObj *surf;
	GLfloat ctrlpoints[CTP_X][CTP_Y][3];
	vector<Partical> particals;
	vector<Spring> springs;

	GLUquadricObj *quadratic;
	static GLfloat knots_x[KNOT_N_X];
	static GLfloat knots_y[KNOT_N_Y];
	static GLfloat model[P_X][P_Y][3];
	static bool first;
	static GLfloat texture[CTP_X][CTP_Y][2];
public:
	Flag(int xx, int yy, int zz, float scale = 1.0);
	void draw();
	void move();
};

#endif