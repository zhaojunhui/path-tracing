#include "light.h"
#include <sstream>
#include <string>
#include <cmath>
#include <cstdlib>

 double Light::Rand01d()
{
	return double(rand() % 65535) / 65535;
}

Light::Light()
{
	sample = rand();
	next = NULL;
}

void Light::Init(std::string variable, std::stringstream& fin)
{
	if(variable == "color=") color.Init(fin);
}

void PointLight::Init(std::string variable, std::stringstream& fin)
{
	if(variable == "O=") O.Init(fin);
	Light::Init(variable, fin);
}

bool PointLight::Intersect(Vector3 ray_O, Vector3 ray_V)
{
	return false;
}

double PointLight::CalShade(Vector3 C, Object* object_head, int shade_quality)
{
	Vector3 V = O - C;
	double dist = V.Module();

	for(Object* tmp = object_head; tmp != NULL; tmp = tmp->GetNext())
		if(tmp->Intersect(C, V) && (tmp->crash.dist < dist)) return 0;

	return 1;
}

void AreaLight::Init(std::string variable, std::stringstream& fin)
{
	if(variable == "O=") O.Init(fin);
	if(variable == "Dx=") Dx.Init(fin);
	if(variable == "Dy=") Dy.Init(fin);
	Light::Init(variable, fin);
}

bool AreaLight::Intersect(Vector3 ray_O, Vector3 ray_V)
{
	ray_V = ray_V.Normalize();
	Vector3 N = (Dx * Dy).Normalize();
	double d = N.Dot(ray_V);
	if(fabs(d) < EPS) return false;
	double l = (N * O.Dot(N) - ray_O).Dot(N) / d;
	if(l < EPS) return false;

	Vector3 C = (ray_O + ray_V * l) - O;
	if(fabs(Dx.Dot(C)) > Dx.Dot(Dx)) return false;
	if(fabs(Dy.Dot(C)) > Dy.Dot(Dy)) return false;

	crash_dist = l;
	return true;
}

double AreaLight::CalShade(Vector3 C, Object* object_head, int shade_quality)
{
	int shade = 0;
	
	for(int i = -2; i < 2; ++i)
		for(int j = -2; j < 2; ++j)
			for(int k = 0; k < shade_quality; ++k)
			{
				Vector3 V = O - C + Dx * ((Rand01d() + i) / 2) + Dy * ((Rand01d() + j) / 2);
				double dist = V.Module();

				for(Object* tmp = object_head; tmp != NULL; tmp = tmp->GetNext())
					if(tmp->Intersect(C, V) && (tmp->crash.dist < dist))
					{
						shade++;
						break;
					}
			}
	
	return 1 - (double) shade / (16.0 * shade_quality);
}
