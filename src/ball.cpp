/// @file ball.cpp  @version 1.0 @date 06/11/2013
/// @author BlahGeek@Gmail.com

#include "ball.hpp"
#include <iostream>
#include <cmath>
using std::cerr;
using std::endl;

double Ball::nearest_intersect(ray_trans &h){
    h.ray_t = sphere.nearest_intersect(h.ray);
    Vec point = h.ray.st+ h.ray_t * h.ray.dir;
    h.normal = Ray(point, sphere.get_normal(point, h.ray));

    return h.ray_t;
}

void Ball::print() const{
	cerr << "The ball is: " << endl;
    cerr << "Center: (" << sphere.center.x << ", " << sphere.center.y 
        << ", " << sphere.center.z << "), Radius: " << sphere.radius << endl;
}
