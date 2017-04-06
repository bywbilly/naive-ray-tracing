#ifndef H_CONTAIN_BOX
#define H_CONTAIN_BOX

#include <vector>
#include "triangle.hpp"
#include "ray.hpp"


class ContainBox{
    public:
        static const int max_objects;

		std::vector<Triangle*> triangles;

        Vec st, size;
        ContainBox(const std::vector<Triangle *> _t);

        ContainBox * children[8];

        void split();
        void fitSize();

        bool isPassing(const Ray & ray);
        double _intersect(double p0, double p, double s);
        void _swap(double &a, double &b){
            double tmp = a;
            a = b;
            b = tmp;
        }
};

#endif

