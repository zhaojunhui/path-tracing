#include <sstream>
#include <cstdio>
#include <string>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include "object.h"
#include "AABB.h"
using namespace Eigen;

Material::Material()
{
	color = absor = Color();
	refl = refr = 0;
	diff = spec = 0;
	rindex = 0;
	drefl = 0;
	texture = NULL;
}

void Material::Init(std::string variable, std::stringstream& fin)
{
	if(variable == "color=") color.Init(fin);
	if(variable == "absor=") absor.Init(fin);
	if(variable == "refl=") fin >> refl;
	if(variable == "refr=") fin >> refr;
	if(variable == "diff=") fin >> diff;
	if(variable == "spec=") fin >> spec;
	if(variable == "drefl=") fin >> drefl;
	if(variable == "rindex=") fin >> rindex;
	if(variable == "texture=")
    {
		std::string file; fin >> file;
		texture = new Png;
		texture->Init(file);
	}
}

Object::Object()
{
	sample = rand();
	material = new Material;
	next = NULL;
}

Object::Object(const Object& obj)
{
	*this = obj;
	material = new Material;
	*material = *obj.material;
}

Object::~Object()
{
	delete material;
}

void Object::Init(std::string variable, std::stringstream& fin)
{
	material->Init(variable, fin);
}

void Plane::Init(std::string variable, std::stringstream& fin)
{
    if(variable == "N=") N.Init(fin);
    if(variable == "R=") fin >> R;
    if(variable == "Dx=") Dx.Init(fin);
    if(variable == "Dy=") Dy.Init(fin);
    Object::Init(variable, fin);
}

bool Plane::Intersect(Vector3 ray_O, Vector3 ray_V)
{
    ray_V = ray_V.Normalize();
    N = N.Normalize();
    double d = N.Dot(ray_V);
    if(fabs(d) < EPS) return false;
    double l = (N * R - ray_O).Dot(N) / d;
    if(l < EPS) return false;

    crash.dist = l;
    crash.front = (d < 0);
    crash.C = ray_O + ray_V * crash.dist;
    crash.N = (crash.front) ? N : -N;
    return true;
}

Color Plane::GetTexture()
{
    double u = crash.C.Dot(Dx) / Dx.Module2();
    double v = crash.C.Dot(Dy) / Dy.Module2();
    return material->texture->GetSmoothColor(u, v);
}

Object* Plane::ObjectCopy()
{
    Plane* ans = new Plane(*this);
    return ans;
}

Sphere::Sphere() : Object()
{
	De = Vector3(0, 0, 1);
	Dc = Vector3(0, 1, 0);
}

void Sphere::Init(std::string variable, std::stringstream& fin)
{
	if(variable == "O=") O.Init(fin);
	if(variable == "R=") fin >> R;
	if(variable == "De=") De.Init(fin);
	if(variable == "Dc=") Dc.Init(fin);
	Object::Init(variable, fin);
}

bool Sphere::Intersect(Vector3 ray_O, Vector3 ray_V)
{
	ray_V = ray_V.Normalize();
	Vector3 P = ray_O - O;
	double b = -P.Dot(ray_V);
	double det = b * b - P.Module2() + R * R;

	if(det > EPS)
    {
		det = sqrt(det);
		double x1 = b - det , x2 = b + det;

		if(x2 < EPS) return false;
		if(x1 > EPS) 
        {
			crash.dist = x1;
			crash.front = true;
		} 
        else
        {
			crash.dist = x2;
			crash.front = false;
		} 
	} 
    else return false;

	crash.C = ray_O + ray_V * crash.dist;
	crash.N = (crash.C - O).Normalize();
	if(crash.front == false) crash.N = -crash.N;
	return true;
}

Color Sphere::GetTexture()
{
	Vector3 I = (crash.C - O).Normalize();
	double a = acos(-I.Dot(De));
	double b = acos(std::min(std::max(I.Dot(Dc) / sin(a), -1.0), 1.0));
	double u = a / PI, v = b / 2 / PI;
	if(I.Dot(Dc * De) < 0) v = 1 - v;
	return material->texture->GetSmoothColor(u, v);
}

Object* Sphere::ObjectCopy()
{
	Sphere* ans = new Sphere(*this);
	return ans;
}

struct Node
{
    Node(const Eigen::Vector3d* controlPoints_, int coeff_u_, int coeff_v_, int step_)
    {
        for(int i = 0; i < 16; ++i)
        {
            controlPoints[i] = controlPoints_[i];
        }
        box.Init(controlPoints);
        coeff_u = coeff_u_;
        coeff_v = coeff_v_;
        step = step_;
    }
    Eigen::Vector3d controlPoints[16];
    AABB box;
    int coeff_u, coeff_v;
    int step;
};

Bezier::Bezier() : Object()
{
	controlPoints[0] = Vector3d(2, 0, 4);                                            
	controlPoints[1] = Vector3d(2, 0, 4);                                                     
	controlPoints[2] = Vector3d(2, 0, 4);                                                     
	controlPoints[3] = Vector3d(2, 0, 4);                                                     
	controlPoints[4] = Vector3d(2, 0, 4);                                                    
	controlPoints[5] = Vector3d(2, 3, 3);                                                     
	controlPoints[6] = Vector3d(2, -1, 3);                                                     
	controlPoints[7] = Vector3d(2, 0, 4);                                                     
	controlPoints[8] = Vector3d(2, 0, 4);                                                     
	controlPoints[9] = Vector3d(0, -1, 3);                                                     
	controlPoints[10] = Vector3d(2, 3, 3);                                                     
	controlPoints[11] = Vector3d(2, 0, 4);                                                     
	controlPoints[12] = Vector3d(2, 0, 4);                                                     
	controlPoints[13] = Vector3d(2, 0, 4);                                                     
	controlPoints[14] = Vector3d(2, 0, 4);                                                     
	controlPoints[15] = Vector3d(2, 0, 4);
}

Vector3d Bezier::evalBezierCurve(const Vector3d *P, const double &t)
{
    double b0 = (1 - t) * (1 - t) * (1 - t);
    double b1 = 3 * t * (1 - t) * (1 - t);
    double b2 = 3 * t * t * (1 - t);
    double b3 = t * t * t;
    return P[0] * b0 + P[1] * b1 + P[2] * b2 + P[3] * b3;
}

Vector3d Bezier::evalBezierPatch(const Vector3d *controlPoints, const double &u, const double &v)
{
    Eigen::Vector3d uCurve[4];
    for(int i = 0; i < 4; ++i)
        uCurve[i] = evalBezierCurve(controlPoints + 4 * i, u);
    return evalBezierCurve(uCurve, v);
}

Vector3d Bezier::derivBezier(const Vector3d *P, const double &t)
{
    double b0 = -3 * (1 - t) * (1 - t);
    double b1 = 3 * (1 - t) * (1 - t) - 6 * t * (1 - t);
    double b2 = 6 * t * (1 - t) - 3 * t * t;
    double b3 = 3 * t * t;
    return P[0] * b0 + P[1] * b1 + P[2] * b2 + P[3] * b3;
}

Vector3d Bezier::dUBezier(const Vector3d *controlPoints, const double &u, const double &v)
{
    Vector3d P[4];
    Vector3d vCurve[4];
    for(int i = 0; i < 4; ++i) 
    {
        P[0] = controlPoints[i];
        P[1] = controlPoints[4 + i];
        P[2] = controlPoints[8 + i];
        P[3] = controlPoints[12 + i];
        vCurve[i] = evalBezierCurve(P, v);
    }
    return derivBezier(vCurve, u);
}

Vector3d Bezier::dVBezier(const Vector3d *controlPoints, const double &u, const double &v)
{
    Vector3d uCurve[4];
    for(int i = 0; i < 4; ++i) 
    {
        uCurve[i] = evalBezierCurve(controlPoints + 4 * i, u);
    }
    return derivBezier(uCurve, v);
}

void Bezier::Init(std::string variable, std::stringstream& fin)
{
	Object::Init(variable, fin);
}

bool Bezier::Intersect(Vector3 ray_O, Vector3 ray_V)
{
	const double eps = 1e-2;
    Vector3d rayO = Vector3d(ray_O.x, ray_O.y, ray_O.z);
    Vector3d rayV = Vector3d(ray_V.x, ray_V.y, ray_V.z);
    rayV.normalize();
    std::queue<Node> q;
    Node root(controlPoints, 1, 1, 0);
    q.push(root);
    while(!q.empty())
    {
        Node front = q.front();
        if(front.box.Distance() < eps) break;
        q.pop();
        Eigen::Vector3d *newpointslist = SplitIPatch(front.controlPoints);
        Node b00(newpointslist, 1, 1, 0);
        if(b00.box.Intersect(rayO, rayV)) 
            q.push(b00);
        Node b01(newpointslist + 16, 1, 1, 0);
        if(b01.box.Intersect(rayO, rayV)) 
            q.push(b01);
        Node b10(newpointslist + 32, 1, 1, 0);
        if(b10.box.Intersect(rayO, rayV)) 
            q.push(b10);
        Node b11(newpointslist + 48, 1, 1, 0);
        if(b11.box.Intersect(rayO, rayV)) 
            q.push(b11);
        delete[] newpointslist;
    }

    const double eps1 = 1e-6;
    const double eps2 = 1e-3;
    Eigen::Vector3d min_x(-1, -1, -1);
    double min_t = RAND_MAX;
    Node min_node(controlPoints, 1, 1, 1);
    while(!q.empty())
    {
        Node front = q.front();
        q.pop();
        Eigen::Vector3d x(0, 0.5, 0.5);
        Eigen::Vector3d x_last(-1, -1, -1);
        int cnt = 0;
        while((x - x_last).lpNorm<Eigen::Infinity>() > eps1 && cnt < 50)
        {
            x_last = x;
            Eigen::Vector3d L = rayO + x_last(0) * rayV;
            Eigen::Vector3d P = evalBezierPatch(front.controlPoints, x_last(1), x_last(2));
            Eigen::Vector3d F = L - P;
            Eigen::Vector3d dU = dUBezier(front.controlPoints, x_last(1), x_last(2));
            Eigen::Vector3d dV = dVBezier(front.controlPoints, x_last(1), x_last(2));
            Eigen::Matrix3d J;
            J(0) = rayV(0), J(1) = rayV(1), J(2) = rayV(2);
            J(3) = -dU(0), J(4) = -dU(1), J(5) = -dU(2);
            J(6) = -dV(0), J(7) = -dV(1), J(8) = -dV(2);
            x = x_last - J.inverse() * F;
            cnt++;
        }
        if(cnt < 50 && x(0) > eps2 && x(1) >= 0 - eps2 && x(1) <= 1 + eps2 && x(2) >= 0 - eps2 && x(2) <= 1 + eps2 && x(0) < min_t)
        {
            min_t = x(0);
            min_x = x;
            min_node = front;
        }
    }
    if(min_x(0) < eps2 || min_x(1) < 0 - eps2 || min_x(1) > 1 + eps2 || min_x(2) < 0 - eps2 || min_x(2) > 1 + eps2)
        return false;
    Eigen::Vector3d dU = dUBezier(min_node.controlPoints, min_x(1), min_x(2));
    Eigen::Vector3d dV = dVBezier(min_node.controlPoints, min_x(1), min_x(2));
    double distance = min_x(0);
    bool inside = false;
    Eigen::Vector3d N = dU.cross(dV);
    N.normalize();
    if(N.dot(rayV) > 0) N = (-1) * N;
    Vector3 Ntmp = Vector3(N(0), N(1), N(2));
    Vector3 P = ray_O + ray_V * distance;
    crash.C = P;
    crash.N = Ntmp;
    crash.dist = distance;
    crash.front = !inside;
    return true;
}


Color Bezier::GetTexture()
{
	return Color(1, 0, 0);
}

Object* Bezier::ObjectCopy()
{
	Bezier* ans = new Bezier(*this);
	return ans;
}

Eigen::Vector3d* Bezier::SplitIPatch( const Eigen::Vector3d* p )
{
    Eigen::Vector3d* iPatches;
    iPatches = new Eigen::Vector3d[64];
    Eigen::Vector3d q[16];
    Eigen::Vector3d r[16];
    Eigen::Vector3d q1[16];
    Eigen::Vector3d r1[16];
    Eigen::Vector3d q2[16];
    Eigen::Vector3d r2[16];

    for(int iv = 0; iv < 4; ++iv)
    {
        Eigen::Vector3d p0_ = p[iv] ;
        q[iv] = p0_;
        Eigen::Vector3d p1_ = p[4 + iv ] ;
        Eigen::Vector3d q1_ = ( ( p0_ + p1_ ) / 2 );
        q[4 + iv] = q1_;
        Eigen::Vector3d p2_ = p[8 + iv] ;
        Eigen::Vector3d q2_ = ( q1_  / 2 ) + ( ( p1_ + p2_ ) / 4 );
        q[8 + iv] = q2_;
        Eigen::Vector3d p3_ = p[12 + iv ] ;
        r[12 + iv] = p3_;
        Eigen::Vector3d r2_ = ( p2_ + p3_ ) / 2;
        r[8 + iv] = r2_;
        Eigen::Vector3d r1_ = ( r2_ / 2 ) + ( ( p1_ + p2_ ) / 4 );
        r[4 + iv] = r1_;
        Eigen::Vector3d q3_ = ( q2_ + r1_ ) / 2 ;
        q[12 + iv] = q3_;
        r[iv] = q3_;
    }
    for(int iv = 0; iv < 4; ++iv)
    {
        Eigen::Vector3d p0_ = q[4 * iv] ;
        q1[4 * iv] = p0_;
        Eigen::Vector3d p1_ = q[4 * iv + 1] ;
        Eigen::Vector3d q1_ = ( ( p0_ + p1_ ) / 2 );
        q1[4 * iv + 1] =  q1_;
        Eigen::Vector3d p2_ = q[4 * iv + 2] ;
        Eigen::Vector3d q2_ = ( q1_  / 2 ) + ( ( p1_ + p2_ ) / 4 );
        q1[4 * iv + 2] = q2_;
        Eigen::Vector3d p3_ = q[4 * iv + 3] ;
        q2[4 * iv + 3] = p3_;
        Eigen::Vector3d r2_ = ( p2_ + p3_ ) / 2;
        q2[4 * iv + 2] = r2_;
        Eigen::Vector3d r1_ = ( r2_ / 2 ) + ( ( p1_ + p2_ ) / 4 );
        q2[4 * iv + 1] =  r1_;
        Eigen::Vector3d q3_ = ( q2_ + r1_ ) / 2 ;
        q1[4 * iv + 3] =  q3_;
        q2[4 * iv] = q3_;
    }
    for(int iv = 0; iv < 4; ++iv)
    {
        Eigen::Vector3d p0_ = r[4 * iv] ;
        r1[4 * iv] = p0_;
        Eigen::Vector3d p1_ = r[4 * iv + 1] ;
        Eigen::Vector3d q1_ = ( ( p0_ + p1_ ) / 2 );
        r1[4 * iv + 1] =  q1_;
        Eigen::Vector3d p2_ = r[4 * iv + 2] ;
        Eigen::Vector3d q2_ = ( q1_  / 2 ) + ( ( p1_ + p2_ ) / 4 );
        r1[4 * iv + 2] = q2_;
        Eigen::Vector3d p3_ = r[4 * iv + 3] ;
        r2[4 * iv + 3] = p3_;
        Eigen::Vector3d r2_ = ( p2_ + p3_ ) / 2;
        r2[4 * iv + 2] = r2_;
        Eigen::Vector3d r1_ = ( r2_ / 2 ) + ( ( p1_ + p2_ ) / 4 );
        r2[4 * iv + 1] =  r1_;
        Eigen::Vector3d q3_ = ( q2_ + r1_ ) / 2 ;
        r1[4 * iv + 3] =  q3_;
        r2[4 * iv] = q3_;
    }
    for(int i = 0; i < 16; ++i)
    {
        iPatches[i] = q1[i];
        iPatches[16 + i] = q2[i];
        iPatches[32 + i] = r1[i];
        iPatches[48 + i] = r2[i];
    }
    return iPatches;
}
