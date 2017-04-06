#include "ray.hpp"
#include <iostream>
using namespace std;

bool is_almost_same(const Vec & a, const Vec & b){
    if(SIGN(a.x-b.x) && SIGN(a.y-b.y) && SIGN(a.z-b.z))
        return true;

    return false;
}

Ray & Ray::operator = (const Ray & r){
    st= r.st;
    dir= r.dir;
    N = r.N;
    return *this;
}

void Ray::print() const {
    cerr << "(" << st.x << ", " << st.y << ", " << st.z << ") -> ("
        << dir.x << ", " << dir.y << ", " << dir.z << ")\n";
}

