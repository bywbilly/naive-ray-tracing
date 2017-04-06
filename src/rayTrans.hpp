#ifndef H_RAYTRANS_
#define H_RAYTRANS_

struct ray_trans {
    Ray ray;
    Ray normal;
    double ray_t;
    ray_trans(const Ray & _r): ray(_r){}
    ray_trans &operator = (const ray_trans &r){
        ray = r.ray; ray_t = r.ray_t; normal = r.normal;
    }
};

#endif
