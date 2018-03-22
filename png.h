#ifndef Png_H
#define Png_H

#include "color.h"
#include <string>

extern const double EPS;

typedef unsigned char uchar;

struct ColorUC
{
	uchar r, g, b;
};

class Png
{	
public:
	Png(int h = 0, int w = 0);
	~Png();

	int GetH() { return H; }
	int GetW() { return W; }
	Color GetColor(int i, int j) { return Color(image[i][j].r, image[i][j].g, image[i][j].b) / 255; }
	void SetColor(int i, int j, Color);

	void Init(std::string);
	void Output(std::string);
	Color GetSmoothColor(double u, double v);
private:
	ColorUC** image;
	unsigned H, W;


	void Initialize(int h, int w);
	void Release();
};

#endif
