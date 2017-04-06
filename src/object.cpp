/// @file object.cpp  @version 1.0 @date 06/12/2013
/// @author BlahGeek@Gmail.com

#include "object.hpp"
#include "ray.hpp"
#include <iostream>
#include <cmath>
using std::cerr;
using std::endl;

Ray Object::reflect(ray_trans &h){
    Vec delta = h.ray.dir.dot(h.normal.dir) * h.normal.dir;
    Ray ret(h.ray.st + h.ray.dir * h.ray_t, h.ray.dir - 2 * delta);

    return ret;
}

Color Object::lambert(ray_trans &h, const Color &light_color){
    double theta = h.normal.dir.dot(-h.ray.dir);
    if (theta < 0){ 
		theta = -theta;
	}

    Color ret = light_color * theta;
	Vec diffuse = getDiffuseFace(h.normal.st);

    ret.x *= diffuse.x;
    ret.y *= diffuse.y;
    ret.z *= diffuse.z;

    return ret;
}

Vec Object::getDiffuseFace(const Vec &p) const{
    return diffuse_fact;
}

Ray Object::refract(ray_trans &h){
    double t = (SIGN(h.ray.N - 1.0) ? (1.0 / this->N) : this->N);
    double cos_theta = h.ray.dir.dot(h.normal.dir);
    Vec normal_dir = (cos_theta < 0 ? h.normal.dir: -h.normal.dir);

    double cos_theta_sqr = pow(cos_theta, 2);
    double cos_theta2_sqr = 1 - t * t * (1 - cos_theta_sqr);

    if (cos_theta2_sqr < 0){
		return h.ray;
	}
    Vec dir = t * h.ray.dir+ (t * normal_dir.dot(-h.ray.dir) - sqrt(cos_theta2_sqr)) * normal_dir;
    Ray ret(h.normal.st, dir);

    if(SIGN(h.ray.N - 1.0)){
		ret.N = this->N;
	} else {
		ret.N = 1.0;
	}

    return ret;
}
