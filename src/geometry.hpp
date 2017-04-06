/// @file geometry.hpp  @version 1.0 @date 06/11/2013
/// @author BlahGeek@Gmail.com
#ifndef H_GEOMETRY_
#define H_GEOMETRY_

#include "common.hpp"
#include "ray.hpp"
#include "object.hpp"

class Geometry{
    public:
        virtual double nearest_intersect(Ray &ray) = 0;
        virtual Vec get_normal(const Vec &p, const Ray &ray) = 0;
};

#endif

