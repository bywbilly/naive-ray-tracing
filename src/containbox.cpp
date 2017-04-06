#include <vector>
#include <iostream>
#include <cstring>
#include <algorithm>
#include "triangle.hpp"
#include "ray.hpp"
#include "containbox.hpp"

using std::cerr;
using std::endl;
using std::vector;
using std::min_element;
using std::max_element;

const int ContainBox::max_objects = 5;

static bool compare_x(Triangle *a, Triangle *b){
    double aa[] = {a->a.x, a->b.x, a->c.x};
    double bb[] = {b->a.x, b->b.x, b->c.x};
    return *min_element(aa, aa + 3) < *min_element(bb, bb + 3);
}

static bool compare_y(Triangle *a, Triangle *b){
    double aa[] = {a->a.y, a->b.y, a->c.y};
    double bb[] = {b->a.y, b->b.y, b->c.y};
    return *min_element(aa, aa + 3) < *min_element(bb, bb + 3);
}

void ContainBox::split(){
    vector<Triangle *> x_dir[2];
	std::sort(triangles.begin(), triangles.end(), compare_x);

	int size = triangles.size();

    x_dir[0] = vector<Triangle*>(triangles.begin(), triangles.begin() + (size >> 1));
    x_dir[1] = vector<Triangle*>(triangles.begin() + (size >> 1), triangles.end());

    for(int x = 0 ; x < 2 ; ++x){
        vector<Triangle *> y_dir[2];
        sort(x_dir[x].begin(), x_dir[x].end(), compare_y);

		int size = x_dir[x].size();
        y_dir[0] = vector<Triangle*>(x_dir[x].begin(), x_dir[x].begin() + (size >> 1));
        y_dir[1] = vector<Triangle*>(x_dir[x].begin() + (size >> 1), x_dir[x].end());

        for(int y = 0 ; y < 2 ; ++y){
            vector<Triangle *> z_dir[2];
            sort(y_dir[y].begin(), y_dir[y].end(), compare_y);

			int size = y_dir[y].size();
            z_dir[0] = vector<Triangle*>(y_dir[y].begin(), y_dir[y].begin() + (size >> 1));
            z_dir[1] = vector<Triangle*>(y_dir[y].begin() + (size >> 1), y_dir[y].end());

            for(int z = 0; z < 2; ++z) {
                children[x * 4 + y * 2 + z] = new ContainBox(z_dir[z]);
			}
        }
    }
    triangles.clear();
}

void ContainBox::fitSize(){
    if(triangles.empty()) {
		return;
	}

    vector<double> nums;

    for(int i = 0; i < triangles.size(); ++i)
        for(int j = 0; j < 3; ++j) {
            nums.push_back((*triangles[i])[j].x);
		}

    st.x = *min_element(nums.begin(), nums.end()) - EPSILON;
    size.x = *max_element(nums.begin(), nums.end()) - st.x + EPSILON;
    nums.clear();

    for(int i = 0; i < triangles.size(); ++i)
        for(int j = 0 ; j < 3 ; ++j) {
            nums.push_back((*triangles[i])[j].y);
		}

    st.y = *min_element(nums.begin(), nums.end()) - EPSILON;
    size.y = (*max_element(nums.begin(), nums.end())) - st.y + EPSILON;
    nums.clear();

    for(int i = 0; i < triangles.size(); ++i)
        for(int j = 0; j < 3; ++j) { 
            nums.push_back((*triangles[i])[j].z);
		}

    st.z = *min_element(nums.begin(), nums.end()) - EPSILON;
    size.z = *max_element(nums.begin(), nums.end()) - st.z + EPSILON;
    nums.clear();
}

ContainBox::ContainBox(const vector<Triangle*> _t){
    triangles = _t;
    fitSize();

    if(triangles.size() > max_objects) {
		split();
	}else{ 
		memset(children, 0, 8 * sizeof(ContainBox *));
	}
}

double ContainBox::_intersect(double p0, double p, double s){
    if (p == 0) {
		return NOT_INTERSECT;
	}

    return (s - p0) / p;
}

bool ContainBox::isPassing(const Ray & ray){
    if(children[0] == NULL && triangles.empty()) return false;
    if(ray.st.x - st.x > -EPSILON && 
       ray.st.y - st.y > -EPSILON &&
       ray.st.z - st.z > -EPSILON &&
       ray.st.x - (st.x + size.x) < EPSILON &&
       ray.st.y - (st.y + size.y) < EPSILON &&
       ray.st.z - (st.z + size.z) < EPSILON) {
        return true;
	}
    double mins[3];
    double maxs[3];

    mins[0] = _intersect(ray.st.x, ray.dir.x, st.x);
    maxs[0] = _intersect(ray.st.x, ray.dir.x, st.x+size.x);
    if(mins[0] > maxs[0]) {
		_swap(mins[0], maxs[0]);
	}

    mins[1] = _intersect(ray.st.y, ray.dir.y, st.y);
    maxs[1] = _intersect(ray.st.y, ray.dir.y, st.y+size.y);
    if(mins[1] > maxs[1]) {
		_swap(mins[1], maxs[1]);
	}

    mins[2] = _intersect(ray.st.z, ray.dir.z, st.z);
    maxs[2] = _intersect(ray.st.z, ray.dir.z, st.z+size.z);
    if(mins[2] > maxs[2]) {
		_swap(mins[2], maxs[2]);
	}

    double max_of_mins = * max_element(mins, mins+3);
    double min_of_maxs = maxs[0];

    if(min_of_maxs == NOT_INTERSECT ||  (maxs[1] != NOT_INTERSECT && maxs[1] < min_of_maxs)) {
        min_of_maxs = maxs[1];
	}
    if(min_of_maxs == NOT_INTERSECT || (maxs[2] != NOT_INTERSECT && maxs[2] < min_of_maxs)) { 
        min_of_maxs = maxs[2];
	}

    return max_of_mins <= min_of_maxs;
}
