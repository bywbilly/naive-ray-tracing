#include "scene.hpp"
#include "ball.hpp"
#include "triangle.hpp"
#include "gridsurface.hpp"
#include "imagesurface.hpp"
#include "body.hpp"
#include "jsonxx.h"
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <fstream>
#include <random>
#include <iostream>
#include "opencv2/core/core.hpp"
using namespace std;
using cv::Mat;
using cv::Mat_;

const double PI = acos(-1.0);

Object *Scene::nearest_intersect(ray_trans &h){
    double dis_t = NOT_INTERSECT;
    Object *ret = NULL;
    ray_trans tmp_h = h;
    for(int i = 0 ; i < objects.size() ; i += 1){
        double tmp = objects[i]->nearest_intersect(tmp_h);
        if(tmp != NOT_INTERSECT && (dis_t == NOT_INTERSECT || tmp < dis_t)){
            dis_t = tmp;
            ret = objects[i];
            h = tmp_h;
        }
    }
    return ret;
}

Color Scene::phong(RayWithCoef & view, RayWithCoef &view_reflect, RayWithCoef &view_refract, Vec &normal, Object *tar_obj){
    Color ret(0, 0, 0);
    ray_trans view_hr(view.first);
    Object *target_obj = this->nearest_intersect(view_hr);
	normal = view_hr.normal.dir;
	tar_obj = target_obj;

    if (target_obj == NULL) return ret;

    view_reflect.first = target_obj->reflect(view_hr);
    view_reflect.second = view.second * target_obj->reflection_fact;
    view_refract.first = target_obj->refract(view_hr);
    view_refract.second = view.second * target_obj->refraction_fact;

    for (int i = 0; i < sampled_lights.size(); ++i) {
        Ray light_ray(sampled_lights[i]->pos, view_reflect.first.st - sampled_lights[i]->pos);
        ray_trans light_hr(light_ray);
        target_obj = nearest_intersect(light_hr);

        if (target_obj == NULL || !is_almost_same(light_hr.normal.st, view_reflect.first.st)) {
            continue;
		}
        ret += target_obj->lambert(light_hr, sampled_lights[i]->color);
        Ray light_reflect = target_obj->reflect(light_hr);

        double phong_term = light_reflect.dir.dot(-(view.first.dir));
        if (phong_term <= 0){
			continue;
		}

        phong_term = pow(phong_term, target_obj->specular_power) ;
        Color add_color = phong_term * (sampled_lights[i]->color);

        add_color.x *= target_obj->specular_fact.x;
        add_color.y *= target_obj->specular_fact.y;
        add_color.z *= target_obj->specular_fact.z;

        ret += add_color;
    }
    return ret * view.second;
}

vector<Vec> Triangle::normals;

void Scene::load_scene(const char * filename){

	//For Calculating the normal vector of each point
	map<int, vector<Vec>> pts;

    cerr << "Reading " << filename << "...\n";
    objects.clear();
    lights.clear();
	sampled_lights.clear();
    ifstream fin(filename);
    json.parse(fin);
    // objects
    assert(json.has<jsonxx::Array>("objects"));
    jsonxx::Array _objs = json.get<jsonxx::Array>("objects");
    for(int i = 0 ; i < _objs.size() ; i += 1){
        Object * obj = NULL;
        jsonxx::Object _obj = _objs.get<jsonxx::Object>(i);
        assert(_obj.has<jsonxx::String>("type"));
        jsonxx::String _type = _obj.get<jsonxx::String>("type");
        cerr << "Reading object " << _type << endl;
        if(_type == "Ball"){
            assert(_obj.has<jsonxx::Array>("center"));
            assert(_obj.has<jsonxx::Number>("radius"));
            obj = new Ball(
                    array2vec(_obj.get<jsonxx::Array>("center")), 
                    _obj.get<jsonxx::Number>("radius")
                    );
        }
        if(_type == "GridSurface"){
            assert(_obj.has<jsonxx::Array>("triangle"));
            assert(_obj.has<jsonxx::Array>("colors"));
            assert(_obj.has<jsonxx::Number>("grid_width"));
            jsonxx::Array _tri = _obj.get<jsonxx::Array>("triangle");
            jsonxx::Array _co = _obj.get<jsonxx::Array>("colors");
            obj = new GridSurface(
                    array2vec(_tri.get<jsonxx::Array>(0)), 
                    array2vec(_tri.get<jsonxx::Array>(1)), 
                    array2vec(_tri.get<jsonxx::Array>(2)), 
                    array2vec(_co.get<jsonxx::Array>(0)), 
                    array2vec(_co.get<jsonxx::Array>(1)), 
                    _obj.get<jsonxx::Number>("grid_width")
                    );
        }
        if(_type == "Body"){
            vector<Triangle*> tris;
            cerr << "Reading triangles..." ;
            if(_obj.has<jsonxx::Array>("triangles")){
                jsonxx::Array _tris = _obj.get<jsonxx::Array>("triangles");
                for(int j = 0 ; j < _tris.size() ; j += 1)
                    tris.push_back(array2triangle(_tris.get<jsonxx::Array>(j)));
            }
            else{
                assert(_obj.has<jsonxx::String>("objfile"));
                ifstream objfile(_obj.get<jsonxx::String>("objfile").c_str());
				assert(objfile.good());
                cerr << "Opened obj file " << _obj.get<jsonxx::String>("objfile") << endl;
                Mat rotate_mat = (Mat_<double>(3, 3) <<
                            1, 0, 0, 
                            0, 1, 0, 
                            0, 0, 1);
                if(_obj.has<jsonxx::Number>("rotate_x")){
                    double theta = _obj.get<jsonxx::Number>("rotate_x");
                    theta = theta * PI / 180.0;
                    rotate_mat *= (Mat_<double>(3, 3) <<
                            1, 0, 0, 
                            0, cos(theta), -sin(theta), 
                            0, sin(theta), cos(theta));
                }
                if(_obj.has<jsonxx::Number>("rotate_y")){
                    double theta = _obj.get<jsonxx::Number>("rotate_y");
                    theta = theta * PI / 180.0;
                    rotate_mat *= (Mat_<double>(3, 3) <<
                            cos(theta), 0, sin(theta), 
                            0, 1, 0, 
                            -sin(theta), 0, cos(theta));
                }
                if(_obj.has<jsonxx::Number>("rotate_z")){
                    double theta = _obj.get<jsonxx::Number>("rotate_z");
					theta = theta * PI / 180.0;
                    rotate_mat *= (Mat_<double>(3, 3) <<
                            cos(theta), -sin(theta), 0, 
                            sin(theta), cos(theta), 0, 
                            0, 0, 1);
                }
                Vec move(0, 0, 0);
                if(_obj.has<jsonxx::Array>("move"))
                    move = array2vec(_obj.get<jsonxx::Array>("move"));
                double resize = 1.0;
                if(_obj.has<jsonxx::Number>("resize"))
                    resize = _obj.get<jsonxx::Number>("resize");
                vector<Vec> vecs;

				//FILE *f = fopen(_obj.get<jsonxx::String>("objfile").c_str(), "r");
				//if (!f) {
				//	cerr << "NO SUCH A OBJ FILE" << endl;
				//}

				//char buf[100];
				//while (fscanf(f, " %s", buf) != EOF) {
				//	if (strcmp(buf, "v") == 0) {
				//		double a, b, c;
				//		fscanf(f, "%f%f%f", &a, &b, &c);
                //        Mat rotated = rotate_mat * (Mat_<Number>(3, 1) << a, b, c);
                //        vecs.push_back(Vec(rotated.at<Number>(0, 0), 
				//		rotated.at<Number>(0, 1), 
                //        rotated.at<Number>(0, 2)) * resize + move);
				//	} else if (strcmp(buf, "f") == 0) {
				//		int v[3];
				//		for (int i = 0; i < 3; ++i) {
				//			fscanf(f, " %s", buf);
				//			sscanf(buf, "%d", &v[i]);
				//		}
				//		tris.push_back(new Triangle(vecs[v[0] - 1], vecs[v[1] - 1], vecs[v[2] - 1]));
				//	} else if (buf[0] == '#'
				//			|| strcmp(buf, "mtllib") == 0
				//			|| strcmp(buf, "vn") == 0
				//			|| strcmp(buf, "vt") == 0
				//			|| strcmp(buf, "s") == 0
				//			|| strcmp(buf, "g") == 0
				//			|| strcmp(buf, "o") == 0
				//			|| strcmp(buf, "usemtl") == 0) {
				//		while (fgetc(f) != '\n');
				//	} else {
				//		cerr << buf << endl;
				//		cerr << "miaomiaomimao on reading meshes" << endl;
				//	}
				//}

                while(objfile.peek() != -1){
                    char buf[100];
                    objfile >> buf;
                    if(strcmp(buf, "v") == 0){
                        double a, b, c;
                        objfile >> a >> b >> c;
                        Mat rotated = rotate_mat * (Mat_<double>(3, 1) << a, b, c);
                        vecs.push_back(Vec(rotated.at<double>(0, 0), 
                                    rotated.at<double>(0, 1), 
                                    rotated.at<double>(0, 2)) * resize + move);
                    }
                    else if (strcmp(buf, "f") == 0) { // f
						//int a, b, c;
                        //objfile >> a >> b >> c;
                        //tris.push_back(new Triangle(vecs[a-1], vecs[b-1], vecs[c-1]));
						int v[3];
						for (int i = 0; i < 3; ++i) {
							objfile >> buf;
							sscanf(buf, "%d", &v[i]);
						}
						auto cur_tri = new Triangle(vecs[v[0] - 1], vecs[v[1] - 1], vecs[v[2] - 1], v[0] - 1, v[1] - 1, v[2] - 1);
						auto cur_normal = cur_tri->trivial_normal();
						for (int ii = 0; ii < 3; ++ii) {
							if (pts.find(v[ii] - 1) == pts.end()) {
								vector<Vec> tmp;
								tmp.push_back(cur_normal);
								pts[v[ii] - 1] = tmp;
							} else {
								pts[v[ii] - 1].push_back(cur_normal);
							}
						}
						tris.push_back(new Triangle(vecs[v[0] - 1], vecs[v[1] - 1], vecs[v[2] - 1], v[0] - 1, v[1] - 1, v[2] - 1));
                    } else {
						if (buf[0] == '#'
						|| strcmp(buf, "mtllib") == 0
						|| strcmp(buf, "vn") == 0
						|| strcmp(buf, "vt") == 0
						|| strcmp(buf, "s" ) == 0
						|| strcmp(buf, "g" ) == 0
						|| strcmp(buf, "o" ) == 0
						|| strcmp(buf, "usemtl") == 0) {
							while(objfile.get() != '\n');
						} else {
							cerr << buf << endl;
							cerr << "wtf on  reading obj file" << endl;
						}
					}
                }
                cerr << "Read " << tris.size() << " faces." << endl;
                objfile.close();
            }
            cerr << " done." << endl;
            obj = new Body(tris);
            obj->print();
        }
        if(_type == "ImageSurface"){
            assert(_obj.has<jsonxx::Array>("triangle"));
            assert(_obj.has<jsonxx::String>("img"));
            jsonxx::Array _tri = _obj.get<jsonxx::Array>("triangle");
            obj = new ImageSurface(
                    array2vec(_tri.get<jsonxx::Array>(0)), 
                    array2vec(_tri.get<jsonxx::Array>(1)), 
                    array2vec(_tri.get<jsonxx::Array>(2)), 
                    _obj.get<jsonxx::String>("img").c_str()
                    );
        }
        if(obj == NULL) continue;
        
        if(_obj.has<jsonxx::Number>("N"))
            obj->N = _obj.get<jsonxx::Number>("N");
        if(_obj.has<jsonxx::Number>("reflection_fact"))
            obj->reflection_fact = _obj.get<jsonxx::Number>("reflection_fact");
        if(_obj.has<jsonxx::Number>("refraction_fact"))
            obj->refraction_fact = _obj.get<jsonxx::Number>("refraction_fact");
        if(_obj.has<jsonxx::Number>("specular_power"))
            obj->specular_power = _obj.get<jsonxx::Number>("specular_power");
        if(_obj.has<jsonxx::Array>("specular_fact"))
            obj->specular_fact = array2vec(_obj.get<jsonxx::Array>("specular_fact"));
        if(_obj.has<jsonxx::Array>("diffuse_fact"))
            obj->diffuse_fact = array2vec(_obj.get<jsonxx::Array>("diffuse_fact"));
        this->objects.push_back(obj);
    }
    cerr << "Objects read ok." << endl;
	
	cerr << "Start prepareing for the ground shading" << endl;

	for (auto iter = pts.begin(); iter != pts.end(); ++iter) {
		auto pts_normals = (*iter).second;
		auto final_normal = pts_normals[0];
		for (int ii = 1; ii < (int)pts_normals.size(); ++ii) {
			final_normal = final_normal + pts_normals[ii];	
		}
		auto denom = (double)pts_normals.size();
		final_normal.x /= denom;
		final_normal.y /= denom;
		final_normal.z /= denom;
		Triangle::normals.push_back(final_normal);
	}
    // lights
    assert(json.has<jsonxx::Array>("lights"));
    jsonxx::Array _ligs = json.get<jsonxx::Array>("lights");
    for(int i = 0 ; i < _ligs.size() ; i += 1){
        jsonxx::Object _lig = _ligs.get<jsonxx::Object>(i);
        this->lights.push_back(new Light(
                    array2vec(_lig.get<jsonxx::Array>("point")), 
                    array2vec(_lig.get<jsonxx::Array>("color"))
                    ));
    }
}

Vec Scene::array2vec(const jsonxx::Array & a){
    Vec ret;
    ret.x = a.get<jsonxx::Number>(0);
    ret.y = a.get<jsonxx::Number>(1);
    ret.z = a.get<jsonxx::Number>(2);

    return ret;
}

Triangle *Scene::array2triangle(const jsonxx::Array & a){
    Triangle *tri = new Triangle;
    assert(json.has<jsonxx::Object>("points"));
    jsonxx::Object points = json.get<jsonxx::Object>("points");

    for(int i = 0; i < 3; ++i){
        if(a.has<jsonxx::Array>(i))
            (*tri)[i] = array2vec(a.get<jsonxx::Array>(i));
        else{
            jsonxx::String tmp = a.get<jsonxx::String>(i);
            assert(points.has<jsonxx::Array>(tmp));
            (*tri)[i] = array2vec(points.get<jsonxx::Array>(tmp));
        }
    }
    return tri;
}

void Scene::copy_lights() {
	for (int i = 0; i < (int)this->lights.size(); ++i) {
		this->sampled_lights.push_back(this->lights[i]);
	}
}

bool Scene::is_sampled(const Vec &pos) {
	for (int i = 0; i < (int)this->sampled_lights.size(); ++i) {
		Vec light_center = this->sampled_lights[i]->pos;
		if (SIGN(pos.x - light_center.x) && SIGN(pos.y - light_center.y) && SIGN(pos.z - light_center.z)) {
			return 1;
		}
	}

	return 0;
}

void Scene::sample_lights(const int radius, const double ratio) {
	int light_sz = (int)this->lights.size();

	for (int i = 0; i < light_sz; ++i) {
		Vec light_center = this->lights[i]->pos;
		Color light_color = this->lights[i]->color;
		double x = light_center.x;
		double y = light_center.y;
		double z = light_center.z;
		
		for (int dx = 0; dx < radius; ++dx) {
			for (int dy = 0; dy < radius; ++dy) {
				for (int dz = 0; dz < radius; ++dz) {
					double xx = x + dx;
					double yy = y + dy;
					double zz = z + dz;
					Vec light_dir(xx, yy, zz); 

					Light *light_new = new Light(light_dir, light_color);
					if (!is_sampled(light_dir)) {
						this->sampled_lights.push_back(light_new);
					}
				}
			}
		}
	}

	int sz = sampled_lights.size();

	vector<Light*> tmp; 
	tmp.clear();

	cerr << "The ratio is: " << ratio << endl;

	for (int i = 0; i < (int)this->sampled_lights.size(); ++i) {
		double ran = (double(rand()) / (RAND_MAX));
		cerr << ran << endl;
		if (ran < ratio) {
			tmp.push_back(sampled_lights[i]);
		}
	}

	sampled_lights.clear();
	for (int i = 0; i < (int)tmp.size(); ++i) {
		sampled_lights.push_back(tmp[i]);
	}

	cerr << "After the sample, the number of lights are: " << (int)this->sampled_lights.size() << endl;

}
