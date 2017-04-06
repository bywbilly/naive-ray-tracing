#ifndef H_BODY_
#define H_BODY_

#include "object.hpp"
#include "triangle.hpp"
#include "containbox.hpp"
#include <vector>
using std::vector;

class Body: public Object{
    public:
        ContainBox * box;
        Body(vector<Triangle *> _t): box(new ContainBox(_t)){}
        double nearest_intersect(ray_trans &h);
        void print() const;
};

#endif

