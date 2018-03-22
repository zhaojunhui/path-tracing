#include "png.h"
#include "lodepng.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>

using namespace std;

Png::Png(int h, int w)
{
	Initialize(h, w);
}

Png::~Png() {
	Release();
}

void Png::Initialize(int h, int w) 
{
	H = h;
	W = w;
	image = new ColorUC*[H];
	for(int i = 0; i < H; ++i)
		image[i] = new ColorUC[W];
}

void Png::Release()
{
	for(int i = 0; i < H; ++i)
		delete[] image[i];

	delete[] image;
}

void Png::Init(std::string filename)
{
	Release();
	unsigned h, w;
	std::vector<uchar> img;
	lodepng::decode(img, w, h, filename);
	H = h;
	W = w;
	Initialize(H, W);
	for(int i = 0; i < H; ++i)
		for(int j = 0; j < W; ++j)
		{
			image[i][j].r = img[i * W * 4 + j * 4 + 0];
			image[i][j].g = img[i * W * 4 + j * 4 + 1];
			image[i][j].b = img[i * W * 4 + j * 4 + 2];
		}
}

void Png::Output(std::string filename)
{
	std::vector<uchar> img;
	for(int i = 0; i < H; ++i)
		for(int j = 0; j < W; ++j)
		{
			img.push_back(image[H - i - 1][j].r);
			img.push_back(image[H - i - 1][j].g);
			img.push_back(image[H - i - 1][j].b);
			img.push_back(255);
		}
	lodepng::encode(filename, img, W, H);
}

void Png::SetColor(int i, int j, Color col)
{
	image[i][j].r = (int) (col.r * 255);
	image[i][j].g = (int) (col.g * 255);
	image[i][j].b = (int) (col.b * 255);
}

Color Png::GetSmoothColor(double u, double v)
{
	double U = (u - floor(u)) * H;
	double V = (v - floor(v)) * W;
	int U1 = (int) floor(U + EPS), U2 = U1 + 1;
	int V1 = (int) floor(V + EPS), V2 = V1 + 1;
	double rat_U = U2 - U;
	double rat_V = V2 - V;
	if(U1 < 0) U1 = H - 1; if(U2 == H) U2 = 0;
	if(V1 < 0) V1 = W - 1; if(V2 == W) V2 = 0;
	Color ans;
	ans = ans + GetColor(U1, V1) * rat_U * rat_V;
	ans = ans + GetColor(U1, V2) * rat_U * (1 - rat_V);
	ans = ans + GetColor(U2, V1) * (1 - rat_U) * rat_V;
	ans = ans + GetColor(U2, V2) * (1 - rat_U) * (1 - rat_V);
	return ans;
}
