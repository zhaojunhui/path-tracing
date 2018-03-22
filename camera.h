#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <sstream>
#include "vector3.h"
#include "color.h"
#include "png.h"

extern const double STD_LENS_WIDTH;
extern const double STD_LENS_HEIGHT;
extern const int STD_IMAGE_WIDTH;
extern const int STD_IMAGE_HEIGTH;
extern const int STD_SHADE_QUALITY;
extern const int STD_DREFL_QUALITY;

class Camera
{
public:
	Camera();
	~Camera();
	
	Vector3 GetO() { return O; }
	int GetW() { return W; }
	int GetH() { return H; }
	void SetColor(int i, int j, Color color) { data[i][j] = color; }
	double GetShadeQuality() { return shade_quality; }
	double GetDreflQuality() { return drefl_quality; }

	Vector3 Emit(double i, double j);
	void Initialize();
	void Init(std::string, std::stringstream&);
	void Output(Png*&);
private:
	Vector3 O, N, Dx, Dy;
	double lens_W, lens_H;
	int W, H;
	Color** data;
	double shade_quality;
	double drefl_quality;
};

#endif
