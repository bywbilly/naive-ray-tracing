#ifndef H_RAY_
#define H_RAY_

#include "common.hpp"
#include <cmath>
#include <utility>

class Ray{
    public:
        Vec st, dir;
        double N; // refractive index
        Ray(): N(1.0) {}
        Ray(const Ray & r): N(1.0), st(r.st), dir(r.dir){}
        Ray(const Vec & _s, const Vec & _d):N(1.0), st(_s), dir(_d * (1.0 / sqrt(_d.dot(_d)))) {}
        Ray & operator = (const Ray & r);
        void print() const;
};
typedef std::pair<Ray, double> RayWithCoef;

bool is_almost_same(const Vec & a, const Vec & b);

#endif

