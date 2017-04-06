#ifndef H_OBJECT_
#define H_OBJECT_

#include "common.hpp"
#include "geometry.hpp"
#include "rayTrans.hpp"
#include <vector>
using std::vector;

class Object{
    public:
        double N; // refractive index
        double reflection_fact, refraction_fact;
        double specular_power;
        Vec specular_fact;
        Vec diffuse_fact;

        Object(): reflection_fact(0.5), N(1.2), refraction_fact(0.5), 
            specular_fact(Vec(1, 1, 1)), 
            specular_power(60.0), 
            diffuse_fact(1, 1, 1){}

        virtual double nearest_intersect(ray_trans &h) = 0;
        virtual Vec getDiffuseFace(const Vec & p) const;
        Ray reflect(ray_trans &h);
        Ray refract(ray_trans &h);

        virtual void print() const = 0;

        Color lambert(ray_trans &h, const Color & light_color);
};


#endif

