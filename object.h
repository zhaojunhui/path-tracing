#ifndef OBJECT_H
#define OBJECT_H

#include <eigen3/Eigen/Dense>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "color.h"
#include "vector3.h"
#include "png.h"

extern const double EPS;
extern const double PI;

class Material
{
public:
	Color color, absor;
	double refl, refr;
	double diff, spec;
	double rindex;
	double drefl;
	Png* texture;

	Material();
	~Material() {}

	void Init(std::string, std::stringstream&);
};

class Crash
{
public:
	Vector3 N, C;
	double dist;
	bool front;
};

class Object
{
public:
	Crash crash;

	Object();
	Object(const Object&);
	~Object();
	
	int GetSample() { return sample; }
	Material* GetMaterial() { return material; }
	Object* GetNext() { return next; }
	void SetNext(Object* obj) { next = obj; }

	virtual void Init(std::string, std::stringstream&);
	virtual bool Intersect(Vector3 ray_O, Vector3 ray_V) = 0;
	virtual Color GetTexture() = 0;
	virtual Object* ObjectCopy() = 0;

protected:
	int sample;
	Material* material;
	Object* next;
};

class Plane : public Object
{
public:
	Plane() : Object() {}
	~Plane() {}

	void Init(std::string, std::stringstream&);
	bool Intersect(Vector3 ray_O, Vector3 ray_V);
	Color GetTexture();
	Object* ObjectCopy();
private:
	Vector3 N, Dx, Dy;
	double R;
};

class Sphere : public Object
{
public:
	Sphere();
	~Sphere() {}

	void Init(std::string, std::stringstream&);
	bool Intersect(Vector3 ray_O, Vector3 ray_V);
	Color GetTexture();
	Object* ObjectCopy();
private:
	Vector3 O, De, Dc;
	double R;
};

class Bezier : public Object
{
public:
	Bezier();
	~Bezier() {}

	Eigen::Vector3d evalBezierCurve(const Eigen::Vector3d *P, const double &t);
	Eigen::Vector3d evalBezierPatch(const Eigen::Vector3d *controlPoints, const double &u, const double &v);
	Eigen::Vector3d derivBezier(const Eigen::Vector3d *P, const double &t);
	Eigen::Vector3d dUBezier(const Eigen::Vector3d *controlPoints, const double &u, const double &v);
	Eigen::Vector3d dVBezier(const Eigen::Vector3d *controlPoints, const double &u, const double &v);

	Eigen::Vector3d* SplitIPatch( const Eigen::Vector3d* p );

	void Init(std::string, std::stringstream&);
	bool Intersect(Vector3 ray_O, Vector3 ray_V);
	Color GetTexture();
	Object* ObjectCopy();
private:
	Eigen::Vector3d controlPoints[16];
};

#endif
