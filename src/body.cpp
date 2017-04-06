#include "body.hpp"
#include <queue>
#include <iostream>
using std::cerr;
using std::endl;
using std::queue;

double Body::nearest_intersect(ray_trans &h){
    queue<ContainBox*> possible_box;
    possible_box.push(box);
    vector<Triangle*> possible_triangle;
    while(!possible_box.empty()){
        ContainBox *current = possible_box.front();

        if (current->isPassing(h.ray)){
            if (current->children[0] != NULL){
                for(int i = 0; i < 8; ++i) {
                    possible_box.push(current->children[i]);
				}
            }else{ 
				for(int i = 0; i < current->triangles.size(); ++i) { 
					possible_triangle.push_back(current->triangles[i]);
				}
			}
        }
        possible_box.pop();
    }

    Triangle *target = NULL;
    h.ray_t = NOT_INTERSECT;

    for(int i = 0 ; i < possible_triangle.size() ; i += 1) {
        double t = possible_triangle[i]->nearest_intersect(h.ray);
        if(t != NOT_INTERSECT && (h.ray_t == NOT_INTERSECT || t < h.ray_t)){
            h.ray_t = t;
            target = possible_triangle[i];
        }
    }

    if(target != NULL){
        Vec p = h.ray.st + h.ray_t * h.ray.dir;
        h.normal= Ray(p, target->get_normal(p, h.ray));
    }
    return h.ray_t;
}

void Body::print() const {
    cerr << "(" << box->st.x << ", " << box->st.y << ", " << box->st.z <<
        ") -> (" << box->size.x << ", " << box->size.y << ", " << box->size.z << ")\n";
}
