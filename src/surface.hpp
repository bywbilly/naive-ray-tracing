#ifndef H_SURFACE_
#define H_SURFACE_

#include "triangle.hpp"
#include "object.hpp"
#include "common.hpp"

class Surface: public Object {
    public:
        Triangle tri;

        Surface(const Vec &a, const Vec &b, const Vec &c):tri(a, b, c, true) {
                N = 1.0; 
        }
        double nearest_intersect(ray_trans &h);
        void print() const {return;}
};

#endif

