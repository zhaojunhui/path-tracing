#ifndef LIGHT_H
#define LIGHT_H

#include "vector3.h"
#include "color.h"
#include "object.h"
#include <sstream>
#include <string>
#include <cmath>

extern const double EPS;

class Light
{
public:
	double crash_dist;

	double Rand01d();

	Light();
	~Light() {}
	
	virtual bool IsPointLight() = 0;
	int GetSample() { return sample; }
	Color GetColor() { return color; }
	Light* GetNext() { return next; }
	void SetNext(Light* light) { next = light; }

	virtual void Init(std::string, std::stringstream&);
	virtual Vector3 GetO() = 0;
	virtual bool Intersect(Vector3 ray_O, Vector3 ray_V) = 0;
	virtual double CalShade(Vector3 C, Object* object_head, int shade_quality) = 0;
protected:
	int sample;
	Color color;
	Light* next;
};

class PointLight : public Light
{
public:
	PointLight() : Light() {}
	~PointLight() {}
	
	bool IsPointLight() { return true; }
	Vector3 GetO() { return O; }
	void Init(std::string, std::stringstream&);
	bool Intersect(Vector3 ray_O, Vector3 ray_V);
	double CalShade(Vector3 C, Object* object_head, int shade_quality);
private:
	Vector3 O;
};

class AreaLight : public Light
{
public:
	AreaLight() : Light() {}
	~AreaLight() {}
	
	bool IsPointLight() { return false; }
	Vector3 GetO() { return O; }
	void Init(std::string, std::stringstream&);
	bool Intersect(Vector3 ray_O, Vector3 ray_V);
	double CalShade(Vector3 C, Object* object_head, int shade_quality);
private:
	Vector3 O, Dx, Dy;
};

#endif
