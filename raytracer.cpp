#include <cstdlib>
#include <iostream>
#include "raytracer.h"

const double SPEC_POWER = 20; 
const int MAX_DREFL_DEP = 2;
const int MAX_RAYTRACING_DEP = 10;
const int HASH_FAC = 7;
const int HASH_MOD = 10000007;

double Raytracer::Rand01d()
{
	return double(rand() % 65535) / 65535;
}

Color Raytracer::CalDiffusion(Object* obj, int* hash) 
{
	Color color = obj->GetMaterial()->color; 
	if(obj->GetMaterial()->texture != NULL) color = color * obj->GetTexture(); 
	
	Color ans = color * scene.GetBackgroundColor() * obj->GetMaterial()->diff; 

	for(Light* light = scene.GetLightHead(); light != NULL; light = light->GetNext()) 
	{
		double shade = light->CalShade(obj->crash.C, scene.GetObjectHead(), scene.GetCamera()->GetShadeQuality());
		if (shade < EPS) continue;
		
		Vector3 R = (light->GetO() - obj->crash.C).Normalize(); 
		double dot = R.Dot(obj->crash.N);
		if(dot > EPS) 
		{
			if(hash != NULL && light->IsPointLight()) *hash = (*hash + light->GetSample()) & HASH_MOD;

			if(obj->GetMaterial()->diff > EPS) 
			{
				double diff = obj->GetMaterial()->diff * dot * shade;
				ans += color * light->GetColor() * diff;
			}
			if(obj->GetMaterial()->spec > EPS) 
			{
				double spec = obj->GetMaterial()->spec * pow(dot, SPEC_POWER) * shade;
				ans += color * light->GetColor() * spec;
			}
		}
	}
	return ans;
}

Color Raytracer::CalReflection(Object* obj, Vector3 ray_V, int dep, int* hash) 
{
	ray_V = ray_V.Reflect(obj->crash.N);

	if(obj->GetMaterial()->drefl < EPS || dep > MAX_DREFL_DEP)
		return RayTracing(obj->crash.C, ray_V, dep + 1, hash) * obj->GetMaterial()->color * obj->GetMaterial()->refl;

	Vector3 Dx = ray_V * Vector3(1, 0, 0);
	if(Dx.IsZeroVector()) Dx = Vector3(1, 0, 0);
	Vector3 Dy = ray_V * Dx;
	Dx = Dx.Normalize() * obj->GetMaterial()->drefl;
	Dy = Dy.Normalize() * obj->GetMaterial()->drefl;

	Color ans;
	for(int k = 0; k < 16 * scene.GetCamera()->GetDreflQuality(); ++k) 
	{
		double x = 1, y = 1;
		while(x * x + y * y > 1)
		{
			x = Rand01d() * 2 - 1;
			y = Rand01d() * 2 - 1;
		}
		x *= obj->GetMaterial()->drefl;
		y *= obj->GetMaterial()->drefl;

		ans += RayTracing(obj->crash.C, ray_V + Dx * x + Dy * y, dep + MAX_DREFL_DEP, NULL);
	}

	ans = ans * obj->GetMaterial()->color * obj->GetMaterial()->refl / (16 * scene.GetCamera()->GetDreflQuality());
	return ans;
}

Color Raytracer::CalRefraction(Object* obj, Vector3 ray_V, int dep, int* hash) 
{
	double n = obj->GetMaterial()->rindex;
	if(obj->crash.front) n = 1 / n;
	
	ray_V = ray_V.Refract(obj->crash.N, n);
	
	Color rcol = RayTracing(obj->crash.C, ray_V, dep + 1, hash);
	if(obj->crash.front) return rcol * obj->GetMaterial()->refr;
	Color absor = obj->GetMaterial()->absor * -obj->crash.dist;
	Color trans = Color(exp(absor.r), exp(absor.g), exp(absor.b));
	return rcol * trans * obj->GetMaterial()->refr;
}


Color Raytracer::RayTracing(Vector3 ray_O, Vector3 ray_V, int dep, int* hash) 
{
	if(dep > MAX_RAYTRACING_DEP) return Color();

	Color ans;
	Object* nearest_object = scene.FindNearestObject(ray_O, ray_V);
	Light* nearest_light = scene.FindNearestLight(ray_O, ray_V);

	if(nearest_light != NULL && (nearest_object == NULL || nearest_light->crash_dist < nearest_object->crash.dist))
	{
		if(hash != NULL) *hash = (*hash + nearest_light->GetSample()) % HASH_MOD;
		ans += nearest_light->GetColor();
	}

	if (nearest_object != NULL)
	{
		if(hash != NULL) *hash = (*hash + nearest_object->GetSample()) % HASH_MOD;
		Object* object = nearest_object->ObjectCopy();
		if(object->GetMaterial()->diff > EPS || object->GetMaterial()->spec > EPS) ans += CalDiffusion(object, hash);
		if(object->GetMaterial()->refl > EPS) ans += CalReflection(object, ray_V, dep, hash);
		if(object->GetMaterial()->refr > EPS) ans += CalRefraction(object, ray_V, dep, hash);
		delete object;
	}

	if(hash != NULL) *hash = (*hash * HASH_FAC) % HASH_MOD;
	ans.Confine();
	return ans;
}

void Raytracer::Render() 
{
	Camera* camera = scene.GetCamera(); 
	scene.CreateScene(input);

	Vector3 ray_O = camera->GetO(); 
	int H = camera->GetH(), W = camera->GetW();
	int** sample = new int*[H];
	for (int i = 0; i < H; ++i)
	{
		sample[i] = new int[W];
		for (int j = 0 ; j < W ; ++j)
			sample[i][j] = 0;
	}

	for (int i = 0; i < H; std::cout << ++i << "/" << H << std::endl)
		for (int j = 0; j < W; ++j)
		{
			Vector3 ray_V = camera->Emit(i, j);
			Color color = RayTracing(ray_O, ray_V, 1, &sample[i][j]);
			camera->SetColor(i, j, color);
		}

	printf("超采样抗锯齿中...\n");
	for (int i = 0; i < H; std::cout << ++i << "/" << H << std::endl)
		for (int j = 0; j < W; ++j)
		{
			if ((i == 0 || sample[i][j] == sample[i - 1][j]) && (i == H - 1 || sample[i][j] == sample[i + 1][j]) &&
			     (j == 0 || sample[i][j] == sample[i][j - 1]) && (j == W - 1 || sample[i][j] == sample[i][j + 1])) continue;

			Color color;
			for (int r = -1; r <= 1; ++r)
				for (int c = -1; c <= 1; ++c)
				{
					Vector3 ray_V = camera->Emit(i + (double) r / 3, j + (double) c / 3);
					color += RayTracing(ray_O, ray_V, 1, NULL) / 9;
				}
			camera->SetColor(i, j, color);
		}
	
	for (int i = 0; i < H; ++i)
		delete[] sample[i];
	delete[] sample;

	Png* png;
	camera->Output(png);
	png->Output(output);
	delete png;
}
