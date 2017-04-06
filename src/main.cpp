#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <cstdio>
#include <random>
#include "common.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "geometry.hpp"
#include "scene.hpp"
#include "object.hpp"
#include "PngOutput.h"
#include "ball.hpp"
#include "light.hpp"
#include "body.hpp"
#include "imagesurface.hpp"
#include "gridsurface.hpp"
#include "OptionParser.h"

using namespace std;
using namespace optparse;

const double PI = acos(-1.0);

inline double randf() {
	return rand() / static_cast<double>(RAND_MAX);
}

inline Vec normalize(const Vec &v) {
	double denom = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	Vec ret_v(v.x / denom, v.y / denom, v.x / denom);
	return ret_v;
}

inline Vec vec_add(const Vec &v, double add) {
	Vec ret_vec;
	ret_vec.x = v.x + add;
	ret_vec.y = v.y + add;
	ret_vec.z = v.z + add;
	return ret_vec;
}

inline Vec vec_multiply(const Vec &v, double eps) {
	Vec ret_vec;
	ret_vec.x = v.x * eps;
	ret_vec.y = v.y * eps;
	ret_vec.z = v.z * eps;
	return ret_vec;
}

bool is_block(const Color &color) {
	if (SIGN(color.x) && SIGN(color.y) && SIGN(color.z)) {
		return true;
	}

	if (color.x < 180.0 && SIGN(color.x - color.y) && SIGN(color.x - color.z)) {
		return true;
	}

	return false;
}


int main ( int argc, char *argv[] )
{

    OptionParser parser = OptionParser()
        .usage("Usage: %prog [OPTION] Input Output")
        .description("Input .json scene file.\nOutput .png image file.");
    parser.add_option("-w", "--width").action("store").type("int").set_default(640)
        .help("width of output image. default: %default.");
    parser.add_option("-h", "--height").action("store").type("int").set_default(480)
        .help("height of output image. default: %default.");
    parser.add_option("-d", "--distance").action("store").type("int").set_default(1000)
        .help("distance from view point to screen. default: %default.");
    parser.add_option("-e", "--epsilon").action("store").type("float").set_default(0.01)
        .help("stop tracing after the intensity of ray reaches epsilon. default: %default.");
    parser.add_option("-s", "--soft").action("store").type("int").set_default(0)
        .help("whether use soft-shadow default: false.");
    parser.add_option("-r", "--radius").action("store").type("int").set_default(4)
        .help("The sample radius using for the soft shadow default: 4.");
    parser.add_option("-a", "--ratio").action("store").type("double").set_default(0.75)
        .help("The sample ratio using for the soft shadow default: 0.75");
    parser.add_option("-b", "--diffuse_num").action("store").type("int").set_default(0)
        .help("Monte Carlo diffuse reflect lights number default: 0");

    Values &options = parser.parse_args(argc, argv);
    vector<string> args = parser.args();
    assert(args.size() == 2);

	srand(time(0));

    Scene scene;
    scene.load_scene(args[0].c_str());

    int width = (int)options.get("width");
    int height = (int)options.get("height");
    double epsilon = (float)options.get("epsilon");

    PngOutput * out = new PngOutput(width, height, args[1].c_str());

    Vec view_point(width / 2, height / 2, -(int)options.get("distance"));

	if ((int)options.get("soft") == 1) { 
		scene.sample_lights((int)options.get("radius"), (double)options.get("ratio"));
	} else {
		scene.copy_lights();
	}

#pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < width; ++i){
        for(int j = 0; j < height; ++j){
            Color color(0, 0, 0);
            vector<RayWithCoef> views;
            views.push_back(RayWithCoef(Ray(view_point, Vec(i, j, 0) - view_point), 1.0));

			int diffuse = 0;

            while(!views.empty()){
                RayWithCoef view = views.back();
                views.pop_back();

				Vec normal;
				Object *tar_obj;
                RayWithCoef view_reflect(view);
                RayWithCoef view_refract(view);

                if(view.second < epsilon) {
					continue;
				}
                color += scene.phong(view, view_reflect, view_refract, normal, tar_obj);

                if(!is_almost_same(view.first.st, view_reflect.first.st)) { 
                    views.push_back(view_reflect);
				}
                if(!is_almost_same(view.first.st, view_refract.first.st)) { 
                    views.push_back(view_refract);
				}
				//For Monte Carlo Method
				
				Ray ray = view.first;
				Vec RP  = ray.dir - 2.0 * ray.dir.dot(normal) * normal;
				Vec RN1(RP.z, RP.y, -RP.x);
				Vec RN2 = RP.cross(RN1);

				ray_trans view_intersect(view.first);

				for (int i = 0; i < (int)options.get("diffuse_num"); ++i) {
					if (diffuse == 1) {
						break;
					}
					if (tar_obj == NULL) {
						break;
					}

					diffuse = 1;
					double len = randf() * 10;
					double angle = static_cast<double>(randf() * 2 * PI);
					double xoff = len * cos(angle), yoff = len * sin(angle);
					Vec R = RP + RN1 * xoff + RN2 * yoff * 10;
					R = normalize(R);
					R = vec_multiply(R, epsilon);
					Ray ray_reflect(view_intersect.ray.dir.dot(view_intersect.normal.dir) * view_intersect.normal.dir + R , R);
					RayWithCoef view_diffuse(ray_reflect, view.second * tar_obj->reflection_fact);
					if (!is_almost_same(view.first.st, view_diffuse.first.st)) {
						views.push_back(view_diffuse);
					}

				}
            }

			//For soft shadow
			//if ((int)options.get("soft") && is_block(color)) {
			//	color.x = 255.0;
			//	color.y = 0;
			//	color.z = 0;

			//	//cerr << "The soft shadow pixel is" << endl;
			//	//cerr << color.x << ", " << color.y << ", " << color.z << endl;
			//}

			if ((int)options.get("soft") == 1) {
				double denom = scene.sampled_lights.size(); 
				color.x /= denom;
				color.y /= denom;
				color.z /= denom;
			}
			
            out->draw(i, j, color);
        }
    }
    out->finish();

    return 0;
}
