/// @file sphere.hpp  @version 1.0 @date 06/10/2013
/// @author BlahGeek@Gmail.com
/// @brief Class for sphere
#ifndef H_SPHERE_
#define H_SPHERE_

#include "common.hpp"
#include "geometry.hpp"

class Sphere: public Geometry{
    public:
        Vec center;
        double radius;
        Sphere(const Vec &_c, const double _r):center(_c), radius(_r) {}

        double nearest_intersect(Ray &ray);
        Vec get_normal(const Vec &p, const Ray &ray);
};

#endif

