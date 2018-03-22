#include "raytracer.h"

int main() 
{
	Raytracer* raytracer = new Raytracer;
	raytracer->SetInput("scene.txt");
	raytracer->SetOutput("picture.png");
	raytracer->Render();
	return 0;
}
