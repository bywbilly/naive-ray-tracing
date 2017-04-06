#include <cmath>
#include "object.hpp"
#include "common.hpp"
#include "surface.hpp"

double Surface::nearest_intersect(ray_trans &h){
    h.ray_t = tri.nearest_intersect(h.ray);
    if(h.ray_t != NOT_INTERSECT){
        Vec p = h.ray.st+ h.ray_t * h.ray.dir;
		h.normal= Ray(p, tri.get_normal(p, h.ray));
		//h.normal= Ray(p, tri.trivial_normal());
    }
    return h.ray_t;
}

