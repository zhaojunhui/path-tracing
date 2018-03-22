#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <fstream>
#include <sstream>
#include "color.h"
#include "vector3.h"
#include "object.h"
#include "light.h"
#include "camera.h"

class Scene
{
public:
	Scene();
	~Scene();
	
	Object* GetObjectHead() { return object_head; }
	Light* GetLightHead() { return light_head; }
	Color GetBackgroundColor() { return background_color; }
	Camera* GetCamera() { return camera; }

	void CreateScene(std::string filename);
	Object* FindNearestObject(Vector3 ray_O, Vector3 ray_V);
	Light* FindNearestLight(Vector3 ray_O, Vector3 ray_V);
private:
    Object* object_head;
	Light* light_head;
	Color background_color;
	Camera* camera;
};

#endif
