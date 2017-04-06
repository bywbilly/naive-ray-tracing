#ifndef H_SCENE_
#define H_SCENE_

#include "common.hpp"
#include "object.hpp"
#include "triangle.hpp"
#include "ray.hpp"
#include "light.hpp"
#include "jsonxx.h"
#include <vector>
using std::vector;

class Scene{
    public:
        jsonxx::Object json;
        vector<Object*> objects;
        vector<Light*> lights;
		vector<Light*> sampled_lights;

        Object* nearest_intersect (ray_trans &h);
        Color phong(RayWithCoef &view, RayWithCoef &view_reflect, RayWithCoef &view_refract, Vec &normal, Object *tar_obj);

        void load_scene(const char *filename);
        Vec array2vec(const jsonxx::Array &a);
        Triangle *array2triangle(const jsonxx::Array &a);

		//For soft shadow
		void sample_lights(const int radius, const double ratio);
		void copy_lights();
		bool is_sampled(const Vec &pos);

};

#endif

