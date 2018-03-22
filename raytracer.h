#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <string>
#include "scene.h"
#include "png.h"

extern const double SPEC_POWER;
extern const int MAX_DREFL_DEP;
extern const int MAX_RAYTRACING_DEP;
extern const int HASH_FAC;
extern const int HASH_MOD;

class Raytracer {
public:
	Raytracer() {}
	~Raytracer() {}
	
	void SetInput(std::string filename) { input = filename; }
	void SetOutput(std::string filename) { output = filename; }
	void Render();
private:
	std::string input, output;
	Scene scene;
	double Rand01d();
	Color CalDiffusion(Object* obj, int* hash);
	Color CalReflection(Object* obj, Vector3 ray_V, int dep, int* hash);
	Color CalRefraction(Object* obj, Vector3 ray_V, int dep, int* hash);
	Color RayTracing(Vector3 ray_O, Vector3 ray_V, int dep, int* hash);
};

#endif
