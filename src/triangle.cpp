/// @file triangle.cpp  @version 1.0 @date 06/12/2013
/// @author BlahGeek@Gmail.com

#include "triangle.hpp"
#include "opencv2/core/core.hpp"
#include <algorithm>
#include <iostream>
using namespace std;
using cv::Mat;
using cv::Mat_;

double Triangle::nearest_intersect(Ray & ray){
    return _nearest_intersect(ray);
}

double Triangle::_nearest_intersect(Ray & ray, int test_count){
    if (test_count > 2){
		return NOT_INTERSECT;
	}

    Mat A = (Mat_<double>(3, 3) <<
            ray.dir.x, a.x - b.x, a.x - c.x, 
            ray.dir.y, a.y - b.y, a.y - c.y, 
            ray.dir.z, a.z - b.z, a.z - c.z);
    Mat b = (Mat_<double>(3, 1) << 
            a.x-ray.st.x, 
            a.y-ray.st.y, 
            a.z-ray.st.z);
    A = A.inv();
    Mat result = A * b;

    double  t = result.at<double>(0, 0);
    double  beta = result.at<double>(0, 1);
    double  gama = result.at<double>(0, 2);
    if (SIGN(t)){ 
        ray.st += ray.dir * EPSILON; 
        return this->_nearest_intersect(ray, test_count + 1);
    }

    if (t < 0 || (!infinity && (beta < 0 || beta > 1 || gama < 0 || gama > 1 || beta + gama > 1)))
        return NOT_INTERSECT;

    return t;
}

//Vec Triangle::get_normal(const Vec &p){
//    Vec A = b - a;
//    Vec B = c - a;
//    return Vec(A.y * B.z - A.z * B.y, 
//			   A.z * B.x - A.x * B.z, 
//          	   A.x * B.y - A.y * B.x);
//}

Vec Triangle::trivial_normal() {
	Vec A = b - a;
	Vec B = c - a;
	return Vec(A.y * B.z - A.z * B.y,
			   A.z * B.x - A.x * B.z,
			   A.x * B.y - A.y * B.x);
}

//For ground shading
Vec Triangle::get_normal(const Vec &p, const Ray &ray) {

	
	if (normals.empty()) {
		return trivial_normal();
	}

	Vec vec_p = ray.dir.cross(c - a);
	Vec vec_t = ray.st - a;
	Vec vec_q = vec_t.cross(b - a);
	double det = vec_p.dot(b - a);
	double u = vec_t.dot(vec_p) / det;
	double v = ray.dir.dot(vec_q) / det;

	return (1.0 - u - v) * normals[index1] + u * normals[index2] + v * normals[index3];
}

Vec & Triangle::operator [] (int i){
    if (i == 0) {
		return a;
	} else if (i == 1) {
		return b;
	} else if (i == 2) {
		return c;
	} else {
		std::cerr << "miaomiaomiao at triangle operator []" << std::endl;
	}
}
