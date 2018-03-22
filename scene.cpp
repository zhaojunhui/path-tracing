#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include "scene.h"

Scene::Scene()
{
	object_head = NULL;
	light_head = NULL;
	background_color = Color();
	camera = new Camera;
}

Scene::~Scene()
{
	while(object_head != NULL)
	{
		Object* next_head = object_head->GetNext();
		if(object_head->GetMaterial()->texture != NULL)
			delete object_head->GetMaterial()->texture;
		delete object_head;
		object_head = next_head;
	}

	while(light_head != NULL)
	{
		Light* next_head = light_head->GetNext();
		delete light_head;
		light_head = next_head;
	}

	delete camera;
}

void Scene::CreateScene(std::string filename)
{
	srand(time(NULL));
	std::ifstream fin(filename.c_str());

	std::string obj;
	while(fin >> obj)
	{
		Object* new_object = NULL;
		Light* new_light = NULL;

		if(obj == "object")
		{
			std::string type; fin >> type;
			if(type == "sphere") new_object = new Sphere;
			if(type == "plane") new_object = new Plane;
			if(type == "bezier") new_object = new Bezier;
			if(new_object != NULL)
			{
				new_object->SetNext(object_head);
				object_head = new_object;
			}
		} 
		else if(obj == "light") 
		{
			std::string type; fin >> type;
			if(type == "point") new_light = new PointLight;
			if(type == "area") new_light = new AreaLight;
			if(new_light != NULL)
			{
				new_light->SetNext(light_head);
				light_head = new_light;
			}
		}
		else if(obj != "background" && obj != "camera") continue;

		fin.ignore(1024, '\n');
		
		std::string order;
		while(getline(fin, order, '\n'))
		{
			std::stringstream fin2(order);
			std::string var; fin2 >> var;
			if(var == "end") break;

			if(obj == "background" && var == "color=") background_color.Init(fin2);
			if(obj == "object" && new_object != NULL) new_object->Init(var, fin2);
			if(obj == "light" && new_light != NULL) new_light->Init(var, fin2);
			if(obj == "camera") camera->Init(var, fin2);
		}
	}

	camera->Initialize();
}

Object* Scene::FindNearestObject(Vector3 ray_O, Vector3 ray_V)
{
	Object* ans = NULL;

	for(Object* tmp = object_head; tmp != NULL; tmp = tmp->GetNext())
		if(tmp->Intersect(ray_O, ray_V) && (ans == NULL || tmp->crash.dist < ans->crash.dist)) ans = tmp;

	return ans;
}

Light* Scene::FindNearestLight(Vector3 ray_O, Vector3 ray_V)
{
	Light* ans = NULL;

	for(Light* tmp = light_head; tmp != NULL; tmp = tmp->GetNext())
		if(tmp->Intersect(ray_O, ray_V) && (ans == NULL || tmp->crash_dist < ans->crash_dist)) ans = tmp;

	return ans;
}
