#ifndef H_GRID_SURFACE
#define H_GRID_SURFACE

#include "triangle.hpp"
#include "object.hpp"
#include "surface.hpp"
#include "common.hpp"

class GridSurface: public Surface {
    public:
        Vec diffuse0, diffuse1;
        double grid_width;

        GridSurface(const Vec & a, const Vec & b, const Vec & c, const Vec & _c0, const Vec & _c1, double _w): Surface(a, b, c), diffuse0(_c0), diffuse1(_c1), grid_width(_w){}
        Vec getDiffuseFace(const Vec & p) const;
};

#endif

