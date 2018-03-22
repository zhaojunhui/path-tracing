#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
#include "camera.h"

const double STD_LENS_WIDTH = 0.88;
const double STD_LENS_HEIGHT = 0.88;
const int STD_IMAGE_WIDTH = 420;
const int STD_IMAGE_HEIGHT = 420;
const int STD_SHADE_QUALITY = 4;
const int STD_DREFL_QUALITY = 4;

Camera::Camera()
{
	O = Vector3(0, 0, 0);
	N = Vector3(0, 1, 0);
	lens_W = STD_LENS_WIDTH;
	lens_H = STD_LENS_HEIGHT;
	W = STD_IMAGE_WIDTH;
	H = STD_IMAGE_HEIGHT;
	shade_quality = STD_SHADE_QUALITY;
	drefl_quality = STD_DREFL_QUALITY;
	data = NULL;
}

Camera::~Camera()
{
	if(data == NULL)
	{
		for(int i = 0; i < H; ++i)
			delete[] data[i];
		delete[] data;
	}
}

void Camera::Initialize()
{
	N = N.Normalize();
	Dx = N.GetAnVerticalVector();
	Dy = Dx * N;
	Dx = Dx * lens_W / 2;
	Dy = Dy * lens_H / 2;

	data = new Color*[H];
	for(int i = 0; i < H; ++i)
		data[i] = new Color[W];
}

Vector3 Camera::Emit(double i, double j)
{
	return N + Dy * (2 * i / H - 1) + Dx * (2 * j / W - 1);
}

void Camera::Init(std::string variable, std::stringstream& fin)
{
	if(variable == "O=") O.Init(fin);
	if(variable == "N=") N.Init(fin);
	if(variable == "lens_W=") fin >> lens_W;
	if(variable == "lens_H=") fin >> lens_H;
	if(variable == "image_W=") fin >> W;
	if(variable == "image_H=") fin >> H;
	if(variable == "shade_quality=") fin >> shade_quality;
	if(variable == "drefl_quality=") fin >> drefl_quality;

}

void Camera::Output(Png *&png)
{
	png = new Png(H, W);

	for(int i = 0; i < H; ++i)
		for(int j = 0; j < W; ++j)
			png->SetColor(i, j, data[i][j]);
}
