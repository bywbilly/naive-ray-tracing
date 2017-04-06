#include <cmath>
#include "object.hpp"
#include "common.hpp"
#include "gridsurface.hpp"

Vec GridSurface::getDiffuseFace(const Vec &p) const{
    double a = remainder(p.x, 2 * grid_width);
    double b = remainder(p.z, 2 * grid_width);
    if(SIGN(a)) a = 0;
    if(SIGN(b)) b = 0;

    if(a > 0 != b > 0){ 
		return diffuse0;
	}
    return diffuse1;
}
