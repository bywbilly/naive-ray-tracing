#ifndef H_BALL_
#define H_BALL_

#include "object.hpp"
#include "sphere.hpp"

class Ball: public Object{
    public:
        Sphere sphere;
        Ball(const Vec & _c, const double _r):sphere(_c, _r) {}
        double nearest_intersect(ray_trans &h);
        void print() const ;
};

#endif

