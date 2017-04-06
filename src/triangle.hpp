#ifndef H_TRIANGLE_
#define H_TRIANGLE_

#include "geometry.hpp"
#include "ray.hpp"
#include <vector>

class Triangle: public Geometry{
    public:

		static std::vector<Vec> normals;

        Vec a, b, c;
		int index1, index2, index3;
        bool infinity;

        Triangle(): infinity(false){}
        Triangle(const Vec & _a, const Vec & _b, const Vec & _c, int _in1 = 0, int _in2 = 0, int _in3 = 0, bool _i = false):a(_a), b(_b), c(_c), index1(_in1), index2(_in2), index3(_in3), infinity(_i) {}
        double nearest_intersect(Ray &ray);
        double _nearest_intersect(Ray &ray, int test_count = 0);
		Vec trivial_normal();
        Vec get_normal(const Vec &p, const Ray &ray);

        Vec & operator [] (int i);
};

#endif

