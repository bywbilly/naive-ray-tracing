/// @file sphere.cpp  @version 1.0 @date 06/11/2013
/// @author BlahGeek@Gmail.com

#include "sphere.hpp"

double Sphere::nearest_intersect(Ray & ray){
    Vec l = this->center - ray.st;
    double tp = l.dot(ray.dir);
    double tmp = l.dot(l) - this->radius * this->radius;

    if (SIGN(tmp)){ // on the surface
        ray.st += ray.dir* EPSILON; // quick hack
        return this->nearest_intersect(ray);
    }

    if (tmp > 0 && tp < 0) { 
		return NOT_INTERSECT;
	}

    double d2 = l.dot(l) - tp * tp;
    double t_ = this->radius * this->radius - d2;

    if(t_ < 0) {
		return NOT_INTERSECT;
	}

    t_ = sqrt(t_);

    if(tmp > 0) {
		return tp - t_;
	}else{
		return tp + t_;
	}
}

Vec Sphere::get_normal(const Vec &r, const Ray &ray){
    return r - this->center;
}

